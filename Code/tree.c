#include "tree.h"
#include "syntax.tab.h"
#include "lab2/rules.h"
TreeNode * treeNodeMalloc(int lineno,int name){
	/* malloc a new treeNode from disk. */
	TreeNode * newNode = (TreeNode *)malloc(sizeof(TreeNode));
	if(newNode==NULL){
		printf("malloc TreeNode failed!\n");
		exit(-1);
	}
	newNode->lineno = lineno;
	newNode->name = name;
	newNode->firstChild = NULL;
	newNode->nextSibling = NULL;
	return newNode;
}

void insertNode(TreeNode *parent, int count, ...){
	va_list arg;
	int i = 0;
	va_start(arg,count);
	TreeNode *present = NULL;
	if(parent->firstChild == NULL){
		present = va_arg(arg,TreeNode*);
		parent->firstChild = present;
	//	printf("%d insert node %d  success!\n",parent->name,present->name);
		++i;
	}
	TreeNode *sons = parent->firstChild;
	for(;sons->nextSibling != NULL; sons = sons->nextSibling);
	for(;i < count; ++i){
		present = va_arg(arg,TreeNode *);
		sons->nextSibling  = present;
		sons = sons->nextSibling;
	//	printf("%d insert node %d success!\n",parent->name,present->name);
	}
	va_end(arg);
	return;
}

char *getName(int name){
	switch(name){
		case INT:return "INT";
		case FLOAT:return "FLOAT";
		case ID:return "ID";
		case SEMI:return "SEMI";
		case COMMA:return "COMMA";
		case ASSIGNOP:return "ASSIGNOP";
		case RELOP:return "RELOP";
		case PLUS:return "PLUS";
		case MINUS:return "MINUS";
		case STAR:return "STAR";
		case DIV:return "DIV";
		case AND:return "AND";
		case OR:return "OR";
		case DOT:return "DOT";
		case NOT:return "NOT";
		case TYPE:return "TYPE";
		case LP:return "LP";
		case RP:return "RP";
		case LB:return "LB";
		case RB:return "RB";
		case LC:return "LC";
		case RC:return "RC";
		case STRUCT:return "STRUCT";
		case RETURN:return "RETURN";
		case IF:return "IF";
		case ELSE:return "ELSE";
		case WHILE:return "WHILE";
		
		case Program:return "Program";
		case ExtDefList:return "ExtDefList";
		case ExtDef:return "ExtDef";
		case ExtDecList:return "ExtDecList";
		case Specifier:return "Specifier";
		case StructSpecifier:return "StructSpecifier";
		case OptTag:return "OptTag";
		case Tag:return "Tag";
		case VarDec:return "VarDec";
		case FunDec:return "FunDec";
		case VarList:return "VarList";
		case ParamDec:return "ParamDec";
		case CompSt:return "CompSt";
		case StmtList:return "StmtList";
		case Stmt:return "Stmt";
		case DefList:return "DefList";
		case Def:return "Def";
		case DecList:return "DecList";
		case Dec:return "Dec";
		case Exp:return "Exp";
		case Args:return "Args";

		case Empty:return "Empty";
		default:return NULL;
	}
	return NULL;
}

void printTree(TreeNode *head,int tabCount){
	if(head == NULL || head->name == Empty)
		return;
	int i = 0;
	for( ; i<tabCount; ++i)
		printf("  ");
	if(head->name >=Program && head->name <= Args){
		printf("%s (%d)\n",getName(head->name),head->lineno);
	}
	else if(head->name != Empty){
		if(head->name == ID || head->name == TYPE || head->name == INT){
			printf("%s: %s\n",getName(head->name),head->data);
		}
		else if(head->name == FLOAT){
			printf("FLOAT: %f\n", atof(head->data));
		}
		else
			printf("%s\n",getName(head->name));
	}
	TreeNode *child = head->firstChild;
	for(;child != NULL;child = child->nextSibling)
		printTree(child,tabCount+1);
	return;
}
