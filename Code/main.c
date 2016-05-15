#include <stdio.h>
#include "syntax.tab.h"
#include "lex.yy.c"
#include "tree.h"
#include "lab2/type.h"
#include "lab2/table.h"
#include "lab2/common.h"

#include "lab3/code_gr.h"
int main(int argc,char **argv){

	if(argc <= 2) {
		printf("Usage: \"./parser test1.cmm output.ir\".\n");
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
		printTree(root, 0);
		test();

		runCode(argv[2]);
	}
	return 0;
}

void yyerror(char* msg) {
	fprintf(stderr, "Error type B at line %d: %s\n", yylineno, msg);
}
