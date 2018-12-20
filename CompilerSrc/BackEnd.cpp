#include"BackEnd.h"
//<macros>
//<data>
//两个文件指针,用于文件输出
FILE* fobj;
FILE* fasm;
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

//<functions>

//两个重命名函数,明确语义,自动换行,就像puts
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

//后端初始化函数
void backEndInit(){
	fobj=fopen("_a.o.txt","w");
	fasm=fopen("_a.asm.txt","w");
	asmSay("includelib kernel32.lib");
	asmSay(".model flat,stdcall");
	asmSay("ExitProcess proto,dwExitCode:dword");
	asmSay(".code");
}

//告诉后端,编译完成,发送结束四元式
void sendEnd(){
	objSay("(ed,,,)");
	asmSay("@main:");
	asmSay("\tmov ebp,offset auto");
	asmSay("\tcall main");
	asmSay("\tpush 0h");
	asmSay("\tcall ExitProcess");

	asmSay(".data");
	asmSay("\tfret dq 0");
	asmSay("\tfcmp dd 0");
	asmSay("\tconst db 0 dup(0)");
	for(int i=0;i<constVector.size();i++){
		Symbol&now=constVector[i];
		int op=now->type->op;
		if(op==TYPE_FLOAT){
			asmSay("\t\tdd %u",now->u.c.u);
		}else if(op==TYPE_DOUBLE){
			asmSay("\t\tdq %ull",now->u.c.uu);
		}else{
			asmSay("\t\tdb %s",
				now->name.substr(1,now->name.size()-2));
		}
	}
	asmSay("\tstatic db %d dup(0)",staticTable->size);
	asmSay("\tauto db 65536 dup(0)");
	asmSay("end @main");
	fclose(fobj);
	fclose(fasm);
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

//根据操作数大小,返回e?x,?x,?l
#define whatReg(small,BIG) \
const char* what##BIG##Reg(int size){\
	switch(size){\
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
char* asmHex(unsigned num){
	static char str[20]={'0'};
	sprintf(str+1,"%XH",num);
	return str+('0'<=str[1]&&str[1]<='9');
}

//返回一个的Symbol地址字符串表示
const char* address(Symbol&s){
	int sz=s->type->size;
	int of=s->offset;
	static char str[50];
	if(s->sclass==SCLASS_STATIC){
		//静态区变量
		sprintf(str,"%s static[%s]",whatPtr(sz),asmHex(of));
	}else if(s->sclass==SCLASS_CONST){
		if(s->addressed){
			//常量区常量
			if(isFloat(s->type->op)){
				//浮点常量
				sprintf(str,"%s const[%s]",whatPtr(sz),asmHex(of));
			}else{
				//字符串常量
				sprintf(str,"%s const[%s]",whatPtr(1),asmHex(of));
			}
		}else{
			//指令常量
			sprintf(str,"%s",asmHex(s->u.c.u));
		}
	}else{
		//动态区变量
		sprintf(str,"%s ebp[%s]",whatPtr(sz),asmHex(of));
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
			asmSay("\tcmp 0,%s",address(tmp));
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
			asmSay("\tcmp 0,%s",address(tmp));
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
/*
func proto stdcall\
	:dword,\
	:dword


include data.inc
.code

func proc stdcall\
	b:dword,\
	d:dword
	mov eax,b
	add eax,d
	ret 8
func endp
*/
//生成函数各阶段代码
void sendFunc(enum FUNC_STATE state,std::queue<Symbol>sq){
	switch(state){
	case FUNC_DEFINE:
		asmSay("%s proc stdcall\\");
		
	case FUNC_CALL:;
	}
}

void sendFloatOp(enum OP op,Symbol first,Symbol second,Symbol result);
//告诉后端,生成(op,first,second,result)这样的四元式
void sendOp(enum OP op,Symbol first,Symbol second,Symbol result){
	objSay("(%s,%s,%s,%s)",op_str[op],first->name.c_str(),
		second->name.c_str(),result->name.c_str());
	if(isFloat(first->type->op)||isFloat(second->type->op)||isFloat(result->type->op)){
		//存在浮点数,调用浮点数的sendOP
		sendFloatOp(op,first,second,result);
		return;
	}
	int fsz=first->type->size;
	int ssz=second->type->size;
	int rsz=result->type->size;
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
			if(eax!=first){
				eax=first;
				asmSay("\tmov %s,%s",
					whatAReg(fsz),
					address(first)
				);
			}
			//操作之后eax存到不是任何符号的内容了
			eax=NULL;
			asmSay("\t%s %s,%s",str,
				whatAReg(ssz),
				address(second)
			);
			//存了之后,eax和result的内容是一样的了
			eax=result;
			asmSay("\tmov %s,%s",
				address(result),
				whatAReg(rsz)
			);
			break;
		}
	case _SHL:case _SHR:
		if(eax!=first){
			eax=first;
			asmSay("\tmov %s,%s",
				whatAReg(fsz),
				address(first)
			);
		}
		if(ecx!=second){
			ecx=second;
			asmSay("\tmov %s,%s",
				whatCReg(fsz),
				address(first)
			);
		}
		//操作之后eax存到不是任何符号的内容了
		eax=NULL;
		asmSay("\t%s %s,cl",
			op==_SHL?"shl":"shr",
			whatAReg(ssz)
		);
		//存了之后,eax和result的内容是一样的了
		eax=result;
		asmSay("\tmov %s,%s",
			address(result),
			whatAReg(rsz)
		);
		break;
	case _MUL:
		if(eax!=first){
			eax=first;
			asmSay("\tmov %s,%s",
				whatAReg(fsz),
				address(first)
			);
		}
		//操作之后eax存到不是任何符号的内容了
		eax=NULL;
		asmSay("\timul %s,%s",
			whatAReg(ssz),
			address(second)
		);
		//存了之后,eax和result的内容是一样的了
		eax=result;
		asmSay("\tmov %s,%s",
			address(result),
			whatAReg(rsz)
		);
		break;
	case _DIV:
		if(eax!=first){
			eax=first;
			asmSay("\tmov eax,%s",address(first));
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
			whatAReg(rsz)
		);
		break;
	case _MOD:
		if(eax!=first){
			eax=first;
			asmSay("\tmov eax,%s",address(first));
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
			whatDReg(rsz)
		);
		break;
	case _NEG:
		if(first!=result){
			eax=first;
			asmSay("\tmov %s,%s",
				whatAReg(fsz),
				address(first)
			);
			asmSay("\tneg %s",whatAReg(fsz));
			//存了之后,eax和result的内容是一样的了
			eax=result;
			asmSay("\tmov %s,%s",
				address(result),
				whatAReg(rsz)
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
	case _LEA:
		asmSay("\tlea eax,%s",address(first));
		asmSay("\tmov %s,%s",address(result),
			whatAReg(rsz)
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
			asmSay("\tmov %s,%s",
				whatPtr(fsz),
				address(first)
			);
			asmSay("\tcmp %s,%s",
				whatAReg(ssz),
				address(second)
			);
			asmSay("\tmov ebx,0");
			asmSay("\t%s $+3",str);
			asmSay("\tinc ebx");
			asmSay("\tmov %s,%s",address(result),
				whatBReg(rsz)
			);
			break;
		}
	case _NOT:
		asmSay("\tmov %s,%s",
			whatAReg(fsz),
			address(first)
		);
		asmSay("\tcmp %s,0",whatAReg(fsz));
		asmSay("\tmov %s,0",whatBReg(fsz));
		asmSay("\tjnz $+3");
		asmSay("\tinc %s",whatBReg(fsz));
		asmSay("\tmov %s,%s",
			address(result),
			whatBReg(fsz)
		);
		break;
	case _MOV:
		if(eax!=first){
			eax=first;
			asmSay("\tmov %s,%s",
				whatAReg(fsz),
				address(first)
			);
		}
		//存了之后,eax和result的内容是一样的了
		if(eax!=result){
			eax=result;
			asmSay("\tmov %s,%s",
				address(result),
				whatAReg(rsz)
			);
		}
		break;
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
		str2=isFloat(second->type->op)?"":"i";
		asmSay("\tf%sld %s",str2,address(second));
		str2=isFloat(first->type->op)?"":"i";
		asmSay("\tf%sld %s",str2,address(first));
		asmSay("\t%s",str1);
		str2=isFloat(result->type->op)?"":"i";
		asmSay("\tf%sstp %s",str2,address(result));
		break;
	case _AND:str1="jnp";x=1;goto _AND_OR_;
	case _OR:str1="jp";x=0;goto _AND_OR_;
_AND_OR_:
		str2=isFloat(first->type->op)?"":"i";
		asmSay("\tf%sld %s",str2,address(first));
		asmSay("\tfldz");
		asmSay("\tfucompp");
		asmSay("\tfnstsw ax");
		asmSay("\ttest ah,44h");
		asmSay("\t%s $%d$",str1,tmpid);

		str2=isFloat(second->type->op)?"":"i";
		asmSay("\tf%sld %s",str2,address(second));
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
			asmSay("\tmov %s,%s",address(result),whatAReg(result->type->size));
		}
		break;
	case _NOT:
		str2=isFloat(first->type->op)?"":"i";
		asmSay("\tf%sld %s",str2,address(first));
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
			asmSay("\tmov %s,%s",address(result),whatAReg(result->type->size));
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
		str2=isFloat(first->type->op)?"":"i";
		asmSay("\tf%sld %s",str2,address(first));
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
		str2=isFloat(first->type->op)?"":"i";
		asmSay("\tf%sld %s",str2,address(first));
		str2=isFloat(second->type->op)?"":"i";
		asmSay("\tf%sld %s",str2,address(second));
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
			asmSay("\tmov %s,%s",address(result),whatAReg(result->type->size));
		}
		break;
	case _MOV:
		str2=isFloat(first->type->op)?"":"i";
		asmSay("\tf%sld %s",str2,address(first));
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
	int sz=val->type->size;
	objSay("(ret,,,val)");
	asmSay("\tmov %s,%s",whatAReg(sz),address(val));
	asmSay("\tmov %s fret,%s",whatPtr(sz),whatAReg(sz));
}

//通知后端,声明了一个s符号
void tellVar(Symbol s){
	if(s->sclass==SCLASS_STATIC){
		s->offset=staticTable->size;
		staticTable->size+=s->type->size;
	}else if(s->sclass==SCLASS_CONST){
		if(s->addressed){
			s->offset=constTable->size;
			constTable->size+=s->type->size;
			//放到常数声明表中
			constVector.push_back(s);
		}
	}else if(s->sclass==SCLASS_AUTO){
		if(s->temporary){
			s->offset=identifierTable->size;
			identifierTable->size+=8;
		}else{
			s->offset=identifierTable->size;
			identifierTable->size+=s->type->size;
		}
	}else
		assert(0);
}

//通知后端,声明了一个t类型
void tellType(struct type* t){
	for(struct type*i=t->kid;i;i=i->next){
		if(isConst(t->op)&&isInteger(t->op)){
			continue;
		}
		if(isConst(t->op)){
			t->offset=constTable->size;
			constTable->size+=t->size;
			//放到常数声明表中
			constVector.push_back(t->constFloat);
		}
	}
}