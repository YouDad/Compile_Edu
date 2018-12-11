#pragma once
#include"CommonHead.h"
//<exported macros>
//<exported types>
enum FOR_STATE{FOR_START=1,FOR_INC,FOR_BLOCK,FOR_END,FOR_BREAK,FOR_CONTINUE};
enum IF_STATE{IF_CMP=1,IF_ELSE,IF_END};
enum OP{
	ADD,//'+'
	SUB,//'-'
	MUL,//'*'
	DIV,//'/'
	MOD,//'%'
	SHL,//'<<'
	SHR,//'>>'
	NOT,//'!'
	NEG,//'~'
	INC,//'++'
	DEC,//'--'
	LEA,//'&'
	PTR,//'*'
	EQU,//'=='
	NEQ,//'!='
	GTR,//'>'
	LES,//'<'
	GEQ,//'>='
	LEQ,//'<='
	AND,//'&&'
	OR,//'||'
	BTA,//'&'
	BTO,//'|'
	BTX,//'^'
	MOV//'='
};
//<exported typedefs>
//<exported functions>

	//告诉后端,编译完成,发送结束四元式
	void sendEnd();

	//告诉后端,生成一个id为labelId的标签
	void sendLabel(int labelId);

	//告诉后端,跳转到id为labelId的标签
	void sendGt(int labelId);

	//告诉后端,生成id为forId的循环的第state阶段四元式和代码
	void sendFor(int forId,enum FOR_STATE state);

	//告诉后端,生成id为ifId的条件的第state阶段四元式和代码
	void sendIf(int ifId,enum IF_STATE state);

	//告诉后端,生成(op,first,second,result)这样的四元式
	void sentOp(enum OP op,Symbol first,Symbol second,Symbol result);

	//通知后端,声明了一个s符号
	void tellVar(Symbol s);

	//通知后端,声明了一个t类型
	void tellType(Type t);

//<exported data>