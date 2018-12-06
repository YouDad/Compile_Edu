#include"LexicalAnalyzer.h"
//<macros>
//<data>
//单词的字符串表示存在token里
std::string token;
//当前读到的字符在原文位置
int line=1,cols=1;
//tval的作用只是为gettok的调用者提供常量的类型和值
struct symbol tval;
//lastChar是上一次调用getChar()的结果,nowChar是这一次的
char lastChar,nowChar=' ';
//tsym为某些单词存放Symbol
struct symbol* tsym;
//src表示当前单词在源程序中的坐标
Coo src;
//掩码map[c]可以将字符c归为上面6种集合
char map[256];
int t;
//<functions>
void mapInit(){
	for(int i=0;i<256;i++){
		if('a'<=i&&i<='z')
			map[i]|=LETTER|ANSIC;
		if('A'<=i&&i<='Z')
			map[i]|=LETTER|ANSIC;
		if('0'<=i&&i<='9')
			map[i]|=DIGIT|HEX|ANSIC;
		if('a'<=i&&i<='f')
			map[i]|=HEX;
		if('A'<=i&&i<='F')
			map[i]|=HEX;
		if(i=='\t')map[i]|=BLANK|ANSIC;
		if(i==' ')map[i]|=BLANK|ANSIC;
		if(i=='\n')map[i]|=NEWLINE|ANSIC;
		for(char*p="~!%^&*()_+-=[]{}\\|;':\",.<>/?";*p;p++)
			map[*p]|=ANSIC;
	}
}
int getChar(){
	lastChar=nowChar;
	nowChar=getchar();
	cols++;
	if(lastChar&NEWLINE)
		line++,cols=1;
	return nowChar;
}
int getToken(){
	if(!map[' '])mapInit();
	while(1){
		//处理非ANSI C的字符
		if(!(map[nowChar]&ANSIC)){
			error("Can't accept %c",nowChar);
		}
		//排除空白字符
		while(map[nowChar]&(BLANK|NEWLINE))
			getChar();
		//清空原文字符串
		token="";
		src.x=line;
		src.y=cols;
		//根据首字符来识别不同token
		switch(nowChar){
#include"LexicalAnalyzer_HXM.cpp"
#include"LexicalAnalyzer_HXH.cpp"
		//识别字符常量
		case '\'':
			token+=nowChar;getChar();
			//escape代表是否遇到了转义字符
			bool escape=false;
			while(escape||nowChar!='\''){
				if(escape)escape=false;
				if(nowChar=='\\')
					escape=true;
				token+=nowChar;
				getChar();
				if(nowChar==EOF){
					error("Invalid Character");
				}
			}
			token+=nowChar;getChar();
			//<对于'\071'的识别>
			int value=0;
			sscanf(token.c_str(),"'%c",&value);
			tval.addressed=0;
			tval.u.c.v.i=value;
			//tval.type=inttype;
			return ICON;
		}
	}
}