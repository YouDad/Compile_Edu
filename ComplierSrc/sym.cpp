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

//��̬������,������Ӧ���ı��������
Table table(Table tp,int level){
	Table newTable=new struct table;
	newTable->previous=tp;
	newTable->level=level;
	if(tp)
		newTable->all=tp->all;
	return newTable;
}

//install����Ϊ������name����һ������
//���Ѹ÷��żӵ���Ӧ����������������Ӧ�ı���
//�������Ҫ,��������һ���±�
//�ú���������һ��ָ����ŵ�ָ��
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