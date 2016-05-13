#ifndef CODE_GR_H_
#define CODE_GR_H_
#include "codestructure.h"
#include "../tree.h"
#include "../lab2/table.h"
#include "../syntax.tab.h"
int new_temp_varname();

int new_temp_labelname();

OperandPoint new_constant_operand(int value);

InterCodes* translate_Exp(TreeNode *root, OperandPoint op);

#endif
