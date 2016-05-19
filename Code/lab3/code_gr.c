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


int new_temp_varname(){
	varIndex++;
	char *varname = VARNAME[varIndex];
	varname[0] = 't';
	if(varIndex>10){
		varname[1] = (char)(varIndex / 10 ) + '0';
		varname[2] = (char)(varIndex % 10 ) + '0';
		varname[3] = '\0';
	}
	else {
		varname[1] = (char)(varIndex) + '0';
		varname[2] = '\0';
	}
	return varIndex;

}

char* new_temp_labelname(){
	labelIndex++;
	char *label = LABELNAME[labelIndex];
	label[0] = 'l';
	if(labelIndex>10){
		label[1] = (char)(labelIndex / 10) + '0';
		label[2] = (char)(labelIndex % 10) + '0';
		label[3] = '\0';
	}
	else {
		label[1] = (char)(labelIndex) + '0';
		label[2] = '\0';
	}
	return LABELNAME[labelIndex];
}

void runCode(char *filename){
	translate_Program(root);
	
	printInterCodes(filename);

	printf("结果已经输出到文件\"%s\".\n",filename);
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
	new_code->code.data.symbol_name = child->data;
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
				temp_code->code.data.symbol_name = tempnode->data;
				
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


InterCodes *translate_DefList(TreeNode *root){
	if(root == NULL || root->name == Empty)
		return NULL;
	TreeNode *def = root->firstChild;//DefList->Def DefList
	
	InterCodes *result = NULL;
	while(1){
		TreeNode *dec = def->firstChild->nextSibling->firstChild;//Def -> Specifier DecList SEMI
																//DecList->Dec | Dec COMMA DecList dec->name = Dec
		while(1){
			TreeNode* vardec = dec->firstChild;	//Dec->VarDec | VarDec ASSIGNOP Exp
			if(vardec->nextSibling != NULL){	//VarDec -> ID | VarDec LB INT RB


				OperandPoint place = (OperandPoint)malloc(sizeof(Operand));
				place->kind = VARIABLE;
				place->data.var_name = vardec->firstChild->data;
				printf("%s  here\n",vardec->firstChild->data);
				InterCodes *temp_codes = translate_Exp(vardec->nextSibling->nextSibling,place);
				result = mergeInterCodes(result,temp_codes);
			}
			else if(vardec->firstChild->name != ID){
				// TODO: array definiton / allocate memory
			}
			if(dec->nextSibling == NULL)
				break;	
			else
				dec = dec->nextSibling->nextSibling->firstChild;
		}
		if(def->nextSibling->name == Empty)
			break;
		else
			def = def->nextSibling->firstChild;
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
		else if(node->name == Exp){
			OperandPoint place = (OperandPoint)malloc(sizeof(Operand));
			result = mergeInterCodes(result,translate_Exp(node, place));
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
	TreeNode *child = root->firstChild;
	TreeNode *operator = child->nextSibling;
	InterCodes *new_code = (InterCodes *)malloc(sizeof(InterCodes));
	new_code->last = new_code->next = NULL;
	if(child->name == INT){
		new_code->code.kind = ONEOP;
		new_code->code.data.oneop.left = place;
		OperandPoint right = (OperandPoint)malloc(sizeof(Operand));
		right->kind = CONSTANT;
		right->data.value = child->data;
		new_code->code.data.oneop.right = right;
	}
	else if(child->name == FLOAT) {
		new_code->code.kind = ONEOP;
		new_code->code.data.oneop.left = place;
		OperandPoint right = (OperandPoint)malloc(sizeof(Operand));
		right->kind = CONSTANT;
		right->data.value = child->data;
		new_code->code.data.oneop.right = right;
	}

	else if(child->name == ID){

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
		
		op1->data.value = "0";

		InterCodes* code1 = translate_Exp(child->nextSibling,new_temp);
		InterCodes* code2 = (InterCodes*)malloc(sizeof(InterCodes));
		code2->code.kind = BINOP;
		code2->code.data.binop.opkind = MINUS;
		code2->code.data.binop.result = place;
		code2->code.data.binop.op1 = op1;
		code2->code.data.binop.op2 = new_temp;
		return mergeInterCodes(code1,code2);
	}
	else if (child->name == NOT || operator->name == AND || operator->name == OR || operator->name == RELOP) {
		
		InterCodes *label1 = (InterCodes *)malloc(sizeof(InterCodes));
		InterCodes *label2 = (InterCodes *)malloc(sizeof(InterCodes));
		label1->code.kind = label2->code.kind = LABEL;
		label1->last = label1->next = label2->last = label2->next = NULL;
		label1->code.data.symbol_name = new_temp_labelname();
		label2->code.data.symbol_name = new_temp_labelname();

		OperandPoint op1 = (OperandPoint)malloc(sizeof(Operand));
		op1->kind = CONSTANT;
		op1->data.value = "0";
		InterCodes *code0 = (InterCodes *)malloc(sizeof(InterCodes));
		code0->last = code0->next = NULL;
		code0->code.kind = ONEOP;
		code0->code.data.oneop.left = place;
		code0->code.data.oneop.right = op1;

		InterCodes *code1 = NULL;

		OperandPoint op2 = (OperandPoint)malloc(sizeof(Operand));
		op2->kind = CONSTANT;
		op2->data.value = "1";		
		InterCodes *codetemp = (InterCodes *)malloc(sizeof(InterCodes));
		codetemp->last = codetemp->next = NULL;		//FIXME!
		codetemp->code.kind = ONEOP;
		codetemp->code.data.oneop.left = place;
		codetemp->code.data.oneop.right = op2;

		InterCodes *code2 = mergeInterCodes(label1, codetemp);

		return mergeInterCodes(mergeInterCodes(mergeInterCodes(code0, code1), code2), label2);
	}
	else if (operator->name == PLUS || operator->name == MINUS || operator->name == STAR || operator->name == DIV) {			//Exp1 PLUS Exp2
		OperandPoint t1 = (OperandPoint)malloc(sizeof(Operand));
		t1->kind = TEMP;
		t1->data.temp_no = new_temp_varname();
		OperandPoint t2 = (OperandPoint)malloc(sizeof(Operand));
		t2->kind = TEMP;
		t2->data.temp_no = new_temp_varname();
		InterCodes *code1 = translate_Exp(child, t1);
		InterCodes *code2 = translate_Exp(operator->nextSibling, t2);
		InterCodes *code3 = (InterCodes *)malloc(sizeof(InterCodes));
		code3->last = code3->next = NULL;
		code3->code.kind = BINOP;
		code3->code.data.binop.opkind = operator->name;
		code3->code.data.binop.result = place;
		code3->code.data.binop.op1 = t1;
		code3->code.data.binop.op2 = t2;
		return mergeInterCodes(mergeInterCodes(code1, code2), code3);
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
