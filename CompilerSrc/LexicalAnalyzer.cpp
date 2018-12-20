#pragma once
#include"LexicalAnalyzer.h"
std::map<String,int> keyWordMap,identifierMap,numberMap,characterMap,stringMap,boundaryMap;
Vector<String> keyWord(_keyWord,_keyWord+33),identifier,number,character,string,boundary(_boundary,_boundary+46);
std::map<String,int> *string2id[7]={0,&keyWordMap,&identifierMap,&numberMap,&characterMap,&stringMap,&boundaryMap};
Vector<String>*id2string[7]={0,&keyWord,&identifier,&number,&character,&string,&boundary};
struct Token Invalid,t;
//��һ���������ַ���nowchar
//�մ�������ַ�lastchar
char nowchar,lastchar;
//token���ַ�����ʾ
std::string token;
//src��ʾ��ǰ������Դ�����е�����
Coordinate src;
struct symbol* tsym;
int LINE=1,COLS=1;
char getNextChar(){
	lastchar=nowchar;
	nowchar=getchar();
	COLS++;
	if(lastchar=='\n')
		LINE++,COLS=1;
	return nowchar;
}
typedef unsigned int uint;
inline bool isDigit(char x){return '0'<=x&&x<='9';}
inline bool isHex(char x){return isDigit(x)||'a'<=x&&x<='f'||'A'<=x&&x<='F';}
inline bool isOct(char x){return '0'<=x&&x<='7';}
inline bool isLowerCase(char x){return 'a'<=x&&x<='z';}
inline bool isAlpha(char x){return isLowerCase(x)||'A'<=x&&x<='Z';}
inline bool isIdfirst(char x){return isAlpha(x)||x=='_';}
inline bool isBlank(char x){return x==' '||x=='\t';}
//ʶ����һ����ʶ����ؼ���
Token nextIdentifier(){
    while(isIdfirst(nowchar)||isDigit(nowchar)){
        token+=nowchar;getNextChar();
    }
    if(!keyWordMap.count(token))
        return Token(_I_);
    return Token(_K_,keyWordMap[token]);
}
Token nextBoundary();
//����ʶ����һ��������
Token nextReal(){
	if(nowchar=='.'){
		token+=nowchar;getNextChar();
		//����"3.e","a.b"���� �����û���ֵ����
		if(!isDigit(nowchar)){
			//��������ǽ��
			if(token==".")
				return nextBoundary();
			error("Invalid Floating Constant");
		}
		//��ȡС������
		while(isDigit(nowchar)){
			token+=nowchar;getNextChar();
		}
	}
	if(nowchar=='e'||nowchar=='E'){
		token+=nowchar;getNextChar();
		//ʶ��ָ��ǰ�ķ���
		if(nowchar=='+'||nowchar=='-'){
			token+=nowchar;getNextChar();
		}
		//����"3.5e"����e����û���ֵ����
		if(!isDigit(nowchar)){
			error("Invalid Floating Constant");
		}
		//��ȡָ��
		while(isDigit(nowchar)){
			token+=nowchar;getNextChar();
		}
	}
	return Token(_N_);
}
//ʶ����һ������
Token nextConst(){
	token+=nowchar;getNextChar();
    bool(*function)(char chr)=isDigit;
    if(token[0]=='0'){
        if((nowchar|32)=='x'){//hex
            token+=nowchar;getNextChar();
            function=isHex;
        }else{//oct
            function=isOct;
        }
    }int LL=0;//no suffix or L or LL
    while(LL==0&&function(nowchar)||LL<=1&&(nowchar|32)=='l'){
        LL+=(nowchar|32)=='l';
        token+=nowchar;getNextChar();
    }
	if(function!=isHex&&LL==0)
    if(nowchar=='.'||(nowchar|32)=='e')
        return nextReal();
    return Token(_N_);
}
//ʶ����һ���ַ�
Token nextCharacter(){
	token+=nowchar;getNextChar();
    bool transfer=false;
    while(transfer||nowchar!='\''){
        transfer=!transfer&&nowchar=='\\';
        token+=nowchar;getNextChar();
        if(nowchar==-1){
			fprintf(stderr,"%d:%d: ",src.x,src.y);
			fprintf(stderr,"unexpected end of file\n");
            return Invalid;
        }
    }token+="'";getNextChar();
    return Token(_C_);
}
//ʶ����һ���ַ���
Token nextString(){
	token+=nowchar;getNextChar();
    bool transfer=false;
    while(transfer||nowchar!='"'){
        transfer=!transfer&&nowchar=='\\';
        token+=nowchar;getNextChar();
        if(nowchar==-1){
			fprintf(stderr,"%d:%d: ",src.x,src.y);
			fprintf(stderr,"unexpected end of file\n");
            return Invalid;
        }
    }token+="\"";getNextChar();
    return Token(_S_);
}
Token next();
//ʶ����һ�����
Token nextBoundary(){
	token+=nowchar;getNextChar();
    if(token[0]=='/'&&nowchar=='/'){//line commit
        while(lastchar!='\n')getNextChar();
        return next();
    }
    if(token[0]=='/'&&nowchar=='*'){//block commit
        while(1){
            do getNextChar();while(nowchar!='*');
            getNextChar();
            if(nowchar=='/'){
                getNextChar();
                return next();
            }
        }
    }
    while(boundaryMap.count(token+nowchar)){
        token+=nowchar;getNextChar();
    }return Token(_B_,boundaryMap[token]);
}
#define nextif(condition,function) \
else if(condition){return function();}
//ʶ����һ��Token
Token next(){
    while(nowchar=='\n')getNextChar();
    token="";
	src.x=LINE;src.y=COLS;
    if(nowchar==EOF){
        return Invalid;
    }else if(isBlank(nowchar)){
        do getNextChar();
        while(isBlank(nowchar));
        return next();
    }
    nextif(isIdfirst(nowchar)              ,nextIdentifier)
    nextif(isDigit(nowchar)                ,nextConst)
    nextif(nowchar=='\''                   ,nextCharacter)
    nextif(nowchar=='"'                    ,nextString)
    nextif(nowchar=='.'                    ,nextReal)
    nextif(true                            ,nextBoundary)
}
Token Next(bool show){
	static int cnt[7]={};
    Token ret=next();
	if(nowchar==EOF&&ret==Invalid)
		return Invalid;
    while(ret==Invalid)ret=next();
    if(show&&ret!=Invalid)
         colorPrintf(ret.type,"%4d of %s\t%s\n",
             ++cnt[ret.type],description[ret.type],token.c_str());
    return ret;
}
//��ʼ������
inline void lexAnalyzerInit(){
    for(uint i=0;i<keyWord .size();i++)keyWordMap [keyWord [i]]=i;
    for(uint i=0;i<boundary.size();i++)boundaryMap[boundary[i]]=i;
    getNextChar();t=Next();
}