#include"types.h"
//<macros>
#define typetablesize 128
//<data>
	//用hash技术可以降低复杂度
	struct entry{
		type type;
		struct entry* next;
	}* typetable[typetablesize];

	struct type* chartype;
	struct type* doubletype;
	struct type* floattype;
	struct type* inttype;
	struct type* longdouble;
	struct type* longtype;
	struct type* shorttype;
	struct type* signedchar;
	struct type* unsignedchar;
	struct type* unsignedlong;
	struct type* unsignedshort;
	struct type* unsignedtype;
	struct type* voidptype;
	struct type* voidtype;

//<functions>

	//利用类型操作符和操作数地址的异或值作为哈希值
type::type(char* n,int o,int sz){
	strcat(name,n);
	op=o;
	size=sz;
	kid=next=NULL;
}
type::type(){
	name=NULL;
	op=size=0;
	kid=next=NULL;
}
//创建新类型时,type初始化参数指定的域
//将类型加入相应的哈希链,并返回新类型
static struct type* newType(char* name,int op,int size){
	//利用类型操作符和操作数地址的异或值作为哈希值
	unsigned h=(op^((unsigned)name>>3))&(typetablesize-1);
	struct entry* tn;
	for(tn=typetable[h];tn;tn=tn->next){
		if(strcmp(tn->type.name,name)==0
			&&tn->type.op==op)
		return &tn->type;
	}
	tn=new entry;
	tn->type.op=op;
	tn->type.size=size;
	tn->next=typetable[h];
	typetable[h]=tn;
	return &tn->type;
}
void typeInit(){

}