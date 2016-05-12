#include "common.h"

void syntaxTest(TreeNode *root){
	if(root == NULL || root->name ==Empty)
		return;
	if(root->name == ExtDef){
		getExtDef(root);
		return;
	}
	if(root->name == Def){
		getDef(root);
		return;
	}
	if(root->name == Exp){
		getExp(root);
		return;
	}
	TreeNode *child = root->firstChild;
	for(;child != NULL; child = child->nextSibling)
		syntaxTest(child);
}

void test(){
	initTable();
	
	syntaxTest(root);

	FuncDefTableNode *p = funcDefTable;
	while (p != NULL) {
		if (p->tableNodePoint->def_state == -1) {
			printf("Error type 18 at Line %d: Undefined but declared function.\n", p->lineno);
		}
		p = p->next;
	}

}
