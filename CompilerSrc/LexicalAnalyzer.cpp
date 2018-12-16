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
//通常使用全局变量t保存当前单词
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
		for(const char*p="~!%^&*()_+-=[]{}\\|;':\",.<>/?";*p;p++)//????
			map[*p]|=ANSIC;
	}
}
void lexAnalyzerInit(){
	mapInit();
	t=getToken();
}
int getChar(){
	lastChar=nowChar;
	nowChar=getchar();
	putchar(nowChar);
	cols++;
	if(lastChar>0&&map[lastChar]&NEWLINE)
		line++,cols=1;
	return nowChar;
}
int getToken(){
	//if(token=="")
	//	putchar(lastChar),putchar('\n');
	//else printf("%s\n",token.c_str());
	while(1){
		//处理文件结束符
		if(nowChar==-1)
			return EOF;
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
		if(map[nowChar]&IDPART){
			token+=nowChar;getChar();
			while(map[nowChar]&IDPART){
				token+=nowChar;getChar();
			}
			for(int i=0,j;i<ID-AUTO;i++){
				for(j=0;j<token.size()&&keyword[i][j];j++)
					if(token[j]!=keyword[i][j]+'a'-'A')
						break;
				if(!keyword[i][j]&&j==token.size())
					return i+AUTO;
			}return ID;
		}
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
			}else if(nowChar=='='){
				getChar();
                return DIV_ASSIGN;///=
			}else{
				return '/';
			}
		case '~':case '(':
		case ')':case '[':case ']':case '{':case '}':
		case ';':case ':':case ',':case '?':case '\\':
			getChar();
			return lastChar;
        case '*':getChar();
            if(nowChar=='='){
                getChar();
                return MUL_ASSIGN;//*=
            }return lastChar;//*
        case '%':getChar();
            if(nowChar=='='){
                getChar();
                return MOD_ASSIGN;//%=
            }return lastChar;//%
        case '^':getChar();
            if(nowChar=='='){
                getChar();
                return XOR_ASSIGN;//^=
            }return lastChar;//^
		case '>':getChar();
			if(nowChar==lastChar){
				getChar();
				if(nowChar=='='){
                    getChar();
                    return SHR_ASSIGN;//>>=
                }
				return SHR;//>>
			}else if(nowChar=='='){
				getChar();
				return GEQ;//>=
			}return lastChar;//>
		case '<':getChar();
			if(nowChar==lastChar){
				getChar();
				if(nowChar=='='){
                    getChar();
                    return SHL_ASSIGN;//<<=
                }
				return SHL;//<<
			}else if(nowChar=='='){
				getChar();
				return LEQ;//<=
			}return lastChar;//<
		case '=':getChar();
			if(nowChar==lastChar){
				getChar();
				return EQ;//==
			}return lastChar;//=
		case '!':getChar();
			if(nowChar=='='){
				getChar();
				return NEQ;//!=
			}return lastChar;//!
		case '&':getChar();
			if(nowChar==lastChar){
				getChar();
				return AND;//&&
			}else if(nowChar=='='){
				getChar();
				return AND_ASSIGN;//&=
			}return lastChar;//&
		case '|':getChar();
			if(nowChar==lastChar){
				getChar();
				return OR;//||
			}else if(nowChar=='='){
				getChar();
				return OR_ASSIGN;//|=
			}return lastChar;//|
		case '+':
			getChar();
			if(nowChar==lastChar){
				getChar();
				return INC;//++
			}else if(nowChar=='='){
			    getChar();
			    return ADD_ASSIGN;//+=
			}return lastChar;//+
		case '-':
			getChar();
			if(nowChar==lastChar){
				getChar();
				return DEC;//--
			}else if(nowChar=='>'){
				getChar();
				return ZZ;//->
			}else if(nowChar=='='){
                getChar();
                return SUB_ASSIGN;//-=
            }return lastChar;//-
		case '0':case '1':case '2':case '3':case '4':
		case '5':case '6':case '7':case '8':case '9':
			token+=nowChar;getChar();
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
						tval.type=btot(TYPE_UINT);
						token+=nowChar;getChar();
						tval.u.c.v.u=v;
					}else{
						tval.type=btot(TYPE_INT);
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
						tval.type=btot(TYPE_UINT);
						token+=nowChar;getChar();
						tval.u.c.v.u=v;
					}else{
						tval.type=btot(TYPE_INT);
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
					tval.type=btot(TYPE_UINT);
					token+=nowChar;getChar();
					tval.u.c.v.u=v;
				}else{
					tval.type=btot(TYPE_INT);
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
			{//<处理浮点常量,字符串常量,和浮点操作>
				tval.addressed=1;
				if(nowChar=='f'||nowChar=='F'){
					token+=nowChar;getChar();
					float vf=0;
					sscanf(token.c_str(),"%f",&vf);
					tval.u.c.v.f=vf;
					tval.type=btot(TYPE_FLOAT);
				}else{
					double vf=0;
					sscanf(token.c_str(),"%lf",&vf);
					tval.u.c.v.d=vf;
					tval.type=btot(TYPE_DOUBLE);
				}
				return FCON;
			}
		//识别字符串常量
		case '"':{
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
				tval.type=btot(TYPE_INT);
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
		assert(0);
	}
}
