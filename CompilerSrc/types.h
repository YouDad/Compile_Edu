#pragma once
#include"CommonHead.h"
//<exported macros>
#define isUnsigned(t) ((t&63)<=TYPE_UINT)
#define isInteger(t) ((t&63)<=TYPE_INT)
#define isFloat(t) (TYPE_FLOAT<=(t&63)&&(t&63)<=TYPE_DOUBLE)
#define isArith(t) ((t&63)<=TYPE_DOUBLE)
#define isStatic(t) (t&64)
#define isConst(t)  (t&128)
#define isArray(t) ((t&63)==TYPE_ARRAY)
#define isPointer(t) ((t&63)==TYPE_POINTER)
#define isStruct(t) ((t&63)==TYPE_STRUCT)
#define isUnion(t) ((t&63)==TYPE_UNION)
//<exported types>

//类型结构体
struct type{
	//类型名字
	String name;
	//类型大小
	int size;
	//类型操作数
	int op;
	//域的偏移量
	int offset;
	//kid是数组,结构体的指向下一级类型的指针
	//next是串联域的指针
	struct type* kid,* next;
};
//类型枚举
enum TYPE{
	TYPE_UCHAR=1,TYPE_USHORT,TYPE_UINT,//小于UINT都是无符号类型
	TYPE_ENUM,TYPE_CHAR,TYPE_SHORT,TYPE_INT,//小于ENUM都是整数类型
	TYPE_FLOAT,TYPE_DOUBLE,//两个浮点类型,以上都是算术类型
	TYPE_ARRAY,TYPE_STRUCT,TYPE_UNION,TYPE_POINTER,TYPE_FUNCTION,//有kid域的类型
	TYPE_VOID,//空类型
	TYPE_STATIC=64,//静态存储类型
	TYPE_CONST=128//常量存储类型
};
struct TypeTable{
	int level;
	struct TypeTable* prev;
	std::hash_map<String,struct type>m;
};
//<exported typedefs>
//<exported functions>
	//初始化函数
	void typeInit();
	//检查域的lev值
	void checkLevel();
	//返回一个作用域为lev的空的Struct的类型指针
	struct type* newStruct(int lev,String&name);
	//返回一个作用域为lev的union类型指针
	struct type* newUnion(int lev,String&name);
	//返回一个作用域为lev的enum类型指针
	struct type* newEnum(int lev,String&name);
	//给结构体或联合增加域
	void addField(struct type* Struct,struct type* fieldType);
	//删掉作用域大于lev的类型
	void remoteType(int lev);
	//返回一个常量的t的类型
	struct type* newConst(struct type* t);
	//返回一个指向t的指针类型
	struct type* ptr(struct type* t);
	//返回一个t所指向的类型,如果t是指针类型的话
	struct type* deref(struct type* t);
	//返回一个与t1和t2都兼容的类型,如没有就返回NULL
	struct type* compatibleType(struct type* t1,struct type* t2);
	//返回一个op指明的类型
	struct type* btot(enum TYPE op);
//<exported data>