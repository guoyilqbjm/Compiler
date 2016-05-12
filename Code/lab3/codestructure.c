#include "codestructure.h"
#include <stdio.h>
void insert(InterCodes* data){
	data->next = NULL;
	if(code_root == NULL){
		code_root = data;
		data->last = NULL;
		code_tail = code_root;
	}
	else{
		code_tail->next = data;
		data->last = code_tail;
		code_tail = data;
	}
	printf("%d %d\n",(int)code_root,(int)code_tail);
}

void printInterCodes(char *filename){
	InterCodes *p = code_root;
	FILE *fp = fopen(filename,"w");
	if(fp == NULL){
		printf("Cannot open file \"%s\".\n",filename);
		return;
	}
	while(p != NULL){
		switch(p->code.kind){
			case LABEL: fprintf(fp,"LABEL %s :\n",p->code.data.defname); break;
			case FUNCTION: fprintf(fp,"FUNCTION %s :\n",p->code.data.defname);break;
			default: fprintf(fp,"Don't Finished the Function.\n"); break;
		}
		p = p->next;
	}
	fclose(fp);
	return;
}

OperandPoint mallocOperand(int kind){
	OperandPoint p = (OperandPoint)malloc(sizeof(Operand));
	p->kind = kind;
	return p;
}

InterCodes* mallocInterCodes(){
	InterCodes *p = (InterCodes *)malloc(sizeof(InterCodes));
	return p;
}

void testCode(char *filename){
	InterCodes *p;
	p = mallocInterCodes();
	p->code.kind = FUNCTION;
	p->code.data.defname = "main";
	insert(p);
	p = mallocInterCodes();
  p->code.kind = READ;
	OperandPoint data = mallocOperand(VARIABLE);
	p->code.data.oneop = data;
	insert(p);
	p = mallocInterCodes();
	p->code.kind = WRITE;
	p->code.data.oneop = data;
	insert(p);
	printInterCodes(filename);
}

int mainn(int argc,char *argv[]){
	if(argc != 3){
		printf("Wrong!\n");
		return 0;
	}
	testCode(argv[2]);
	printInterCodes(argv[2]);

}
