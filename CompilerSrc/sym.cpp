#include"sym.h"
//<macros>
//<data>
	struct symbolTable
		cns={SCOPE_CONST},
		stt={SCOPE_STATIC},
		idt={SCOPE_GLOBAL};
	SymbolTable constTable=&cns;
	SymbolTable staticTable=&stt;
	SymbolTable identifierTable=&idt;
	//levelָ����ǰ������
	int level=SCOPE_GLOBAL;
	std::hash_map<String,struct label>lbt;
	int labelid=1;
	std::stack<Symbol> sem;

//<functions>

//���ű��ʼ������
void symInit(){
	identifierTable->previous=staticTable;
	staticTable->previous=constTable;
}

//install����Ϊ������name����һ������
//���Ѹ÷��żӵ���Ӧ����������������Ӧ�ı���
//�ú���������һ��ָ����ŵ�ָ��
Symbol install(String&name,int level){
	struct symbol sym;
	sym.name=name;
	sym.scope=level;
	SymbolTable t=identifierTable;
	while(t->level>level)
		t=t->previous;
	t->m[name]=sym;
	return &t->m[name];
}

//�¶���һ�����㳣���ķ���
//���������żӵ���������
Symbol newFloatConst(Symbol s,Coordinate c){
	static char str[30];
	if(s->type->op==TYPE_FLOAT)
		sprintf(str,"$float%u",s->u.c.v.u);
	else
		sprintf(str,"$double%ull",s->u.c.v.uu);
	String name=str;
	Symbol sym=install(name,SCOPE_CONST);
	sym->addressed=1;
	sym->sclass=SCLASS_CONST;
	sym->src=c;
	sym->temporary=0;
	sym->type=s->type;
	sym->u=s->u;
	return sym;
}

//�¶���һ���ַ��������ķ���
//���������żӵ���������
Symbol newStringConst(String&str,Coordinate c){
	Symbol sym=install(str,SCOPE_CONST);
	sym->addressed=1;
	sym->sclass=SCLASS_CONST;
	sym->src=c;
	sym->temporary=0;
	sym->type=newConst(ptr(newConst(btot(TYPE_CHAR))));
	return sym;
}

//�¶���һ��ö�ٳ����ı�ʶ��
//name�����ö�ٳ���������
//val�����ö�ٳ����ĳ���ֵ
//t�����ö�ٳ���������
//c��������ų��ֵ�λ��
Symbol newEnumConst(String&name,int val,struct type* t,Coordinate c){
	Symbol sym=install(name,level);
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
	for(SymbolTable t=identifierTable;t;t=t->previous){
		auto it=t->m.find(name);
		if(it!=t->m.end())
			return &it->second;
	}return NULL;
}

//�ҵ����ֽ�name�ĳ�����ʶ��
//û�ҵ��ͷ���NULL
Symbol findConstSymbol(String&name){
	auto it=constTable->m.find(name);
	if(it!=constTable->m.end())
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
	SymbolTable newTable=new struct symbolTable();
	newTable->level=++level;
	newTable->previous=identifierTable;
	identifierTable=newTable;
}

//���߷��ű�,�˳���һ��������
//���ű��Ѳ���ʱ�˵ķ���ɾ��
void exitScope(){
	--level;
	SymbolTable oldTable=identifierTable;
	identifierTable=identifierTable->previous;
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