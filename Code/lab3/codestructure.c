#include "codestructure.h"
#include "code_gr.h"
#include <stdio.h>
#include <assert.h>
struct InterCodes* code_root = NULL;
struct InterCodes* code_tail = NULL;


char *getOperandName(OperandPoint p){
	assert(p != NULL);
	char *data = NULL;
	switch(p->kind){
		case TEMP: return get_temp_varname(p->data.temp_no);
		case VARIABLE: return p->data.var_name;
		case CONSTANT: 
			data = (char*)malloc(32);
			strcpy(&(data[1]),p->data.value);
			data[0] = '#';
			return data;
		case ADDRESS:printf("Don't finish Address %s %d.\n",__FILE__,__LINE__);break;
		default:printf("Wrong in getOperandName()！\n");return NULL;
	}
}

void printInterCodes(char *filename){
	InterCodes *p = code_root;
	if(p == NULL){
		printf("生成中间代码出错.\n");
		return;
	}
	printf("输出到文件：\n");
	FILE *fp = fopen(filename,"w");
	if(fp == NULL){
		printf("Cannot open file \"%s\".\n",filename);
		return;
	}
	int i = 0;
	while(p != NULL){
		//printf("%d ", i); i++;
		switch(p->code.kind){
			case LABEL:
				fprintf(fp,"LABEL %s :\n",p->code.data.symbol_name);
				break;
			case FUNCTIONLABEL:
				fprintf(fp,"FUNCTION %s :\n",p->code.data.symbol_name);
				break;
			case PARAM:
				fprintf(fp,"PARAM %s\n",p->code.data.symbol_name);
				break;
			case GOTO:
				fprintf(fp,"GOTO %s\n",p->code.data.symbol_name);
				break;
			case RETURNFUNCTION:
				fprintf(fp,"RETURN %s\n\n",getOperandName(p->code.data.return_value));
				break;
			case ARG:
				fprintf(fp,"ARG %s\n",p->code.data.symbol_name);
				break;
			case ONEOP:
				fprintf(fp,"%s := %s\n",getOperandName(p->code.data.oneop.left),getOperandName(p->code.data.oneop.right));
				break;
			case BINOP:
				fprintf(fp,"%s := %s ",getOperandName(p->code.data.binop.result),getOperandName(p->code.data.binop.op1));
				int opkind = p->code.data.binop.opkind;
				if(opkind == PLUS)			fprintf(fp,"+ ");
				else if(opkind == MINUS)	fprintf(fp,"- ");
				else if(opkind == STAR)		fprintf(fp,"* ");
				else						fprintf(fp,"/ ");
				fprintf(fp,"%s\n",getOperandName(p->code.data.binop.op2));
				break;
			case CALL:
				fprintf(fp,"%s := CALL %s\n",getOperandName(p->code.data.funcall.left),p->code.data.funcall.fun_name);
				break;
			case DEC:
				fprintf(fp,"DEC %s %d\n",getOperandName(p->code.data.decstmt.left),p->code.data.decstmt.size);
				break;
			case IFSTMT:
				fprintf(fp, "IF %s %s %s GOTO %s\n", getOperandName(p->code.data.ifstmt.left), p->code.data.ifstmt.relop, getOperandName(p->code.data.ifstmt.right), p->code.data.ifstmt.label);
				break;
			case READ:
				fprintf(fp, "READ %s\n", p->code.data.symbol_name);
				break;
			case WRITE:
				fprintf(fp, "WRITE %s\n", getOperandName(p->code.data.operand_point));
				break;
			default:
				fprintf(fp, "Not Finish the Function.%d\n", p->code.kind);
				break;
		}
		p = p->next;
	}
	fclose(fp);
	return;
}


InterCodes *mergeInterCodes(InterCodes *a, InterCodes *b){
	if(a == NULL && b == NULL)
		return NULL;
	else if(a == NULL)
		return b;
	else if(b == NULL)
		return a;
	else{
		InterCodes *tail = a,*p = a;

		while(p != NULL){
			tail = p;
			p=p->next;

		}
		tail->next = b;
		b->last = tail;
		return a;
	}
}

