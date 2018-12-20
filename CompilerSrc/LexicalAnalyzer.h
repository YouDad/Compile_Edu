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
//K:�ؼ���,I:��ʶ��,N:����,C:�ַ�,S:�ַ���,B:���,T:���ս��
//<exported typedefs>
//<exported functions>
	//��ʼ���ʷ�������
	extern void lexAnalyzerInit();
	//������һ��Token
	extern Token Next(bool show=true);
	inline bool isDigit(char x);
	inline bool isHex(char x);
	inline bool isOct(char x);
	inline bool isLowerCase(char x);
	inline bool isAlpha(char x);
	inline bool isIdfirst(char x);
	inline bool isBlank(char x);
//<exported data>
	//Invalid��ζ�ŷǷ���Token
	extern struct Token Invalid;
	//һ��ʹ��ȫ�ֱ���t�����浱ǰToken
	extern struct Token t;
	//nowchar��ʾ��һ����������ַ�
	extern char nowchar;
	//token��ǰ���ʵ�ԭ�ı�
	extern std::string token;
	//tsymΪĳЩ���ʴ��Symbol
	extern struct symbol* tsym;
	//src��ʾ��ǰ������Դ�����е�����
	extern struct coord src;
	//����Token�����ַ�������
	const char description[7][11]={"","KeyWord   ","Identifier","Number    ","Character ","String    ","Boundary  "};
	//������ֵĹؼ����ַ�����ʾ
	const String _keyWord[]={"volatile","goto","break","continue","short","signed","unsigned","int","float","void","long","double","for","while","return","do","char","sizeof","struct","class","static","const","auto","register","if","else","switch","case","default","enum","union","typedef","extern"};
	//������ֵĽ���ַ�����ʾ
	const String _boundary[]={"=","+","-","~","&","*","!","+=","-=","*=","/=","%=","&=","^=","|=","<<=",">>=",";","[","]","(",")","{","}",",","<",">","|","/","?",".","%","^",":","->","::","++","--","<<",">>","<=",">=","==","!=","&&","||",};
	//
	extern std::map<String,int> keyWordMap,identifierMap,numberMap,characterMap,stringMap,boundaryMap;
	extern Vector<String> keyWord,identifier,number,character,string,boundary;
	extern std::map<String,int> *string2id[7];
	extern Vector<String>*id2string[7];
