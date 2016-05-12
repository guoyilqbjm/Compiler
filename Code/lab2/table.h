#ifndef TABLE_H_
#define TABLE_H_
#include "type.h"


struct TableNode{
	char name[32];
	TypePoint type;
	int def_state;
	struct TableNode* next;
};
typedef struct TableNode TableNode;

struct FuncDefTableNode {
	TableNode *tableNodePoint;
	int lineno;
	struct FuncDefTableNode *next;
};
typedef struct FuncDefTableNode FuncDefTableNode;

#define HashSize 0x3fff
TableNode** hashTable;
FuncDefTableNode *funcDefTable;

unsigned int hash(char *name);
void initTable();
TableNode *insertTableNode(char *name,TypePoint type);//Return the position in the HashTable. If there exists the variable, return NULL;
TableNode *findTableNode(char *name);

FuncDefTableNode *insertFuncDefTable(int lineno, TableNode *tnp);
#endif
