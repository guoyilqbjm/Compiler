#ifndef CODE_GR_H_
#define CODE_GR_H_
#include "codestructure.h"
#include "../tree.h"
#include "../lab2/table.h"
#include "../syntax.tab.h"

int new_temp_varname();//生成临时变量名序号
int new_temp_labelname();//生成临时标号名序号

char *get_temp_varname(int i);//获取临时变量名
char *get_temp_labelname(int i);//获取临时标号名


InterCodes* translate_Exp(TreeNode *root, OperandPoint place);//To-finish
InterCodes* translate_Args(TreeNode *root, OperandPoint place);//To-do
InterCodes* translate_DefList(TreeNode *root);
InterCodes* translate_FunDec(TreeNode *root);
InterCodes* translate_CompSt(TreeNode *root);
InterCodes* translate_Program(TreeNode *root);
InterCodes* translate_StmtList(TreeNode *root);//To-do
void runCode(char *filename);
#endif
