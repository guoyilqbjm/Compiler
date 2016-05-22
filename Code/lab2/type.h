#ifndef TYPE_H_
#define TYPE_H_

struct FieldList{
	char *name;//field name
	struct Type* type;//field type
	int lineno;
	struct FieldList* tail;//next field
};
typedef struct FieldList FieldList;

struct Type{
	enum {BASIC, ARRAY,STRUCTURE,FUNCTION} kind;
	union{
		int basic;		//base type
		struct {struct Type* elem; int size;int offset;} array; //array type is consisted of the element type and the array size.
		struct FieldList* structure;		//strcture and function type is a link.
	} data;
};
typedef struct Type Type;

typedef struct Type* TypePoint;
typedef struct FieldList* FieldListPoint;

#endif
