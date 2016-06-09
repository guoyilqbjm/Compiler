#include "msp.h"
#include "../syntax.tab.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "../lab3/code_gr.h"

int safetyStrcmp(char *op1, char *op2){
	if(op1 == NULL || op2 == NULL){
		printf("Error in strcmp function.\n");
		return -1;
	}
	return strcmp(op1,op2);
}

/* store present offset in present stack.*/
int stackOffset = 0;

/* store all variables or temp_variables' offset in stack. */
struct Var_Offset{
	char *name;
	int offset;
};
typedef struct Var_Offset Var_Offset;
#define MAXOFFSET 100
int preOffsetSize = 0;//store variables' count.
Var_Offset offsetArray[MAXOFFSET];

void setOffset(char *name){
	offsetArray[preOffsetSize].offset = stackOffset;
	stackOffset = stackOffset + 4;
	offsetArray[preOffsetSize].name = name;
	preOffsetSize++;
}

int getOffset(char *name){
	int i=0;
	while(i<preOffsetSize){
		if(safetyStrcmp(offsetArray[i].name,name)==0)
			return offsetArray[i].offset;
		++i;
	}
	printf("present variable %s has not allocation space in stack.\n",name);
	return -1;
}
struct RegStruct{
	OperandPoint operand;
	char name[4];
};
typedef struct RegStruct RegStruct;
#define regSize 8
RegStruct allTempReg[regSize];
OperandPoint TempOperand[regSize];


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
	assert(p != NULL);
	if(p->kind == CONSTANT)
		return p->data.value;
	else if(p->kind == TEMP)
		return get_temp_varname(p->data.temp_no);
	else if(p->kind == VARIABLE)
		return p->data.var_name;
	else if(p->kind == ADDRESS)
		return get_temp_varname(p->data.temp_no);
	else if(p->kind == REFERENCE)
		return p->data.refer_name;
	return NULL;
}
int testEqual(OperandPoint op1, OperandPoint op2){
	if(op1 == NULL || op2 == NULL){
		printf("illegal operation!\n");
		return -1;
 	}
 	//printf("opkind:%d %s op2kind:%d %s\n", op1->kind,get_operand_label(op1), op2->kind,get_operand_label(op2));
 	if(op1->kind == TEMP && op2->kind == ADDRESS && op1->data.temp_no == op2->data.temp_no)
 		return 1;
 	if(op2->kind == TEMP && op1->kind == ADDRESS && op1->data.temp_no == op2->data.temp_no)
 		return 1;
	if(op1->kind != op2->kind)
		return -1;
	if(op1->kind == VARIABLE && safetyStrcmp(op1->data.var_name,op2->data.var_name)==0)
		return 1;
	if(op2->kind == TEMP && op1->data.temp_no == op2->data.temp_no)
		return 1;
	if(op2->kind == CONSTANT && safetyStrcmp(op1->data.value, op2->data.value)==0)
		return 1;
	if(op1->kind == ADDRESS && op1->data.temp_no == op2->data.temp_no)
		return 1;
	if(op1->kind == REFERENCE && safetyStrcmp(op1->data.refer_name,op2->data.refer_name)==0)
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

	TempOperand[i] = allTempReg[i].operand;
	char *name = get_operand_label(TempOperand[i]);
	int offset = getOffset(name);
	if(offset == -1){
		printf("Assume never reached here!\n");
		printf("Your program has some problems.\n");
		fprintf(fp,"sw %s, %d($sp)\n",allTempReg[i].name, stackOffset);
	}
	else{
		fprintf(fp, "sw %s, %d($sp)\n", allTempReg[i].name, offset);
	}
	allTempReg[i].operand = NULL;
	return;
}

void restoreReg(FILE *fp){
	int i = 0;
	while(i<regSize){
		if(TempOperand[i] != NULL){
			allTempReg[i].operand = TempOperand[i];
			char *name = get_operand_label(TempOperand[i]);
			fprintf(fp, "lw %s, %d($sp)\n", allTempReg[i].name, getOffset(name));
			TempOperand[i] = NULL;
		}
		else{
			allTempReg[i].operand = NULL;
		}
		++i;
	}
}

