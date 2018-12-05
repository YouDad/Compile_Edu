#include"LexicalAnalyzer.h"
//<macros>
//<data>
//tval的作用只是为gettok的调用者提供常量的类型和值
struct symbol tval;
//lastChar是上一次调用getChar()的结果,nowChar是这一次的
char lastChar,nowChar;
//<functions>
void mapInit(){
	for(int i=0;i<256;i++){
		if('a'<=i&&i<='z')
			map[i]|=LETTER;
		if('A'<=i&&i<='Z')
			map[i]|=LETTER;
		if('0'<=i&&i<='9')
			map[i]|=DIGIT|HEX;
		if('a'<=i&&i<='f')
			map[i]|=HEX;
		if('A'<=i&&i<='F')
			map[i]|=HEX;
		if(i=='\t')map[i]|=BLANK;
		if(i==' ')map[i]|=BLANK;
		if(i=='\n')map[i]|=NEWLINE;
		//<NOT ENOUGH,ASCII>
	}
}
int getChar(){
	//更新<line><cols>
	lastChar=nowChar;
	return nowChar=getchar();
}
int getToken(){
	while(1){
		//<some things>
		switch(lastChar){
			//<identifier>
			//<keywords>
			//<boundary>
		}
	}
}