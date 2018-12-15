#include"Translator.h"
//<macros>
//<data>
std::map<String,std::map<int,int>>m;
//<functions>
void expect(int token);void BSent();void Store();
void IDevList();void IDecList();void Sent();void Expr();
void Pop();void Uop();
//void ConstExpr();
void StructDim();void SdimList();void Field();void IDList();
void Aexpr();void Cexpr();void Aop();void Bexpr();
void Bop();void Type();void Uexpr();void Sents();

int trans(char*s){
		 if(strcmp(s,"AUTO"))return AUTO;
	else if(strcmp(s,"BREAK"))return BREAK;
	else if(strcmp(s,"CASE"))return CASE;
	else if(strcmp(s,"CHAR"))return CHAR;
	else if(strcmp(s,"CONST"))return CONST;
	else if(strcmp(s,"CONTINUE"))return CONTINUE;
	else if(strcmp(s,"DEFAULT"))return DEFAULT;
	else if(strcmp(s,"DO"))return DO;
	else if(strcmp(s,"DOUBLE"))return DOUBLE;
	else if(strcmp(s,"ELSE"))return ELSE;
	else if(strcmp(s,"ENUM"))return ENUM;
	else if(strcmp(s,"EXTERN"))return EXTERN;
	else if(strcmp(s,"FLOAT"))return FLOAT;
	else if(strcmp(s,"FOR"))return FOR;
	else if(strcmp(s,"GOTO"))return GOTO;
	else if(strcmp(s,"IF"))return IF;
	else if(strcmp(s,"INT"))return INT;
	else if(strcmp(s,"LONG"))return LONG;
	else if(strcmp(s,"RETURN"))return RETURN;
	else if(strcmp(s,"SHORT"))return SHORT;
	else if(strcmp(s,"SIGNED"))return SIGNED;
	else if(strcmp(s,"SIZEOF"))return SIZEOF;
	else if(strcmp(s,"STATIC"))return STATIC;
	else if(strcmp(s,"STRUCT"))return STRUCT;
	else if(strcmp(s,"SWITCH"))return SWITCH;
	else if(strcmp(s,"TYPEDEF"))return TYPEDEF;
	else if(strcmp(s,"UNSIGNED"))return UNSIGNED;
	else if(strcmp(s,"VOID"))return VOID;
	else if(strcmp(s,"WHILE"))return WHILE;
	else if(strcmp(s,"REGISTER"))return REGISTER;
	else if(strcmp(s,"UNION"))return UNION;
	else if(strcmp(s,"VOLATILE"))return VOLATILE;
	else if(strcmp(s,"ID"))return ID;
	else if(strcmp(s,"ICON"))return ICON;
	else if(strcmp(s,"FCON"))return FCON;
	else if(strcmp(s,"SCON"))return SCON;
	else{
		char c;
		sscanf(s,"'%c'",&c);
		return c;
	}
}

void initTranslator(){
	FILE*fp=fopen("first.txt","r");
	char str[20]={},head[20];int ret;
	while(~fscanf(fp,"%s %d:",head,&ret)){
		std::map<int,int>mm;
		while(str[0]!='\n'){
			fscanf(fp,"%[^`\n]%c",str+1,str);
			mm[trans(str+1)]=ret;
		}m[str]=mm;
	}fclose(fp);
}

int inFirst(String s){
	return m[s][t];
}


void Start(){
	while (1){
		switch(inFirst("Start")){
		case 1:Store();
		case 2:
			Type();
			//_setvartype
			IDevList();
			expect(';');
			break;
		case 3:
			expect(TYPEDEF);
			if(t==ID)
				expect(ID);
			else
				Type();
			//_storetype
			expect(ID);
			//_newtype
			expect(';');
			break;
		case 4:
			expect(ENUM);
			if(t==ID)
				expect(ID);
			//_newenumtype
			expect('{');
			IDecList();
			expect('}');
			expect(';');
			break;
		default:
			goto out;
		}
	}
out:expect(VOLATILE);
	expect('{');
	Sents();
	expect('}');
	expect(EOF);
	//_addend
}

void Sents(){
	switch (inFirst("Sents")){
		case 1:
			expect(ID);
			expect(':');
			//_addlabel
		case 2:
			Sent();
			if(inFirst("Sents"))
				Sents();
			break;
	}
}

void BSent(){
	switch (inFirst("BSent")){
	case 1:
		Sent();
		break;
	case 2:
		expect('{');
		if(inFirst("Sents")){
			Sents();
		}
		expect('}');
		break;
	}
}

void BSent_bc(){
	switch (inFirst("BSent_bc")){
	case 1:
		BSent();
		break;
	case 2:
		expect(BREAK);
		expect(';');
		//_gotoloopend
		break;
	case 3:
		expect(CONTINUE);
		expect(';');
		//_gotoloopstart
		break;
	}
}

