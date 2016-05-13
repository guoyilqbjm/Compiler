#include "rules.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int isInsert = 1;
int opType = 0;// -1 variable exists in the symbol table.
							// 0 variable create success but not insert into the symbol table, such as the variable in the structure.

void getExtDef(TreeNode *head){
	TreeNode *child = head->firstChild, *temp = child;
	TypePoint specifier = getSpecifier(child);
	if(specifier == NULL)
		return;
	child = child->nextSibling;
	if(child->name == ExtDecList){
		//ExtDef->Specifier ExtDecList SEMI;
		getExtDecList(specifier,child);
	}
	else if (child->name == FunDec){
		//Extdef -> Specifier FunDec CompSt | Specifier FunDec SEMI;
		TypePoint type = getFunDec(specifier,child);
		TableNode *result = NULL;
		TableNode *findResult = findTableNode(type->data.structure->name);
		if (findResult == NULL){
			result = insertTableNode(type->data.structure->name, type);	//insert into the table
			FieldListPoint fp = type->data.structure->tail;
			for(;fp!=NULL;fp=fp->tail){
				if(insertTableNode(fp->name,fp->type)==NULL){
					printf("Error type 3 at Line %d: Redefine variable \"%s\".\n",fp->lineno,fp->name);
				}
			}
			if(child->nextSibling->name == SEMI){
				// 将声明的函数加入“声明表”以待后续检验
				result->def_state = -1;		//声明
				insertFuncDefTable(child->lineno, result);
			}
			else{
				result->def_state = 1;
				getCompSt(specifier,child->nextSibling);
			}
		}
		else { //findResult != NULL
			// 判断在表中的类型，如果不符合，返回
			//printf(">>>>%d\n", testType(findResult->type, type));
			if (testType(findResult->type, type) == -1) {
				printf("Errot type 19 at Line %d: Conflict between function declarations.\n", child->lineno);
				return;
			}	
			if(child->nextSibling->name == CompSt){
				// set def_state to 1 and judge redefinition
				if (findResult->def_state == -1) {
					findResult->def_state = 1;
				}
				else {
					printf("Error type 4 at Line %d: Redefined function name \"%s\".\n",root->lineno,type->data.structure->name);
					return;
				}
				getCompSt(specifier,child->nextSibling);
			}

		}
		//getCompSt(specifier,child->nextSibling);
	}
	else{
		//ExtDef -> Specifier SEMI;
	}
	return;
}

void getCompSt(TypePoint returnType,TreeNode *head){
	TreeNode *child = head->firstChild->nextSibling;//child->name=DefList;
	syntaxTest(child);
	child = child->nextSibling;
	for(;;){
		if(child->name == Empty)
			break;
		child = child->firstChild;
		getStmt(returnType,child);
		child = child->nextSibling;
	}
}

void getStmt(TypePoint returnType,TreeNode *head){
	TreeNode *child = head->firstChild;
	if(child->name == Exp){
		getExp(child);
	}
	else if(child->name == CompSt)
		getCompSt(returnType,child);
	else if(child->name == RETURN){
		TypePoint expType = getExp(child->nextSibling);
		if(testType(expType,returnType)==-1){
			printf("Error type 8 at Line %d: Type mismatched for return.\n",child->nextSibling->lineno);
			return;
		}
	}
	else if(child->name == WHILE){
		child = child->nextSibling->nextSibling;
		TypePoint p = getExp(child);
		if(!(p->kind==BASIC && p->data.basic==INT)){
			printf("Error type 7 at Line %d: Type mismatched for while condition.\n",child->lineno);
			return;
		}
		child = child->nextSibling->nextSibling;
		getStmt(returnType,child);
	}
	else{
		child = child->nextSibling->nextSibling;
		TypePoint p = getExp(child);
		if(!(p->kind == BASIC && p->data.basic == INT)){
			printf("Error type 7 at Line %d: Type mismatched for if condition.\n",child->lineno);
			return;
		}
		child = child->nextSibling->nextSibling;
		getStmt(returnType,child);
		child = child->nextSibling;
		if(child != NULL){
			getStmt(returnType,child->nextSibling);
		}
	}
}

