#include"types.h"
//<macros>
//<data>
TypeTable*table,globalTable;
int structid=1;
int unionid=1;
int enumid=1;
//<functions>

//初始化函数
void typeInit(){
	globalTable.level=SCOPE_GLOBAL;
	globalTable.prev=NULL;
	table=&globalTable;
	std::hash_map<String,struct type>&i=table->m;
	struct type tmp;
	tmp.kid=0;
	tmp.offset=0;
	tmp.next=0;
#define xx(n,o,s) tmp.name=n;tmp.op=o;tmp.size=s;i[n]=tmp
	xx("unsigned char",TYPE_UCHAR,1);
	xx("unsigned short",TYPE_USHORT,2);
	xx("unsigned short int",TYPE_USHORT,2);
	xx("unsigned int",TYPE_UINT,4);
	xx("unsigned long",TYPE_UINT,4);
	xx("unsigned long int",TYPE_UINT,4);
	xx("signed char",TYPE_UCHAR,1);
	xx("signed short",TYPE_USHORT,2);
	xx("signed short int",TYPE_USHORT,2);
	xx("signed int",TYPE_UINT,4);
	xx("signed long",TYPE_UINT,4);
	xx("signed long int",TYPE_UINT,4);
	xx("char",TYPE_CHAR,1);
	xx("short",TYPE_SHORT,2);
	xx("short int",TYPE_SHORT,2);
	xx("int",TYPE_INT,4);
	xx("long",TYPE_INT,4);
	xx("long int",TYPE_INT,4);
	xx("float",TYPE_FLOAT,4);
	xx("double",TYPE_DOUBLE,8);
	xx("void",TYPE_VOID,0);
#undef xx
}

//在lev表中新加一个类型
struct type* newType(int lev,String&name){
	TypeTable*i=table;
	while(i->level>lev)i=i->prev;
	struct type tmp;
	tmp.kid=NULL;
	tmp.next=NULL;
	tmp.name=name;
	tmp.offset=0;
	i->m[name]=tmp;
	return &i->m[name];
}

//检查域的lev值
void checkLevel(int lev){
	if(lev>table->level){
		while(lev>table->level){
			TypeTable* newTable=new TypeTable();
			newTable->level=table->level+1;
			newTable->prev=table;
			table=newTable;
		}
	}
}

//寻找一个名字叫做name的类型
struct type* findType(String name){
	for(TypeTable*i=table;i;i=i->prev)
		if(i->m.count(name))
			return &i->m[name];
}

//返回一个作用域为lev的空的Struct的类型指针
struct type* newStruct(int lev,String name){
	checkLevel(lev);
	for(TypeTable*i=table;i;i=i->prev){
		if(i->level==lev){
			if(name==""){
				static char str[20];
				sprintf(str,"$Struct%d",structid++);
				name=str;
			}
			auto it=i->m.find(name);
			if(it!=i->m.end())
				error("Redefined");
			struct type*t=newType(lev,name);
			t->op=TYPE_STRUCT;
			t->size=0;
			return t;
		}
	}
	assert(0);
	return NULL;
};

//返回一个作用域为lev的union类型指针
struct type* newUnion(int lev,String name){
	checkLevel(lev);
	for(TypeTable*i=table;i;i=i->prev){
		if(i->level==lev){
			if(name==""){
				static char str[20];
				sprintf(str,"$Union%d",unionid++);
				name=str;
			}
			auto it=i->m.find(name);
			if(it!=i->m.end())
				error("Redefined");
			struct type*t=newType(lev,name);
			t->op=TYPE_UNION;
			t->size=0;
			return t;
		}
	}
	assert(0);
	return NULL;
};

//返回一个作用域为lev的enum类型指针
struct type* newEnum(int lev,String name){
	checkLevel(lev);
	for(TypeTable*i=table;i;i=i->prev){
		if(i->level==lev){
			if(name==""){
				static char str[20];
				sprintf(str,"$Enum%d",enumid++);
				name=str;
			}
			auto it=i->m.find(name);
			if(it!=i->m.end())
				error("Redefined");
			struct type*t=newType(lev,name);
			t->op=TYPE_ENUM;
			t->size=0;
			return t;
		}
	}
	assert(0);
	return NULL;
}

