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
	asmSay("main proc");
	asmSay("\tmov ebp,offset auto");
}

//告诉后端,编译完成,发送结束四元式
void sendEnd(){
	objSay("(ed,_,_,_)");
	asmSay("\tpush 0h");
	asmSay("\tcall ExitProcess");
	asmSay("main endp");
	asmSay(".data");
	asmSay("\tfcmp dw 0");
	asmSay("\tconst db 0 dup(0)");
	for(int i=0;i<constVector.size();i++){
		Symbol&now=constVector[i];
		int op=now->type->op;
		if(op==TYPE_FLOAT){
			asmSay("\t\tdd %u",now->u.c.v.u);
		}else if(op==TYPE_DOUBLE){
			asmSay("\t\tdq %ull",now->u.c.v.uu);
		}else{
			asmSay("\t\tdb %s",
				now->name.substr(1,now->name.size()-2));
		}
	}
	asmSay("\tstatic db %d dup(0)",staticTable->size);
	asmSay("\tauto db 65536 dup(0)");
	asmSay("end main");
	fclose(fobj);
	fclose(fasm);
}

//告诉后端,生成一个id为labelId的标签
void sendLabel(int labelId){
	struct label lb=getLabel(labelId);

	//必须是已经出现定义的才能生成
	assert(lb.defined==1);

	objSay("(lb,%d,_,_)",labelId);
	asmSay("$%s:",lb.name.c_str());
}

//告诉后端,跳转到id为labelId的标签
void sendGt(int labelId){
	struct label lb=getLabel(labelId);
	objSay("gt,%d,_,_)",labelId);
	asmSay("\tjmp $%s",lb.name.c_str());
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
			sprintf(str,"%s",asmHex(s->u.c.v.u));
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
		objSay("(lb,$for%dcmp,_,_)",forId);
		asmSay("$for%dcmp:",forId);
		//语义栈必须有符号,是初始化表达式的值
		assert(sem.size());
		//不过这个值没有用
		sem.pop();
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
			objSay("(lb,$for%dinc,_,_)",forId);
			asmSay("$for%dinc:",forId);
			break;
		}
	case FOR_BLOCK:
		objSay("(gt,$for%dcmp,_,_)",forId);
		asmSay("\tjmp $for%dcmp",forId);
		objSay("(lb,$for%dblk,_,_)",forId);
		asmSay("$for%dblk:",forId);
		break;
	case FOR_END:
		objSay("(gt,$for%dinc,_,_)",forId);
		asmSay("\tjmp $for%dinc",forId);
		objSay("(lb,$for%dend,_,_)",forId);
		asmSay("$for%dend:",forId);
		break;
	case FOR_BREAK:
		objSay("(gt,$for%dend,_,_)",forId);
		asmSay("\tjmp $for%dend",forId);
		break;
	case FOR_CONTINUE:
		objSay("(gt,$for%dinc,_,_)",forId);
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
			objSay("(if,%s,_,$if%delse)",
				tmp->name.c_str(),ifId,ifId);
			asmSay("\tcmp 0,%s",address(tmp));
			asmSay("\tjz $if%delse",ifId);
			break;
		}
	case IF_ELSE:
		objSay("(gt,$if%dend,_,_)",ifId);
		asmSay("\tjmp $if%dend",ifId);
		objSay("(lb,$if%delse,_,_)",ifId);
		asmSay("$if%delse:",ifId);
		break;
	case IF_END:
		objSay("(lb,$if%dend,_,_)",ifId);
		asmSay("$if%dend:",ifId);
		break;
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
			op==SHL?"shl":"shr",
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
	int fsz=first->type->size;
	int ssz=second->type->size;
	int rsz=result->type->size;
	const char* str1,* str2,* str3;
	switch(op){
	case _ADD:str1="fadd";goto BinaryOperator;
	case _SUB:str1="fsub";goto BinaryOperator;
	case _MUL:str1="fmul";goto BinaryOperator;
	case _DIV:str1="fdiv";goto BinaryOperator;
BinaryOperator:
		str2=isFloat(second->type->op)?"":"i";
		asmSay("f%sld %s",str2,address(second));
		str2=isFloat(first->type->op)?"":"i";
		asmSay("f%sld %s",str2,address(first));
		asmSay("%s",str1);
		str2=isFloat(result->type->op)?"":"i";
		asmSay("f%sstp %s",str2,address(result));
		break;
	case _AND:
	case _OR:;

/*
fld         qword ptr [x]
fldz
fucompp
fnstsw      ax
test        ah,44h
jnp         main+87h (11513D7h)  ;jp          main+0C2h (1151412h)
fld         qword ptr [y]
fldz
fucompp
fnstsw      ax
test        ah,44h
jnp         main+87h (11513D7h)  ;jp          main+0C2h (1151412h)
mov         dword ptr [ebp-124h],1  ;mov         dword ptr [ebp-124h],0
jmp         main+91h (11513E1h)
mov         dword ptr [fcmp],0  ;mov         dword ptr [ebp-124h],1
fild        dword ptr [ebp-124h]
fstp        qword ptr [z]
*/
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
	}
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
		}
		if(isStatic(t->op)){
			t->offset=staticTable->size;
			staticTable->size+=t->size;
		}
	}
}