void getExtDecList(TypePoint lastType,TreeNode *head){
	TreeNode *child = head->firstChild;
	for(;;){
		getVarDec(lastType,child);//anaylysis a variable and insert into symbol table.
		child = child->nextSibling;
		if(child == NULL)
			//ExtDecList->VarDec;
			break;
		else{
			//ExtDecList->VarDec COMMA ExtDecList;
			child = child->nextSibling->firstChild;
		}
	}
}


TypePoint getFunDec(TypePoint lastType,TreeNode *root){
	TypePoint type = (TypePoint)malloc(sizeof(Type));
	TreeNode *child = root->firstChild;
	type->kind = FUNCTION;
	type->data.structure = (FieldListPoint)malloc(sizeof(FieldList));
	type->data.structure->name = child->data;
	type->data.structure->type = lastType;
	FieldListPoint field = type->data.structure;
	child = child->nextSibling->nextSibling;
	if(child->name == RP){//FunDec->ID LP RP
		field->tail = NULL;
	}
	else{
		isInsert = 0;
		//FunDec->ID LP VarList Rp
		for(child = child->firstChild;;){
			
			TypePoint t = getSpecifier(child->firstChild);
			if(t!=NULL){
				FieldListPoint fieldTemp=getVarDec(t,child->firstChild->nextSibling);
				
				if(opType == 0){//insert Success!
					field->tail = fieldTemp;
					field = fieldTemp;
				}
			}
			child=child->nextSibling;
			if(child == NULL)
				break;
			else
				child = child->nextSibling->firstChild;
		}
		isInsert = 1;
	}
	return type;
}

TypePoint getSpecifier(TreeNode *head){
	TreeNode *child = head->firstChild;
	if(child->name == TYPE){
		TypePoint type = (TypePoint)malloc(sizeof(Type));
		type->kind = BASIC;
		if(strcmp(child->data,"INT")==0)
			type->data.basic = INT;
		else
			type->data.basic = FLOAT;
		return type;
	}
	else{
		//Specifier -> StructSpecifier
		child=child->firstChild;
		TreeNode *temp= child;
		TreeNode *brother = child->nextSibling;
		if(brother->name == OptTag || brother->name == Empty){
			isInsert++;
			char *name = NULL; 
			int lineno = 0;
			if(brother->name == OptTag){
				name = brother->firstChild->data;
				lineno = brother->firstChild->lineno;
			}
			brother=brother->nextSibling->nextSibling;//brother->name = DefList;
			FieldListPoint field = NULL;
			for(;;){
				if(brother->name == Empty)
					break;
				child = brother->firstChild;
				FieldListPoint temp = getDef(child);
				if(field == NULL)
					field = temp;
				else{
					FieldListPoint fieldhead = field,tail = field;
					for(;fieldhead!=NULL;)
						tail=fieldhead,fieldhead=fieldhead->tail;
					tail->tail = temp;
				}
				brother = child->nextSibling;
			}
			FieldListPoint Head = field;
			for(;Head!=NULL;Head = Head->tail){
				FieldListPoint p=Head;
				if(p==NULL)
					break;
				for(p=p->tail;p!=NULL;p=p->tail){
					if(strcmp(p->name,Head->name)==0){
						printf("Error type 15 at Line %d: Redefined field \"%s\".\n",p->lineno,p->name);
					}
				}
			}
			TypePoint type = (TypePoint)malloc(sizeof(Type));
			type->kind = STRUCTURE;
			type->data.structure = (FieldListPoint)malloc(sizeof(FieldList));
			type->data.structure->type = (TypePoint)malloc(sizeof(Type));
			type->data.structure->type->kind = STRUCTURE;
			type->data.structure->type->data.structure = field;
			type->data.structure->name = name;
			type->data.structure->tail = NULL;
			type->data.structure->lineno = lineno;
			if(name != NULL && isInsert == 2){//when the isInsert value equals 2,the type is a structure and should be inserted into the symbol talbe.
				TableNode *table = insertTableNode(name,type);
				if(table == NULL)
					printf("Error type 16 at Line %d: Duplicated name \"%s\".\n",type->data.structure->lineno,name);
			}
			isInsert--;
			return type;
		}
		else{
			brother = brother->firstChild;
			char *name = brother->data;
			TableNode *find = findTableNode(name);
			if(find == NULL){
				printf("Error type 17 at Line %d: Undefined structure \"%s\".\n",brother->lineno,name);
				return NULL;
			}
			else if(find->type->kind != STRUCTURE){
				printf("Error type 17 at Line %d: \"%s\" is a variable, isn't a structure.\n",brother->lineno,name);
				return NULL;
			}
			else if(strcmp(find->type->data.structure->name,name)!=0){
				printf("Error type 17 at Line %d: \"%s\"  is a struct variable, isn't a structure.\n",brother->lineno,name);
				return NULL;
			}
			else{
				return find->type;
			}
		}
		return NULL;
	}
}

