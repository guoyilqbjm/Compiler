#include "code_gr.h"
#include <stdlib.h>
#include <stdio.h>
int varIndex=-1, labelIndex=-1;
char VARNAME[100][4], LABELNAME[100][4];


int const_table[200];//store every constant value for every varIndex;


int new_temp_varname(){
	varIndex++;
	char *varname = VARNAME[varIndex];
	varname[0] = 't';
	varname[1] = (char)(varIndex / 10);
	varname[2] = (char)(varIndex % 10);
	varname[3] = '\0';
	return varIndex;
}

int new_temp_labelname(){
	labelIndex++;
	char *label = LABELNAME[labelIndex];
	label[0] = 'l';
	label[1] = (char)(labelIndex / 10);
	label[2] = (char)(labelIndex % 10);
	label[3] = '\0';
	return labelIndex;
}

OperandPoint new_constant_operand(int value){
	OperandPoint new_constant = (OperandPoint)malloc(sizeof(Operand));
	new_constant->kind = CONSTANT;
	int i = 0;
	while(i <= varIndex){
		if(const_table[i] == value)
			break;
		else
			++i;
	}
	if(i>varIndex){
		i = new_temp_varname();
		const_table[i] = value;
		new_constant->data.temp_no = i;
	}
	else
		new_constant->data.temp_no = i;
	return new_constant;
}
