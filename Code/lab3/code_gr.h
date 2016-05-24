#ifndef CODE_GR_H_
#define CODE_GR_H_
#include "codestructure.h"
#include "../tree.h"
#include "../lab2/table.h"
#include "../syntax.tab.h"

int allocate_varname();//生成临时变量名序号
char *allocate_labelname();//生成临时标号名序号
char *get_temp_varname(int i);//获取临时变量名


InterCodes* translate_Exp(TreeNode *root, OperandPoint place);//To-finish
InterCodes* translate_Args(TreeNode *root, ArgListNode *arg_list);//To-do
InterCodes* translate_DefList(TreeNode *root);
InterCodes* translate_FunDec(TreeNode *root);
InterCodes* translate_CompSt(TreeNode *root);
InterCodes* translate_Program(TreeNode *root);
InterCodes* translate_StmtList(TreeNode *root);//To-do
InterCodes* translate_Stmt(TreeNode *root);
InterCodes* translate_Cond(TreeNode *exp, InterCodes *label_true, InterCodes *label_false);
void runCode(char *filename);
#endif
