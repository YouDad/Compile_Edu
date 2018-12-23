#include"BackEnd.h"
//<macros>
//<data>
//三个文件指针,用于文件输出
FILE* fobj;
FILE* fasm;
FILE* fdim;
//ENUM OP的字符串表示
const char* op_str[]={
	"ADD","SUB","MUL","DIV","MOD","SHL","SHR","NOT",
	"NEG","INC","DEC","LEA","PTR","EQU","NEQ","GTR",
	"LES","GEQ","LEQ","AND","OR","BTA","BTO","BTX",
	"BTN","MOV"
};
//eax寄存器所保存的符号
Symbol eax,ecx;
//保存常数声明的顺序
Vector<Symbol> constVector;
//临时标号id
int tmpid;
//临时变量用了tmpsz的空间
int tmpsz=0;
//返回特殊内存地址的时候需要加ptr
//specialSz就是指明了另一个操作数的大小
int specialSz;
//碰到main函数时,符号表大小
int mainSize;
Symbol CON;
Symbol FRET;
Symbol FCMP;
//<functions>

//三个重命名函数,明确语义,自动换行,就像puts
void objSay(const char* fmt,...){
    va_list ap;
    va_start(ap,fmt);
	vfprintf(fobj,fmt,ap);
	fprintf(fobj,"\n");
    va_end(ap);
}
void asmSay(const char* fmt,...){
    va_list ap;
    va_start(ap,fmt);
	vfprintf(fasm,fmt,ap);
	fprintf(fasm,"\n");
    va_end(ap);
}
void dimSay(const char* fmt,...){
    va_list ap;
    va_start(ap,fmt);
	vfprintf(fdim,fmt,ap);
	fprintf(fdim,"\n");
    va_end(ap);
}

//后端初始化函数
void backEndInit(){
	fobj=fopen("E:/Programming Source/C++/asm_try/asm_try/a.o","w");
	fasm=fopen("E:/Programming Source/C++/asm_try/asm_try/a.asm","w");
	fdim=fopen("E:/Programming Source/C++/asm_try/asm_try/a.inc","w");
	/*fobj=fopen("a.o","w");
	fasm=fopen("a.asm","w");
	fdim=fopen("a.inc","w");*/
	dimSay("includelib kernel32.lib");
	dimSay(".model flat,stdcall");
	dimSay("ExitProcess proto,dwExitCode:dword");
	dimSay("printf proto,c:vararg");
	dimSay("scanf proto,c:vararg");
	asmSay("include a.inc");
	asmSay(".code");
	CON=newSymbol("con",btot(TYPE_INT),SCLASS_STATIC,src);
	FRET=newSymbol("fret",btot(TYPE_INT),SCLASS_STATIC,src);
	FCMP=newSymbol("fcmp",btot(TYPE_INT),SCLASS_STATIC,src);
}

//告诉后端进入或者退出作用域
void enterScopeB(){
	if(level>SCOPE_LOCAL){
		asmSay("add esi,%d",identifierTable->previous->size);
	}
}
void exitScopeB(){
	if(level>=SCOPE_LOCAL){
		asmSay("sub esi,%d",identifierTable->size);
	}
}

int charVal(const char*&s);
//告诉后端,编译完成,发送结束四元式
void sendEnd(){
	objSay("(ed,,,)");
	asmSay("@main:");
	asmSay("\tmov esi,offset auto");
	asmSay("\tadd esi,%d",mainSize);
	asmSay("\tmov ebx,esi");
	asmSay("\tcall ?main");
	asmSay("\tpush 0h");
	asmSay("\tcall ExitProcess");
	asmSay("end @main");

	dimSay(".data");
	dimSay("\tcon dq 0");
	dimSay("\tfret dq 0");
	dimSay("\tfcmp dd 0");
	dimSay("\tftmp dd 0");
	dimSay("\tconst db 0 dup(0)");
	for(int i=0;i<constVector.size();i++){
		Symbol&now=constVector[i];
		int op=now->type->op;
		if(op==TYPE_FLOAT){
			dimSay("\t\tdd %u",now->u.c.u);
		}else if(op==TYPE_DOUBLE){
			dimSay("\t\tdq %llu",now->u.c.uu);
		}else{
			fprintf(fdim,"\t\tdb ");
			const char* str=now->name.c_str()+1;
			while(str[1])
				fprintf(fdim,"%d,",charVal(str));
			dimSay("0");
		}
	}
	dimSay("\tstatic db %d dup(0)",staticTable->size);
	dimSay("\tauto db 65536 dup(0)");
	dimSay("\ttemp db 65536 dup(0)");
	fclose(fobj);
	fclose(fasm);
	fclose(fdim);
}

