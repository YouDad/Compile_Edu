#pragma once
#include"CommonHead.h"
//<exported macros>
#define HASHSIZE 256
//<exported types>

	typedef struct coord{
		String file;//该定义的文件名
		unsigned x,y;//行号和字符位置
	}Coordinate;

	typedef union value{
		char sc;
		short ss;
		int i;
		unsigned char uc;
		unsigned short us;
		unsigned u;
		float f;
		double d;
		void* p;
	}Value;

	typedef struct symbol{
	public:
		String name;
		//scope域说明了符号是常量,标号,全局变量,参数或局部变量
		int scope;
		Coordinate src;
		//把符号表中所有符号连成了一个链表
		//最后载入符号表的符号为链首
		//从后向前遍历该链表可以访问到当前作用域内的所有符号
		//包括因内层同名声明而被隐藏的符号
		struct symbol* up;
		//符号的扩展存储类型
		//有:无(常量,标号),AUTO,REGISTER,STATIC,EXTERN,TYPEDEF,ENUM
		int sclass;
		//type域保存了变量,函数,常量,结构,联合和枚举等类型信息
		class type* type;
		union{
			//<enum constants>
			int value;
			//<constans>
			struct{
				//常量的值存放在v中
				Value v;
				//有些(浮点)常量不能存储在指令中,所以编译器
				//生成一个静态变量并将其初始化成该浮点常量的值
				//对于这些常量,loc指向了产生的变量所对应的符号
				struct symbol* loc;
			}c;
		}u;
		//<symbol flags>
		//如果该符号是临时变量,则temporary为1
		unsigned temporary:1;
		//如果该符号已被定义,则defined为1
		unsigned defined:1;
		//如果该类型的常量可以作为指令的一部分
		//即不能被寻址,则addressed的值为0
		unsigned addressed:1;
	}* Symbol;

	//注:在第k层声明的局部变量,其scope域等于LOCAL+k
	enum SCOPE{SCOPE_CONSTANTS=1,SCOPE_GLOBAL,SCOPE_PARAM,SCOPE_LOCAL};//scope

	enum SCLASS{SCLASS_AUTO=1,SCLASS_STATIC,SCLASS_EXTERN,SCLASS_TYPEDEF,SCLASS_ENUM};//sclass

	typedef struct table{
		//作用域
		int level;
		//外层作用域对应的table
		struct table* previous;
		//保存符号
		std::hash_map<String,struct symbol>m;
		//由当前及外层作用域中所有符号组成的列表的头
		Symbol all;
	}* Table;

//<exported typedefs>
//<exported functions>

	//install函数为给定的name分配一个符号
	//并把该符号加到对应给定作用域层数相对应的表中
	//如果有需要,还将建立一个新表
	//该函数将返回一个指向符号的指针
	Symbol install(char* name,Table&tp,int lev);

	//lookup函数在表中查找一个名字
	//查找的关键字是符号的name
	//如果查找到了就返回一个指向符号的指针
	//否则返回空指针
	Symbol lookup(const char* name,Table tp);

//<exported data>

	//静态或常量
	extern Table constants;
	// //声明为extern的标识符
	// extern Table externals;
	// //保存具有文件作用域的标识符
	// extern Table globals;

	//一般标识符
	extern Table identifiers;
	//level和对应的表表示了一个作用域
	extern int level;