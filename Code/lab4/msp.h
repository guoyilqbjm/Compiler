#ifndef MSP_H_
#define MSP_H_
#include "../lab3/codestructure.h"
char *reg(FILE *fp, OperandPoint operand, int load);

extern void irtomsp(char *filename);

#endif
