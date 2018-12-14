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
	//level指明当前作用域
	int level=SCOPE_GLOBAL;
	std::hash_map<String,struct label>lbt;
	int labelid=1;
	std::stack<Symbol> sem;

//<functions>

//符号表初始化函数
void symInit(){
	identifierTable->previous=staticTable;
	staticTable->previous=constTable;
}

//install函数为给定的name分配一个符号
//并把该符号加到对应给定作用域层数相对应的表中
//该函数将返回一个指向符号的指针
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

//新定义一个浮点常量的符号
//会把这个符号加到常量表中
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

//新定义一个字符串常量的符号
//会把这个符号加到常量表中
Symbol newStringConst(String&str,Coordinate c){
	Symbol sym=install(str,SCOPE_CONST);
	sym->addressed=1;
	sym->sclass=SCLASS_CONST;
	sym->src=c;
	sym->temporary=0;
	sym->type=newConst(ptr(newConst(btot(TYPE_CHAR))));
	return sym;
}

//新定义一个枚举常量的标识符
//name是这个枚举常量的名字
//val是这个枚举常量的常量值
//t是这个枚举常量的类型
//c是这个符号出现的位置
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

//找到名字叫name的标识符
//没找到就返回NULL
Symbol findSymbol(String&name){
	for(SymbolTable t=identifierTable;t;t=t->previous){
		auto it=t->m.find(name);
		if(it!=t->m.end())
			return &it->second;
	}return NULL;
}

//找到名字叫name的常量标识符
//没找到就返回NULL
Symbol findConstSymbol(String&name){
	auto it=constTable->m.find(name);
	if(it!=constTable->m.end())
		return &it->second;
	else return NULL;
}

//新定义一个标识符
//name是这个标识符的名字
//t是这个标识符的类型
//s是这个标识符的存储类型
//c是这个符号出现的位置
Symbol newSymbol(String&name,struct type* t,enum SCLASS s,Coordinate c){
	Symbol sym=install(name,level);
	sym->sclass=s;
	sym->src=c;
	sym->temporary=0;
	sym->type=t;
	return sym;
}

//告诉符号表,进入了一个作用域
void enterScope(){
	SymbolTable newTable=new struct symbolTable();
	newTable->level=++level;
	newTable->previous=identifierTable;
	identifierTable=newTable;
}

//告诉符号表,退出了一个作用域
//符号表会把不合时宜的符号删除
void exitScope(){
	--level;
	SymbolTable oldTable=identifierTable;
	identifierTable=identifierTable->previous;
	delete oldTable;
}

//下一个临时变量id
int tempid=1;
//向符号表申请一个临时变量
Symbol newTemp(){
	static char str[11];
	sprintf(str,"$%d",tempid++);
	String name=str;
	Symbol sym=install(name,level);
	sym->temporary=1;
	return sym;
}

//查询标签表是否有名叫name的标签
//如果有,返回id>0,如果没有返回0
int findLabel(String&name){
	auto it=lbt.find(name);
	if(it==lbt.end())
		return 0;
	else
		return it->second.id;
}

//添加一个标签,并返回一个id
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

//给后端使用,给一个id,返回一个标签结构
struct label getLabel(int id){
	for(auto it=lbt.begin();it!=lbt.end();it++)
		if(it->second.id==id)
			return it->second;
	assert(0);
	return label();
}