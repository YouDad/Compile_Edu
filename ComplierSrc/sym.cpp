#include"sym.h"
//<macros>
//<data>
	struct table
		cns={SCOPE_CONSTANTS},
		ids={SCOPE_GLOBAL};
	Table constants=&cns;
	Table idt=&ids;
	//levelָ����ǰ������
	int level=SCOPE_GLOBAL;
	std::hash_map<String,struct label>lbt;
	int labelid=1;
	std::stack<Symbol> sem;

//<functions>

//���ű��ʼ������
void symInit(){
	idt->previous=constants;
}

//install����Ϊ������name����һ������
//���Ѹ÷��żӵ���Ӧ����������������Ӧ�ı���
//�ú���������һ��ָ����ŵ�ָ��
Symbol install(String&name,int level){
	struct symbol sym;
	sym.name=name;
	sym.scope=level;
	Table t=idt;
	while(t->level>level)
		t=t->previous;
	sym.offset=t->size;
	t->m[name]=sym;
	return &t->m[name];
}

//�¶���һ��ö�ٳ����ı�ʶ��
//name�����ö�ٳ���������
//val�����ö�ٳ����ĳ���ֵ
//t�����ö�ٳ���������
//c��������ų��ֵ�λ��
Symbol newEnumConst(String&name,int val,struct type* t,Coordinate c){
	Symbol sym=install(name,SCOPE_CONSTANTS);
	sym->addressed=0;
	sym->sclass=SCLASS_CONST;
	sym->src=c;
	sym->temporary=0;
	sym->type=t;
	sym->u.ev=val;
	return sym;
}

//�ҵ����ֽ�name�ı�ʶ��
//û�ҵ��ͷ���NULL
Symbol findSymbol(String&name){
	for(Table t=idt;t;t=t->previous){
		auto it=t->m.find(name);
		if(it!=t->m.end())
			return &it->second;
	}return NULL;
}

//�ҵ����ֽ�name�ĳ�����ʶ��
//û�ҵ��ͷ���NULL
Symbol findConstSymbol(String&name){
	auto it=constants->m.find(name);
	if(it!=constants->m.end())
		return &it->second;
	else return NULL;
}

//�¶���һ����ʶ��
//name�������ʶ��������
//t�������ʶ��������
//s�������ʶ���Ĵ洢����
//c��������ų��ֵ�λ��
Symbol newSymbol(String&name,struct type* t,enum SCLASS s,Coordinate c){
	Symbol sym=install(name,level);
	sym->sclass=s;
	sym->src=c;
	sym->temporary=0;
	sym->type=t;
	return sym;
}

//���߷��ű�,������һ��������
void enterScope(){
	Table newTable=new struct table();
	newTable->level=++level;
	newTable->previous=idt;
	idt=newTable;
}

//���߷��ű�,�˳���һ��������
//���ű��Ѳ���ʱ�˵ķ���ɾ��
void exitScope(){
	--level;
	Table oldTable=idt;
	idt=idt->previous;
	delete oldTable;
}

//��һ����ʱ����id
int tempid=1;
//����ű�����һ����ʱ����
Symbol newTemp(){
	static char str[11];
	sprintf(str,"$%d",tempid++);
	String name=str;
	Symbol sym=install(name,level);
	sym->temporary=1;
	return sym;
}

//��ѯ��ǩ���Ƿ�������name�ı�ǩ
//�����,����id>0,���û�з���0
int findLabel(String&name){
	auto it=lbt.find(name);
	if(it==lbt.end())
		return 0;
	else
		return it->second.id;
}

//���һ����ǩ,������һ��id
int newLabel(String&name){
	if(findLabel(name)==0){
		struct label tmp;
		tmp.defined=0;
		tmp.id=labelid++;
		tmp.name=name;
		tmp.src=src;
		lbt[name]=tmp;
	}
	return findLabel(name);
}

//�����ʹ��,��һ��id,����һ����ǩ�ṹ
struct label getLabel(int id){
	for(auto it=lbt.begin();it!=lbt.end();it++)
		if(it->second.id==id)
			return it->second;
	assert(0);
	return label();
}