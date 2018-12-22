#include"types.h"
//<macros>
//<data>
TypeTable*table,globalTable;
int structid=1;
int unionid=1;
int enumid=1;
//<functions>

//��ʼ������
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

//��lev�����¼�һ������
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

//������levֵ
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

//Ѱ��һ�����ֽ���name������
struct type* findType(String name){
	for(TypeTable*i=table;i;i=i->prev)
		if(i->m.count(name))
			return &i->m[name];
}

//����һ��������Ϊlev�Ŀյ�Struct������ָ��
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

//����һ��������Ϊlev��union����ָ��
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

//����һ��������Ϊlev��enum����ָ��
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

//dfs����,��ȫ��������,i=j
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

//����һ��������Ϊlev�ĺ�������ָ��,����ֵ����Ϊt
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

//��tmp������д��str��
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

//��t����������������ͱ���(ԭ����$int(*Func)����û�в���)
//���ڼ��ϲ����б�
struct type* saveFunc(struct type*t){
	for(TypeTable*i=table;i;i=i->prev){
		if(i->m.count(t->name)){
			//���ƺ���������
			static char str[1000];
			sprintf(str,"$%s(*Func)()",t->kid->name.c_str());
			if(t->kid->next)
			for(struct type*j=t->kid->next;j;j=j->next){
				printType(str+strlen(str)-1,j);
			}
			String name=str;
			if(name.find(',')!=-1)
				name.erase(name.find(','),1);
			//��ӵ����ͱ�
			i->m[name]=*t;
			//ɾ��ԭ���Ĳ���������
			auto it=i->m.find(t->name);
			i->m.erase(it);
			i->m[name].name=name;
			return &i->m[name];
		}
	}
	assert(0);
	return NULL;
}

//���ṹ�������������
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

//ɾ�����������lev������
void remoteType(int lev){
	while(table->level>lev){
		TypeTable*oldTable=table;
		table=table->prev;
		delete oldTable;
	}
}

//����һ��������t������
struct type* newConst(struct type* t){
	//�Ѿ��ǳ������͵���
	if(isConst(t->op))return t;
	String name=t->name+" const";
	struct type* ret=newType(SCOPE_GLOBAL,name);
	copyField(ret->kid,t->kid);
	ret->next=0;
	ret->op=t->op|TYPE_CONST;
	ret->size=t->size;
	return ret;
}

//����һ��ָ��t��ָ������
struct type* ptr(struct type* t){
	String name=t->name+" *";
	struct type* ret=newType(SCOPE_GLOBAL,name);
	copyField(ret->kid,t);
	ret->next=0;
	ret->op=TYPE_POINTER;
	ret->size=4;
	return ret;
};

//����һ��Ԫ����t����������
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

//����һ��t��ָ�������,���t��ָ�����͵Ļ�
struct type* deref(struct type* t){
	if(isPointer(t->op)||isArray(t->op))
		return &table->m[t->kid->name];
	return NULL;
};

//dfs�ж������������Ƿ����
bool eqtree(struct type* it,struct type* jt){
	bool ret=true;
	struct type* i=it->kid,*j=jt->kid;
	for(;i&&j;i=i->next,j=j->next)
		ret&=eqtree(i,j);
	if(!i&&!j)
		return ret;
	return false;
}

//�������ֵ�ĺ���
int max(int a,int b){return a>b?a:b;}

//����һ����t1��t2�����ݵ�����,��û�оͷ���NULL
struct type* compatibleType(struct type* t1,struct type* t2){
	static struct type tmp;
	tmp.kid=0;
	tmp.name;
	if(isArith(t1->op)&&isArith(t2->op)){
		if(isUnsigned(t1->op)&&isUnsigned(t2->op)||
			(isFloat(t1->op)||isFloat(t2->op))){
			//�����޷��Ż��и�����
			int maxop=max(t1->op,t2->op);
			return btot((enum TYPE)maxop);
		}else{
			//���з��ŵ�
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
//����һ��opָ��������
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