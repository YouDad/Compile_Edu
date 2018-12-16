#pragma once
#include"CommonHead.h"
//<exported macros>
//<exported types>
	enum{
		BLANK=01,//���c��һ���ǻ��з��Ŀհ��ַ�,��map[c]&BLANK����
		NEWLINE=02,//����'\n'
		LETTER=04,//����a~z,A~Z
		DIGIT=010,//����0~9
		HEX=020,//����0~9,a~f,A~F
		ANSIC=040,//������׼���������Դ�����ASCII�ַ�,'@'�Ͳ���
		IDPART=0100//����Ϊ��ʶ����һ����(�����ַ�)
	};

	enum{
		AUTO=128,BREAK,CASE,CHAR,CONST,CONTINUE,
		DEFAULT,DO,DOUBLE,ELSE,ENUM,EXTERN,
		FLOAT,FOR,GOTO,IF,INT,LONG,RETURN,SHORT,
		SIGNED,SIZEOF,STATIC,STRUCT,SWITCH,TYPEDEF,
		UNSIGNED,VOID,WHILE,REGISTER,UNION,VOLATILE,
		ID,ICON,FCON,SCON,
		INC_OP,DEC_OP,PTR_OP,GE_OP,LE_OP,EQ_OP,NEQ_OP,//++,--,->,>=,<=,==,!=
		OR_OP,AND_OP,SHL_OP,SHR_OP,//&&,||,<<,>>
		MUL_ASSIGN,DIV_ASSIGN,MOD_ASSIGN,ADD_ASSIGN,SUB_ASSIGN,//*=,/=,%=,+=,-=
		SHL_ASSIGN,SHR_ASSIGN,AND_ASSIGN,XOR_ASSIGN,OR_ASSIGN//<<=,>>=,&=,^=,|=
	};
//<exported typedefs>
//<exported functions>
	//��ʼ���ʷ�������
	extern void lexAnalyzerInit();
	//������һ��������ķǿհ��ַ�
	extern int getChar();
	//������һ������,�������ַ���ö�ٳ���,�ؼ���,Ԥ���峣��
	extern int getToken();
//<exported data>
	//ͨ��ʹ��ȫ�ֱ���t���浱ǰ����
	extern int t;
	//token��ǰ���ʵ�ԭ�ı�
	extern std::string token;
	//tsymΪĳЩ���ʴ��Symbol
	extern struct symbol* tsym;
	//src��ʾ��ǰ������Դ�����е�����
	extern struct coord src;
	extern char map[256];
