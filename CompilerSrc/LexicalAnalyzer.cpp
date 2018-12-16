#include"LexicalAnalyzer.h"
//<macros>
//<data>
//���ʵ��ַ�����ʾ����token��
std::string token;
//��ǰ�������ַ���ԭ��λ��
int line=1,cols=1;
//tval������ֻ��Ϊgettok�ĵ������ṩ���������ͺ�ֵ
struct symbol tval;
//lastChar����һ�ε���getChar()�Ľ��,nowChar����һ�ε�
char lastChar,nowChar=' ';
//tsymΪĳЩ���ʴ��Symbol
struct symbol* tsym;
//src��ʾ��ǰ������Դ�����е�����
Coordinate src;
//����map[c]���Խ��ַ�c��Ϊ����7�ּ���
char map[256];
//ͨ��ʹ��ȫ�ֱ���t���浱ǰ����
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
		//�����ļ�������
		if(nowChar==-1)
			return EOF;
		//�����ANSI C���ַ�
		if(nowChar<0||(!(map[nowChar]&ANSIC))){
			error("%c isn't ANSI C character",nowChar);
		}
		//�ų��հ��ַ�
		while(map[nowChar]&(BLANK|NEWLINE))
			getChar();
		//���ԭ���ַ���
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
		//�������ַ���ʶ��ͬtoken
		switch(nowChar){
		//ע�ͻ����ǳ���
		case '/':
			token+=nowChar;getChar();
			if(nowChar=='/'){
				//������ע��
				while(getChar()!='\n');
				//�������ڲ������ų��հ��ַ�
				//���Եݹ����
				return getToken();
			}else if(nowChar=='*'){
				//�����ע��
				do
					getChar();
				while(lastChar!='*'||nowChar!='/');
				getChar();
				//�������ڲ������ų��հ��ַ�
				//���Եݹ����
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
				//0��ͷһ������ʮ������
				if(nowChar=='x'||nowChar=='X'){
					//ʶ��ʮ��������
					token+=nowChar;getChar();
					if(!(map[nowChar]&HEX)){
						//����Ҫ��һ��ʮ��������,������Ϊ����Чʮ��������
						error("Invalid Hexadecimal Number");
					}
					while(map[nowChar]&HEX){
						token+=nowChar;getChar();
					}
					//���ʮ������ֵ
					int v=0;
					sscanf(token.c_str(),"%x",&v);
					tval.addressed=0;
					//ʶ��u��׺
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
					//ʶ��˽�����
					if(!('0'<=nowChar&&nowChar<='7')){
						//����Ҫ��һ���˽�����,������Ϊ����Ч�˽�����
						error("Invalid Octal Number");
					}
					while('0'<=nowChar&&nowChar<='7'){
						token+=nowChar;getChar();
					}
					//���010.1,010e3Ӧ��Ϊ������
					if(nowChar=='.'||nowChar=='e'||nowChar=='E')
						goto fcon;
					//��ð˽���ֵ
					int v=0;
					sscanf(token.c_str(),"%o",&v);
					tval.addressed=0;
					//ʶ��u��׺
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
				//ʶ��ʮ����������
				while(map[nowChar]&DIGIT){
					token+=nowChar;getChar();
				}
				//���10.1,10e3Ӧ��Ϊ������
				if(nowChar=='.'||nowChar=='e'||nowChar=='E')
					goto fcon;
				//���ʮ����ֵ
				int v=0;
				sscanf(token.c_str(),"%d",&v);
				tval.addressed=0;
				//ʶ��u��׺
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
		//ʶ�𸡵���
		case '.':fcon:
			//�ܵ������ֻ��nowCharΪ.,e,E
			if(nowChar=='.'){
				token+=nowChar;getChar();
				//����"3.e","a.b"����.����û���ֵ����
				if(!(map[nowChar]&DIGIT)){
					//��������ǽ��
					if(token==".")
						return '.';
					error("Invalid Floating Constant");
				}
				//��ȡС������
				while(map[nowChar]&DIGIT){
					token+=nowChar;getChar();
				}
			}
			if(nowChar=='e'||nowChar=='E'){
				token+=nowChar;getChar();
				//ʶ��ָ��ǰ�ķ���
				if(nowChar=='+'||nowChar=='-'){
					token+=nowChar;getChar();
				}
				//����"3.5e"����e����û���ֵ����
				if(!(map[nowChar]&DIGIT)){
					error("Invalid Floating Constant");
				}
				//��ȡָ��
				while(map[nowChar]&DIGIT){
					token+=nowChar;getChar();
				}
			}
			{//<�����㳣��,�ַ�������,�͸������>
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
		//ʶ���ַ�������
		case '"':{
				token+=nowChar;getChar();
				//escape�����Ƿ�������ת���ַ�
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
		//ʶ���ַ�����
		case '\'':
			{
				token+=nowChar;getChar();
				//escape�����Ƿ�������ת���ַ�
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
					//����ת���ַ���ʶ��
					switch(token[2]){
					case 'a':v='\a';break;
					case 'b':v='\b';break;
					case 'f':v='\f';break;
					case 'n':v='\n';break;
					case 'r':v='\r';break;
					case 't':v='\t';break;
					case 'v':v='\v';break;
					//\xhhʮ������ת���ַ�
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
					//\ddd�˽���ת���ַ�
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
