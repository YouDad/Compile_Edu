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
Coordinate src;
//掩码map[c]可以将字符c归为上面7种集合
char map[256];
int t;
//<functions>
void mapInit(){
	for(int i=0;i<256;i++){
		if('a'<=i&&i<='z')
			map[i]|=LETTER|ANSIC|IDPART;
		if('A'<=i&&i<='Z')
			map[i]|=LETTER|ANSIC|IDPART;
		if('0'<=i&&i<='9')
			map[i]|=DIGIT|HEX|ANSIC|IDPART;
		if('a'<=i&&i<='f')
			map[i]|=HEX;
		if('A'<=i&&i<='F')
			map[i]|=HEX;
		if(i=='\t')map[i]|=BLANK|ANSIC;
		if(i==' ')map[i]|=BLANK|ANSIC;
		if(i=='\n')map[i]|=NEWLINE|ANSIC;
		map['_']|=IDPART;
		for(char*p="~!%^&*()_+-=[]{}\\|;':\",.<>/?";*p;p++)
			map[*p]|=ANSIC;
	}
}
int getChar(){
	lastChar=nowChar;
	nowChar=getchar();
	cols++;
	if(lastChar>0&&map[lastChar]&NEWLINE)
		line++,cols=1;
	return nowChar;
}
int getToken(){
	if(!map[' '])mapInit();
	while(1){
		//处理非ANSI C的字符
		if(nowChar<0||(!(map[nowChar]&ANSIC))){
			error("%c isn't ANSI C character",nowChar);
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
		//注释或者是除号
		case '/':
			token+=nowChar;getChar();
			if(nowChar=='/'){
				//处理行注释
				while(getChar()!='\n');
				//但是由于不能再排除空白字符
				//所以递归调用
				return getToken();
			}else if(nowChar=='*'){
				//处理块注释
				do
					getChar();
				while(lastChar!='*'||nowChar!='/');
				getChar();
				//但是由于不能再排除空白字符
				//所以递归调用
				return getToken();
			}else{
				return '/';
			}
		case '~':case '!':case '%':case '^':case '&':case '*':case '(':
		case ')':case '=':case '[':case ']':case '{':case '}':case '\\':
		case '|':case ';':case ':':case ',':case '<':case '>':case '?':
			return nowChar;
		case 'h':case 'j':case 'k':case 'm':case 'n':case 'o':case 'p':case 'q':
		case 'x':case 'y':case 'z':case 'A':case 'B':case 'C':case 'D':case 'E':
		case 'F':case 'G':case 'H':case 'I':case 'J':case 'K':case 'M':case 'N':
		case 'O':case 'P':case 'Q':case 'R':case 'S':case 'T':case 'U':case 'V':
		case 'W':case 'X':case 'Y':case 'Z':case '_':
		id:
			if(map[nowChar]&IDPART){
				token+=nowChar;getChar();
				while(map[nowChar]&IDPART){
					token+=nowChar;getChar();
				}
			}
			//根据原文字符串得到(如果有的话)符号指针
			tsym=lookup(token.c_str(),identifiers);
			return ID;
#define recognize(string,RET) \
	for(char*p=string;*p;p++){\
		if(nowChar==*p){\
			token+=nowChar;getChar();\
		}else if(map[nowChar]&(DIGIT|LETTER)){\
			goto id;\
		}else{\
			tsym=lookup(token.c_str(),identifiers);\
			return ID;\
		}\
	}\
	if(!(map[nowChar]&IDPART))\
		return RET;\
	else goto id;
		//AUTO
		case 'a':recognize("auto",AUTO);
		//BREAK
		case 'b':recognize("break",BREAK);
		//CASE&CHAR&CONST&CONTINUE
		case 'c':
			token+=nowChar;getChar();
			if(nowChar=='a'){
				recognize("ase",CASE);
			}else if(nowChar=='h'){
				recognize("har",CHAR);
			}else if(nowChar=='o'){
				token+=nowChar;getChar();
				if(nowChar=='n'){
					token+=nowChar;getChar();
					if(nowChar=='s'){
						recognize("st",CONST);
					}else if(nowChar=='t'){
						recognize("tinue",CONTINUE);
					}else goto id;
				}else goto id;
			}else goto id;
		//DEFAULT&DO&DOUBLE
		case 'd':
			token+=nowChar;getChar();
			if(nowChar=='e'){
				recognize("efault",DEFAULT);
			}else if(nowChar=='o'){
				token+=nowChar;getChar();
				if(map[nowChar]&IDPART){
					recognize("uble",DOUBLE);
				}else return DO;
			}
		//ELSE&ENUM&EXTERN
		case 'e':
			token+=nowChar;getChar();
			switch(nowChar){
			case 'l':recognize("lse",ELSE);
			case 'n':recognize("num",ENUM);
			case 'x':recognize("xtern",EXTERN);
			default:goto id;
			}
		//FLOAT&FOR
		case 'f':
			token+=nowChar;getChar();
			if(nowChar=='l'){
				recognize("loat",FLOAT);
			}else{
				recognize("or",FOR);
			}
		//GOTO
		case 'g':recognize("goto",GOTO);
		//IF&INT
		case 'i':
			token+=nowChar;getChar();
			if(nowChar=='n'){
				recognize("nt",INT);
			}else{
				recognize("f",IF);
			}
		//LONG
		case 'l':recognize("long",LONG);
		//REGISTER&RETURN
		case 'r':
			token+=nowChar;getChar();
			if(nowChar=='e'){
				token+=nowChar;getChar();
				if(nowChar=='g'){
					recognize("gister",REGISTER);
				}else{
					recognize("turn",RETURN);
				}
			}else goto id;
		//SHORT&SIGNED&SIZEOF&STATIC&STRUCT&SWITCH
		case 's':
			token+=nowChar;getChar();
			switch(nowChar){
			case 'h':recognize("hort",SHORT);
			case 'i':token+=nowChar;getChar();
				if(nowChar=='g'){
					recognize("gned",SIGNED);
				}else{
					recognize("zeof",SIZEOF);
				}
			case 't':token+=nowChar;getChar();
				if(nowChar=='a'){
					recognize("atic",STATIC);
				}else{
					recognize("ruct",STRUCT);
				}
			case 'w':recognize("witch",SWITCH);
			}
		//TYPEDEF
		case 't':recognize("typedef",TYPEDEF);
		//UNION&UNSIGNED
		case 'u':
			token+=nowChar;getChar();
			if(nowChar=='n'){
				token+=nowChar;getChar();
				if(nowChar=='i'){
					recognize("ion",UNION);
				}else{
					recognize("signed",UNSIGNED);
				}
			}else goto id;
		//VOID&VOLATILE
		case 'v':
			token+=nowChar;getChar();
			if(nowChar=='o'){
				token+=nowChar;getChar();
				if(nowChar=='i'){
					recognize("id",VOID);
				}else{
					recognize("latile",VOLATILE);
				}
			}else goto id;
		//WHILE
		case 'w':recognize("while",WHILE);
		case '0':case '1':case '2':case '3':case '4':
		case '5':case '6':case '7':case '8':case '9':
		case '+':case '-':
			token+=nowChar;getChar();
			//识别数前的符号
			if(lastChar=='+'||lastChar=='-'){
				//处理不是数前面的加减号
				if(!(map[nowChar]&DIGIT))
					return nowChar;
				token+=nowChar;getChar();
			}
			if(lastChar=='0'){
				//0开头一定不是十进制数
				if(nowChar=='x'||nowChar=='X'){
					//识别十六进制数
					token+=nowChar;getChar();
					if(!(map[nowChar]&HEX)){
						//必须要有一个十六进制数,否则认为是无效十六进制数
						error("Invalid Hexadecimal Number");
					}
					while(map[nowChar]&HEX){
						token+=nowChar;getChar();
					}
					//获得十六进制值
					int v=0;
					sscanf(token.c_str(),"%x",&v);
					tval.addressed=0;
					//识别u后缀
					if(nowChar=='u'||nowChar=='U'){
						tval.type=unsignedtype;
						token+=nowChar;getChar();
						tval.u.c.v.u=v;
					}else{
						tval.type=inttype;
						tval.u.c.v.i=v;
					}
					return ICON;
				}else{
					//识别八进制数
					if(!('0'<=nowChar&&nowChar<='7')){
						//必须要有一个八进制数,否则认为是无效八进制数
						error("Invalid Octal Number");
					}
					while('0'<=nowChar&&nowChar<='7'){
						token+=nowChar;getChar();
					}
					//如果010.1,010e3应该为浮点数
					if(nowChar=='.'||nowChar=='e'||nowChar=='E')
						goto fcon;
					//获得八进制值
					int v=0;
					sscanf(token.c_str(),"%o",&v);
					tval.addressed=0;
					//识别u后缀
					if(nowChar=='u'||nowChar=='U'){
						tval.type=unsignedtype;
						token+=nowChar;getChar();
						tval.u.c.v.u=v;
					}else{
						tval.type=inttype;
						tval.u.c.v.i=v;
					}
					return ICON;
				}
			}else{
				//识别十进制数整数
				while(map[nowChar]&DIGIT){
					token+=nowChar;getChar();
				}
				//如果10.1,10e3应该为浮点数
				if(nowChar=='.'||nowChar=='e'||nowChar=='E')
					goto fcon;
				//获得十进制值
				int v=0;
				sscanf(token.c_str(),"%d",&v);
				tval.addressed=0;
				//识别u后缀
				if(nowChar=='u'||nowChar=='U'){
					tval.type=unsignedtype;
					token+=nowChar;getChar();
					tval.u.c.v.u=v;
				}else{
					tval.type=inttype;
					tval.u.c.v.i=v;
				}
				return ICON;
			}
		//识别浮点数
		case '.':fcon:
			//能到这里的只有nowChar为.,e,E
			if(nowChar=='.'){
				token+=nowChar;getChar();
				//处理"3.e","a.b"这样.后面没数字的情况
				if(!(map[nowChar]&DIGIT)){
					//这种情况是界符
					if(token==".")
						return '.';
					error("Invalid Floating Constant");
				}
				//读取小数部分
				while(map[nowChar]&DIGIT){
					token+=nowChar;getChar();
				}
			}
			if(nowChar=='e'||nowChar=='E'){
				token+=nowChar;getChar();
				//识别指数前的符号
				if(nowChar=='+'||nowChar=='-'){
					token+=nowChar;getChar();
				}
				//处理"3.5e"这样e后面没数字的情况
				if(!(map[nowChar]&DIGIT)){
					error("Invalid Floating Constant");
				}
				//读取指数
				while(map[nowChar]&DIGIT){
					token+=nowChar;getChar();
				}
			}
			{
				tval.addressed=1;
				if(nowChar=='f'||nowChar=='F'){
					token+=nowChar;getChar();
					float vf=0;
					sscanf(token.c_str(),"%f",&vf);
					tval.u.c.v.f=vf;
					tval.type=floattype;
				}else{
					double vf=0;
					sscanf(token.c_str(),"%lf",&vf);
					tval.u.c.v.d=vf;
					tval.type=doubletype;
				}
				return FCON;
			}
		//识别字符串常量
		case '"':
			{
				token+=nowChar;getChar();
				//escape代表是否遇到了转义字符
				bool escape=false;
				while(escape||nowChar!='\"'){
					if(escape&&nowChar=='\n'){
						escape=false;
						token.pop_back();
						getChar();
					}else if(escape){
						escape=false;
						token+=nowChar;getChar();
					}else if(nowChar=='\\'){
						escape=true;
						token+=nowChar;getChar();
					}else if(nowChar=='\n'){
						error("String missing right quotation marks");
					}else{
						token+=nowChar;getChar();
					}
					if(nowChar==EOF){
						error("String missing right quotation marks");
					}
				}
				token+=nowChar;getChar();
				tval.addressed=1;
				//tval.type=ptr(chartype);
				return SCON;
			}
		//识别字符常量
		case '\'':
			{
				token+=nowChar;getChar();
				//escape代表是否遇到了转义字符
				bool escape=false;
				while(escape||nowChar!='\''){
					if(escape)
						escape=false;
					else if(nowChar=='\\')
						escape=true;
					token+=nowChar;getChar();
					if(nowChar==EOF){
						error("Invalid Character");
					}
				}
				token+=nowChar;getChar();
				int v=0;
				tval.addressed=0;
				tval.type=inttype;
				if(token[1]=='\\'){
					//对于转义字符的识别
					switch(token[2]){
					case 'a':v='\a';break;
					case 'b':v='\b';break;
					case 'f':v='\f';break;
					case 'n':v='\n';break;
					case 'r':v='\r';break;
					case 't':v='\t';break;
					case 'v':v='\v';break;
					//\xhh十六进制转义字符
					case 'x':
						if(map[token[3]]&HEX)
							if(map[token[4]]&HEX){
								char tmp[5]={'0','x',token[3],token[4],0};
								sscanf(tmp,"%x",&v);
							}else{
								char tmp[4]={'0','x',token[3],0};
								sscanf(tmp,"%x",&v);
							}
						else
							v='x';
						break;
					//\ddd八进制转义字符
					case '0':case '1':case '2':case '3':
					case '4':case '5':case '6':case '7':
						if('0'<=token[3]&&token[3]<='7')
							if('0'<=token[4]&&token[4]<='7'){
								char tmp[5]={'0',token[2],token[3],token[4],0};
								sscanf(tmp,"%o",&v);
							}
							else{
								char tmp[4]={'0',token[2],token[3],0};
								sscanf(tmp,"%o",&v);
							}
						else{
							char tmp[3]={'0',token[2],0};
							sscanf(tmp,"%o",&v);
						}
						break;
					default:
						v=token[2];break;
					}
				}else{
					sscanf(token.c_str(),"'%c",&v);
				}
				tval.u.c.v.i=v;
				return ICON;
			}
		}
	}
}