void saveReg(FILE *fp){
	int i = 0;
	while(i<regSize){
		if(allTempReg[i].operand != NULL){
			if(allTempReg[i].operand->kind == CONSTANT){
				allTempReg[i].operand = NULL;
				TempOperand[i] = NULL;
			}
			else{
				TempOperand[i] = allTempReg[i].operand;
				fprintf(fp, "sw %s, %d($sp)\n", allTempReg[i].name, getOffset(get_operand_label(allTempReg[i].operand)));
				allTempReg[i].operand = NULL;
			}
		}
		++i;
	}
}

int ttt = 0;

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
		i = ttt;
		ttt = (ttt + 1)%regSize;
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

	InterCodes* head = code_root;
	char *relop = NULL;

	/*temp variable*/
	InterCodes* temp_code;
	int temp_int;
	char *temp_char;

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
				if(head->code.kind == FUNCTIONLABEL){
					stackOffset = 0;
					for(temp_int = 0; temp_int < regSize; ++temp_int){
						TempOperand[temp_int] = NULL;
						allTempReg[temp_int].operand = NULL;
					}
					fprintf(fp, "\n");
				}
				fprintf(fp,"%s:\n", head->code.data.symbol_name);
				break;
			case GOTO:
				fprintf(fp, "j %s\n", head->code.data.symbol_name);
				break;
			case ONEOP: 
				/* 这里考虑了数组的使用 但应该使有bug的，还需要你来完成 */		
				if(head->code.data.oneop.left->kind != ADDRESS){
					assert(head->code.data.oneop.right->kind != REFERENCE);
					/*if(head->code.data.oneop.right->kind == CONSTANT){
						//fprintf(fp, "li %s, %s\n", reg(fp, head->code.data.oneop.left,0), head->code.data.oneop.right->data.value);
			 		}*/
					if(head->code.data.oneop.right->kind != ADDRESS){
						fprintf(fp, "move %s, %s\n", reg(fp,head->code.data.oneop.left,0), reg(fp, head->code.data.oneop.right, 1));//表示右边的值需要从内村中取出来便于使用，左边的变量需要申请栈空间。
					}
					else {
						fprintf(fp, "add $s1, %s, $sp\n", reg(fp, head->code.data.oneop.right, 1));
						fprintf(fp, "lw %s, 0($s1)\n", reg(fp,head->code.data.oneop.left, 0));
					}
			 	}
				else {
					temp_char = NULL;
					if(head->code.data.oneop.right->kind == CONSTANT){
						fprintf(fp, "li $s0, %s\n", head->code.data.oneop.right->data.value);
						temp_char = "$s0";
				 	}
					else if(head->code.data.oneop.right->kind != ADDRESS)
						temp_char = reg(fp, head->code.data.oneop.right, 1);
				 	else{
						fprintf(fp, "lw $s0, 0(%s)\n", reg(fp, head->code.data.oneop.right, 1));
						temp_char = "$s0";
					}
					fprintf(fp, "add $s1, %s, $sp\n", reg(fp, head->code.data.oneop.left, 1));
					fprintf(fp, "sw %s, 0($s1)\n", temp_char);
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
					}
				break;
			case CALL:	case READ:	case WRITE:
				//fprintf(fp, "sw %d, %d($sp)\n", stackOffset, stackOffset);
				fprintf(fp, "sw $ra, %d($sp)\n", stackOffset);
				/* prepare arguments. */
				if(head->code.kind == WRITE){
					if(head->code.data.operand_point->kind == CONSTANT){
						fprintf(fp, "li $a0, %s\n", head->code.data.operand_point->data.value);
					}
					else{
						fprintf(fp, "move $a0, %s\n", reg(fp, head->code.data.operand_point, 1));
					}
				}
				else if(head->code.kind == CALL){
					temp_code = head->last;
					temp_int = 0;
					while(temp_code->code.kind == ARG && temp_int < 4){
						if(temp_code->code.data.arg_value->kind == CONSTANT)
							fprintf(fp, "move $a%d, %s\n",temp_int, temp_code->code.data.arg_value->data.value);
						else
							fprintf(fp, "move $a%d, %s\n", temp_int, reg(fp, temp_code->code.data.arg_value, 1));
						temp_code = temp_code->last;
						temp_int = temp_int + 1;
					}

					if(temp_code != NULL && temp_code->code.kind == ARG){
						assert(temp_int == 4);
						temp_int = stackOffset + 4;
						while(temp_code->code.kind == ARG){
							fprintf(fp, "sw %s, %d($sp)\n", reg(fp, temp_code->code.data.arg_value, 1), temp_int);
							temp_int += 4;
							temp_code = temp_code->last;
						}
					}

				}
					
				if(head->code.kind == CALL)
					saveReg(fp);
				fprintf(fp, "addi $sp, $sp, %d\n", stackOffset + 4);
				if(head->code.kind == WRITE)
					temp_char = "write";
				else if(head->code.kind == READ)
					temp_char = "read";
				else
					temp_char = head->code.data.funcall.fun_name;
				fprintf(fp, "jal %s\n", temp_char);

				/* restore $sp and $ra.*/
				fprintf(fp, "addi $sp, $sp, -%d\n", stackOffset + 4);
				fprintf(fp, "lw $ra, %d($sp)\n", stackOffset);

				/* restore reg */
				/*
				if(head->code.kind == CALL)
					restoreReg(fp);
				*/
				/* store return value. */
				if(head->code.kind == READ){
					//printf(" read function %d  %s \n", atoi(&(head->code.data.symbol_name[1])), head->code.data.symbol_name);
					fprintf(fp, "move %s, $v0\n", reg(fp, mallocOperand(TEMP, atoi(&(head->code.data.symbol_name[1]))), 0));
				}
				else if(head->code.kind == CALL){
					fprintf(fp, "move %s, $v0\n", reg(fp, head->code.data.funcall.left, 0));
				}
				break;
			case RETURNFUNCTION:
				fprintf(fp, "move $v0, %s\n", reg(fp, head->code.data.return_value, 1));
				fprintf(fp, "jr $ra\n");
				break;
			case IFSTMT:
				relop = NULL;
				temp_char = head->code.data.ifstmt.relop;
				if(safetyStrcmp(temp_char,"==") == 0)
					relop = "beq";
				else if(safetyStrcmp(temp_char, "!=") == 0)
					relop = "bne";
				else if(safetyStrcmp(temp_char, ">") == 0)
					relop = "bgt";
				else if(safetyStrcmp(temp_char, "<") == 0)
					relop = "blt";
				else if(safetyStrcmp(temp_char, ">=") == 0)
					relop = "bge";
				else if(safetyStrcmp(temp_char, "<=") == 0)
					relop = "ble";
				else
					printf("Error relop in %s at %d.\n", __FILE__, __LINE__);
				fprintf(fp, "%s %s, %s, %s\n", relop, reg(fp, head->code.data.ifstmt.left, 1), reg(fp, head->code.data.ifstmt.right, 1), head->code.data.ifstmt.label);
				break;
			case ARG: /* 过程调用 */
				/* finish in call.*/
				break;
			case PARAM:
				temp_int = 0;
				while(head->code.kind == PARAM && temp_int < 4){
					fprintf(fp, "move %s $a%d\n", reg(fp, mallocOperand(VARIABLE, head->code.data.symbol_name), 0), temp_int);
					head = head->next;
					temp_int++;
				}
				if(head->code.kind == PARAM){
					assert(temp_int == 4);
					temp_int = 0;
					while(head->code.kind == PARAM){
						fprintf(fp, "lw %s %d($sp)\n", reg(fp, mallocOperand(VARIABLE, head->code.data.symbol_name), 0), temp_int);
						temp_int += 4;
						head = head->next;
					}
				}
				head = head->last;
				break;
			case DEC:
				printf("don't finish a dimession arryay. Please finish it.\n");
				break;
			default:
				printf("Don't finish!\n");
				break;
	 	}
		head = head->next;
	} 
	fclose(fp);

	printf("目标代码已经输出到文件:%s\n", filename);
}
