#pragma once
#include"CommonHead.h"
//<exported macros>
	//宏实现的类型断言
	#define isqual(t) ((t)->op >=CONST)
	//unqual返回类型的未限定形式,即去掉类型中const和volatile
	#define unqual(t) (isqual(t)?(t)->type:(t))
	#define isvolatile(t) ((t)->op==VOLATILE \
						|| (t)->op==CONST+VOLATILE)
	#define isconst(t) ((t)->op==CONST \
					 || (t)->op==CONST+VOLATILE)
	//以下作用于为限定类型
	#define isarray(t)		(unqual(t)->op ==ARRAY)
	#define isstruct(t)		(unqual(t)->op ==STRUCT|| \
							 unqual(t)->op ==UNION)
	#define isunion(t)		(unqual(t)->op ==UNION)
	#define isfunc(t)		(unqual(t)->op ==FUNCTION)
	#define isptr(t)		(unqual(t)->op ==POINTER)
	#define ischar(t)		(unqual(t)->op ==CHAR)
	#define isint(t)		(unqual(t)->op >=CHAR&& \
							 unqual(t)->op <=UNSIGNED)
	#define isfloat(t)		(unqual(t)->op <=DOUBLE)
	#define isarith(t)		(unqual(t)->op <=UNSIGNED)
	#define isunsigned(t)	(unqual(t)->op ==UNSIGNED)
	#define isdouble(t)		(unqual(t)->op ==DOUBLE)
	#define isscalar(t)		(unqual(t)->op <=POINTER|| \
							 unqual(t)->op ==ENUM)
	#define isenum(t)		(unqual(t)->op ==ENUM)

//<exported types>
	struct type{
	public:
		//字符串表示
		char* name;
		//类型操作符编码
		int op;
		//以字节为单位的对象大小
		int size;
		//kid:子节点,next:兄弟节点
		type* kid,* next;
		// union{
		// 	//<types with names or tags>
		// 	//u.sym指向符号表入口
		// 	Symbol sym;
		// 	// //<function types>
		// 	// //f.oldstyle区分两种函数类型
		// 	// //f.protp指向以空指针结尾的Type数组
		// 	// //proto[i]是第i+1个参数类型
		// 	// struct{
		// 	// 	struct type** proto;
		// 	// }f;
		// }u;
		type(char*n,int o,int sz);
		type();
	};
	enum{TYPE_CHAR=1,TYPE_INT,TYPE_UNSIGNED,TYPE_SHORT,
		TYPE_LONG,TYPE_ENUM,//以上为整数类型,除了ENUM以外没有操作数
		TYPE_FLOAT,TYPE_DOUBLE,//以上为算术类型,没有操作数
		TYPE_ARRAY,TYPE_STRUCT,TYPE_UNION,//左边为聚合类型,STRUCT和UNION没有操作数
		TYPE_POINTER,//指针类型,操作数指明被引用类型
		TYPE_FUNCTION,//函数类型,操作数指明返回类型
		TYPE_CONST,//限定类型,操作数指明未限定形式
		TYPE_VOID//void类型,没有操作数
	};//op的可选值
	//ENUM的操作数是枚举标识符的类型,总是整型
	//ARRAY的操作数是数组元素类型

//<exported functions>
	//rmtypes从typetable中删除因作用域失效的类型
	//maxlevel是所有类型的作用域中最大的缓存
	void rmtypes(int lev);
	//如果ty1或ty2是不完全类型,则返回ret的值
	//eqtype函数用于测试类型是否兼容
	bool eqtype(struct type* ty1,struct type* ty2,bool ret);
	//btot接受一个类型后缀,返回一个相应类型
	struct type* btot(int op);

//<exported data>
	//<需要初始化>
	extern struct type* chartype;
	extern struct type* doubletype;
	extern struct type* floattype;
	extern struct type* inttype;
	extern struct type* longdouble;
	extern struct type* longtype;
	extern struct type* shorttype;
	extern struct type* signedchar;
	extern struct type* unsignedchar;
	extern struct type* unsignedlong;
	extern struct type* unsignedshort;
	extern struct type* unsignedtype;
	extern struct type* voidptype;
	extern struct type* voidtype;