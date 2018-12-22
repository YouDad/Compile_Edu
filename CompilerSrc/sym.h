#pragma once
#include"CommonHead.h"
//<exported macros>
#define HASHSIZE 256
//<exported types>

	typedef struct coord{
		// char* file;//该定义的文件名
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
		unsigned long long uu;
	}Value;

	typedef struct symbol{
		//name域存这个符号的字符串表示
		String name;
		//scope域说明了符号是常量,标号,全局变量,参数或局部变量
		int scope;
		//src域说明了这个符号定义出现的位置
		Coordinate src;
		//符号的扩展存储类型
		//有:无=0,SCLASS_AUTO=1,SCLASS_CONST
		//SCLASS_STATIC,SCLASS_EXTERN,SCLASS_TYPEDEF
		int sclass;
		//type域保存了变量,函数,常量,结构,联合和枚举等类型信息
		struct type* type;
		//offset域保存了这个符号在对应表内的偏移量
		int offset;
		union{
			//enum constants value
			int ev;
			//constants
			Value c;
		}u;
		//<symbol flags>
		//如果该符号是临时变量,则temporary为1
		unsigned temporary:1;
		//如果该类型的常量可以作为指令的一部分
		//即不能被寻址,则addressed的值为0
		unsigned addressed:1;
		unsigned needebx:1;
	}* Symbol;

	//注:在第k层声明的局部变量,其scope域等于LOCAL+k
	enum SCOPE{SCOPE_CONST=1,SCOPE_STATIC,SCOPE_GLOBAL,SCOPE_PARAM,SCOPE_LOCAL};//scope

	enum SCLASS{SCLASS_AUTO=1,SCLASS_EXTERN,SCLASS_TYPEDEF,SCLASS_CONST,SCLASS_STATIC};//sclass

	//符号表类型
	typedef struct symbolTable{
		//作用域
		int level;
		//外层作用域对应的table
		struct symbolTable* previous;
		//保存符号
		std::hash_map<String,struct symbol>m;
		//整个表所有符号的大小
		int size;
	}* SymbolTable;

//<exported typedefs>
//<exported functions>

	//符号表初始化函数
	void symInit();

	//只是用于存储v的int常数,不在表中
	Symbol newIntConst(int v);

	//新定义一个浮点常量的符号
	//会把这个符号加到常量表中
	//如果已经存在就直接返回表中符号
	Symbol newFloatConst(Symbol s,Coordinate c);

	//新定义一个字符串常量的符号
	//会把这个符号加到常量表中
	Symbol newStringConst(String&str,Coordinate c);

	//新定义一个枚举常量的标识符
	//name是这个枚举常量的名字
	//val是这个枚举常量的常量值
	//t是这个枚举常量的类型
	//c是这个符号出现的位置
	Symbol newEnumConst(String&name,int val,struct type* t,Coordinate c);

	//找到名字叫name的标识符
	//没找到就返回NULL
	Symbol findSymbol(String name);

	//找到名字叫name的常量标识符
	//没找到就返回NULL
	Symbol findConstSymbol(String&name);

	//新定义一个标识符
	//name是这个标识符的名字
	//t是这个标识符的类型
	//s是这个标识符的存储类型
	//c是这个符号出现的位置
	Symbol newSymbol(String name,type* t,enum SCLASS s,Coordinate c);

	//告诉符号表,进入了一个作用域
	void enterScope();

	//告诉符号表,退出了一个作用域
	//符号表会把不合时宜的符号删除
	void exitScope();

	//向符号表申请一个临时变量
	Symbol newTemp(struct type*t,bool tell=true);

//<exported data>

	// //声明为extern的标识符
	// extern Table externals;
	// //保存具有文件作用域的标识符
	// extern Table globals;

	//常量
	extern SymbolTable constTable;
	//静态
	extern SymbolTable staticTable;
	//标识符
	extern SymbolTable identifierTable;

	//level和对应的表表示了一个作用域
	extern int level;