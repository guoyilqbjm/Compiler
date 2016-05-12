#ifndef CODE_GR_H_
#define CODE_GR_H_
#include "codestructure.h"
#include "../tree.h"
#include "../lab2/table.h"

int varIndex = 0, labelIndex = 0;

char *new_temp_varname();

char *new_temp_labelname();

InterCodes* translate_Exp(TreeNode *root, char * varname);
#endif
