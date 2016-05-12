#include "table.h"
#include "../syntax.tab.h" //lab3 added!
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
unsigned int hash(char *name){
	unsigned int val =0,i;
	for(;*name;++name){
		val=(val<<2)+*name;
		if(i=val&~HashSize)
			val=(val^(i>>12))&HashSize;
	}
	return val;
}

void initTable(){
	hashTable = (TableNode**)malloc(sizeof(TableNode*)*HashSize);
	memset(hashTable,0,sizeof(TableNode**)*HashSize);
	funcDefTable = NULL;

	/* 为实验三添加 int read();函数和int write(int);函数 */
	//插入int read()函数
	TypePoint p1 = (TypePoint)malloc(sizeof(Type));
	p1->kind = FUNCTION;
	FieldListPoint q1 = (FieldListPoint)malloc(sizeof(FieldList));
	q1->name = "read";
	q1->type = (TypePoint)malloc(sizeof(Type));
	q1->type->kind = BASIC;
	q1->type->data.basic = INT;
	q1->lineno = -1;
	q1->tail = NULL;
	p1->data.structure = q1;
	insertTableNode("read",p1);

	//插入int write(int)函数
	TypePoint p2=(TypePoint)malloc(sizeof(Type));
	p2->kind = FUNCTION;
	FieldListPoint q2 = (FieldListPoint)malloc(sizeof(FieldList));
	q2->name = "write";
	q2->type = (TypePoint)malloc(sizeof(Type));
	q2->type->kind = BASIC;
	q2->type->data.basic = INT;
	q2->lineno = -1;
	FieldListPoint q3 = (FieldListPoint)malloc(sizeof(FieldList));
	q3->name = NULL;
	q3->type = (TypePoint)malloc(sizeof(Type));
	q3->type->kind = BASIC;
	q3->type->data.basic = INT;
	q3->lineno = -1;
	q3->tail = NULL;
	q2->tail = q3;
	p2->data.structure = q2;
	insertTableNode("write",p2);
}

FuncDefTableNode *insertFuncDefTable(int lineno, TableNode *tnp) {
	FuncDefTableNode *p = (FuncDefTableNode *)malloc(sizeof(FuncDefTableNode));
	p->tableNodePoint = tnp;
	p->lineno = lineno;
	p->next = funcDefTable;
	funcDefTable = p;
	return funcDefTable;
}

TableNode* insertTableNode(char *name, TypePoint type){
	TableNode *newNode = (TableNode *)malloc(sizeof(TableNode));
	strcpy(newNode->name,name);
	newNode->type = type;
	newNode->def_state = 0;
	unsigned int index = hash(name);
	TableNode *head = hashTable[index];
	for(; head != NULL; head=head->next)
		if(strcmp(head->name,newNode->name)==0)
			break;
	if(head != NULL)
		return NULL;
	newNode->next=hashTable[index];
	hashTable[index]=newNode;
	return hashTable[index];
}

TableNode *findTableNode(char *name){
	unsigned int index = hash(name);
	TableNode *result = hashTable[index];
	for(;result != NULL; result=result->next){
		if(strcmp(result->name,name)==0)
			break;
	}
	return result;
}
