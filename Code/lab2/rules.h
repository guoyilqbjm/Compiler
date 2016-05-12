#ifndef RULES_H_
#define RULES_H_

#include "table.h"
#include "../syntax.tab.h"
#include "../tree.h"

/*-----ExtDef->Specifier ExtDecList SEMI | Specifier SEMI | Specifier FunDec CompSt;-----*/
void getExtDef(TreeNode *head);//insert all fundec or extdeclist into symbol table.

/*-----ExtDecList->VarDec | VarDec COMMA ExtDecList;-----*/
void getExtDecList(TypePoint lastType,TreeNode *head);


/*-----Specifier->TYPE | StructSpecifier;-----*/
TypePoint getSpecifier(TreeNode *head);

/*-----VarDec->Id | VarDec LB INT RB;-----*/
//When inserting the id success, return the position in the hashTable, otherwise, return NULL.The id type is lastType.
FieldListPoint getVarDec(TypePoint lastType,TreeNode *head);

/*-----FunDec->ID LP VarList RP | ID LP RP;-----*/
TypePoint getFunDec(TypePoint lastType,TreeNode *head);

/*-----Def->Specifier DecList SEMI;-----*/
//Return the variable list. If a variable is redefined,it doesn't belong to the FieldListPoint.
FieldListPoint getDef(TreeNode *head);

/*-----CompSt-----*/
void getCompSt(TypePoint returnType,TreeNode *head);

/*-----Stmt-----*/
void getStmt(TypePoint returnType,TreeNode *head);

//analysis the tree which root is head;
void syntaxTest(TreeNode *head);

/*-----return the Exp's Type-----*/
TypePoint getExp(TreeNode *head);

int testType(TypePoint type1,TypePoint type2);
#endif
