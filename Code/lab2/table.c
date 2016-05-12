#include "table.h"
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
