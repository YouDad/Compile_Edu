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

//��̬������,������Ӧ���ı��������
Table table(Table tp,int lev=level){
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
	return NULL;
}
Symbol lookup(const char* name,Table tp){
	return NULL;
}