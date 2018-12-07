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
//	if(map[lastChar]&NEWLINE)
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
		//识别整数
		case '0':case '1':case '2':case '3':case '4':
		case '5':case '6':case '7':case '8':case '9':
		case '+':case '-':
			token+=nowChar;getChar();
			//识别数前的符号
			if(lastChar=='+'||lastChar=='-'){
				token+=nowChar;getChar();
			}
			if(lastChar=='0'){
				//0开头一定不是十进制数
				if(nowChar=='x'||nowChar=='X'){
					//识别十六进制数
					token+=nowChar;getChar();
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
				//处理"3.e"这样.后面没数字的情况
				if(!(map[nowChar]&DIGIT)){
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
//		//识别字符常量
//		case '\'':
//			token+=nowChar;getChar();
//			//escape代表是否遇到了转义字符
//			bool escape=false;
//			while(escape||nowChar!='\''){
//				if(escape)escape=false;
//				if(nowChar=='\\')
//					escape=true;
//				token+=nowChar;
//				getChar();
//				if(nowChar==EOF){
//					error("Invalid Character");
//				}
//			}
//			token+=nowChar;getChar();
//			//<对于'\071'的识别>
//			int v=0;
//			sscanf(token.c_str(),"'%c",&v);
//			tval.addressed=0;
//			tval.u.c.v.i=v;
//			tval.type=inttype;
//			return ICON;
//		}
//	}
//}