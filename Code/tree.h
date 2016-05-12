#ifndef TREE_H_
#define TREE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
enum {
	Empty = -1,
	Program,
	ExtDefList,
	ExtDef,
	ExtDecList,
	Specifier,
	StructSpecifier,
	OptTag,
	Tag,
	VarDec,
	FunDec,
	VarList,
	ParamDec,
	CompSt,
	StmtList,
	Stmt,
	DefList,
	Def,
	DecList,
	Dec,
	Exp,
	Args
};

struct TreeNode{
	int lineno;
	int name;
	char data[32];
	int visited;
	struct TreeNode *firstChild;// point to its firstchild node
	struct TreeNode *nextSibling;// point to its brother node
};
typedef struct TreeNode TreeNode;

extern TreeNode *root;

extern TreeNode * treeNodeMalloc(int lineno, int name);

extern void insertNode(TreeNode *parent, int count, ...);

extern void printTree(TreeNode *head, int tabCount);

extern char* getName(int name);
#endif
