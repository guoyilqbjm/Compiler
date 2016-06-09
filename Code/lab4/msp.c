#include "msp.h"
#include "../syntax.tab.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "../lab3/code_gr.h"

struct Var_Offset{
	char *name;
	int offset;
};
#define MAXOFFSET 100
typedef struct Var_Offset Var_Offset;
int preOffsetSize = 0;
Var_Offset offsetArray[MAXOFFSET];
int getOffset(char *name){
	int i=0;
	while(i<preOffsetSize){
		if(strcmp(offsetArray[i].name,name)==0)
			return offsetArray[i].offset;
		++i;
	}
	return -1;
}
void setOffset(char *name){
	offsetArray[preOffsetSize].offset = preOffsetSize;
	preOffsetSize = preOffsetSize + 4;
	offsetArray[preOffsetSize].name = name;
	preOffsetSize++;
}
struct RegStruct{
	OperandPoint operand;
	char name[4];
};
typedef struct RegStruct RegStruct;
#define regSize 8
RegStruct allTempReg[regSize];
OperandPoint TempOperand[regSize];
int stackOffset = 0;


void regInit(){
	int i = 0;
	stackOffset = 0;
	while(i<regSize){
		allTempReg[i].operand == NULL;
		allTempReg[i].name[0] = '$';
		allTempReg[i].name[1] = 't';
		allTempReg[i].name[2] = i + '0';
		allTempReg[i].name[3] = '\0';
		++i;
	} 
	srand(time(NULL));
}

char *get_operand_label(OperandPoint p){
	if(p->kind == CONSTANT)
		return p->data.value;
	else if(p->kind == TEMP)
		return get_temp_varname(p->data.temp_no);
	else if(p->kind == VARIABLE)
		return p->data.var_name;
	else if(p->kind == ADDRESS)
		return get_temp_varname(p->data.temp_no);
	return NULL;
}
int testEqual(OperandPoint op1, OperandPoint op2){
	if(op1 == NULL || op2 == NULL){
		printf("illegal operation!\n");
		return -1;
 	}
 	printf("opkind:%d %s op2kind:%d %s\n", op1->kind,get_operand_label(op1), op2->kind,get_operand_label(op2));
 	if(op1->kind == TEMP && op2->kind == VARIABLE && strcmp(get_temp_varname(op1->data.temp_no),op2->data.var_name)==0)
 		return 1;
 	if(op2->kind == TEMP && op1->kind == VARIABLE && strcmp(get_temp_varname(op2->data.temp_no),op1->data.var_name)==0)
 		return 1;
	if(op1->kind != op2->kind)
		return -1;
	printf("hhh %d\n", op1->kind -= op2->kind);
	if(op1->kind == VARIABLE && strcmp(op1->data.var_name,op2->data.var_name)==0)
		return 1;
	if(op2->kind == TEMP && op1->data.temp_no == op2->data.temp_no)
		return 1;
	if(op2->kind == CONSTANT && strcmp(op1->data.value,op2->data.value)==0)
		return 1;
	if(op1->kind == ADDRESS && op1->data.temp_no == op2->data.temp_no)
		return 1;
	return -1;
}

void freeReg(FILE *fp, int i){
	if(i<0 || i >=regSize){
		printf("Wrror!\n");
		return;
	}
	assert(allTempReg[i].operand != NULL);
	if(allTempReg[i].operand->kind == CONSTANT){
		allTempReg[i].operand = NULL;
		return;
	}
	char *name = NULL;
	TempOperand[i] = allTempReg[i].operand;
	if(allTempReg[i].operand->kind == TEMP)
		name = get_temp_varname(allTempReg[i].operand->data.temp_no);
	else if(allTempReg[i].operand->kind == VARIABLE)
		name = allTempReg[i].operand->data.var_name;
	int offset = getOffset(name);
	if(offset == -1){
		printf("Assume never reached here!\n");
		printf("error name:%s\n",getOperandName(allTempReg[i].operand));
		fprintf(fp,"sw %s, %d($sp)\n",allTempReg[i].name, stackOffset);
	}
	else{
		fprintf(fp, "sw %s, %d($sp)\n", allTempReg[i].name, offset);
	}
	allTempReg[i].operand = NULL;
	return;
}

