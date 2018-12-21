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
	int labelid=1;

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
	sym.needebx=0;
	SymbolTable t=identifierTable;
	while(t->level>level)
		t=t->previous;
	t->m[name]=sym;
	return &t->m[name];
}

//只是用于存储v的int常数,不在表中
Symbol newIntConst(int v){
	Symbol ret=new struct symbol;
	ret->addressed=0;
	ret->sclass=SCLASS_CONST;
	ret->temporary=1;
	ret->type=btot(TYPE_INT);
	ret->u.c.i=v;
	ret->needebx=0;
	return ret;
}

//新定义一个浮点常量的符号
//会把这个符号加到常量表中
//如果已经存在就直接返回表中符号
Symbol newFloatConst(Symbol s,Coordinate c){
	static char str[30];
	if(s->type->op==TYPE_FLOAT)
		sprintf(str,"$float%u",s->u.c.u);
	else
		sprintf(str,"$double%ull",s->u.c.uu);
	String name=str;
	Symbol sym=findConstSymbol(name);
	if(sym)return sym;
	sym=install(name,SCOPE_CONST);
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
Symbol findSymbol(String name){
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
Symbol newSymbol(String name,struct type* t,enum SCLASS s,Coordinate c){
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
	newTable->size=0;
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
Symbol newTemp(struct type*t,bool tell){
	static char str[11];
	sprintf(str,"$%d",tempid++);
	String name=str;
	Symbol sym=install(name,level);
	sym->temporary=1;
	sym->sclass=SCLASS_AUTO;
	copyField(sym->type,t);
	if(tell)tellVar(sym);
	return sym;
}