FieldListPoint getVarDec(TypePoint lastType,TreeNode *head){
	//VarDec->ID | VarDec LB iNT RB
	TypePoint tempLastType=lastType;
	TreeNode *child = head->firstChild,*next;
	FieldListPoint result = (FieldListPoint)malloc(sizeof(FieldList));
	while(child->name != ID){
		TypePoint newType = (TypePoint)malloc(sizeof(Type));
		newType->kind = ARRAY;
		next=child->nextSibling;
		next=next->nextSibling;
		newType->data.array.size = atoi(next->data);
		newType->data.array.elem = tempLastType;
		tempLastType = newType;
		next = next->nextSibling;
		child = child->firstChild;
	}
	result->name = child->data;
	result->type = tempLastType;


	result->lineno = child->lineno;
	result->tail = NULL;
	if(isInsert == 1){
		TableNode *tt = insertTableNode(child->data,tempLastType);
		if(tt == NULL){
			printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",child->lineno,child->data);
			opType = -1;
		}
		else
			opType = 1;
	}
	else
		opType = 0;	
	return result;
}

FieldListPoint getDef(TreeNode *head){
	TreeNode* child = head->firstChild;
	TypePoint type = getSpecifier(child);
	if(type == NULL)
		return NULL;
	FieldListPoint r = NULL;
	child = child->nextSibling->firstChild;//child->name = Dec
	for(;;){
		TreeNode *dec = child->firstChild;//dec->name = VarDec;
		FieldListPoint result = getVarDec(type,dec);//Here may exist error type 5:Type mismatched for assignment; Aslo  set dec inital.
		if(opType == 0 || opType == 1){
			if(dec->nextSibling!=NULL){//Dec->VarDec ASSIGNOP Exp{
				if(opType == 0){
					printf("Error type 15 at Line %d: Illegal initial in structure for variable \"%s\".\n",result->lineno,result->name);
					child = child->nextSibling;
					if(child != NULL)
						child = child->nextSibling->firstChild;
					else
						break;
					continue;
					printf("Never arrive this in %s at Line%d.\n",__FILE__,__LINE__);
				}
				else{
					TypePoint t = getExp(dec->nextSibling->nextSibling);
					if(t != NULL && testType(t,type) == -1){
						printf("Error type 7 at Line %d: Type mismatched for \"=\".\n",dec->lineno);
					}
				}
			}
			FieldListPoint newField = (FieldListPoint)malloc(sizeof(FieldList));
			newField->name = result->name;
			newField->type = result->type;
			newField->lineno = dec->lineno;
			newField->tail = r;
			r = newField;
		}
		child = child->nextSibling;
		if(child!=NULL){
			child = child->nextSibling->firstChild;
		}
		else
			break;
	}
	return r;
}

