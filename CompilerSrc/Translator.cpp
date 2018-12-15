#include"Translator.h"
//<macros>
//<data>
std::map<String,std::map<int,int>>m;
std::stack<struct symbol*> sem;
const char* keyword[]={"AUTO","BREAK","CASE","CHAR","CONST","CONTINUE",
	"DEFAULT","DO","DOUBLE","ELSE","ENUM","EXTERN","FLOAT","FOR","GOTO",
	"IF","INT","LONG","RETURN","SHORT","SIGNED","SIZEOF","STATIC","STRUCT",
	"SWITCH","TYPEDEF","UNSIGNED","VOID","WHILE","REGISTER","UNION","VOLATILE",
	"ID","ICON","FCON","SCON","INC","DEC","ZZ","GEQ","LEQ","EQ","NEQ","OR",
	"AND","SHL","SHR",0};
//<functions>
void expect(int token);void BSent();void Store();
void IDevList();void IDecList();void Sent();void Expr();
void Pop();void Uop();
//void ConstExpr();
void StructDim();void SdimList();void Field();void IDList();
void Aexpr();void Cexpr();void Aop();void Bexpr();
void Bop();void Type();void Uexpr();void Sents();
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
void Start(){
	while(1){
		switch(inFirst("Start")){
		case 1:Store();
		case 2:Type();
			//_setvartype
			IDevList();
			expect(';');
			break;
		case 3:expect(TYPEDEF);
			if(t==ID)
				expect(ID);
			else
				Type();
			//_storetype
			expect(ID);
			//_newtype
			expect(';');
			break;
		case 4:expect(ENUM);
			if(t==ID)
				expect(ID);
			//_newenumtype
			expect('{');
			IDecList();
			expect('}');
			expect(';');
			break;
		default:goto out;
		}
	}
out:expect(VOLATILE);
	expect('{');
	if(inFirst("Sents"))
		Sents();
	expect('}');
	expect(EOF);
	//_addend
}
void Sents(){
	switch(inFirst("Sents")){
		case 1:expect(ID);
			expect(':');
			//_addlabel
			break;
		case 2:Sent();
			if(inFirst("Sents"))
				Sents();
			break;
	}
}
void BSent(){
	switch(inFirst("BSent")){
	case 1:Sent();
		break;
	case 2:expect('{');
		if(inFirst("Sents")){
			Sents();
		}
		expect('}');
		break;
	}
}
void BSent_bc(){
	switch(inFirst("BSent_bc")){
	case 1:BSent();
		break;
	case 2:expect(BREAK);
		expect(';');
		//_gotoloopend
		break;
	case 3:expect(CONTINUE);
		expect(';');
		//_gotoloopstart
		break;
	}
}
void Sent(){
	switch(inFirst("Sent")){
	case 1:Store();
	case 2:Type();
		//_setvartype
		IDevList();
		expect(';');
		break;
	case 3:expect(GOTO);
		expect(ID);
		//_gotoid
		expect(';');
		break;
	case 4:expect(RETURN);
		Expr();
		expect(';');
		break;
	case 5:expect(TYPEDEF);
		if(t==ID){
			expect(ID);
		}else
		{
			Type();
		}
		// _storetype
		expect(ID);
		//_newtype
		expect(';');
		break;
	case 6:expect(ENUM);
		if(t==ID){
			expect(ID);
		}
		//__newnumtype
		expect('{');
		IDecList();
		expect('}');
		expect(';');
		break;
	case 7:expect(FOR);
		expect('(');
		Expr();
		expect(')');
		//__loopstart
		Expr();
		expect(';');
		//_loopinc
		Expr();
		expect(')');
		//_loopblock
		BSent_bc();
		//_loopend
		break;
	case 8:expect(IF);
		expect('(');
		Expr();
		expect(')');
		//_ifcmp
		BSent();
		// _ifelse
		if(t==ELSE){
			expect(ELSE);
			BSent();
		}
		//_ifend
		break;
	case 9:Expr();
		expect(';');
	}
}
void Store(){
	switch(inFirst("Store")){
	case 1:expect(AUTO);
		//_setstoreclass
		break;
	case 2:expect(CONST);
		//_setstoreclass
		break;
	case 3:expect(STATIC);
		//_setstoreclass
		break;
	case 4:expect(EXTERN);
		//_setstoreclass
		break;
	}
}
void IDevList(){
	while(1){
		switch(inFirst("IDevList")){
		case 1:expect('*');
			//_ptr
			break;
		default:goto out;
		}
	}
out:expect(ID);
	//_addsym
	if(t=='='){
		expect('=');
		Expr();
	}
	//__setval
	if(t==','){
		expect(',');
		IDevList();
	}
}
void IDecList(){
	expect(ID);
	//if(t=='='){
	//	expect('=');
	//	ConstExpr();
	//	//_savenum
	//}
	//_addenum
	if(t==','){
		expect(',');
		IDecList();
	}
}
void Type(){
	switch(inFirst("Type")){
	case 1:expect(STRUCT);
		if(t==ID){
			expect(ID);
			//_savesid
			if(inFirst("StructDim"))
				StructDim();
		}else{
			//_newunnamedsid
			StructDim();
		}
		return;
	case 2:expect(UNION);
		if(t==ID){
			expect(ID);
			//_saveuid
			if(inFirst("StructDim"))
				StructDim();
		}else{
			//_newunnameduid
			StructDim();
		}
		return;
	case 3:expect(DOUBLE);
		//_setvartyp
		return;
	case 4:expect(FLOAT);
		//_setvartyp
		return;
	case 5:expect(VOID);
		expect('*');
		//_setvartyp
		return;
	case 6:expect(SIGNED);
		break;
	case 7:expect(UNSIGNED);
		// _setvarunsigned
		break;
	case 8:switch(t){
		case CHAR:expect(CHAR);
			//_setvartype
			break;
		case SHORT:expect(SHORT);
			if(t == INT)
				expect(INT);
			//_setvartype
			break;
		case LONG:expect(LONG);
			if(t == INT)
				expect(INT);
			//_setvartype
			break;
		case INT:expect(INT);
			//_setvartype
			break;
		}
		return;
	case 9:expect(ENUM);
		expect(ID);
		return;
	}
	if(t==INT){
		expect(INT);
		//_setvartype
	}else if(t==LONG){
		expect(LONG);
		if(t==INT)
			expect(INT);
		//_setvartype
	}else if(t==SHORT){
		expect(SHORT);
		if(t==INT)
			expect(INT);
		//_setvartype
	}else{
		expect(CHAR);
		//_setvartype
	}
}
void StructDim(){
	expect('{');
	if(inFirst("SdimList"))
		SdimList();
	expect('}');
}
void SdimList(){
	Field();
	expect(';');
	if(inFirst("SdimList"))
		SdimList();
}
void Field(){
	switch(inFirst("Field")){
	case 1:expect(AUTO);
		//_setstoreclass
		break;
	case 2:expect(CONST);
		//_setstoreclass
		break;
	case 3:expect(STATIC);
		break;
	}
	//case 2 4 6
	Type();
	//_setvartype
	IDList();
}
void IDList(){
	while(1){
		switch(inFirst("IDList")){
		case 1:expect('*');
			//_ptr
			break;
		default:goto out;
		}
	}
out:expect(ID);
	//_addfield
	// _clearptr
	if(t==','){
		expect(',');
		IDList();
	}
}
void Expr(){
	Aexpr();
	//_popdeinc
	while(t==','){
		expect(',');
		Aexpr();
		//_popdeinc
	}
}
void Aexpr(){
	switch(inFirst("Aexpr")){
	case 1:Cexpr();
		break;
	case 2:Uexpr();
		Aop();
		//_genqua
		//_storevar
		Aexpr();
		//_popdeinc
		break;
	}
}
void Aop(){
	switch(inFirst("Aop")){
	case 1:expect('+');
		//__setop
		expect('=');
		break;
	case 2:expect('-');
		//__setop
		expect('=');
		break;
	case 3:expect('*');
		//__setop
		expect('=');
		break;
	case 4:expect('/');
		//__setop
		expect('=');
		break;
	case 5:expect('%');
		//__setop
		expect('=');
		break;
	case 6:expect('<');
		expect('<');
		//__setop
		expect('=');
		break;
	case 7:expect('>');
		expect('>');
		//__setop
		expect('=');
		break;
	case 8:expect('&');
		//__setop
		expect('=');
		break;
	case 9:expect('|');
		//__setop
		expect('=');
		break;
	case 10:expect('^');
		//__setop
		expect('=');
		break;
	case 11://__setop
		expect('=');
		break;
	}
}
void Cexpr(){
	Bexpr();
	if(t=='?'){
		expect('?');
		//_popandgt
		Expr();
		expect(':');
		//_else
		Expr();
		//_ifend
	}
}
void Bexpr(){
	Uexpr();
	while(inFirst("Bop")){
		Bop();
		//_setop
		Uexpr();
		//_genqua
	}
}
void Bop(){
	switch(inFirst("Bop")){
	case 1:expect('|');
		if(t=='|')
			expect('|');
		break;
	case 2:expect('&');
		if(t=='&')
			expect('&');
		break;
	case 3:expect('^');
		break;
	case 4:expect('=');
		expect('=');
		break;
	case 5:expect('!');
		expect('=');
		break;
	case 6:expect('<');
		if(t=='=')
			expect('=');
		else if(t=='<')
			expect('<');
		break;
	case 7:expect('>');
		if(t=='=')
			expect('=');
		else if(t=='>')
			expect('>');
		break;
	case 8:expect('+');
		break;
	case 9:expect('-');
		break;
	case 10:expect('*');
		break;
	case 11:expect('/');
		break;
	case 12:expect('%');
		break;
	}
}
void Uexpr(){
	switch(inFirst("Uexpr")) {
	case 1:expect(ID);
		//_push
		while(inFirst("Pop")){
			Pop();
		}
		break;
	case 2:expect(ICON);
		//_push
		while(inFirst("Pop")){
			Pop();
		}
		break;
	case 3:expect(FCON);
		//_push
		while(inFirst("Pop")){
			Pop();
		}
		break;
	case 4:expect(SCON);
		//_push
		while(inFirst("Pop")){
			Pop();
		}
		break;
	case 5:Uop();
		Uexpr();
		break;
	case 6:expect(SIZEOF);
		if(t=='(') {
			expect('(');
			Type();
			expect(')');
		}else{
			Uexpr();
		}
		//_push
		break;
	case 7:expect('(');
		if(inFirst("Type")) {
			Type();
			//_pushtype
			expect(')');
			Uexpr();
		}else{
			Expr();
			expect(')');
			//_poptype
			//_push
			while(inFirst("Pop")){
				Pop();
			}
			break;
		}
	}
}
void Uop(){
	switch(inFirst("Uop")){
	case 1:expect('+');
		if(t=='+')
			expect('+');
		break;
	case 2:expect('-');
		if(t=='-')
			expect('-');
		break;
	case 3:expect('&');
		break;
	case 4:expect('*');
		break;
	case 5:expect('~');
		break;
	case 6:expect('!');
		break;
	}
}
void Pop(){
	switch(inFirst("Pop")){
	case 1:expect('(');
		if(inFirst("Aexpr")){
			Aexpr();
			//_popdeinc
			while(t==','){
				expect(',');
				Aexpr();
				//_popdeinc
			}
		}
		expect(')');
		break;
	case 2:expect('.');
		expect(ID);
		//_tpush
		break;
	case 3:expect('-');
		if(t=='-'){
			expect('-');
			//_pushdec
		}else{
			expect('>');
			expect(ID);
			//_tpush
		}break;
	case 4:expect('+');
		expect('+');
		//_pushinc
		break;
	}
}
//期待下一个是token
void expect(int token){
	if(t==token)
		t=getToken();
	else{
		const char* _keyword[]={"AUTO","BREAK","CASE","CHAR","CONST","CONTINUE",
			"DEFAULT","DO","DOUBLE","ELSE","ENUM","EXTERN","FLOAT","FOR","GOTO",
			"IF","INT","LONG","RETURN","SHORT","SIGNED","SIZEOF","STATIC","STRUCT",
			"SWITCH","TYPEDEF","UNSIGNED","VOID","WHILE","REGISTER","UNION","VOLATILE",
			"ID","ICON","FCON","SCON","++","--","->",">=","<=","==","!=","||",
			"&&","<<",">>",0};
		static char str[20];
		if(AUTO<=t&&t<=SHR){
			strcpy(str,_keyword[t-AUTO]);
			if(t<=SCON)
				for(int i=0;str[i];i++)
					str[i]+='a'-'A';
		}else
			sprintf(str,"'%c'",t);
		fprintf(stderr,"Error %d,%d:expected %s\n",src.x,src.y,str);
	}
}