//根据操作数大小,返回byte/word/dword/qword ptr
const char* whatPtr(int size){
	switch(size){
	case 1:return "byte ptr";
	case 2:return "word ptr";
	case 4:return "dword ptr";
	case 8:return "qword ptr";
	default:
		//操作数大小有问题
		assert(0);
		return "";
	}
}

//根据大小,返回byte,word,dword,qword
const char* whatSize(int size){
	switch(size){
	case 1:return "byte";
	case 2:return "word";
	case 4:return "dword";
	case 8:return "qword";
	default:
		//操作数大小有问题
		assert(0);
		return "";
	}
}

//根据操作数大小,返回e?x,?x,?l
#define whatReg(small,BIG) \
const char* what##BIG##Reg(Symbol s){\
	int sz=s->type->size;\
	if(isArray(s->type->op))sz=4;\
	switch(sz){\
	case 1:return #small"l";\
	case 2:return #small"x";\
	case 4:return "e"#small"x";\
	default:\
		assert(0);\
		return "";\
	}\
}
whatReg(a,A)
whatReg(b,B)
whatReg(c,C)
whatReg(d,D)

//返回汇编形式的十六进制数
char* asmHex(int num){
	static char str[20]={'0'};
	sprintf(str,"%d",num);
	return str;
}

//返回一个的Symbol地址字符串表示
const char* address(Symbol&s,int funcOffset=0){
	static char str[50];
	if(s==FRET){
		sprintf(str,"%s fret",whatPtr(specialSz));
		return str;
	}
	if(s==FCMP){
		sprintf(str,"%s fcmp",whatPtr(specialSz));
		return str;
	}
	if(s==CON){
		sprintf(str,"%s con",whatPtr(specialSz));
		return str;
	}
	int sz=s->type->size;
	if(isArray(s->type->op))
		sz=4;
	int of=s->offset;
	int lev=level;
	if(s->scope==SCOPE_GLOBAL){
		sprintf(str,"%s [%d+auto]",
			whatPtr(sz),of);
		return str;
	}
	if(s->scope==SCOPE_PARAM){
		sprintf(str,"%s %s",
			whatPtr(sz),
			s->name.c_str()
		);
		return str;
	}
	SymbolTable tab=identifierTable;
	while(s->scope>=SCOPE_GLOBAL&&
		s->addressed&&s->scope<lev){
		tab=tab->previous;lev--;
		of-=tab->size;
	}
	if(s->needebx){
		Symbol same=s;
		same->needebx=0;
		asmSay("\tmov ebx,%s",address(same));
		same->needebx=1;
		sprintf(str,"%s [ebx]",whatPtr(sz));
		return str;
	}
	if(s->sclass==SCLASS_STATIC){
		//静态区变量
		sprintf(str,"%s [%s+static]",whatPtr(sz),asmHex(of));
	}else if(s->sclass==SCLASS_CONST){
		if(s->addressed){
			//常量区常量
			if(isFloat(s->type->op)){
				//浮点常量
				sprintf(str,"%s [%s+const]",whatPtr(sz),asmHex(of));
			}else{
				//字符串常量
				sprintf(str,"offset const+%s",asmHex(of));
			}
		}else{
			//指令常量
			sprintf(str,"%s",asmHex(s->u.c.u));
		}
	}else{
		//动态区变量
		if(s->temporary)
			sprintf(str,"%s [%s+temp]",whatPtr(sz),asmHex(of));
		else
			sprintf(str,"%s [%s+esi]",whatPtr(sz),asmHex(of-funcOffset));
	}
	return str;
}

