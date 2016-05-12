%{	
	#include "syntax.tab.h"
	#include "tree.h"
	#include <stdlib.h>
	int yycolumn = 1;
	int errorFlag = 0;
	#define YY_USER_ACTION \
		yylloc.first_line = yylloc.last_line = yylineno; \
		yylloc.first_column = yycolumn; \
		yylloc.last_column = yycolumn + yyleng - 1; \
		yycolumn += yyleng; 	
	#define PRINT_FLAG 0
%}

letter		[a-zA-Z]
digit		[0-9]
integers	(0)|([1-9]{digit}{0,31})
floats 		{digit}+\.{digit}+
Efloats 	({floats}|{integers})[Ee][+-]?{digit}+
id 		({letter}|_)({letter}|{digit}|_){0,31}
relop		>|<|(<=)|(>=)|(==)|(!=)
ws		[ \t]+

%%
int	{	if (PRINT_FLAG) printf("TYPE_INT\n");
		yylval = (int)treeNodeMalloc(yylineno,TYPE);
		strcpy(((TreeNode *)yylval)->data,"INT");
		return(TYPE); }
float {	if (PRINT_FLAG) printf("Type_FLOAT\n"); 
		yylval = (int)treeNodeMalloc(yylineno,TYPE);
		strcpy(((TreeNode *)yylval)->data,"float");
		return(TYPE); }
return { if (PRINT_FLAG) printf("RETURN \n");
		yylval = (int)treeNodeMalloc(yylineno,RETURN);
		return(RETURN); }
if {	if (PRINT_FLAG) printf("IF\n");
		yylval = (int)treeNodeMalloc(yylineno,IF);
		return(IF); }
else {	if (PRINT_FLAG) printf("ELSE\n");
		yylval = (int)treeNodeMalloc(yylineno,ELSE);
		return(ELSE); }
while {	if (PRINT_FLAG) printf("WHILE\n");
		yylval = (int)treeNodeMalloc(yylineno,WHILE);
		return(WHILE); }
struct { if (PRINT_FLAG) printf("STRUCT\n");
		yylval = (int)treeNodeMalloc(yylineno,STRUCT);
		return(STRUCT); }
{id} {	if (PRINT_FLAG) printf("ID %s\n", yytext);
		yylval = (int)treeNodeMalloc(yylineno,ID);
		strcpy(((TreeNode *)yylval)->data,yytext);
		return(ID); }
{integers} { if (PRINT_FLAG) printf("INT %s\n", yytext);
		yylval = (int)treeNodeMalloc(yylineno,INT);
		strcpy(((TreeNode *)yylval)->data,yytext);
		return(INT); }
{floats}|{Efloats} { if (PRINT_FLAG) printf("FLOAT %s\n", yytext);
		yylval = (int)treeNodeMalloc(yylineno,FLOAT);
		strcpy(((TreeNode *)yylval)->data,yytext);
		return(FLOAT); }
";" {		if (PRINT_FLAG) printf("SEMI\n");
		yylval = (int)treeNodeMalloc(yylineno,SEMI);
		return(SEMI); }
"," {		if (PRINT_FLAG) printf("COMMA ,\n");
		yylval = (int)treeNodeMalloc(yylineno,COMMA);
		return(COMMA); }
{relop}	{ if (PRINT_FLAG) printf("RELOP %s\n", yytext);
		yylval = (int)treeNodeMalloc(yylineno,RELOP);
		strcpy(((TreeNode *)yylval)->data,yytext);
		return(RELOP); }
"+" {	if (PRINT_FLAG) printf("PLUS +\n");
		yylval = (int)treeNodeMalloc(yylineno,PLUS);
		return(PLUS); }
"-" {	if (PRINT_FLAG) printf("MINUS -\n");
		yylval = (int)treeNodeMalloc(yylineno,MINUS);
		return(MINUS); }
"*" {	if (PRINT_FLAG) printf("STAR *\n");
		yylval = (int)treeNodeMalloc(yylineno,STAR);
		return(STAR); }
"/" {	if (PRINT_FLAG) printf("DIV /\n");
		yylval = (int)treeNodeMalloc(yylineno,DIV);
		return(DIV); }
"=" {	if (PRINT_FLAG) printf("ASSIGNOP =\n");
		yylval = (int)treeNodeMalloc(yylineno,ASSIGNOP);
		return (ASSIGNOP); }
"&&" {	if (PRINT_FLAG) printf("AND &&\n");
		yylval = (int)treeNodeMalloc(yylineno,AND);
		return(AND); }
"||" {	if (PRINT_FLAG) printf("OR ||\n");
		yylval = (int)treeNodeMalloc(yylineno,OR);
		return(OR); }
"."	{	if (PRINT_FLAG) printf("DOT .\n");
		yylval = (int)treeNodeMalloc(yylineno,DOT);
		return(DOT); }
"!" { if (PRINT_FLAG) printf("NOT !\n");
		yylval = (int)treeNodeMalloc(yylineno,NOT);
		return(NOT); }
"("	{	if (PRINT_FLAG) printf("LP (\n");
		yylval = (int)treeNodeMalloc(yylineno,LP);
		return(LP); }
")"	{	if (PRINT_FLAG) printf("RP )\n");
		yylval = (int)treeNodeMalloc(yylineno,RP);
		return(RP); }
"{"	{	if (PRINT_FLAG) printf("LC {\n");
		yylval = (int)treeNodeMalloc(yylineno,LC);
		return(LC); }
"}"	{	if (PRINT_FLAG) printf("RC }\n");
		yylval = (int)treeNodeMalloc(yylineno,RC);
		return(RC); }
"["	{	if (PRINT_FLAG) printf("LB [\n");
		yylval = (int)treeNodeMalloc(yylineno,LB);
		return(LB); }
"]" {	if (PRINT_FLAG) printf("RB ]\n");
		yylval = (int)treeNodeMalloc(yylineno,RB);
		return(RB); }
{ws} { /*No action. */}
[\r] {}
[\n] { yylineno ++; }
. {		errorFlag = 1;
		printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
%%
