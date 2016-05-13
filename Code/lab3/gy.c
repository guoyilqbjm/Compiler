#include "code_gr.h"

InterCodes* translate_Exp(TreeNode *root, OperandPoint place){
	TreeNode *child = root->firstChild;
	InterCodes *new_code = (InterCodes *)malloc(sizeof(InterCodes));
	new_code->last = new_code->next = NULL;
	if(child->name == INT){
		new_code->code.kind = ONEOP;
		new_code->code.data.oneop.opkind = ASSIGNOP;
		new_code->code.data.oneop.left = place;
		OperandPoint right = (OperandPoint)malloc(sizeof(Operand));
		right->kind = CONSTANT;
		right->data.value = atoi(child->data);
		new_code->code.data.oneop.right = right;
	}
	else if(child->name == ID){
		new_code->code.kind = ONEOP;
		new_code->code.data.oneop.opkind = ASSIGNOP;
		new_code->code.data.oneop.left = place;
		OperandPoint right = (OperandPoint)malloc(sizeof(Operand));
		right->kind = VARIABLE;
		right->data.var_name = child->data;
		new_code->code.data.oneop.right = right;
	}
	else if(child->name == MINUS){
		OperandPoint new_temp = (OperandPoint)malloc(sizeof(Operand));
		new_temp->kind = TEMP;
		new_temp->data.temp_no = new_temp_varname();
		OperandPoint op1 = (OperandPoint)malloc(sizeof(Operand));
		op1->kind = CONSTANT;
		op1->data.value = 0;

		InterCodes* code1 = translate_Exp(child->nextSibling,new_temp);
		InterCodes* code2 = (InterCodes*)malloc(sizeof(InterCodes));
		code2->code.kind = BINOP;
		code2->code.data.binop.opkind = MINUS;
		code2->code.data.binop.result = place;
		code2->code.data.binop.op1 = op1;
		code2->code.data.binop.op2 = new_temp;
		return mergeInterCodes(code1,code2);
	}
	else{
		printf("Don't finished the Function.\n");
		return NULL;
	}
	return new_code;
}