//告诉后端,生成id为forId的循环的第state阶段四元式和代码
void sendFor(int forId,enum FOR_STATE state){
	switch(state){
	case FOR_CMP:
		objSay("(lb,$for%dcmp,,)",forId);
		asmSay("$for%dcmp:",forId);
		break;
	case FOR_INC:{
			//语义栈必须有符号,是比较表达式的值
			assert(sem.size());
			//取出表达式的值
			Symbol tmp=sem.top();sem.pop();
			objSay("(if,%s,$for%dblk,$for%dend)",
				tmp->name.c_str(),forId,forId);
			asmSay("\tcmp %s,0",address(tmp));
			asmSay("\tjz $for%dend",forId);
			asmSay("\tjmp $for%dblk",forId);
			objSay("(lb,$for%dinc,,)",forId);
			asmSay("$for%dinc:",forId);
			break;
		}
	case FOR_BLOCK:
		assert(sem.size());sem.pop();
		objSay("(gt,$for%dcmp,,)",forId);
		asmSay("\tjmp $for%dcmp",forId);
		objSay("(lb,$for%dblk,,)",forId);
		asmSay("$for%dblk:",forId);
		break;
	case FOR_END:
		objSay("(gt,$for%dinc,,)",forId);
		asmSay("\tjmp $for%dinc",forId);
		objSay("(lb,$for%dend,,)",forId);
		asmSay("$for%dend:",forId);
		break;
	case FOR_BREAK:
		objSay("(gt,$for%dend,,)",forId);
		asmSay("\tjmp $for%dend",forId);
		break;
	case FOR_CONTINUE:
		objSay("(gt,$for%dinc,,)",forId);
		asmSay("\tjmp $for%dinc",forId);
		break;
	}
}

//告诉后端,生成id为ifId的条件的第state阶段四元式和代码
void sendIf(int ifId,enum IF_STATE state){
	switch(state){
	case IF_THEN:{
			//语义栈必须有符号,是比较表达式的值
			assert(sem.size());
			//取出表达式的值
			Symbol tmp=sem.top();sem.pop();
			objSay("(if,%s,,$if%delse)",
				tmp->name.c_str(),ifId,ifId);
			asmSay("\tcmp %s,0",address(tmp));
			asmSay("\tjz $if%delse",ifId);
			break;
		}
	case IF_ELSE:
		objSay("(gt,$if%dend,,)",ifId);
		asmSay("\tjmp $if%dend",ifId);
		objSay("(lb,$if%delse,,)",ifId);
		asmSay("$if%delse:",ifId);
		break;
	case IF_END:
		objSay("(lb,$if%dend,,)",ifId);
		asmSay("$if%dend:",ifId);
		break;
	}
}

//生成函数各阶段代码
void sendFunc(enum FUNC_STATE state,std::stack<Symbol>ss){
	switch(state){
	case FUNC_DEFINE:{
			Symbol s=ss.top();
			assert(s->type->op==TYPE_FUNCTION);
			if(s->type->kid->next){
				if(s->name=="main"){
					asmSay("?main proc stdcall");
					dimSay("?main proto stdcall");
					SymbolTable t=identifierTable;
					while(t->level>=SCOPE_GLOBAL)
						mainSize+=t->size,
						t=t->previous;
				}else{
					asmSay("%s proc stdcall\\",s->name.c_str());
					dimSay("\t\t%s proto stdcall\\",s->name.c_str());
				}
				struct type*i;
				for(i=s->type->kid->next;i->next;i=i->next)
					asmSay("\t\t%s:%s,\\",i->name.c_str(),whatSize(i->size));
				asmSay("\t\t%s:%s",i->name.c_str(),whatSize(i->size));
				for(i=s->type->kid->next;i->next;i=i->next)
					dimSay("\t\t%s:%s,\\",i->name.c_str(),whatSize(i->size));
				dimSay("\t\t%s:%s",i->name.c_str(),whatSize(i->size));
			}else{
				if(s->name=="main"){
					asmSay("?main proc stdcall");
					dimSay("?main proto stdcall");
					SymbolTable t=identifierTable;
					while(t->level>=SCOPE_GLOBAL)
						mainSize+=t->size,
						t=t->previous;
				}else{
					asmSay("%s proc stdcall",s->name.c_str());
					dimSay("%s proto stdcall",s->name.c_str());
				}
			}
		}break;
	case FUNC_END:{
			Symbol s=ss.top();
			assert(s->type->op==TYPE_FUNCTION);
			if(s->name=="main")
				asmSay("?main endp");
			else
				asmSay("%s endp",s->name.c_str());
		}break;
	case FUNC_CALL:
		asmSay("\tpush esi");
		asmSay("\tadd esi,%s",asmHex(identifierTable->size));
		Symbol function=ss.top();ss.pop();
		int addsize=0;
		while(ss.size()){
			objSay("(arg,%s,,)",ss.top()->name.c_str());
			if((ss.top()->type->op&127)==TYPE_DOUBLE){
				asmSay("\tsub esp,8");
				asmSay("\tfld %s",address(ss.top()));
				asmSay("\tfstp qword ptr [esp]");
			}else{
				asmSay("\tpush %s",address(ss.top(),identifierTable->size));
			}
			if(isArray(ss.top()->type->op))
				addsize+=4;
			else addsize+=ss.top()->type->size;
			ss.pop();
		}
		asmSay("\tcall %s",function->name.c_str());
		if((int)function->type==1)
			asmSay("\tadd esp,%s",asmHex(addsize));
		asmSay("\tpop esi");
		objSay("(call,%s,,)",function->name.c_str());
		break;
	}
}

