#include "code_gr.h"
#include <stdlib.h>
#include <stdio.h>

char *new_temp_varname(){
	char *varname = (char *)malloc(sizeof(4));
	varname[0] = 't';
	varname[1] = (char)(varIndex / 10);
	varname[2] = (char)(varIndex % 10);
	varname[3] = '\0';
	varIndex++;
	return varname;
}

char *new_temp_labelname(){
	char *label = (char*)malloc(sizeof(4));
	label[0] = 'v';
	label[1] = (char)(labelIndex / 10);
	label[2] = (char)(labelIndex % 10);
	label[3] = '\0';
	labelIndex++;
	return label;
}