//dfs函数,完全复制类型,i=j
void copyField(struct type*&i,struct type*&j){
	if(j){
		i=new struct type();
		i->offset=j->offset;
		i->name=j->name;
		i->op=j->op;
		i->size=j->size;
	}else return;
	if(j->kid)
		copyField(i->kid,j->kid);
	else i->kid=0;
	if(j->next)
		copyField(i->next,j->next);
	else i->next=0;
}

//返回一个作用域为lev的函数类型指针,返回值类型为t
struct type* newFunc(int lev,struct type*t){
	checkLevel(lev);
	for(TypeTable*i=table;i;i=i->prev){
		if(i->level==lev){
			static char str[20];
			sprintf(str,"$%s(*Func)",t->name.c_str());
			String name=str;
			auto it=i->m.find(name);
			if(it!=i->m.end())
				error("Redefined");
			struct type*tmp=newType(lev,name);
			tmp->op=TYPE_FUNCTION;
			tmp->size=0;
			copyField(tmp->kid,t);
			return tmp;
		}
	}
	assert(0);
	return NULL;
}

//把tmp的类型写到str里
void printType(char*str,struct type*tmp){
	str+=sprintf(str,",");
	if(isConst(tmp->op))
		str+=sprintf(str,"const ");
	switch(tmp->op){
		case TYPE_UCHAR:sprintf(str,"unsigned char)");break;
		case TYPE_USHORT:sprintf(str,"unsigned short)");break;
		case TYPE_UINT:sprintf(str,"unsigned int)");break;
		case TYPE_ENUM:sprintf(str,"enum %s",tmp->name.c_str());break;
		case TYPE_CHAR:sprintf(str,"char)");break;
		case TYPE_SHORT:sprintf(str,"short)");break;
		case TYPE_INT:sprintf(str,"int)");break;
		case TYPE_FLOAT:sprintf(str,"float)");break;
		case TYPE_DOUBLE:sprintf(str,"double)");break;
		case TYPE_ARRAY:str+=sprintf(str,"[%d]",tmp->size/tmp->kid->size);
			printType(str,tmp->kid);break;
		case TYPE_STRUCT:sprintf(str,"struct %s)",tmp->name.c_str());break;
		case TYPE_UNION:sprintf(str,"union %s)",tmp->name.c_str());break;
		case TYPE_POINTER:str+=sprintf(str,"* ");printType(str,tmp->kid);break;
		case TYPE_FUNCTION:sprintf(str,"%s)",tmp->name.c_str());break;
		case TYPE_VOID:sprintf(str,"void)");break;
	}
}

//把t这个函数定死在类型表里(原来是$int(*Func)后面没有参数)
//现在加上参数列表
struct type* saveFunc(struct type*t){
	for(TypeTable*i=table;i;i=i->prev){
		if(i->m.count(t->name)){
			//完善函数类型名
			static char str[1000];
			sprintf(str,"$%s(*Func)()",t->kid->name.c_str());
			if(t->kid->next)
			for(struct type*j=t->kid->next;j;j=j->next){
				printType(str+strlen(str)-1,j);
			}
			String name=str;
			if(name.find(',')!=-1)
				name.erase(name.find(','),1);
			//添加到类型表
			i->m[name]=*t;
			//删除原来的不完整类型
			auto it=i->m.find(t->name);
			i->m.erase(it);
			i->m[name].name=name;
			return &i->m[name];
		}
	}
	assert(0);
	return NULL;
}

//给结构体或联合增加域
void addField(struct type* Struct,struct type* fieldType){
	if(Struct->op!=TYPE_UNION)
		Struct->size+=fieldType->size;
	int delta=Struct->size-fieldType->size;
	if(Struct->kid){
		Struct=Struct->kid;
		while(Struct->next)
			Struct=Struct->next;
		copyField(Struct->next,fieldType);
		if(Struct->op!=TYPE_UNION)
			Struct->next->offset=delta;
	}else{
		copyField(Struct->kid,fieldType);
		if(Struct->op!=TYPE_UNION)
			Struct->kid->offset=delta;
	}
}