void sendFloatOp(enum OP op,Symbol first,Symbol second,Symbol result);
//告诉后端,生成(op,first,second,result)这样的四元式
void sendOp(enum OP op,Symbol first,Symbol second,Symbol result){
	if(isConst(result->type->op))
		error("Constant can't be change");
	if(result->temporary&&result->addressed==0)
		error("Constant can't be left value");
	if(second){
		objSay("(%s,%s,%s,%s)",op_str[op],first->name.c_str(),
			second->name.c_str(),result->name.c_str());
	}else{
		objSay("(%s,%s,,%s)",op_str[op],first->name.c_str(),
			result->name.c_str());
	}
	if(op==_MOV||op==_LEA||op==_NEG||op==_NOT||op==_INC||op==_DEC||op==_BTN)
		asmSay(";%s=%s %s",result->name.c_str(),op_str[op],first->name.c_str());
	else
		asmSay(";%s=%s %s %s",result->name.c_str(),first->name.c_str(),
			op_str[op],second->name.c_str());
	if(isFloat(first->type->op)||(second&&isFloat(second->type->op))||isFloat(result->type->op)){
		//存在浮点数,调用浮点数的sendOP
		sendFloatOp(op,first,second,result);
		return;
	}
	const char* mov="mov";
	if(first->addressed&&isArray(first->type->op)&&
		!(second&&second->addressed&&isArray(second->type->op)))
		if(op!=_MOV)
			mov="lea";
	switch(op){
	case _ADD:case _SUB:case _AND:case _OR:
	case _BTA:case _BTO:case _BTX:{
			const char* str="or";
			switch(op){
				case _ADD:str="add";break;
				case _SUB:str="sub";break;
				case _AND:str="and";break;
				case _BTA:str="and";break;
				case _BTX:str="xor";break;
			}
			eax=first;
			asmSay("\t%s %s,%s",mov,
				whatAReg(first),
				address(first)
			);
			if((op==_ADD||op==_SUB)&&
				(isPointer(first->type->op)||isArray(first->type->op))&&
				!((isPointer(second->type->op)||isArray(second->type->op)))){
				asmSay("\tmov %s,%s",
					whatBReg(second),
					address(second)
				);
				asmSay("\timul %s,%s",
					whatBReg(second),
					asmHex(first->type->kid->size)
				);
				eax=NULL;
				asmSay("\tadd %s,%s",
					whatAReg(second),
					whatBReg(second)
				);
			}else{
				//操作之后eax存到不是任何符号的内容了
				eax=NULL;
				asmSay("\t%s %s,%s",str,
					whatAReg(second),
					address(second)
				);
			}
			asmSay("\tmov %s,%s",
				address(result),
				whatAReg(result)
			);
			break;
		}
	case _SHL:case _SHR:
		if(eax!=first){
			eax=first;
			asmSay("\t%s %s,%s",mov,
				whatAReg(first),
				address(first)
			);
		}
		if(ecx!=second){
			ecx=second;
			asmSay("\tmov %s,%s",
				whatCReg(first),
				address(first)
			);
		}
		//操作之后eax存到不是任何符号的内容了
		eax=NULL;
		asmSay("\t%s %s,cl",
			op==_SHL?"shl":"shr",
			whatAReg(second)
		);
		//存了之后,eax和result的内容是一样的了
		eax=result;
		asmSay("\tmov %s,%s",
			address(result),
			whatAReg(result)
		);
		break;
	case _MUL:
		if(eax!=first){
			eax=first;
			asmSay("\t%s %s,%s",mov,
				whatAReg(first),
				address(first)
			);
		}
		//操作之后eax存到不是任何符号的内容了
		eax=NULL;
		asmSay("\timul %s,%s",
			whatAReg(second),
			address(second)
		);
		//存了之后,eax和result的内容是一样的了
		eax=result;
		asmSay("\tmov %s,%s",
			address(result),
			whatAReg(result)
		);
		break;
	case _DIV:
		if(eax!=first){
			eax=first;
			asmSay("\t%s eax,%s",mov,address(first));
		}
		//进行一波符号扩展
		asmSay("\tcdq");
		//操作之后eax存到不是任何符号的内容了
		if(ecx!=second){
			ecx=second;
			asmSay("\tmovsx ecx,%s",address(second));
		}
		asmSay("\tidiv ecx");
		//存了之后,eax和result的内容是一样的了
		eax=result;
		asmSay("\tmov %s,%s",
			address(result),
			whatAReg(result)
		);
		break;
	case _MOD:
		if(eax!=first){
			eax=first;
			asmSay("\t%s eax,%s",mov,address(first));
		}
		//进行一波符号扩展
		asmSay("\tcdq");
		//操作之后eax存到不是任何符号的内容了
		if(ecx!=second){
			ecx=second;
			asmSay("\tmovsx ecx,%s",address(second));
		}
		asmSay("\tidiv ecx");
		//存了之后,eax和result的内容是一样的了
		eax=result;
		asmSay("\tmov %s,%s",
			address(result),
			whatDReg(result)
		);
		break;
	case _NEG:
		if(first!=result){
			eax=first;
			asmSay("\t%s %s,%s",mov,
				whatAReg(first),
				address(first)
			);
			asmSay("\tneg %s",whatAReg(first));
			//存了之后,eax和result的内容是一样的了
			eax=result;
			asmSay("\tmov %s,%s",
				address(result),
				whatAReg(result)
			);
		}else{
			//因为是同一个位置,就直接neg
			asmSay("\tneg %s",address(result));
		}
		break;
	case _BTN:case _INC:case _DEC:
		asmSay("\t%s %s",
			op==_INC?"inc":op==_NOT?"not":"dec",
			address(first)
		);
		break;
	case _PTR:
		asmSay("\tmov ebx,%s",address(first));
		asmSay("\tmov %s,%s[ebx]",
			whatBReg(result),
			whatPtr(result->type->size)
		);
		asmSay("\tmov %s,%s",
			address(result),
			whatBReg(result)
		);
		break;
	case _LEA:
		asmSay("\tlea eax,%s",address(first));
		asmSay("\tmov %s,%s",address(result),
			whatAReg(result)
		);
		break;
	case _EQU:case _NEQ:case _GTR:case _LES:
	case _GEQ:case _LEQ:{
			const char* str="je";
			//是否为无符号数
			bool usign=isUnsigned(first->type->op)&&isUnsigned(second->type->op);
			switch(op){
			case _NEQ:str="jne";break;
			case _GTR:str=usign?"ja":"jg";break;
			case _LES:str=usign?"jb":"jl";break;
			case _GEQ:str=usign?"jae":"jge";break;
			case _LEQ:str=usign?"jbe":"jle";break;
			}
			asmSay("\t%s %s,%s",mov,
				whatAReg(first),
				address(first)
			);
			asmSay("\tcmp %s,%s",
				whatAReg(second),
				address(second)
			);
			asmSay("\tmov ebx,1");
			asmSay("\t%s $+3",str);
			asmSay("\tdec ebx");
			asmSay("\tmov %s,%s",
				address(result),
				whatBReg(result)
			);
			break;
		}
	case _NOT:
		asmSay("\t%s %s,%s",mov,
			whatAReg(first),
			address(first)
		);
		asmSay("\tcmp %s,0",whatAReg(first));
		asmSay("\tmov %s,0",whatBReg(first));
		asmSay("\tjnz $+3");
		asmSay("\tinc %s",whatBReg(first));
		asmSay("\tmov %s,%s",
			address(result),
			whatBReg(first)
		);
		break;
	case _MOV:
		if(first->needebx==result->needebx){
			if(!compatibleType(first->type,result->type))
				error("Type error, Can't assign");
		}else if(first->needebx){
			if(!compatibleType(first->type->kid,result->type))
				error("Type error, Can't assign");
		}
		eax=first;
		asmSay("\t%s %s,%s",mov,
			whatAReg(first),
			address(first)
		);
		specialSz=first->type->size;
		eax=result;
		asmSay("\tmov %s,%s",
			address(result),
			whatAReg(result)
		);
		break;
	}
}

