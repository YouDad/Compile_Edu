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
	typedef struct type{
		//字符串表示
		char* name;
		//类型操作符编码
		int op;
		//类型操作数
		struct type* type;
		//以字节为单位的对象大小
		int size;
		//kid:子节点,next:兄弟节点
		struct type* kid,next;
		// union{
		// 	//<types with names or tags>
		// 	//u.sym指向符号表入口
		// 	Symbol sym;
		// 	// //<function types>
		// 	// //f.oldstyle区分两种函数类型
		// 	// //f.protp指向以空指针结尾的Type数组
		// 	// //proto[i]是第i+1个参数类型
		// 	// struct{
		// 	// 	Type* proto;
		// 	// }f;
		// }u;
	}* Type;

	enum{CHAR=1,INT,UNSIGNED,SHORT,
		LONG,ENUM,//以上为整数类型,除了ENUM以外没有操作数
		FLOAT,DOUBLE,//以上为算术类型,没有操作数
		ARRAY,STRUCT,UNION,//左边为聚合类型,STRUCT和UNION没有操作数
		POINTER,//指针类型,操作数指明被引用类型
		FUNCTION,//函数类型,操作数指明返回类型
		CONST,//限定类型,操作数指明未限定形式
		VOID//void类型,没有操作数
	};//op的可选值
	//ENUM的操作数是枚举标识符的类型,总是整型
	//ARRAY的操作数是数组元素类型

//<exported functions>
	//rmtypes从typetable中删除因作用域失效的类型
	//maxlevel是所有类型的作用域中最大的缓存
	void rmtypes(int lev);
	//如果ty1或ty2是不完全类型,则返回ret的值
	//eqtype函数用于测试类型是否兼容
	bool eqtype(Type ty1,Type ty2,bool ret);
	//btot接受一个类型后缀,返回一个相应类型
	Type btot(int op);

//<exported data>
	//<需要初始化>
	extern Type chartype;
	extern Type doubletype;
	extern Type floattype;
	extern Type inttype;
	extern Type longdouble;
	extern Type longtype;
	extern Type shorttype;
	extern Type signedchar;
	extern Type unsignedchar;
	extern Type unsignedlong;
	extern Type unsignedshort;
	extern Type unsignedtype;
	extern Type voidptype;
	extern Type voidtype;