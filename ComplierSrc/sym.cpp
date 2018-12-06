#include"sym.h"
//<macros>
//<data>
	struct table
		cns={SCOPE_CONSTANTS},
		ext={SCOPE_GLOBAL},
		ids={SCOPE_GLOBAL},
		tys={SCOPE_GLOBAL};
	Table constants   =&cns;
	Table externals   =&ext;
	Table globals     =&ids;
	Table identifiers =&ids;
	Table types       =&tys;
	Table labels;
	int level=SCOPE_GLOBAL;
//<functions>

//动态创建表,并与相应外层的表进行连接
Table table(Table tp,int level){
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
	struct table::entry* p;
	unsigned h=(unsigned)name&(HASHSIZE-1);
	if(level>0&&tp->level<level)
		tp=table(tp,level);
	p=new struct table::entry;
	p->sym.name=name;
	p->sym.scope=level;
	p->sym.up=tp->all;
	tp->all=&p->sym;
	p->next=tp->buckets[h];
	tp->buckets[h]=p;
	return &p->sym;
}
Symbol lookup(char* name,Table tp){
	return NULL;
}