void Sent(){
	switch (inFirst("Sent")){
	case 1:Store();
	case 2:
		Type();
		//_setvartype
		IDevList();
		expect(';');
		break;
	case 3:
		expect(GOTO);
		expect(ID);
		//_gotoid
		expect(';');
		break;
	case 4:
		expect(RETURN);
		Expr();
		expect(';');
		break;
	case 5:
		expect(TYPEDEF);
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
	case 6:
		expect(ENUM);
		if(t==ID){
			expect(ID);
		}
		//__newnumtype
		expect('{');
		IDecList();
		expect('}');
		expect(';');
		break;
	case 7:
		expect(FOR);
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
	case 8:
		expect(IF);
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
	case 9:
		Expr();
		expect(';');
	}
}

void Store(){
	switch(inFirst("Store")){
	case 1:
		expect(EXTERN);
		//_setstoreclass
		break;
	case 2:
		expect(AUTO);
		//_setstoreclass
		break;
	case 3:
		expect(CONST);
		//_setstoreclass
		break;
	case 4:
		expect(STATIC);
		//_setstoreclass
		break;
	}
}

void IDevList(){
	while (1){
		switch (inFirst("IDevList")){
		case 1:
			expect('*');
			//_ptr
			break;

		default:
			goto out;
		}
	}
out:
	expect(ID);
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
	switch (inFirst("Type")){
	case 1:
		expect(STRUCT);
		if(t==ID){
			expect(ID);
			//_savesid
			if(inFirst("StructDim")){
				StructDim();
			}
		}else{
			//_newunnamedsid
			StructDim();
		}
		return;
	case 2:
		expect(UNION);
		if(t==ID){
			//_saveuid
			if(inFirst("StructDim")){
				StructDim();
			}else
				//_newunnameduid
				StructDim();
		}
		return;
	case 3:
		expect(DOUBLE);
		//_setvartyp
		return;
	case 4:
		expect(FLOAT);
		//_setvartyp
		return;
	case 5:
		expect(VOID);
		expect('*');
		//_setvartyp
		return;
	case 6:
		expect(SIGNED);
		break;
	case 7:
		expect(UNSIGNED);
		// _setvarunsigned
		break;
	case 8:
		switch (t)
		{
		case CHAR:
			expect(CHAR);
			//_setvartype
			break; 
		case SHORT:
			expect(SHORT);
			if (t == INT)
				expect(INT);
			//_setvartype
			break;
		case LONG:
			expect(LONG);
			if (t == INT)
				expect(INT);
			//_setvartype
			break;
		case INT:
			expect(INT);
			//_setvartype
			break;
		}
		return;
	case 9:
		expect(ENUM);
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
	if(inFirst("SdimList")){
		SdimList();
	}
	expect('}');
}

void SdimList(){
	Field();
	expect(';');
	if(inFirst("SdimList")){
		SdimList();
	}
}

void Field(){
	switch (inFirst("Field")){
	case 1:
		expect(AUTO);
		//_setstoreclass
		break;
	case 3:
		expect(CONST);
		//_setstoreclass
		break;
	case 5:
		expect(STATIC);
		break;
	}
	//case 2 4 6
	Type();
	//_setvartype
	IDList();
}

void IDList(){
	while(1){

		switch (inFirst("IDList")){
		case 1:
			expect('*');
			//_ptr
			break;

		default:
			goto out;
		}
	}
out:
	expect(ID);
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
	while (t==',')/////////////////////////检查
	{
		expect(',');
		Aexpr();
		//_popdeinc
	}
}

void Aexpr(){
	switch (inFirst("Aexpr")){
	case 1:
		Cexpr();
		break;
	case 2:
		Uexpr();
		Aop();
		//_genqua
		//_storevar
		Aexpr();
		//_popdeinc
		break;

	}


}

void Aop(){
	switch (inFirst("Aop")){
	case 1:
		expect('+');
		//__setop
		expect('=');
		break;
	case 2:
		expect('-');
		//__setop
		expect('=');
		break;
	case 3:
		expect('*');
		//__setop
		expect('=');
		break;
	case 4:
		expect('/');
		//__setop
		expect('=');
		break;
	case 5:
		expect('%');
		//__setop
		expect('=');
		break;
	case 6:
		expect('<');
		expect('<');
		//__setop
		expect('=');
		break;
	case 7:
		expect('>');
		expect('>');
		//__setop
		expect('=');
		break;
	case 8:
		expect('&');
		//__setop
		expect('=');
		break;
	case 9:
		expect('|');
		//__setop
		expect('=');
		break;
	case 10:
		expect('^');
		//__setop
		expect('=');
		break;
	case 11:
		//__setop
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
	while (inFirst("Bop")){
		Bop();
		//_setop
		Uexpr();
		//_genqua
	}
}

void Bop(){
	switch (inFirst("Bop")){
	case 1:
		expect('|');
		if(t=='|')
			expect('|');
		break;
	case 2:
		expect('&');
		if(t=='&')
			expect('&');
		break;
	case 3:
		expect('^');
		break;
	case 4:
		expect('=');
		expect('=');
		break;
	case 5:
		expect('!');
		expect('=');
		break;
	case 6:
		expect('<');
		if(t=='=')
			expect('=');
		else if(t=='<')
			expect('<');
		break;
	case 7:
		expect('>');
		if(t=='=')
			expect('=');
		else if(t=='>')
			expect('>');
		break;
	case 8:
		expect('+');
		break;
	case 9:
		expect('-');
		break;
	case 10:
		expect('*');
		break;
	case 11:
		expect('/');
		break;
	case 12:
		expect('%');
		break;
	}
}

void Uexpr(){
	switch (inFirst("Uexpr")) {
	case 1:
		expect(ID);
		//_push
		while (inFirst("Pop")){
			Pop();
		}
		break;
	case 2:
		expect(ICON);
		//_push
		while (inFirst("Pop")){
			Pop();
		}
		break;
	case 3:
		expect(FCON);
		//_push
		while (inFirst("Pop")){
			Pop();
		}
		break;
	case 4:
		expect(SCON);
		//_push
		while (inFirst("Pop")){
			Pop();
		}
		break;
	case 5:
		Uop();
		Uexpr();
		break;
	case 6:
		expect(SIZEOF);
		if(t=='(') {
			expect('(');
			Type();
			expect(')');
		}else{
			Uexpr();
		}
		//_push
		break;
	case 7:
		expect('(');
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
			while (inFirst("Pop")){
				Pop();
			}
			break;
		}
	}
}

void Uop(){
	switch (inFirst("Uop")){
	case 1:
		expect('+');
		if(t=='+')
			expect('+');
		break;
	case 2:
		expect('-');
		if(t=='-')
			expect('-');
		break;
	case 3:
		expect('&');
		break;
	case 4:
		expect('*');
		break;
	case 5:
		expect('~');
		break;
	case 6:
		expect('!');
		break;
	}
}

void Pop(){
	switch (inFirst("Pop")){
	case 1:
		expect('(');
		if(inFirst("Aexpr")){
			Aexpr();
			//_popdeinc
			while (t==','){
				expect(',');
				Aexpr();
				//_popdeinc
			}
		}
		expect(')');
		break;
	case 2:
		expect('.');
		expect(ID);
		//_tpush
		break;
	case 3:
		expect('-');
		if(t=='-'){
			expect('-');
			//_pushdec
		}else
		{
			expect('>');
			expect(ID);
			//_tpush
		}
		break;
	case 4:
		expect('+');
		expect('+');
		//_pushinc
		break;
	}
}
//期待下一个是token
void expect(int token){
	if(t==token)
		t = getToken();
	else{
		static char str[20];
		switch(t){
		case INT:		strcpy(str,"int"		);break;
		case AUTO:		strcpy(str,"auto"		);break;
		case BREAK:		strcpy(str,"break"		);break;
		case CASE:		strcpy(str,"case"		);break;
		case CHAR:		strcpy(str,"char"		);break;
		case CONST:		strcpy(str,"const"		);break;
		case CONTINUE:	strcpy(str,"continue"	);break;
		case DEFAULT:	strcpy(str,"default"	);break;
		case DO:		strcpy(str,"do"			);break;
		case DOUBLE:	strcpy(str,"double"		);break;
		case ELSE:		strcpy(str,"else"		);break;
		case ENUM:		strcpy(str,"enum"		);break;
		case EXTERN:	strcpy(str,"extern"		);break;
		case FLOAT:		strcpy(str,"float"		);break;
		case FOR:		strcpy(str,"for"		);break;
		case GOTO:		strcpy(str,"goto"		);break;
		case IF:		strcpy(str,"if"			);break;
		case LONG:		strcpy(str,"long"		);break;
		case RETURN:	strcpy(str,"return"		);break;
		case SHORT:		strcpy(str,"short"		);break;
		case SIGNED:	strcpy(str,"signed"		);break;
		case SIZEOF:	strcpy(str,"sizeof"		);break;
		case STATIC:	strcpy(str,"static"		);break;
		case STRUCT:	strcpy(str,"struct"		);break;
		case SWITCH:	strcpy(str,"switch"		);break;
		case TYPEDEF:	strcpy(str,"typedef"	);break;
		case UNSIGNED:	strcpy(str,"unsigned"	);break;
		case VOID:		strcpy(str,"void"		);break;
		case WHILE:		strcpy(str,"while"		);break;
		case REGISTER:	strcpy(str,"register"	);break;
		case UNION:		strcpy(str,"union"		);break;
		case VOLATILE:	strcpy(str,"volatile"	);break;
		case ID:		strcpy(str,"identifier"	);break;
		case ICON:		strcpy(str,"integer"	);break;
		case FCON:		strcpy(str,"float"		);break;
		case SCON:		strcpy(str,"string"		);break;
		default:
			sprintf(str,"'%c'",t);
		}
		fprintf(stderr,"Error %d,%d:expected %s\n",src.x,src.y,str);
	}
}
