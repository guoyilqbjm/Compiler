#include "code_gr.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int varIndex=-1, labelIndex=-1;
char VARNAME[100][4], LABELNAME[100][4];


int const_table[200];//store every constant value for every varIndex;

char *get_temp_varname(int i){
	return VARNAME[i];
}
char *get_temp_labelname(int i){
	return LABELNAME[i];
}


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


InterCodes *translate_Program(TreeNode *root){
	if(root == NULL || root->name == Empty)
		return NULL;

	if(root->name == Program){
		code_root = translate_Program(root->firstChild);
		return code_root;
	}

	if(root->name == ExtDefList)
		return mergeInterCodes(translate_Program(root->firstChild),translate_Program(root->firstChild->nextSibling));

	if(root->name == ExtDef){
		TreeNode *child = root->firstChild->nextSibling;
		if(child->name == FunDec){
			assert(child->nextSibling->name == CompSt);			
			InterCodes *fundec = translate_FunDec(child), *compstmt = translate_CompSt(child->nextSibling);
			return  mergeInterCodes(fundec,compstmt);
		}
		else
			return NULL;
	}
	printf("Never reaches here but run error! %s %d\n",__FILE__,__LINE__);
	return NULL;
}


InterCodes* translate_FunDec(TreeNode *root){
	assert(root != NULL);
	TreeNode *child = root->firstChild;
	InterCodes *new_code = (InterCodes*)malloc(sizeof(InterCodes));
	new_code->last = new_code->next = NULL;
	new_code->code.kind = FUNCTIONLABEL;
	strcpy(new_code->code.data.symbol_name,child->data);
	child = child->nextSibling->nextSibling;
	if(child->name == VarList){
		InterCodes* tail = new_code;
		child = child->firstChild;//child->name == ParamDec VarList->ParamDec COMMA VarList | ParamDec
		while(1){
				TreeNode *tempnode = child->firstChild->nextSibling;//ParamDec->Specifier VarDec
				tempnode = tempnode->firstChild;//VarDec->ID|VarDec LB INT RB
				while(tempnode->name != ID)
					tempnode = tempnode->firstChild;
				InterCodes *temp_code = (InterCodes*)malloc(sizeof(InterCodes));
				temp_code->next = NULL;
				temp_code->last = tail;
				tail->next = temp_code;
				tail = temp_code;

				temp_code->code.kind = PARAM;
				strcpy(temp_code->code.data.symbol_name,tempnode->data);
				
				child = child->nextSibling;
				if(child == NULL)
					break;
				else
					child = child->nextSibling->firstChild;//VarList->ParamDec COMMA VarList child->name = ParamDec
		}
	}
	return new_code;
}


InterCodes* translate_CompSt(TreeNode *root){
	TreeNode *child = root->firstChild->nextSibling;
	return mergeInterCodes(translate_DefList(child),translate_StmtList(child->nextSibling));
}

void runCode(char *filename){
	translate_Program(root);
	
	printInterCodes(filename);

	printf("结果已经输出到文件\"%s\".\n",filename);
}

InterCodes *translate_DefList(TreeNode *root){
	if(root == NULL || root->name == Empty)
		return NULL;
	TreeNode *child = root->firstChild;//DefList->Def DefList
	TreeNode *dec = child->firstChild->nextSibling->firstChild;//DecList->Dec | Dec COMMA DecList dec->name = Dec
	InterCodes *result = NULL;
	while(1){
		TreeNode* vardec = dec->firstChild;//DecList->Dec | Dec COMMA DecList
		if(vardec->nextSibling != NULL){//Dec->VarDec ASSIGNOP Exp

			OperandPoint place = (OperandPoint)malloc(sizeof(Operand));
			place->kind = VARIABLE;
			place->data.var_name = vardec->firstChild->data;
			InterCodes *temp_codes = translate_Exp(vardec->nextSibling->nextSibling,place);
			result = mergeInterCodes(result,temp_codes);
		}
		dec = dec->nextSibling;
		if(dec == NULL)
			break;	
		else
			dec = dec->nextSibling->firstChild;
	}
	return result;
}


InterCodes *translate_StmtList(TreeNode *root){
	if(root == NULL || root->name == Empty)
		return NULL;
	TreeNode *child = root->firstChild;
	InterCodes *result = NULL;
	while(1){
		if(child == NULL || child->name == Empty)
			break;
		TreeNode * node = child->firstChild;
		if(node->name == RETURN){
			OperandPoint place = (OperandPoint)malloc(sizeof(Operand));
			InterCodes *expcode = translate_Exp(node->nextSibling,place);
			result = mergeInterCodes(result,expcode);
			InterCodes *new_code = (InterCodes*)malloc(sizeof(InterCodes));
			new_code->next = new_code->last = NULL;
			new_code->code.kind = RETURNFUNCTION;
			new_code->code.data.return_value = place;
			result = mergeInterCodes(result,new_code);
		}
		else if(node->name == CompSt){
			result = mergeInterCodes(result,translate_CompSt(node));
		}
		else{
			printf("Don't finish in %s at %d.\n",__FILE__,__LINE__);
			return NULL;
		}
		child = child->nextSibling->firstChild;
	}
	/*To-do*/
	return result;
}


InterCodes* translate_Exp(TreeNode *root, OperandPoint place){
	/*To-finish*/
	TreeNode *child = root->firstChild;
	InterCodes *new_code = (InterCodes *)malloc(sizeof(InterCodes));
	new_code->last = new_code->next = NULL;
	if(child->name == INT){
		new_code->code.kind = ONEOP;
		new_code->code.data.oneop.left = place;
		OperandPoint right = (OperandPoint)malloc(sizeof(Operand));
		right->kind = CONSTANT;
		right->data.value = atoi(child->data);
		new_code->code.data.oneop.right = right;
		printf("int test\n");
	}
	else if(child->name == ID){
		/*new_code->code.kind = ONEOP;
		new_code->code.data.oneop.opkind = ASSIGNOP;
		new_code->code.data.oneop.left = place;
		OperandPoint right = (OperandPoint)malloc(sizeof(Operand));
		right->kind = VARIABLE;
		right->data.var_name = child->data;
		new_code->code.data.oneop.right = right;*/

		place->kind = VARIABLE;
		place->data.var_name = child->data;
		return NULL;
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

InterCodes* translate_Args(TreeNode *root, OperandPoint place){
	/*To-do*/
	return NULL;
}
