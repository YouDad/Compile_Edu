#include"sym.h"
//<macros>
//<data>
	struct table
		cns={SCOPE_CONSTANTS},
		ids={SCOPE_GLOBAL};
	Table constants   =&cns;
	Table identifiers =&ids;
	int level=SCOPE_GLOBAL;
//<functions>

//动态创建表,并与相应外层的表进行连接
Table table(Table tp,int lev=level){
	Table newTable=new struct table;
	newTable->previous=tp;
	newTable->level=level;
	if(tp)
		newTable->all=tp->all;
	return newTable;
}

//install函数为给定的name分配一个符号
//并把该符号加到对应给定作用域层数相对应的表中
//如果有需要,还将建立一个新表
//该函数将返回一个指向符号的指针
Symbol install(char* name,Table&tp,int level){
	return NULL;
}
Symbol lookup(const char* name,Table tp){
	return NULL;
}