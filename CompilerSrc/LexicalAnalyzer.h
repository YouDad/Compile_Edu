#pragma once
#include"CommonHead.h"
//<exported macros>
//<exported types>
struct Token{
    int type,id;
    Token(int t=-1,int i=-1):type(t),id(i){}
    inline bool operator==(const Token&other){
		return id==other.id&&type==other.type;
	}
    inline bool operator!=(const Token&other){
		return !(*this==other);
	}
	inline bool operator< (const Token&other)const{
		if(type==other.type)
			return id<other.id;
		return type<other.type;
	}
};
enum{_K_=1,_I_,_N_,_C_,_S_,_B_,_T_};
//K:关键字,I:标识符,N:数字,C:字符,S:字符串,B:界符,T:非终结符
//<exported typedefs>
//<exported functions>
	//初始化词法分析器
	extern void lexAnalyzerInit();
	//返回下一个Token
	extern Token Next(bool show=true);
	inline bool isDigit(char x);
	inline bool isHex(char x);
	inline bool isOct(char x);
	inline bool isLowerCase(char x);
	inline bool isAlpha(char x);
	inline bool isIdfirst(char x);
	inline bool isBlank(char x);
//<exported data>
	//Invalid意味着非法的Token
	extern struct Token Invalid;
	//一般使用全局变量t来保存当前Token
	extern struct Token t;
	//nowchar表示下一个待处理的字符
	extern char nowchar;
	//token当前单词的原文本
	extern std::string token;
	//tsym为某些单词存放Symbol
	extern struct symbol* tsym;
	//src表示当前单词在源程序中的坐标
	extern struct coord src;
	//六大Token类别的字符串描述
	const char description[7][11]={"","KeyWord   ","Identifier","Number    ","Character ","String    ","Boundary  "};
	//允许出现的关键字字符串表示
	const String _keyWord[]={"volatile","goto","break","continue","short","signed","unsigned","int","float","void","long","double","for","while","return","do","char","sizeof","struct","class","static","const","auto","register","if","else","switch","case","default","enum","union","typedef","extern"};
	//允许出现的界符字符串表示
	const String _boundary[]={"=","+","-","~","&","*","!","+=","-=","*=","/=","%=","&=","^=","|=","<<=",">>=",";","[","]","(",")","{","}",",","<",">","|","/","?",".","%","^",":","->","::","++","--","<<",">>","<=",">=","==","!=","&&","||",};
	//
	extern std::map<String,int> keyWordMap,identifierMap,numberMap,characterMap,stringMap,boundaryMap;
	extern Vector<String> keyWord,identifier,number,character,string,boundary;
	extern std::map<String,int> *string2id[7];
	extern Vector<String>*id2string[7];
