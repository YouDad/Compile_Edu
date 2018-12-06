//#include"LexicalAnalyzer.h"
////<macros>
////<data>
////单词的字符串表示存在token里
//std::string token;
////当前读到的字符在原文位置
//int line=1,cols=1;
////tval的作用只是为gettok的调用者提供常量的类型和值
//struct symbol tval;
////lastChar是上一次调用getChar()的结果,nowChar是这一次的
//char lastChar,nowChar=' ';
////tsym为某些单词存放Symbol
//struct symbol* tsym;
////src表示当前单词在源程序中的坐标
//Coo src;
////掩码map[c]可以将字符c归为上面6种集合
//char map[256];
//int t;
////<functions>
//void mapInit(){
//	for(int i=0;i<256;i++){
//		if('a'<=i&&i<='z')
//			map[i]|=LETTER|ANSIC;
//		if('A'<=i&&i<='Z')
//			map[i]|=LETTER|ANSIC;
//		if('0'<=i&&i<='9')
//			map[i]|=DIGIT|HEX|ANSIC;
//		if('a'<=i&&i<='f')
//			map[i]|=HEX;
//		if('A'<=i&&i<='F')
//			map[i]|=HEX;
//		if(i=='\t')map[i]|=BLANK|ANSIC;
//		if(i==' ')map[i]|=BLANK|ANSIC;
//		if(i=='\n')map[i]|=NEWLINE|ANSIC;
//		for(char*p="~!%^&*()_+-=[]{}\\|;':\",.<>/?";*p;p++)
//			map[*p]|=ANSIC;
//	}
//}
//int getChar(){
//	lastChar=nowChar;
//	nowChar=getchar();
//	cols++;
//	if(lastChar&NEWLINE)
//		line++,cols=1;
//	return nowChar;
//}
//int getToken(){
//	if(!map[' '])mapInit();
//	while(1){
//		//处理非ANSI C的字符
//		if(!(map[nowChar]&ANSIC)){
//			error("Can't accept %c",nowChar);
//		}
//		//排除空白字符
//		while(map[nowChar]&(BLANK|NEWLINE))
//			getChar();
//		//清空原文字符串
//		token="";
//		src.x=line;
//		src.y=cols;
//		//根据首字符来识别不同token
//		switch(nowChar){
		//这些情况是不可能为其他种Token的
		case 'h':case 'j':case 'k':case 'm':case 'n':case 'o':case 'p':case 'q':
		case 'x':case 'y':case 'z':case 'A':case 'B':case 'C':case 'D':case 'E':
		case 'F':case 'G':case 'H':case 'I':case 'J':case 'K':case 'M':case 'N':
		case 'O':case 'P':case 'Q':case 'R':case 'S':case 'T':case 'U':case 'V':
		case 'W':case 'X':case 'Y':case 'Z':case '_':
		id:
			token+=nowChar;getChar();
			while(map[nowChar]&(DIGIT|LETTER)){
				token+=nowChar;getChar();
			}
			//根据原文字符串得到(如果有的话)符号指针
			tsym=lookup(token.c_str(),identifiers);
			return ID;
// 		}
// 	}
// }
