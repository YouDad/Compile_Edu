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
void SendEnd();
void SendLabel(int labelId);
void SendGt(int labelId);
void SendFor(int forId,enum FOR_STATE state);
void SendIf(int ifId,enum IF_STATE state);
void SentOp(enum OP op,Symbol first,Symbol second,Symbol result);
//<exported data>