//删掉作用域大于lev的类型
void remoteType(int lev){
	while(table->level>lev){
		TypeTable*oldTable=table;
		table=table->prev;
		delete oldTable;
	}
}

//返回一个常量的t的类型
struct type* newConst(struct type* t){
	//已经是常量类型的了
	if(isConst(t->op))return t;
	String name=t->name+" const";
	struct type* ret=newType(SCOPE_GLOBAL,name);
	copyField(ret->kid,t->kid);
	ret->next=0;
	ret->op=t->op|TYPE_CONST;
	ret->size=t->size;
	return ret;
}

//返回一个指向t的指针类型
struct type* ptr(struct type* t){
	String name=t->name+" *";
	struct type* ret=newType(SCOPE_GLOBAL,name);
	copyField(ret->kid,t);
	ret->next=0;
	ret->op=TYPE_POINTER;
	ret->size=4;
	return ret;
};

//返回一个元素是t的数组类型
struct type* newArray(struct type* t,int size){
	static char str[100];
	sprintf(str,"%s[%d]",t->name.c_str(),size);
	String name=str;
	struct type* ret=newType(SCOPE_GLOBAL,name);
	copyField(ret->kid,t);
	ret->next=0;
	ret->op=TYPE_ARRAY|TYPE_CONST;
	ret->size=t->size*size;
	return ret;
}

//返回一个t所指向的类型,如果t是指针类型的话
struct type* deref(struct type* t){
	if(isPointer(t->op)||isArray(t->op))
		return &table->m[t->kid->name];
	return NULL;
};

//dfs判断两颗类型树是否相等
bool eqtree(struct type* it,struct type* jt){
	bool ret=true;
	struct type* i=it->kid,*j=jt->kid;
	for(;i&&j;i=i->next,j=j->next)
		ret&=eqtree(i,j);
	if(!i&&!j)
		return ret;
	return false;
}

//返回最大值的函数
int max(int a,int b){return a>b?a:b;}

//返回一个与t1和t2都兼容的类型,如没有就返回NULL
struct type* compatibleType(struct type* t1,struct type* t2){
	static struct type tmp;
	tmp.kid=0;
	tmp.name;
	if(isArith(t1->op)&&isArith(t2->op)){
		if(isUnsigned(t1->op)&&isUnsigned(t2->op)||
			(isFloat(t1->op)||isFloat(t2->op))){
			//两个无符号或有浮点数
			int maxop=max(t1->op,t2->op);
			return btot((enum TYPE)maxop);
		}else{
			//有有符号的
			int t1op=t1->op,t2op=t2->op;
			if(isUnsigned(t1->op))t1op+=4;
			if(isUnsigned(t2->op))t2op+=4;
			int maxop=max(t1op,t2op);
			return btot((enum TYPE)maxop);
		}
	}else if(isArray(t1->op)&&isArray(t2->op)){
		if(eqtree(t1->kid,t2->kid)){
			if(t1->size!=t2->size)
				return NULL;
			if(t1->size)return t1;
			return t2;
		}return NULL;
	}else if(isStruct(t1->op)&&isStruct(t2->op)){
		if(eqtree(t1,t2))
			return t1;
		return NULL;
	}else if(isUnion(t1->op)&&isUnion(t2->op)){
		if(eqtree(t1,t2))
			return t1;
		return NULL;
	}
	return NULL;
}
//返回一个op指明的类型
struct type* btot(enum TYPE op){
	switch(op){
	case TYPE_INT:return &globalTable.m["int"];
	case TYPE_SHORT:return &globalTable.m["short"];
	case TYPE_CHAR:return &globalTable.m["char"];
	case TYPE_FLOAT:return &globalTable.m["float"];
	case TYPE_DOUBLE:return &globalTable.m["double"];
	case TYPE_UCHAR:return &globalTable.m["unsigned char"];
	case TYPE_USHORT:return &globalTable.m["unsigned short"];
	case TYPE_UINT:return &globalTable.m["unsigned int"];
	case TYPE_VOID:return &globalTable.m["void"];
	}
	assert(0);
	return NULL;
}