//处理浮点压栈情况
void f_ld(Symbol s){
	if(isFloat(s->type->op))
		asmSay("\tfld %s",address(s));
	else{
		asmSay("\tmov ftmp,%s",address(s));
		asmSay("\tfild ftmp");
	}
}

//处理浮点情况
void sendFloatOp(enum OP op,Symbol first,Symbol second,Symbol result){
	//int fsz=first->type->size;
	//int ssz=second->type->size;
	//int rsz=result->type->size;
	const char* str1,* str2;
	int x;
	switch(op){
	case _ADD:str1="fadd";goto BinaryOperator;
	case _SUB:str1="fsub";goto BinaryOperator;
	case _MUL:str1="fmul";goto BinaryOperator;
	case _DIV:str1="fdiv";goto BinaryOperator;
BinaryOperator:
		f_ld(first);
		f_ld(second);
		asmSay("\t%s",str1);
		str2=isFloat(result->type->op)?"":"i";
		asmSay("\tf%sstp %s",str2,address(result));
		break;
	case _AND:str1="jnp";x=1;goto _AND_OR_;
	case _OR:str1="jp";x=0;goto _AND_OR_;
_AND_OR_:
		f_ld(first);
		asmSay("\tfldz");
		asmSay("\tfucompp");
		asmSay("\tfnstsw ax");
		asmSay("\ttest ah,44h");
		asmSay("\t%s $%d$",str1,tmpid);

		f_ld(second);
		asmSay("\tfldz");
		asmSay("\tfucompp");
		asmSay("\tfnstsw ax");
		asmSay("\ttest ah,44h");
		asmSay("\t%s $%d$",str1,tmpid);

		asmSay("\tmov fcmp,%d",x);
		asmSay("\tjmp $%d$N",tmpid);
		asmSay("$%d$:",tmpid);
		asmSay("\tmov fcmp,%d",1-x);

		asmSay("$%d$N:",tmpid++);
		if(isFloat(result->type->op)){
			asmSay("\tfild fcmp");
			asmSay("\tfstp %s",address(result));
		}else{
			asmSay("\tmov eax,fcmp");
			asmSay("\tmov %s,%s",address(result),whatAReg(result));
		}
		break;
	case _NOT:
		f_ld(first);
		asmSay("\tfldz");
		asmSay("\tfucompp");
		asmSay("\tfnstsw ax");
		asmSay("\ttest ah,44h");
		asmSay("\tjp $%d$",tmpid);
		asmSay("\tmov fcmp,1");
		asmSay("\tjmp $%d$N",tmpid);
		asmSay("$%d$:",tmpid);
		asmSay("\tmov fcmp,0");

		asmSay("$%d$N:",tmpid++);
		if(isFloat(result->type->op)){
			asmSay("\tfild fcmp");
			asmSay("\tfstp %s",address(result));
		}else{
			asmSay("\tmov eax,fcmp");
			asmSay("\tmov %s,%s",address(result),whatAReg(result));
		}
		break;
	case _NEG:
		str2=isFloat(first->type->op)?"":"i";
		asmSay("\tf%sld %s",str2,address(first));
		asmSay("\tfchs");
		asmSay("\tfstp %s",address(result));
		break;
	case _INC:str1="fadd";goto _INC_DEC_;
	case _DEC:str1="fsub";goto _INC_DEC_;
_INC_DEC_:
		asmSay("\tfld1");
		f_ld(first);
		asmSay("\t%s",str1);
		str2=isFloat(result->type->op)?"":"i";
		asmSay("\tf%sstp %s",str2,address(result));
		break;
	case _EQU:x=44;str1="jp"; goto _GLE_;
	case _NEQ:x=44;str1="jnp";goto _GLE_;
	case _GTR:x=05;str1="jp"; goto _GLE_;
	case _GEQ:x=41;str1="jp"; goto _GLE_;
	case _LES:x=41;str1="jne";goto _GLE_;
	case _LEQ:x=01;str1="jne";goto _GLE_;
_GLE_:
		f_ld(first);
		f_ld(second);
		asmSay("\tfucompp");
		asmSay("\tfnstsw ax");
		asmSay("\ttest ah,%02dh",x);
		asmSay("\t%s $%d$",str1,tmpid);
		asmSay("\tmov fcmp,1");
		asmSay("\tjmp $%d$N",tmpid);
		asmSay("$%d$:",tmpid);
		asmSay("\tmov fcmp,0");
		asmSay("$%d$N:",tmpid++);
		if(isFloat(result->type->op)){
			asmSay("\tfild fcmp");
			asmSay("\tfstp %s",address(result));
		}else{
			asmSay("\tmov eax,fcmp");
			asmSay("\tmov %s,%s",address(result),whatAReg(result));
		}
		break;
	case _MOV:
		f_ld(first);
		specialSz=first->type->size;
		str2=isFloat(result->type->op)?"":"i";
		asmSay("\tf%sstp %s",str2,address(result));
		break;
	default:
		//应该不能进来
		//不支持的操作
		assert(0);
		break;
	}
}

