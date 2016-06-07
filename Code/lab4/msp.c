#include "msp.h"
#include "../syntax.tab.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
struct RegStruct{
	OperandPoint operand;
	char name[4];
};
typedef struct RegStruct RegStruct;
#define regSize 8
RegStruct allTempReg[regSize];
void regInit(){
	int i = 0;
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

int testEqual(OperandPoint op1, OperandPoint op2){
	if(op1 == NULL || op2 == NULL){
		printf("illegal operation!\n");
		return -1;
	}
	if(op1->kind != op2->kind)
		return 0;
	if(op1->kind == VARIABLE && strcmp(op1->data.var_name,op2->data.var_name)==0)
		return 1;
	if(op2->kind == TEMP && op1->data.temp_no == op2->data.temp_no)
		return 1;
	if(op2->kind == CONSTANT && strcmp(op1->data.value,op2->data.value)==0)
		return 1;
	return -1;
}

char *reg(FILE *fp,OperandPoint operand, int load){
	int i = 0;
	while(i<regSize){
		if(allTempReg[i].operand == NULL){
			allTempReg[i].operand = operand;
			if(load == 1){
				char *temp = NULL;
				if(operand->kind == TEMP)
					temp = get_temp_varname(operand->data.temp_no);
				else if(operand->kind == CONSTANT)
					temp = operand->data.value;
				else if(operand->kind == VARIABLE)
					temp = operand->data.var_name;
				char *optype;
				if(operand->kind == CONSTANT)
					optype = "li";
				else
					optype = "lw";
				fprintf(fp,"%s  %s,%s\n",optype, allTempReg[i].name, temp);
			}
			return allTempReg[i].name;
		}
		if(testEqual(operand, allTempReg[i].operand) == 1)
			return allTempReg[i].name;
		++i;
	}
	/* juti fenpeifangan*/

	return "0";
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
	char *preTemp; 
	fprintf(fp, ".data\n");
	fprintf(fp, "_prompt: .asciiz \"Enter an integer:\"\n");
	fprintf(fp, "_reg: .asciiz \"\\n\"\n");
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
				if(head->code.data.oneop.right->kind == CONSTANT)
					fprintf(fp, "li %s, %s\n",reg(fp, head->code.data.oneop.left,0), head->code.data.oneop.right->data.value);
				else 
					fprintf(fp, "move %s, %s\n", reg(fp,head->code.data.oneop.left,0), reg(fp, head->code.data.oneop.right,1));

				break;
			case BINOP: 
				switch(head->code.data.binop.opkind){
					case PLUS: fprintf(fp, "add %s, %s, %s\n",reg(fp,head->code.data.binop.result,0), reg(fp,head->code.data.binop.op1,1), reg(fp, head->code.data.binop.op2,1)); break;
					case MINUS: if(head->code.data.binop.op2->kind == CONSTANT)
									fprintf(fp, "addi %s, %s, -%s",reg(fp,head->code.data.binop.result,0), reg(fp,head->code.data.binop.op1,1),head->code.data.binop.op2->data.value);
								else
									fprintf(fp, "sub %s, %s, %s\n",reg(fp,head->code.data.binop.result,0), reg(fp,head->code.data.binop.op1,1), reg(fp, head->code.data.binop.op2,1));
								break;
					case STAR: fprintf(fp, "mul %s, %s, %s\n",reg(fp,head->code.data.binop.result,0), reg(fp,head->code.data.binop.op1,1), reg(fp, head->code.data.binop.op2,1)); break;
					case DIV: fprintf(fp, "div %s, %s\n", reg(fp,head->code.data.binop.op1,1), reg(fp, head->code.data.binop.op2,1));
								fprintf(fp, "mflo %s", reg(fp, head->code.data.binop.result,0));
								break;
							}
				break;
			case CALL:
				fprintf(fp, "jal %s\n", head->code.data.funcall.fun_name);
				fprintf(fp, "%s, $v0\n", reg(fp, head->code.data.funcall.left,0));
				break;
			case RETURNFUNCTION:
				fprintf(fp, "move $v0, %s\n", reg(fp, head->code.data.return_value,0));
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
					printf("Never reach here! in %s at %d.\n", __FILE__, __LINE__);
				fprintf(fp, "%s %s, %s, %s\n",relop, reg(fp, head->code.data.ifstmt.left,1), reg(fp, head->code.data.ifstmt.right,1), head->code.data.ifstmt.label);
				break;
			case ARG:
			case DEC: printf("don't finish a dimession arryay."); break;
			case READ:
			case WRITE:
			default:printf("Don't finish!\n");
		}
		head = head->next;
	}
	fclose(fp);
}
