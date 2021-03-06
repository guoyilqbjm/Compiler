#include <stdio.h>
#include "syntax.tab.h"
#include "lex.yy.c"
#include "tree.h"
#include "lab2/type.h"
#include "lab2/table.h"
#include "lab2/common.h"
#define PRINTSYNTAXTREE 0 
#include "lab3/code_gr.h"

#include "lab4/msp.h"
int main(int argc,char **argv){

	if(argc <= 2) {
		printf("Usage: \"./parser testfilename.cmm outputfilename.s\".\n");
		return 1;
	}
	FILE *f = fopen(argv[1],"r");
	if(!f){
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();
	if (!errorFlag) {
	   if(PRINTSYNTAXTREE == 1)	
			printTree(root, 0);
		test();
		runCode("output.ir");
		char filename[] = "test.s";
		irtomsp(argv[2]);
	} 
	return 0;
}

void yyerror(char* msg) {
	fprintf(stderr, "Error type B at line %d: %s\n", yylineno, msg);
}