int testType(TypePoint type1,TypePoint type2){
	//return -1 when type1 and type2 are different kind type.
	//otherwise, return 1;
	
	if(type1 == NULL || type2 == NULL)
		return -1;
	if(type1->kind == type2->kind){
		if(type1->kind == BASIC){
			return (type1->data.basic == type2->data.basic) ? 1 : -1;
		}
		else if(type1->kind == STRUCTURE){
			return (type1->data.structure->name == type2->data.structure->name) ? 1 : -1;
		}
		else if(type1->kind == FUNCTION){
			FieldListPoint a = type1->data.structure,
						   b = type2->data.structure;
			while (a != NULL && b != NULL){
				if(testType(a->type,b->type) == -1)
					return -1;
				a = a->tail;
				b = b->tail;
			}
			return (a != NULL || b != NULL) ? -1 : 1;
		}
		else{
			return testType(type1->data.array.elem, type2->data.array.elem);
		}
	}
	else
		return -1;
}

TypePoint getExp(TreeNode *head){
	TreeNode *child = head->firstChild;
	if(child->name == Exp && child->nextSibling->name == LB){
		//Exp -> Exp LB Exp RB
		TypePoint type1 = getExp(child);
		if(type1 == NULL)
			return NULL;
		if(type1->kind != ARRAY){
			printf("Error type 10 at Line %d: The variable isn't an array.\n",child->lineno);
			return NULL;
		}
		else{
			child = child->nextSibling->nextSibling;
			TypePoint type2 = getExp(child);
			if(type2 == NULL)
				return NULL;
			if(!(type2->kind == BASIC && type2->data.basic == INT)){
				printf("Error type 12 at Line %d:Array index should be an integer.\n",child->lineno);
				return NULL;
			}
			else{
				return type1->data.array.elem;
			}
		}
	}
	else if(child->name == Exp && child->nextSibling->name == DOT){
		//Exp -> Exp DOT ID
		TypePoint type1 = getExp(child);
		if(type1 == NULL)
			return NULL;
		if(type1->kind != STRUCTURE){
			printf("Error type 13 at Line %d: Illegal use of \".\".\n",child->nextSibling->lineno);
			return NULL;
		}
		else{
			child = child->nextSibling->nextSibling;
			FieldListPoint field = type1->data.structure->type->data.structure;
			for(;field!=NULL;field = field->tail){
				if(strcmp(field->name,child->data)==0)
					break;
			}
			if(field == NULL){
				printf("Error type 14 at Line %d: Non-existent field \"%s\".\n",child->lineno,child->data);
				return NULL;
			}
			else{
				return field->type;
			}
		}
	}
	else if(child->name == Exp){
		if(child->nextSibling->name == ASSIGNOP){
			//Exp -> Exp ASSIGNOP Exp
			TreeNode *node = child->firstChild;
			if(!((node->name == ID && node->nextSibling == NULL)||(node->name == Exp && node->nextSibling->name == DOT)||(node->name == Exp && node->nextSibling->name  == LB && node->nextSibling->nextSibling->name == Exp))){//When the left is a variable, return NULL;
				printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n",child->lineno);
				return NULL;
			}
		}
		TypePoint type1 = getExp(child), type2 = getExp(child->nextSibling->nextSibling);

		if(type1 == NULL || type2 == NULL)
			return NULL;

		if(testType(type1,type2)==-1){
			printf("Error type 5 at Line %d: Type mismatched for \"%s\".\n",child->lineno,getName(child->nextSibling->name));
			return NULL;
		}
		if(child->nextSibling->name == ASSIGNOP)
			return type1;
		child = child->nextSibling;
		if(child->name  == AND || child->name == OR){
			//Exp -> Exp AND Exp | Exp OR Exp
			if(!(type1->kind == BASIC && type1->data.basic == INT && type2->kind == BASIC && type2->data.basic == INT)){
				printf("Error type 6 at Line %d: Type mismatched for \"%s\". Two variables' type should be int.\n",child->lineno,child->data);
				return NULL;
			}
		}
		else{
			//Exp -> Exp RELOP Exp | Exp PLUS Exp | Exp MINUS Exp | Exp DIV Exp
			if(!(type1->kind == BASIC && type2->kind == BASIC && type1->data.basic == type2->data.basic)){
				printf("Error type 6 at Line %d: Type mismatched for \"%s\".\n",child->lineno,child->data);
				return NULL;
			}
			if(child->name == RELOP){//比较结果应该返回int型的值
				TypePoint newType = (TypePoint)malloc(sizeof(Type));
				newType->kind = BASIC;
				newType->data.basic = INT;
				return newType;
		 	}
		}

		return type1;
	}
	else if(child->name == LP){
		return getExp(child->nextSibling);
	}
	else if(child->name == MINUS || child->name == NOT){
		//Exp -> MINUS Exp | NOT Exp
		TypePoint p = getExp(child->nextSibling);
		if(!(p->kind == BASIC && p->data.basic == INT)){
			printf("Error type 6 at Line %d: Type mismatched for \"%s\".\n",child->lineno,(child->name==NOT)?"!":".");
			return NULL;
		}
		return p;
	}
	else if(child->name == ID){
		if(child->nextSibling != NULL){
			//Exp -> ID LP RP | ID LP Args RP
			TableNode* find = findTableNode(child->data);
			if(find == NULL){
				printf("Error type 2 at Line %d: Undefined function \"%s\".\n",child->lineno,child->data);
				return NULL;
			}
			else if(find->type->kind != FUNCTION){
				printf("Error type 11 at Line %d: \"%s\" is not a function.\n",child->lineno,child->data);
				return NULL;
			}
			else{
				TypePoint tempType = find->type;
				if(child->nextSibling->nextSibling->name == RP){
					if(tempType->data.structure->tail == NULL){
						return tempType->data.structure->type;
					}
					else{
						printf("Error type 9 at Line %d: Too few arguments for function \"%s\".\n",child->lineno,find->name);
						return NULL;
					}
				}
				else{
					FieldListPoint argList = tempType->data.structure->tail;
					if(argList == NULL){
						printf("Error type 9 at Line %d: Too many arguments for function \"%s\".\n",child->lineno,child->data);
						return NULL;
					}
					for(child = child->nextSibling->nextSibling->firstChild;;){
						TypePoint childType = getExp(child);
						if(childType == NULL){
							return NULL;
						}
						if(testType(childType,argList->type)==-1){
							printf("Error type 9 at Line %d: Arguments type mismatched.\n",child->lineno);
							return NULL;
						}
						argList = argList->tail;
						//child = child->nextSibling;
						if(argList == NULL && child->nextSibling != NULL){
							printf("Error type 9 at Line %d: Too many arguments for function \"%s\".\n",child->lineno,find->name);
							return NULL;
						}
						if(argList != NULL && child->nextSibling == NULL){
							printf("Error type 9 at Line %d: Too few arguments for function \"%s\".\n",child->lineno,find->name);
							return NULL;
						}
						if(argList == NULL && child->nextSibling == NULL)
							return find->type->data.structure->type;
						child = child->nextSibling->nextSibling->firstChild;
					}
				}
				printf("Should never arrivs this %s %d.\n",__FILE__,__LINE__);
			return tempType->data.structure->type;
			}
		}
		TableNode *find = findTableNode(child->data);
		if(find == NULL){
			if(child->nextSibling == NULL){
				printf("Error type 1 at Line %d: Undefined variable \"%s\".\n",child->lineno,child->data);
				return NULL;
			}
			else{
				printf("Error type 2 at Line %d: Undefined function \"%s\".\n",child->lineno,child->data);
				return NULL;
			}
		}
		else{
			if(find->type->kind == STRUCTURE && find->type->data.structure->name != NULL && strcmp(find->type->data.structure->name,find->name)==0){
				printf("Error type 13 at Line %d: \"%s\" is a struct, isn't a variable.\n",child->lineno,find->name);
				return NULL;
			}
			return find->type;
		}
	}
	else if(child->name == INT){
		TypePoint t = (TypePoint)malloc(sizeof(Type));
		t->kind = BASIC;
		t->data.basic = INT;
		return t;
	}
	else {
		if(child->name != FLOAT){
			printf("Error in %s %d.\n",__FILE__,__LINE__);
		}
		TypePoint t = (TypePoint)malloc(sizeof(Type));
		t->kind = BASIC;
		t->data.basic = FLOAT;
		return t;
	}
}