void restore(FILE *fp){
	int i = 0;
	while(i<regSize){
		if(TempOperand[i] != NULL){
			allTempReg[i].operand = TempOperand[i];
			char *name = NULL;
			if(TempOperand[i]->kind == TEMP)
				name = get_temp_varname(TempOperand[i]->data.temp_no);
			else if(TempOperand[i]->kind == VARIABLE)
				name = TempOperand[i]->data.var_name;
			fprintf(fp, "lw %s, %d($sp)\n", allTempReg[i].name, getOffset(name));
			TempOperand[i] = NULL;
		}
		++i;
	}
}

char *reg(FILE *fp,OperandPoint operand, int load){
	int i = 0;
	while(i<regSize){
		if(allTempReg[i].operand == NULL){
			break;
 		}
		if(testEqual(operand, allTempReg[i].operand) == 1){//寄存器中已经存在改变量 拿来使用即可。
			return allTempReg[i].name;
		} 
		++i;
 	}
	/* 随机分配方案 */
	if(i == regSize){
		i = rand() % regSize;
		freeReg(fp, i);
	}
	allTempReg[i].operand = operand;
	char *temp = get_operand_label(operand); 
	if(load == 1){//说明生成的寄存器里面已经包含了要用到的值
		if(operand->kind == CONSTANT){//表示当前常量需要存放到寄存器中使用了，此时没有必要为这个常量分配空间。
			fprintf(fp, "li %s, %s\n", allTempReg[i].name, temp);
		}
		else{
			//执行到这里说明需要把变量从内存中加载出来
			fprintf(fp, "lw %s, %d($sp)\n",allTempReg[i].name, getOffset(temp));
		}
 	}
 	else{//用于保存计算结果的寄存器 因此需要为变量分配栈空间
 		if(operand->kind != CONSTANT)//常量不需要分配栈
			setOffset(temp);
 	}
	return allTempReg[i].name;
}

