%{	#include "tree.h"
	#define YYSTYPE TreeNode* 
	TreeNode *root = NULL;
	extern int errorFlag;
%}
/* declared types */
/* declared tokens*/
%error-verbose

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%token INT
%token FLOAT
%token TYPE ID
%token LC RC
%nonassoc SEMI COMMA STRUCT RETURN IF WHILE

/* declared non-terminals */
%%
/* High-level Definitions */
Program: ExtDefList { 
	$$ = treeNodeMalloc(@$.first_line,Program);
	insertNode($$,1,$1);
	root = $$;
	}
	;
ExtDefList: ExtDef ExtDefList { 
	$$ = treeNodeMalloc(@$.first_line,ExtDefList);
	insertNode($$,2,$1,$2);
	}
 | /* empty */ {
	$$  = treeNodeMalloc(@$.first_line,Empty);
	}
 ;
ExtDef: Specifier ExtDecList SEMI {
	$$ = treeNodeMalloc(@$.first_line,ExtDef);
	insertNode($$,3,$1,$2,$3);
	}
 | Specifier SEMI {
	$$ = treeNodeMalloc(@$.first_line,ExtDef);
	insertNode($$,2,$1,$2);
	}
 | Specifier FunDec CompSt {
	$$ = treeNodeMalloc(@$.first_line,ExtDef);
	insertNode($$,3,$1,$2,$3);
	}
 | Specifier FunDec SEMI {
 	$$ = treeNodeMalloc(@$.first_line,ExtDef);
 	insertNode($$,3,$1,$2,$3);
 	}
 | error SEMI {
	errorFlag = 1;
	//yyerrok;
	}
 ;

ExtDecList: VarDec {
	$$ = treeNodeMalloc(@$.first_line,ExtDecList);
	insertNode($$,1,$1);
	}
 | VarDec COMMA ExtDecList {
	$$ = treeNodeMalloc(@$.first_line,ExtDecList);
	insertNode($$,3,$1,$2,$3);
	}
 ;

/* Specifiers */
Specifier: TYPE {
	$$ = treeNodeMalloc(@$.first_line,Specifier);
	insertNode($$,1,$1);
	}
 | StructSpecifier {
	$$ = treeNodeMalloc(@$.first_line,Specifier);
	insertNode($$,1,$1);
	}
 ;
StructSpecifier: STRUCT OptTag LC DefList RC {
	$$ = treeNodeMalloc(@$.first_line,StructSpecifier);
	insertNode($$,5,$1,$2,$3,$4,$5);
	}
 | STRUCT Tag {
	$$ = treeNodeMalloc(@$.first_line,StructSpecifier);
	insertNode($$,2,$1,$2);
	}
 ;
OptTag: ID {
	$$ = treeNodeMalloc(@$.first_line,OptTag);
	insertNode($$,1,$1);
	}
 | /* empty */ {
	$$ = treeNodeMalloc(@$.first_line,Empty);
	}
 ;
Tag: ID {
	$$ = treeNodeMalloc(@$.first_line,Tag);
	insertNode($$,1,$1);
	}
 ;

/* Declarators */
VarDec: ID {
	$$ = treeNodeMalloc(@$.first_line,VarDec);
	insertNode($$,1,$1);
	}
 | VarDec LB INT RB {
	$$ = treeNodeMalloc(@$.first_line,VarDec);
	insertNode($$,4,$1,$2,$3,$4);
	}
 ;
FunDec: ID LP VarList RP {
	$$ = treeNodeMalloc(@$.first_line,FunDec);
	insertNode($$,4,$1,$2,$3,$4);
	}
 | ID LP RP {
	$$ = treeNodeMalloc(@$.first_line,FunDec);
	insertNode($$,3,$1,$2,$3);
	}
 | error RP{
	errorFlag = 1;
	//yyerrok;
	}
 ;
VarList:ParamDec COMMA VarList {
	$$ = treeNodeMalloc(@$.first_line,VarList);
	insertNode($$,3,$1,$2,$3);
	}
 | ParamDec {
	$$ = treeNodeMalloc(@$.first_line,VarList);
	insertNode($$,1,$1);
	}
 ;
ParamDec: Specifier VarDec {
	$$ = treeNodeMalloc(@$.first_line,ParamDec);
	insertNode($$,2,$1,$2);
	}
 | error COMMA {
	errorFlag = 1;
	}
 | error RB {
	errorFlag = 1;
	}
 ;

/* Statements */
CompSt: LC DefList StmtList RC {
	$$ = treeNodeMalloc(@$.first_line,CompSt);
	insertNode($$,4,$1,$2,$3,$4);
	}
 | error RC {
	errorFlag = 1;
	//yyerrok;
	}
 ;
