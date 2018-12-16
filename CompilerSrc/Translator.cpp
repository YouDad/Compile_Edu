#include"Translator.h"
//<macros>
//<data>
std::map<String,std::map<int,int>>m;
std::stack<struct symbol*> sem;
const char* keyword[]={"AUTO","BREAK","CASE","CHAR","CONST","CONTINUE",
	"DEFAULT","DO","DOUBLE","ELSE","ENUM","EXTERN","FLOAT","FOR","GOTO",
	"IF","INT","LONG","RETURN","SHORT","SIGNED","SIZEOF","STATIC","STRUCT",
	"SWITCH","TYPEDEF","UNSIGNED","VOID","WHILE","REGISTER","UNION","VOLATILE",
	"ID","ICON","FCON","SCON","INC_OP","DEC_OP","PTR_OP","GE_OP","LE_OP","EQ_OP",
	"NEQ_OP","OR_OP","AND_OP","SHL_OP","SHR_OP","MUL_ASSIGN","DIV_ASSIGN","MOD_ASSIGN",
	"ADD_ASSIGN","SUB_ASSIGN","SHL_ASSIGN","SHR_ASSIGN","AND_ASSIGN","XOR_ASSIGN","OR_ASSIGN",0};
//<functions>
//期待下一个是token
void expect(int token){
	if(t==token)
		t=getToken();
	else{
		const char* _keyword[]={"AUTO","BREAK","CASE","CHAR","CONST","CONTINUE",
			"DEFAULT","DO","DOUBLE","ELSE","ENUM","EXTERN","FLOAT","FOR","GOTO",
			"IF","INT","LONG","RETURN","SHORT","SIGNED","SIZEOF","STATIC","STRUCT",
			"SWITCH","TYPEDEF","UNSIGNED","VOID","WHILE","REGISTER","UNION","VOLATILE",
			"identifier","integer","float","string","++","--","->",">=","<=","==","!=","||",
			"&&","<<",">>","*=","/=","%=","+=","-=","<<=",">>=","&=","^=","|=",0};
		static char str[20];
		if(AUTO<=t&&t<=OR_ASSIGN){
			strcpy(str,_keyword[t-AUTO]);
			if(t<=SCON)
				for(int i=0;str[i];i++)
					str[i]+='a'-'A';
		}else
			sprintf(str,"'%c'",t);
		fprintf(stderr,"Error %d,%d:expected %s\n",src.x,src.y,str);
	}
}
int trans(char*s){
	for(int i=0;keyword[i];i++)
		if(strcmp(s,keyword[i])==0)
			return i+AUTO;
	char c;
	sscanf(s,"'%c'",&c);
	return c;
}
void translatorInit(){
	FILE*fp=fopen("first.txt","r");
	char str[20]={},head[20];int ret;
	while(~fscanf(fp,"%s %d:",head,&ret)){
		while(str[0]!='\n'){
			fscanf(fp,"%[^`\n]%c",str+1,str);
			m[head][trans(str+1)]=ret;
		}
		str[0]=0;
	}fclose(fp);
}
int inFirst(const char* s){
	if(m[s].count(t))
		return m[s][t];
	return 0;
}
void translation_unit(){
	//海哥,梦从这里开始
}