void irtomsp(char *filename){
	regInit();
	FILE *fp = fopen(filename,"w");
	if(fp == NULL){
		printf("Open file %s failed!\n",filename);
		return;
	}

	InterCodes* head = code_root,*tempCode;
	char *relop = NULL;
	char *preTemp; 
	int tempi;
	fprintf(fp, ".data\n");
	fprintf(fp, "_prompt: .asciiz \"Enter an integer:\"\n");
	fprintf(fp, "_ret: .asciiz \"\\n\"\n");
	fprintf(fp, ".globl main\n");
	fprintf(fp, ".text\n");
	fprintf(fp,"read:\n");
	fprintf(fp,"li $v0, 4\nla $a0, _prompt\nsyscall\nli $v0, 5\nsyscall\njr $ra\n");
	fprintf(fp, "\nwrite:\n");
	fprintf(fp, "li $v0, 1\nsyscall\nli $v0, 4\nla $a0, _ret\nsyscall\nmove $v0, $0\njr $ra\n");
	while(head != NULL){
		switch(head->code.kind){
			case LABEL: case FUNCTIONLABEL: 
				if(head->code.kind == FUNCTIONLABEL)
					fprintf(fp, "\n");
				fprintf(fp,"%s:\n", head->code.data.symbol_name);
				break;
			case GOTO:
				fprintf(fp, "j %s\n", head->code.data.symbol_name);
				break;
			case ONEOP: 
				if(head->code.data.oneop.left->kind != ADDRESS){
					if(head->code.data.oneop.right->kind == CONSTANT){
						preTemp = reg(fp, head->code.data.oneop.left,0);
						fprintf(fp, "li %s, %s\n", preTemp, head->code.data.oneop.right->data.value);
			 		}
					else if(head->code.data.oneop.right->kind != ADDRESS)
						fprintf(fp, "move %s, %s\n", reg(fp,head->code.data.oneop.left,0), reg(fp, head->code.data.oneop.right, 1));//表示右边的值需要从内村中取出来便于使用，左边的变量需要申请栈空间。
					else 
						fprintf(fp, "lw %s, 0(%s)\n", reg(fp,head->code.data.oneop.left,0), reg(fp, head->code.data.oneop.right, 1));
			 	}
				else {
					preTemp = NULL;
					if(head->code.data.oneop.right->kind == CONSTANT){
						fprintf(fp, "li $s0, %s\n", head->code.data.oneop.right->data.value);
						preTemp = "$s0";
				 	}
					else if(head->code.data.oneop.right->kind != ADDRESS)
						preTemp = reg(fp, head->code.data.oneop.right, 1);
				 	else{
						fprintf(fp, "lw $s0, 0(%s)\n", reg(fp, head->code.data.oneop.right, 1));
						preTemp = "$s0";
					}
					fprintf(fp, "sw %s, 0(%s)\n", preTemp, reg(fp,head->code.data.oneop.left, 1));
				} 
				break;

			case BINOP: 
				switch(head->code.data.binop.opkind){
					case PLUS:
						fprintf(fp, "add %s, %s, %s\n",reg(fp,head->code.data.binop.result, 0), reg(fp,head->code.data.binop.op1,1), reg(fp, head->code.data.binop.op2,1));
						break;
					case MINUS:
						if(head->code.data.binop.op2->kind == CONSTANT)
							fprintf(fp, "addi %s, %s, -%s\n",reg(fp,head->code.data.binop.result, 0), reg(fp, head->code.data.binop.op1, 1),head->code.data.binop.op2->data.value);
						else
							fprintf(fp, "sub %s, %s, %s\n",reg(fp, head->code.data.binop.result, 0), reg(fp, head->code.data.binop.op1, 1), reg(fp, head->code.data.binop.op2, 1));
						break;
					case STAR:
						fprintf(fp, "mul %s, %s, %s\n",reg(fp, head->code.data.binop.result, 0), reg(fp, head->code.data.binop.op1, 1), reg(fp, head->code.data.binop.op2, 1));
						break;
					case DIV:
						fprintf(fp, "div %s, %s\n", reg(fp, head->code.data.binop.op1, 1), reg(fp, head->code.data.binop.op2, 1));
						fprintf(fp, "mflo %s", reg(fp, head->code.data.binop.result, 0));
						break;
					default:
						printf("Error opration in %s at %d.\n",__FILE__,__LINE__);
						break;
					} d
				printf("binop opkind = %d\n",head->code.data.binop.opkind);
				break;
			case CALL:	case READ:	case WRITE:
				/*To-do*/
				printf("finish call please!\n");
				break;
			case RETURNFUNCTION:
				fprintf(fp, "move $v0, %s\n", reg(fp, head->code.data.return_value, 1));
				fprintf(fp, "jr $ra\n");
				break;
			case IFSTMT:
				relop = NULL;
				preTemp = head->code.data.ifstmt.relop;
				if(strcmp(preTemp,"==") == 0)
					relop = "beq";
				else if(strcmp(preTemp, "!=") == 0)
					relop = "bne";
				else if(strcmp(preTemp, ">") == 0)
					relop = "bgt";
				else if(strcmp(preTemp, "<") == 0)
					relop = "blt";
				else if(strcmp(preTemp, ">=") == 0)
					relop = "bge";
				else if(strcmp(preTemp, "<=") == 0)
					relop = "ble";
				else
					printf("Error relop in %s at %d.\n", __FILE__, __LINE__);
				fprintf(fp, "%s %s, %s, %s\n", relop, reg(fp, head->code.data.ifstmt.left, 1), reg(fp, head->code.data.ifstmt.right, 1), head->code.data.ifstmt.label);
				break;
			case ARG: /* 过程调用 */
				printf("finish arg please.\n");
				break;
			case PARAM:

				printf("finish parama please!\n");
				break;
			case DEC:
				printf("don't finish a dimession arryay.");
				break;
			default:
				printf("Don't finish!\n");
				break;
	 	}
		head = head->next;
	} 
	fclose(fp);
}