StmtList: Stmt StmtList {
	$$ = treeNodeMalloc(@$.first_line,StmtList);
	insertNode($$,2,$1,$2);
	}
 | /* empty */ {
	$$ = treeNodeMalloc(@$.first_line,Empty);
	}
 ;
Stmt: Exp SEMI {
	$$ = treeNodeMalloc(@$.first_line,Stmt);
	insertNode($$,2,$1,$2);
	}
 | CompSt {
	$$ = treeNodeMalloc(@$.first_line,Stmt);
	insertNode($$,1,$1);
	}
 | RETURN Exp SEMI {
	$$ = treeNodeMalloc(@$.first_line,Stmt);
	insertNode($$,3,$1,$2,$3);
	}
 | IF LP Exp RP Stmt  %prec LOWER_THAN_ELSE {
	$$ = treeNodeMalloc(@$.first_line,Stmt);
	insertNode($$,5,$1,$2,$3,$4,$5);
	}
 | IF LP Exp RP Stmt ELSE Stmt {
	$$ = treeNodeMalloc(@$.first_line,Stmt);
	insertNode($$,7,$1,$2,$3,$4,$5,$6,$7);
	}
 | WHILE LP Exp RP Stmt {
	$$ = treeNodeMalloc(@$.first_line,Stmt);
	insertNode($$,5,$1,$2,$3,$4,$5);
	}
 | error SEMI  {
	errorFlag = 1;
	//yyerrok;
	}
 ;

/* Local Definitions */
DefList: Def DefList {
	$$ = treeNodeMalloc(@$.first_line,DefList);
	insertNode($$,2,$1,$2);
	}
 | /* empty */ {
	$$ = treeNodeMalloc(@$.first_line,Empty);
	}
 ;
Def: Specifier DecList SEMI {
	$$ = treeNodeMalloc(@$.first_line,Def);
	insertNode($$,3,$1,$2,$3);
	}
 | error SEMI {
	errorFlag = 1;
	//yyerrok;
	}
 ;
DecList: Dec {
	$$ = treeNodeMalloc(@$.first_line,DecList);
	insertNode($$,1,$1);
	}
 | Dec COMMA DecList {
	$$ = treeNodeMalloc(@$.first_line,DecList);
	insertNode($$,3,$1,$2,$3);
	}
 ;
Dec: VarDec {
	$$ = treeNodeMalloc(@$.first_line,Dec);
	insertNode($$,1,$1);
	}
 | VarDec ASSIGNOP Exp {
	$$ = treeNodeMalloc(@$.first_line,Dec);
	insertNode($$,3,$1,$2,$3);
	}
;

/* Expressions */
Exp: Exp ASSIGNOP Exp {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,3,$1,$2,$3);
	}
 | Exp AND Exp {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,3,$1,$2,$3);
	}
 | Exp OR Exp {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,3,$1,$2,$3);
	}
 | Exp RELOP Exp {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,3,$1,$2,$3);
	}
 | Exp PLUS Exp {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,3,$1,$2,$3);
	}
 | Exp MINUS Exp {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,3,$1,$2,$3);
	}
 | Exp STAR Exp {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,3,$1,$2,$3);
	}
 | Exp DIV Exp {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,3,$1,$2,$3);
	}
 | LP Exp RP {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,3,$1,$2,$3);
	}
 | MINUS Exp {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,2,$1,$2);
	}
 | NOT Exp {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,2,$1,$2);
	}
 | ID LP Args RP {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,4,$1,$2,$3,$4);
	}
 | ID LP RP {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,3,$1,$2,$3);
	}
 | Exp LB Exp RB {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,4,$1,$2,$3,$4);
	}
 | Exp DOT ID {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,3,$1,$2,$3);
	}
 | ID {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,1,$1);
	}
 | INT {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,1,$1);
	}
 | FLOAT {
	$$ = treeNodeMalloc(@$.first_line,Exp);
	insertNode($$,1,$1);
	}
/* | LP Exp error {
	errorFlag = 1;
	//yyerror("Missing )");
	yyerrok;
	}
 | ID LP Args error {
	errorFlag = 1;
	//yyerror("Missing )");
	yyerrok;
	}
 | ID LP error {
	errorFlag = 1;
	//yyerror("Missing )");
	yyerrok;
	}
 | Exp LB Exp error {
	errorFlag = 1;
	//yyerror("Missing ]");
	yyerrok;
	}*/
 ;
Args: Exp COMMA Args {
	$$ = treeNodeMalloc(@$.first_line,Args);
	insertNode($$,3,$1,$2,$3);
	}
 | Exp {
	$$ = treeNodeMalloc(@$.first_line,Args);
	insertNode($$,1,$1);
	}
 ;

%%
