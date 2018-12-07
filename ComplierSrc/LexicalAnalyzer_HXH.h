//#include"LexicalAnalyzer.h"
////<macros>
////<data>
////���ʵ��ַ�����ʾ����token��
//std::string token;
////��ǰ�������ַ���ԭ��λ��
//int line=1,cols=1;
////tval������ֻ��Ϊgettok�ĵ������ṩ���������ͺ�ֵ
//struct symbol tval;
////lastChar����һ�ε���getChar()�Ľ��,nowChar����һ�ε�
//char lastChar,nowChar=' ';
////tsymΪĳЩ���ʴ��Symbol
//struct symbol* tsym;
////src��ʾ��ǰ������Դ�����е�����
//Coo src;
////����map[c]���Խ��ַ�c��Ϊ����6�ּ���
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
//		//�����ANSI C���ַ�
//		if(!(map[nowChar]&ANSIC)){
//			error("Can't accept %c",nowChar);
//		}
//		//�ų��հ��ַ�
//		while(map[nowChar]&(BLANK|NEWLINE))
//			getChar();
//		//���ԭ���ַ���
//		token="";
//		src.x=line;
//		src.y=cols;
//		//�������ַ���ʶ��ͬtoken
//		switch(nowChar){
		//ʶ������
		case '0':case '1':case '2':case '3':case '4':
		case '5':case '6':case '7':case '8':case '9':
		case '+':case '-':
			token+=nowChar;getChar();
			//ʶ����ǰ�ķ���
			if(lastChar=='+'||lastChar=='-'){
				token+=nowChar;getChar();
			}
			if(lastChar=='0'){
				//0��ͷһ������ʮ������
				if(nowChar=='x'||nowChar=='X'){
					//ʶ��ʮ��������
					token+=nowChar;getChar();
					while(map[nowChar]&HEX){
						token+=nowChar;getChar();
					}
					//���ʮ������ֵ
					int v=0;
					sscanf(token.c_str(),"%x",&v);
					tval.addressed=0;
					//ʶ��u��׺
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
					//ʶ��˽�����
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
					tval.type=unsignedtype;
					token+=nowChar;getChar();
					tval.u.c.v.u=v;
				}else{
					tval.type=inttype;
					tval.u.c.v.i=v;
				}
				return ICON;
			}
		//ʶ�𸡵���
		case '.':fcon:
			//�ܵ������ֻ��nowCharΪ.,e,E
			if(nowChar=='.'){
				token+=nowChar;getChar();
				//����"3.e"����.����û���ֵ����
				if(!(map[nowChar]&DIGIT)){
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
//		//ʶ���ַ�����
//		case '\'':
//			token+=nowChar;getChar();
//			//escape�����Ƿ�������ת���ַ�
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
//			//<����'\071'��ʶ��>
//			int v=0;
//			sscanf(token.c_str(),"'%c",&v);
//			tval.addressed=0;
//			tval.u.c.v.i=v;
//			tval.type=inttype;
//			return ICON;
//		}
//	}
//}