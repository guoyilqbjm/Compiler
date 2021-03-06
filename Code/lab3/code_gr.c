#include "code_gr.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../lab2/rules.h"
int varIndex=-1, labelIndex=-1;
char VARNAME[100][4], LABELNAME[100][4];


int const_table[200];//store every constant value for every varIndex;

char *get_temp_varname(int i){
	return VARNAME[i];
}

int allocate_varname(){
	varIndex++;
	char *varname = VARNAME[varIndex];
	varname[0] = 't';
	if(varIndex>=10){
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

char* allocate_labelname(){
	labelIndex++;
	char *label = LABELNAME[labelIndex];
	label[0] = 'l';
	if(labelIndex>=10){
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

int getSize(TypePoint t){
	if(t->kind == BASIC)
		return 4;
	else if(t->kind == STRUCTURE)
		return get_structure_size(t);
	else if(t->kind == ARRAY){
		return t->data.array.size*getSize(t->data.array.elem);
	}
	else{
		printf("don't finished!\n");
		return 0;
	}
}


int get_structure_size(TypePoint type){
	assert(type->kind == STRUCTURE);
	assert(type->data.structure->type->kind == STRUCTURE);
	int size = 0;
	FieldListPoint head = type->data.structure->type->data.structure;
	for(;head != NULL; head=head->tail){
		assert(head->type->kind == BASIC || head->type->kind == STRUCTURE || head->type->kind == ARRAY);
		if(head->type->kind == BASIC)
			size = size + 4;
		else if (head->type->kind == STRUCTURE)
			size = size + get_structure_size(head->type);
		else if (head->type->kind == ARRAY) {
			size = getSize(head->type) + size;
		}
	}
	return size;
}

int get_array_size(TypePoint type){
	assert(type->kind == ARRAY);
	int size = 0;
	TypePoint elem = type->data.array.elem;
	assert(type->data.array.elem == BASIC || type->data.array.elem->kind == STRUCTURE);
	if(elem->kind == BASIC)
		size = 4;
	else
		size = get_structure_size(elem);
	return size * type->data.array.size;
}
int get_structure_offset(TypePoint type,char *fieldname){
	assert(type->kind == STRUCTURE);
	int offset = 0;
	FieldListPoint head = type->data.structure->type->data.structure;
	for(;strcmp(head->name,fieldname)!=0;head=head->tail){
		assert(head != NULL);
		if(head->type->kind == BASIC)
			offset = offset + 4;
		else
			offset = offset + get_structure_size(head->type);
	}
	return offset;
}

void runCode(char *filename){
	translate_Program(root);
	
	printInterCodes(filename);
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

// FunDec -> ID LP VarList RP | ID LP RP
InterCodes* translate_FunDec(TreeNode *root){	
	assert(root != NULL);
	TreeNode *child = root->firstChild;
	InterCodes *new_code = mallocInterCodes();
	new_code->code.kind = FUNCTIONLABEL;
	new_code->code.data.symbol_name = child->data;
	child = child->nextSibling->nextSibling;
	if(child->name == VarList){
		InterCodes* tail = new_code;
		child = child->firstChild;				//child->name == ParamDec 	VarList -> ParamDec COMMA VarList | ParamDec
		while(1){
			TreeNode *tempnode = child->firstChild->nextSibling;		// ParamDec -> Specifier VarDec
			tempnode = tempnode->firstChild;				// VarDec -> ID | VarDec LB INT RB
			while(tempnode->name != ID)
				tempnode = tempnode->firstChild;
			InterCodes *temp_code = mallocInterCodes();
			temp_code->last = tail;
			tail->next = temp_code;
			tail = temp_code;
			temp_code->code.kind = PARAM;
			temp_code->code.data.symbol_name = tempnode->data;
			findTableNode(tempnode->data)->is_arg = 1;//present variable is an argument.
			child = child->nextSibling;
			if (child == NULL)
				break;
			else
				child = child->nextSibling->firstChild;	// VarList -> ParamDec COMMA VarList child->name = ParamDec
		}
	}
	return new_code;
}


InterCodes* translate_CompSt(TreeNode *root){
	TreeNode *child = root->firstChild->nextSibling;
	InterCodes* code1 = translate_DefList(child);
	InterCodes* code2 = translate_StmtList(child->nextSibling);
	return mergeInterCodes(code1, code2);
}

InterCodes *translate_DefList(TreeNode *root){
	if(root == NULL || root->name == Empty)
		return NULL;
	TreeNode *def = root->firstChild;			// DefList->Def DefList | empty
	
	InterCodes *result = NULL;
	while(1){
		TreeNode *dec = def->firstChild->nextSibling->firstChild;// Def -> Specifier DecList SEMI
																// DecList->Dec | Dec COMMA DecList dec->name = Dec
		TreeNode *specifier = def->firstChild;
		TypePoint type = getSpecifier(specifier);

		while(1){
			assert(!(type->kind == STRUCTURE && dec->firstChild->nextSibling != NULL));	//不存在结构体类型的初始化操作
			TreeNode* vardec = dec->firstChild;	//Dec->VarDec | VarDec ASSIGNOP Exp
			if(vardec->nextSibling != NULL){	//Dec->VarDec ASSIGNOP Exp
				OperandPoint place = mallocOperand(VARIABLE, vardec->firstChild->data);
				OperandPoint t1 = (OperandPoint)malloc(sizeof(Operand));
				InterCodes *temp_code = translate_Exp(vardec->nextSibling->nextSibling,t1);
				InterCodes *code2 = mallocInterCodes();
				code2->code.kind = ONEOP;
				code2->code.data.oneop.left = place;
				code2->code.data.oneop.right = t1;
				result = mergeInterCodes(mergeInterCodes(result,temp_code), code2);
			}
			else if(vardec->firstChild->name != ID){
				assert(vardec->firstChild->firstChild->name == ID);
				TreeNode* id = vardec->firstChild->firstChild;
				TableNode *table = findTableNode(id->data);
				assert(type->kind == BASIC || type->kind == STRUCTURE);
				if(type->kind == BASIC)
					table->type->data.array.offset = 4;
				else{

					table->type->data.array.offset = get_structure_size(type);
				}
				InterCodes *temp_code = mallocInterCodes();
				temp_code->code.kind = DEC;
				temp_code->code.data.decstmt.size = table->type->data.array.offset * table->type->data.array.size;
				temp_code->code.data.decstmt.left = mallocOperand(VARIABLE, id->data);
				result = mergeInterCodes(result, temp_code);
				//我们不需要实现数组的定义
				//printf("don't finish here in %s at %d.\n",__FILE__,__LINE__);
				// TODO: array definiton / allocate memory
			}
			else if(type->kind == STRUCTURE){
				int size = get_structure_size(type);
				InterCodes* code1 = mallocInterCodes();
				OperandPoint var = mallocOperand(VARIABLE, vardec->firstChild->data);

				findTableNode(var->data.var_name)->is_arg = 0;//the variable is not an argument.
				code1->code.kind = DEC;
				code1->code.data.decstmt.left = var;
				code1->code.data.decstmt.size = size;
				result = mergeInterCodes(result,code1);
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

InterCodes *translate_StmtList(TreeNode *root){//StmtList -> Stmt StmtList | empty
	if(root == NULL || root->name == Empty){
		return NULL;
	}
	else{
		TreeNode *stmt = root->firstChild;
		return mergeInterCodes(translate_Stmt(stmt), translate_StmtList(stmt->nextSibling));
	}
}


InterCodes *translate_Stmt(TreeNode *root){			

	InterCodes *result = NULL;
	TreeNode * node = root->firstChild;			// root = Stmt
	if(node->name == RETURN){						// Stmt -> RETURN Exp SEMI
		OperandPoint place = (OperandPoint)malloc(sizeof(Operand));
		place->kind = TEMP;
		place->data.temp_no = allocate_varname();//assingn a varname for return value;
			
		InterCodes *expcode = translate_Exp(node->nextSibling,place);
		InterCodes *new_code = (InterCodes*)malloc(sizeof(InterCodes));
		new_code->next = new_code->last = NULL;
		new_code->code.kind = RETURNFUNCTION;
		new_code->code.data.return_value = place;
		result = mergeInterCodes(expcode,new_code);
	}
	else if(node->name == CompSt){					// Stmt -> CompSt
		result = translate_CompSt(node);
	}
	else if(node->name == Exp){						//Stmt -> Exp SEMI
		OperandPoint place = (OperandPoint)malloc(sizeof(Operand));
		result = translate_Exp(node, place);
	}
	else if(node->name == IF){
		InterCodes *label1 = mallocInterCodes();
		label1->code.kind = LABEL;
		label1->code.data.symbol_name = allocate_labelname();

		InterCodes *label2 = mallocInterCodes();
		label2->code.kind = LABEL;
		label2->code.data.symbol_name = allocate_labelname();

		TreeNode *exp = node->nextSibling->nextSibling;
		TreeNode *stmt1 = exp->nextSibling->nextSibling;
		InterCodes *code1 = translate_Cond(exp, label1, label2);
		InterCodes *code2 = translate_Stmt(stmt1);
		if(stmt1->nextSibling == NULL){
			//return code1 + [LABEL label1] + code2 + [LABEL label2]
			result = mergeInterCodes(code1, label1);
			result = mergeInterCodes(result, code2);
			result = mergeInterCodes(result, label2);
		}
		else{
			TreeNode *stmt2 = stmt1->nextSibling->nextSibling;
			InterCodes *label3 = mallocInterCodes();
			label3->code.kind = LABEL;
			label3->code.data.symbol_name = allocate_labelname();
			InterCodes *code3 = translate_Stmt(stmt2);
			InterCodes *goto_code = mallocInterCodes();
			goto_code->code.kind = GOTO;
			goto_code->code.data.symbol_name = label3->code.data.symbol_name;
			//return code1 + [LABEL label1] + code2 + [GOTO label3] + [LABEL label2] + code3 + [LABEL label3]
			result = mergeInterCodes(code1, label1);
			result = mergeInterCodes(result, code2);
			result = mergeInterCodes(result, goto_code);
			result = mergeInterCodes(result, label2);
			result = mergeInterCodes(result, code3);
			result = mergeInterCodes(result, label3);
		}
	}
	else if(node->name == WHILE){
		InterCodes *label1 = mallocInterCodes();
		label1->code.kind = LABEL;
		label1->code.data.symbol_name = allocate_labelname();

		InterCodes *label2 = mallocInterCodes();
		label2->code.kind = LABEL;
		label2->code.data.symbol_name = allocate_labelname();

		InterCodes *label3 = mallocInterCodes();
		label3->code.kind = LABEL;
		label3->code.data.symbol_name = allocate_labelname();

		TreeNode *exp = node->nextSibling->nextSibling;
		InterCodes *code1 = translate_Cond(exp, label2, label3);
		InterCodes *code2 = translate_Stmt(exp->nextSibling->nextSibling);

		InterCodes *goto_code = mallocInterCodes();
		goto_code->code.kind = GOTO;
		goto_code->code.data.symbol_name = label1->code.data.symbol_name;

		//return [LABEL label1] + code1 + [LABEL label2] + code2 + [GOTO label1] + [LABEL label3]
		result = mergeInterCodes(label1, code1);
		result = mergeInterCodes(result, label2);
		result = mergeInterCodes(result, code2);
		result = mergeInterCodes(result, goto_code);
		result = mergeInterCodes(result, label3);
	}
	else{
		
		printf("Don't finish in %s at %d.\n",__FILE__,__LINE__);
		return NULL;
	}
	return result;
}

// Args -> Exp COMMA Args | Exp
InterCodes* translate_Args(TreeNode *root, ArgListNode *arg_list){
	TreeNode *child = root->firstChild;
	TreeNode *comma = child->nextSibling;
	int hhh = allocate_varname();
	OperandPoint t1 = mallocOperand(TEMP,hhh);
	InterCodes *code1 = translate_Exp(child, t1);
	ArgListNode *new_arg_node = (ArgListNode *)malloc(sizeof(ArgListNode));
	new_arg_node->operand_point = t1;
	new_arg_node->next = arg_list->next;
	arg_list->next = new_arg_node;

	if (comma == NULL) {
		return code1;
	}
	else {
		InterCodes *code2 = translate_Args(comma->nextSibling, arg_list);
		return mergeInterCodes(code1, code2);
	}
}


InterCodes* translate_Exp(TreeNode *root, OperandPoint place){
	TreeNode *child = root->firstChild;
	TreeNode *operator = child->nextSibling;
	InterCodes *new_code = (InterCodes *)malloc(sizeof(InterCodes));
	new_code->last = new_code->next = NULL;
	if (child->name == LP) {
		return translate_Exp(child->nextSibling, place);
	}
	else if (child->name == INT){
		place->kind = CONSTANT;
		place->data.value = child->data;
		return NULL;
	}
	else if(child->name == FLOAT) {
		new_code->code.kind = ONEOP;
		new_code->code.data.oneop.left = place;
		OperandPoint right = (OperandPoint)malloc(sizeof(Operand));
		right->kind = CONSTANT;
		right->data.value = child->data;
		new_code->code.data.oneop.right = right;
	}
	//  Exp -> ID LP Args RP | ID LP RP | ID
	else if(child->name == ID){
		if (child->nextSibling == NULL) {
			TableNode *table = findTableNode(child->data);
			if(table->type->kind != BASIC && table->is_arg == 0){//is not an argument, should use its reference.
				place->kind = REFERENCE;
				place->data.refer_name = child->data;
			}
			else{
				place->kind = VARIABLE;
				place->data.var_name = child->data;
				
			}
			return NULL;
		}
		else {
			TreeNode *args = child->nextSibling->nextSibling;
			OperandPoint new_temp = mallocOperand(TEMP, allocate_varname());
			if (args->name == Args) {		// Exp -> ID LP Args RP    / Args -> Exp COMMA Args | Exp
				ArgListNode *arg_list = (ArgListNode *)malloc(sizeof(ArgListNode));		// head node with no content
				arg_list->next = NULL;
				InterCodes *code1 = translate_Args(args, arg_list);
				assert(arg_list->next != NULL);
				if (strcmp(child->data, "write") == 0) {
					new_code->code.kind = WRITE;
					new_code->code.data.operand_point = arg_list->next->operand_point;
					return mergeInterCodes(code1, new_code);
				}
				else {
					ArgListNode *arg_node = arg_list->next;
					InterCodes *code2 = NULL;
					while (arg_node != NULL) {
						InterCodes *code_arg = (InterCodes *)malloc(sizeof(InterCodes));
						code_arg->last = code_arg->next = NULL;
						code_arg->code.kind = ARG;
						code_arg->code.data.arg_value = arg_node->operand_point;
						code2 = mergeInterCodes(code2, code_arg);
						arg_node = arg_node->next;
					}

					InterCodes *code_temp = mallocInterCodes();
					code_temp->code.kind = CALL;
					code_temp->code.data.funcall.left = new_temp;
					code_temp->code.data.funcall.fun_name = child->data;

					InterCodes *code3 = (InterCodes*)malloc(sizeof(InterCodes));
					code3->last = code3->next = NULL;
					code3->code.kind = ONEOP;
					code3->code.data.oneop.left = place;
					code3->code.data.oneop.right = new_temp;
					code3 = mergeInterCodes(code_temp, code3);
					return mergeInterCodes(mergeInterCodes(code1, code2), code3);
				}
			}
			else {							// Exp -> ID LP RP
				if (strcmp(child->data, "read") == 0) {
					new_code->code.kind = READ;
					new_code->code.data.symbol_name = get_temp_varname(new_temp->data.temp_no);
				}
				else {
					new_code->code.kind = CALL;
					new_code->code.data.funcall.left = new_temp;
					new_code->code.data.funcall.fun_name = child->data;
				}
				InterCodes *code2 = mallocInterCodes();
				code2->code.kind = ONEOP;
				code2->code.data.oneop.left = place;
				code2->code.data.oneop.right = new_temp;
				return mergeInterCodes(new_code, code2);
			}
		}
	}
	else if(child->name == MINUS){
		OperandPoint new_temp = (OperandPoint)malloc(sizeof(Operand));
		new_temp->kind = TEMP;
		new_temp->data.temp_no = allocate_varname();
		OperandPoint op1 = (OperandPoint)malloc(sizeof(Operand));
		op1->kind = CONSTANT;
		
		op1->data.value = "0";

		InterCodes* code1 = translate_Exp(child->nextSibling,new_temp);
		InterCodes* code2 = (InterCodes*)malloc(sizeof(InterCodes));
		code2->last = code2->next = NULL;
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
		label1->last = label1->next = label2->last = label2->next = NULL;
		label1->code.kind = label2->code.kind = LABEL;
		label1->code.data.symbol_name = allocate_labelname();
		label2->code.data.symbol_name = allocate_labelname();

		OperandPoint op1 = (OperandPoint)malloc(sizeof(Operand));
		op1->kind = CONSTANT;
		op1->data.value = "0";
		InterCodes *code0 = (InterCodes *)malloc(sizeof(InterCodes));
		code0->last = code0->next = NULL;
		code0->code.kind = ONEOP;
		code0->code.data.oneop.left = place;
		code0->code.data.oneop.right = op1;

		InterCodes *code1 = translate_Cond(root, label1, label2);

		OperandPoint op2 = (OperandPoint)malloc(sizeof(Operand));
		op2->kind = CONSTANT;
		op2->data.value = "1";		
		InterCodes *codetemp = (InterCodes *)malloc(sizeof(InterCodes));
		codetemp->last = codetemp->next = NULL;
		codetemp->code.kind = ONEOP;
		codetemp->code.data.oneop.left = place;
		codetemp->code.data.oneop.right = op2;

		InterCodes *code2 = mergeInterCodes(label1, codetemp);

		return mergeInterCodes(mergeInterCodes(mergeInterCodes(code0, code1), code2), label2);
	}
	else if (operator->name == PLUS || operator->name == MINUS || operator->name == STAR || operator->name == DIV) {			//Exp1 PLUS Exp2
		OperandPoint t1 = (OperandPoint)malloc(sizeof(Operand));
		t1->kind = TEMP;
		t1->data.temp_no = allocate_varname();
		OperandPoint t2 = (OperandPoint)malloc(sizeof(Operand));
		t2->kind = TEMP;
		t2->data.temp_no = allocate_varname();
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
	else if(operator->name == DOT){//Exp->Exp DOT ID
		OperandPoint t1 = (OperandPoint)malloc(sizeof(Operand));
		InterCodes* code1 = translate_Exp(child, t1);
		TypePoint type = getExp(child);
		int offset = get_structure_offset(type, operator->nextSibling->data);
		assert(type->kind == STRUCTURE);
		if(code1 == NULL){//child->firstChild->name == ID 
			place->kind = ADDRESS;
			place->data.temp_no = allocate_varname();
			code1 = mallocInterCodes();
			code1->code.kind = BINOP;
			code1->code.data.binop.opkind = PLUS;
			code1->code.data.binop.result = mallocOperand(VARIABLE, get_temp_varname(place->data.temp_no));
			code1->code.data.binop.op1 = t1;
			code1->code.data.binop.op2 = mallocOperand(CONSTANT, offset);
			return code1;
		}
		else{
			//assert(t1->kind == STRUCTADDRESS);
			InterCodes *code2 = mallocInterCodes();
			code2->code.kind = BINOP;
			code2->code.data.binop.opkind = PLUS;
			place->data.temp_no = allocate_varname();
			place->kind = ADDRESS;
			code2->code.data.binop.op1 = mallocOperand(VARIABLE, get_temp_varname(t1->data.temp_no));
			code2->code.data.binop.op2 = mallocOperand(CONSTANT, offset);
			code2->code.data.binop.result = mallocOperand(VARIABLE, get_temp_varname(place->data.temp_no));
			return mergeInterCodes(code1, code2);
		}
	}
	else if(operator->name == ASSIGNOP){
		InterCodes* code1 = translate_Exp(child,place);
		assert(place->kind == ADDRESS || place->kind == VARIABLE || place->kind == TEMP);
		OperandPoint temp = (OperandPoint)malloc(sizeof(Operand));
		InterCodes* code2 = translate_Exp(operator->nextSibling, temp);
		InterCodes* code3 = mallocInterCodes();
		code3->code.kind = ONEOP;
		code3->code.data.oneop.left = place;
		code3->code.data.oneop.right = temp;
		return mergeInterCodes(mergeInterCodes(code1, code2), code3);
	}
	else if(child->name == Exp && operator->name == LB){//Exp->Exp1 LB Exp2 RB
		TreeNode* exp1 = child->firstChild;
		/*calculate translate(Exp1)*/
		InterCodes* code1 = NULL;
		OperandPoint t1 = NULL;
		if(exp1->name == ID){
			t1 = mallocOperand(REFERENCE, exp1->data);
		}
		else{
			t1 = mallocOperand(TEMP, allocate_varname());
			code1 = translate_Exp(child, t1);
			assert(t1->kind == ADDRESS);
		}
		/*calcualte translate(Exp2)*/
		OperandPoint t2 = (OperandPoint)malloc(sizeof(Operand));
		InterCodes* code2 = translate_Exp(operator->nextSibling, t2);

		/*calculate offset of the per element in the array.*/
		TypePoint rootType = getExp(root);//calcuate the array element type.
		InterCodes* offset_code = mallocInterCodes();
		offset_code->code.kind = BINOP;
		offset_code->code.data.binop.opkind = STAR;
		offset_code->code.data.binop.op1 = mallocOperand(CONSTANT, getSize(rootType));
		offset_code->code.data.binop.op2 = t2;
		offset_code->code.data.binop.result = mallocOperand(TEMP, allocate_varname());

		InterCodes* code3 = mallocInterCodes();
		code3->code.kind = BINOP;
		code3->code.data.binop.opkind = PLUS;
		if(code1 == NULL){
			code3->code.data.binop.op1 = t1;
		}
		else{
			code3->code.data.binop.op1 = mallocOperand(TEMP, t1->data.temp_no);
		}
		code3->code.data.binop.op2 = offset_code->code.data.binop.result;
		place->data.temp_no = allocate_varname();
		code3->code.data.binop.result = mallocOperand(TEMP, place->data.temp_no);
		if(rootType->kind == STRUCTURE){
			place->kind == STRUCTADDRESS;
		}
		else{
			place->kind = ADDRESS;
		}
		return mergeInterCodes(mergeInterCodes(code1, code2), mergeInterCodes(offset_code, code3));

	}
	else{
		printf("Don't finished the Function.\n");
		return NULL;
	}
	return new_code;
}

InterCodes* translate_Cond(TreeNode *exp, InterCodes *label_true, InterCodes *label_false) {
	TreeNode *child = exp->firstChild;
	TreeNode *operator = child->nextSibling;
	// InterCodes *new_code = (InterCodes *)malloc(sizeof(InterCodes));
	// new_code->last = new_code->next = NULL;
	//assert(label_true->last == NULL && label_true->next == NULL && label_false->last == NULL && label_false->next == NULL);afffasdfsdfaf sfa DSFS
	if (operator != NULL) {
		if (child->name == NOT) {
			return translate_Cond(child->nextSibling, label_false, label_true);
		}
		else if (operator->name == AND) {
			InterCodes *label1 = (InterCodes *)malloc(sizeof(InterCodes));
			label1->code.kind = LABEL;
			label1->last = label1->next = NULL;
			label1->code.data.symbol_name = allocate_labelname();

			InterCodes *code1 = translate_Cond(child, label1, label_false);
			InterCodes *code2 = translate_Cond(operator->nextSibling, label_true, label_false);
			return mergeInterCodes(mergeInterCodes(code1, label1), code2);
		}
		else if (operator->name == OR) {
			InterCodes *label1 = (InterCodes *)malloc(sizeof(InterCodes));
			label1->code.kind = LABEL;
			label1->last = label1->next = NULL;
			label1->code.data.symbol_name = allocate_labelname();

			InterCodes *code1 = translate_Cond(child, label_true, label1);
			InterCodes *code2 = translate_Cond(operator->nextSibling, label_true, label_false);

			return mergeInterCodes(mergeInterCodes(code1, label1), code2);
		}
		else if (operator->name == RELOP) {

			OperandPoint t1 = (OperandPoint)malloc(sizeof(Operand));
			t1->kind = TEMP;
			t1->data.temp_no = allocate_varname();
			OperandPoint t2 = (OperandPoint)malloc(sizeof(Operand));
			t2->kind = TEMP;
			t2->data.temp_no = allocate_varname();
			
			InterCodes *code1 = translate_Exp(child, t1);
			InterCodes *code2 = translate_Exp(operator->nextSibling, t2);

			InterCodes *code3 = (InterCodes *)malloc(sizeof(InterCodes));
			code3->last = code3->next = NULL;
			code3->code.kind = IFSTMT;
			//assert(operator->data != NULL);
			strcpy(code3->code.data.ifstmt.relop, operator->data);
			code3->code.data.ifstmt.left = t1;
			code3->code.data.ifstmt.right = t2;
			//assert(label_true->code.data.symbol_name != NULL);
			code3->code.data.ifstmt.label = label_true->code.data.symbol_name;

			InterCodes *code4 = (InterCodes *)malloc(sizeof(InterCodes));
			code4->last = code4->next = NULL;
			code4->code.kind = GOTO;
			//assert(label_false->code.data.symbol_name != NULL);
			code4->code.data.symbol_name = label_false->code.data.symbol_name;

			return mergeInterCodes(mergeInterCodes(mergeInterCodes(code1, code2), code3), code4);
		}
		else {
			OperandPoint t1 = (OperandPoint)malloc(sizeof(Operand));
			OperandPoint t2 = (OperandPoint)malloc(sizeof(Operand));
			t1->kind = TEMP;
			t2->kind = CONSTANT;
			t1->data.temp_no = allocate_varname();
			t2->data.value = "0";

			InterCodes *code1 = translate_Exp(exp, t1);
			InterCodes *code2 = (InterCodes *)malloc(sizeof(InterCodes));
			code2->last = code2->next = NULL;
			code2->code.kind = IFSTMT;
			code2->code.data.ifstmt.relop[0] = '!';
			code2->code.data.ifstmt.relop[1] = '=';
			code2->code.data.ifstmt.relop[2] = '\0';
			code2->code.data.ifstmt.label = label_true->code.data.symbol_name;
			code2->code.data.ifstmt.left = t1;
			code2->code.data.ifstmt.right = t2;

			InterCodes *code3 = (InterCodes *)malloc(sizeof(InterCodes));
			code3->last = code3->next = NULL;
			code3->code.kind = GOTO;
			code3->code.data.symbol_name = label_false->code.data.symbol_name;

			return mergeInterCodes(mergeInterCodes(code1, code2), code3);
		}
	}
}
