#include "code_gr.h"

InterCodes* translate_Exp(TreeNode *root, OperandPoint op1){
	TreeNode *child = root->firstChild;
	InterCodes *new_code = (InterCodes *)malloc(sizeof(InterCodes));
	new_code->last = new_code->next = NULL;
	if(child->name == INT){
		new_code->code.kind = ONEOP;
		new_code->code.oneop.opkind = ASSIGN;
		new_code->code.oneop.left = op1;
		OperandPoint right = (OperandPoint)malloc(sizeof(Operand));
		right->kind = CONSTANT;
		right->data.value = atoi(child->data);
		new_code->code.oneop.right = right;
	}
	else if(child->name == ID){
		
	}
	else{
		printf("Don't finished the Function.\n");
		return NULL;
	}
	return new_code;
}
