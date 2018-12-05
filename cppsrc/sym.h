#pragma once
#include"CommonHead.h"
//<exported macros>
//<exported types>

	typedef struct coord{
		char* file;//该定义的文件名
		uint x,y;//行号和字符位置
	}Coordinate;

	typedef union value{
		char sc;
		short ss;
		int i;
		uchar uc;
		unsigned short us;
		uint u;
		float f;
		double d;
		void* p;
	}Value;

	typedef struct symbol{
		char* name;
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
		Type type;
		union{
			//<struct types>
			struct{
				//flist域指向用next域连起来的field结构
				Field flist;
			}s;
			//<enum constants>
			int value;
			//<enum types>
			//对于枚举类型包含的枚举常量,idlist指向
			//以空结尾的Symbol数组,这些符号加载到
			//identifiers表,每个入口包含该枚举常量的值
			struct symbol** idlist;
			//<constans>
			struct{
				//常量的值存放在v中
				Value v;
				//有些(浮点)常量不能存储在指令中,所以编译器
				//生成一个静态变量并将其初始化成该浮点常量的值
				//对于这些常量,loc指向了产生的变量所对应的符号
				struct symbol* loc;
			}c;
			//<function symbols>
			//<globals>
			//<temporaries>
		}u;
		//<symbol flags>
		uint temporary:1;
		// uint generated:1;
		uint defined:1;
		//如果该类型的常量可以作为指令的一部分,addressed的值为0
		uint addressed:1;
	}* Symbol;

	//注:在第k层声明的局部变量,其scope域等于LOCAL+k
	enum{CONSTANTS=1,LABELS,GLOBAL,PARAM,LOCAL};//scope

	enum{AUTO=1,REGISTER,STATIC,EXTERN,TYPEDEF,ENUM};//sclass

	typedef struct table{
		//作用域
		int level;
		//外层作用域对应的table
		struct table* previous;
		//256个哈希链
		struct entry{
			symbol sym;
			entry* next;
		}* buckets[256];
		//由当前及外层作用域中所有符号组成的列表的头
		Symbol all;
	}* Table;

//<exported typedefs>
//<exported functions>

	//install函数为给定的name分配一个符号
	//并把该符号加到对应给定作用域层数相对应的表中
	//如果有需要,还将建立一个新表
	//该函数将返回一个指向符号的指针
	Symbol install(char* name,Table&tp,int level);

	//lookup函数在表中查找一个名字
	//查找的关键字是符号的name
	//如果查找到了就返回一个指向符号的指针
	//否则返回空指针
	Symbol lookup(char* name,Table tp);

//<exported data>
	//静态或常量
	extern Table constants;
	//声明为extern的标识符
	extern Table externals;
	//保存具有文件作用域的标识符
	extern Table globals;
	//一般标识符
	extern Table identifiers;
	//编译器内部标号
	extern Table labels;
	//类型标记
	extern Table types;
	//level和对应的表表示了一个作用域
	extern int level;