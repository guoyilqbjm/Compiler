#ifndef CODE_H_
#define CODE_H_
#include <stdio.h>
#include <stdlib.h>
struct Operand{
	enum {TEMP, VARIABLE, CONSTANT, ADDRESS, REFERENCE, STRUCTADDRESS} kind;
	union{
		int temp_no;//address 存储生成的临时变量的编号，调用int new_temp_varname();可生成临时变量序号，调用char *get_temp_varname(int i);可获取临时便利的名字
		char *var_name;//程序中定义的变量名，可以直接使var_name指向类型为ID的树节点的data即可。
		char *value;//常量值
		char *refer_name;
		/* 关于地址 或者引用的存储未考虑 你自己设计一下了*/
	} data;
	int offset;//实验四 用于计算变量在栈中相对于栈顶的偏移量
};
typedef struct Operand Operand;
typedef struct Operand* OperandPoint;

struct InterCode{
//	enum { LABEL, FUNCTION, ASSIGN, ADD, SUB, MUL, DIV, GOTO, DEC, ARG, CALL, PARAM, READ, WRITE} kind;
	enum { PARAM, FUNCTIONLABEL, LABEL, GOTO, RETURNFUNCTION, ARG, ONEOP, BINOP, CALL, DEC, IFSTMT, READ, WRITE} kind;//CALL 存储在funcall，DEC存储在decstmt IFSTMT存储在ifstmt;
	union{
		char *symbol_name;
		OperandPoint operand_point;
		OperandPoint return_value;//存储return返回值
		OperandPoint arg_value;//
		struct{ OperandPoint left; char *fun_name;} funcall;
		struct{ OperandPoint left; int size;} decstmt;
		struct{ OperandPoint left, right;} oneop;
		struct{ OperandPoint result, op1, op2; int opkind;} binop;
		struct{ OperandPoint left, right; char relop[3]; char *label;} ifstmt;//if left relop right goto label;
	} data;
};
typedef struct InterCode InterCode;

struct InterCodes{
	struct InterCode code;
	struct InterCodes* last;
	struct InterCodes* next;
};
typedef struct InterCodes InterCodes;

extern struct InterCodes* code_root;
extern struct InterCodes* code_tail;//未使用

struct ArgListNode{
	OperandPoint operand_point;
	struct ArgListNode *next;
};
typedef struct ArgListNode ArgListNode;

OperandPoint mallocOperand(int kind,...);
InterCodes* mallocInterCodes();

InterCodes* mergeInterCodes(InterCodes* a,InterCodes *b);//将中间代码a和b连接起来并且返回生成的新的代码的头节点指针

void printInterCodes(char *filename);//输出中间代码到文件filename中

#endif