//告诉后端,生成(ret,val,,fret)这样的四元式
void sendRet(struct symbol*val){
	objSay("(ret,,,val)");
	sendOp(_MOV,val,0,FRET);
	asmSay("\tret");
}

//告诉后端,生成(=,s,,con)四元式
void sendCON(struct symbol*s){
	objSay("(=,%s,,con)",s->name.c_str());
	sendOp(_MOV,s,0,CON);
}

int charVal(const char*&s){
	int v;
	if(s[0]!='\\'){
		char c=s++[0];
		return c;
	}
	//对于转义字符的识别
	switch(s[1]){
	case 'a':s+=2;return'\a';
	case 'b':s+=2;return'\b';
	case 'f':s+=2;return'\f';
	case 'n':s+=2;return'\n';
	case 'r':s+=2;return'\r';
	case 't':s+=2;return'\t';
	case 'v':s+=2;return'\v';
	//\xhh十六进制转义字符
	case 'x':
		if(isHex(s[2])){
			char tmp[5]={'0','x',s[2],s[3],0};
			s+=3;
			if(!isHex(s[3]))
				tmp[3]=0;
			else
				s++;
			sscanf(tmp,"%x",&v);
			return v;
		}else{
			s+=2;
			return'x';
		}
		break;
	//\ddd八进制转义字符
	case '0':case '1':case '2':case '3':
	case '4':case '5':case '6':case '7':{
			char tmp[5]={'0',s[1],s[2],s[3],0};
			if(isOct(tmp[2])){
				s+=3;
				if(!isOct(tmp[3]))
					tmp[3]=0;
				else
					s++;
			}else{
				tmp[2]=0;
				s+=2;
			}
			sscanf(tmp,"%o",&v);
			return v;
		}
	default:{
			char c=s[1];
			s+=2;
			return c;
		}
	}
}

//通知后端,声明了一个s符号
void tellVar(Symbol s){
	if(s->sclass==SCLASS_STATIC){
		s->offset=staticTable->size;
		staticTable->size+=s->type->size;
	}else if(s->sclass==SCLASS_CONST){
		if(s->addressed){
			s->offset=constTable->size;
			if(isFloat(s->type->op)){
				constTable->size+=s->type->size;
			}else{
				const char* str=s->name.c_str()+1;
				while(*str){
					charVal(str);
					constTable->size++;
				}
			}
			//放到常数声明表中
			constVector.push_back(s);
		}
	}else if(s->sclass==SCLASS_AUTO){
		if(s->temporary){
			s->offset=tmpsz;
			tmpsz+=8;
		}else{
			s->offset=identifierTable->size;
			identifierTable->size+=s->type->size;
		}
	}else
		assert(0);
}