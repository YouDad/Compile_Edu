#pragma once
#include"CommonHead.h"
//<exported macros>
//<exported types>
	enum{
		BLANK=01,//如果c是一个非换行符的空白字符,则map[c]&BLANK非零
		NEWLINE=02,//包含'\n'
		LETTER=04,//包含a~z,A~Z
		DIGIT=010,//包含0~9
		HEX=020,//包含0~9,a~f,A~F
		ANSIC=040,//包含标准允许出现在源程序的ASCII字符,'@'就不是
		IDPART=0100//允许为标识符的一部分(非首字符)
	};

	enum{
		AUTO=1,BREAK,CASE,CHAR,CONST,CONTINUE,
		DEFAULT,DO,DOUBLE,ELSE,ENUM,EXTERN,
		FLOAT,FOR,GOTO,IF,INT,LONG,RETURN,SHORT,
		SIGNED,SIZEOF,STATIC,STRUCT,SWITCH,TYPEDEF,
		UNSIGNED,VOID,WHILE,REGISTER,UNION,VOLATILE,
		ID,ICON,FCON,SCON
	};//VOLATILE,REGISTER,UNION
//<exported typedefs>
//<exported functions>
	//返回下一个待处理的非空白字符
	extern int getChar();
	//返回下一个单词,可以是字符或枚举常量,关键字,预定义常量
	extern int getToken();
//<exported data>
	//通常使用全局变量t保存当前单词
	extern int t;
	//token当前单词的原文本
	extern std::string token;
	//tsym为某些单词存放Symbol
	extern struct symbol* tsym;
	//src表示当前单词在源程序中的坐标
	extern struct coord src;
	extern char map[256];