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
//<functions>

//两个重命名函数,明确语义,自动换行,就像puts
void objsay(const char* fmt,...){
    va_list ap;
    va_start(ap,fmt);
	vfprintf(fobj,fmt,ap);
	fprintf(fobj,"\n");
    va_end(ap);
}
void asmsay(const char* fmt,...){
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
	asmsay("includelib kernel32.lib");
	asmsay(".model flat,stdcall");
	asmsay("ExitProcess proto,dwExitCode:dword");
	asmsay(".code");
	asmsay("main proc");
	asmsay("\tmov ebp,offset auto");
}

//告诉后端,编译完成,发送结束四元式
void sendEnd(){
	objsay("(ed,_,_,_)");
	asmsay("\tpush 0h");
	asmsay("\tcall ExitProcess");
	asmsay("main endp");
	asmsay(".data");
	asmsay("\tstatic db %d dup(0)",constants->size);
	asmsay("\tauto db 65536 dup(0)");
	asmsay("end main");
	fclose(fobj);
	fclose(fasm);
}

//告诉后端,生成一个id为labelId的标签
void sendLabel(int labelId){
	struct label lb=getLabel(labelId);

	//必须是已经出现定义的才能生成
	assert(lb.defined==1);

	objsay("(lb,%d,_,_)",labelId);
	asmsay("$%s:",lb.name.c_str());
}

//告诉后端,跳转到id为labelId的标签
void sendGt(int labelId){
	struct label lb=getLabel(labelId);
	objsay("gt,%d,_,_)",labelId);
	asmsay("\tjmp $%s",lb.name.c_str());
}

//根据操作数大小,返回byte/word/dword/qword ptr
const char* whatPtr(int size){
	switch(size){
	case 1:return "byte ptr";
	case 2:return "word ptr";
	case 4:return "dword ptr";
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
char* asmHex(int num){
	static char str[20];
	sprintf(str,"%xh",num);
	if('0'<=str[2]&&str[2]<='9'){
		return str+2;
	}else{
		str[1]='0';
		return str+1;
	}
}

//告诉后端,生成id为forId的循环的第state阶段四元式和代码
void sendFor(int forId,enum FOR_STATE state){
	switch(state){
	case FOR_CMP:
		objsay("(lb,$for%dcmp,_,_)",forId);
		asmsay("$for%dcmp:",forId);
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
			objsay("(if,%s,$for%dblk,$for%dend)",
				tmp->name.c_str(),forId,forId);
			asmsay("\tcmp 0,%s[ebp+%s]",
				whatPtr(tmp->type->size),
				asmHex(tmp->offset));
			asmsay("\tjz $for%dend",forId);
			asmsay("\tjmp $for%dblk",forId);
			objsay("(lb,$for%dinc,_,_)",forId);
			asmsay("$for%dinc:",forId);
			break;
		}
	case FOR_BLOCK:
		objsay("(gt,$for%dcmp,_,_)",forId);
		asmsay("\tjmp $for%dcmp",forId);
		objsay("(lb,$for%dblk,_,_)",forId);
		asmsay("$for%dblk:",forId);
		break;
	case FOR_END:
		objsay("(gt,$for%dinc,_,_)",forId);
		asmsay("\tjmp $for%dinc",forId);
		objsay("(lb,$for%dend,_,_)",forId);
		asmsay("$for%dend:",forId);
		break;
	case FOR_BREAK:
		objsay("(gt,$for%dend,_,_)",forId);
		asmsay("\tjmp $for%dend",forId);
		break;
	case FOR_CONTINUE:
		objsay("(gt,$for%dinc,_,_)",forId);
		asmsay("\tjmp $for%dinc",forId);
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
			objsay("(if,%s,_,$if%delse)",
				tmp->name.c_str(),ifId,ifId);
			asmsay("\tcmp 0,%s[ebp+%s]",
				whatPtr(tmp->type->size),
				asmHex(tmp->offset));
			asmsay("\tjz $if%delse",ifId);
			break;
		}
	case IF_ELSE:
		objsay("(gt,$if%dend,_,_)",ifId);
		asmsay("\tjmp $if%dend",ifId);
		objsay("(lb,$if%delse,_,_)",ifId);
		asmsay("$if%delse:",ifId);
		break;
	case IF_END:
		objsay("(lb,$if%dend,_,_)",ifId);
		asmsay("$if%dend:",ifId);
		break;
	}
}

/*
好东西:
	CBW 功能：将AL中8位带符号数，进行带符号扩展为16位，送AX中。带符号扩展是指对正数高位扩展为全“0”，对负数高位扩展为全“1”。
	CWDE 功能：将AX中16位带符号数，进行带符号扩展为32位，送EAX中。
	CDQ 功能:将EAX中32位带符号数,进行带符号扩展为64位,高位在EDX中.
	符号扩展指令对标志位无影响。
*/
//告诉后端,生成(op,first,second,result)这样的四元式
		//mov eax,first
		//op eax,second
		//mov result,eax
void sendOp(enum OP op,Symbol first,Symbol second,Symbol result){
	objsay("(%s,%s,%s,%s)",op_str[op],first->name.c_str(),
		second->name.c_str(),result->name.c_str());
	switch(op){
	case ADD:case SUB:case AND:case OR:
	case BTA:case BTO:case BTX:{
			const char* str="or";
			switch(op){
				case ADD:str="add";break;
				case SUB:str="sub";break;
				case AND:str="and";break;
				case BTA:str="and";break;
				case BTX:str="xor";break;
			}
			if(eax!=first){
				eax=first;
				asmsay("\tmov %s,[ebp+%s]",
					whatAReg(first->type->size),
					asmHex(first->offset)
				);
			}
			//操作之后eax存到不是任何符号的内容了
			eax=NULL;
			asmsay("\t%s %s,[ebp+%s]",str,
				whatAReg(second->type->size),
				asmHex(second->offset)
			);
			//存了之后,eax和result的内容是一样的了
			eax=result;
			asmsay("\tmov [ebp+%s],%s",
				asmHex(result->offset),
				whatAReg(result->type->size)
			);
			break;
		}
		//mov eax,first
		//op eax,second
		//mov result,eax
	case SHL:case SHR:
		if(eax!=first){
			eax=first;
			asmsay("\tmov %s,[ebp+%s]",
				whatAReg(first->type->size),
				asmHex(first->offset)
			);
		}
		if(ecx!=second){
			ecx=second;
			asmsay("\tmov %s,[ebp+%s]",
				whatCReg(first->type->size),
				asmHex(first->offset)
			);
		}
		//操作之后eax存到不是任何符号的内容了
		eax=NULL;
		asmsay("\t%s %s,cl",
			op==SHL?"shl":"shr",
			whatAReg(second->type->size)
		);
		//存了之后,eax和result的内容是一样的了
		eax=result;
		asmsay("\tmov [ebp+%s],%s",
			asmHex(result->offset),
			whatAReg(result->type->size)
		);
		break;
	case MUL:
		if(eax!=first){
			eax=first;
			asmsay("\tmov %s,[ebp+%s]",
				whatAReg(first->type->size),
				asmHex(first->offset)
			);
		}
		//操作之后eax存到不是任何符号的内容了
		eax=NULL;
		asmsay("\timul %s,[ebp+%s]",
			whatAReg(second->type->size),
			asmHex(second->offset)
		);
		//存了之后,eax和result的内容是一样的了
		eax=result;
		asmsay("\tmov [ebp+%s],%s",
			asmHex(result->offset),
			whatAReg(result->type->size)
		);
		break;
	case DIV:
		if(eax!=first){
			eax=first;
			asmsay("\tmov eax,%s[ebp+%s]",
				whatPtr(first->type->size),
				asmHex(first->offset)
			);
		}
		//进行一波符号扩展
		asmsay("\tcdq");
		//操作之后eax存到不是任何符号的内容了
		if(ecx!=second){
			ecx=second;
			asmsay("\tmovsx ecx,%s[ebp+%s]",
				whatPtr(second->type->size),
				asmHex(second->offset)
			);
		}
		asmsay("\tidiv ecx");
		//存了之后,eax和result的内容是一样的了
		eax=result;
		asmsay("\tmov [ebp+%s],%s",
			asmHex(result->offset),
			whatAReg(result->type->size)
		);
		break;
	case MOD:
		if(eax!=first){
			eax=first;
			asmsay("\tmov eax,%s[ebp+%s]",
				whatPtr(first->type->size),
				asmHex(first->offset)
			);
		}
		//进行一波符号扩展
		asmsay("\tcdq");
		//操作之后eax存到不是任何符号的内容了
		if(ecx!=second){
			ecx=second;
			asmsay("\tmovsx ecx,%s[ebp+%s]",
				whatPtr(second->type->size),
				asmHex(second->offset)
			);
		}
		asmsay("\tidiv ecx");
		//存了之后,eax和result的内容是一样的了
		eax=result;
		asmsay("\tmov [ebp+%s],%s",
			asmHex(result->offset),
			whatDReg(result->type->size)
		);
		break;
	case NEG:
		if(first!=result){
			eax=first;
			asmsay("\tmov %s,[ebp+%s]",
				whatAReg(first->type->size),
				asmHex(first->offset)
			);
			asmsay("\tneg %s",whatAReg(first->type->size));
			//存了之后,eax和result的内容是一样的了
			eax=result;
			asmsay("\tmov [ebp+%s],%s",
				asmHex(result->offset),
				whatAReg(result->type->size)
			);
		}else{
			//因为是同一个位置,就直接neg
			asmsay("\tneg %s[ebp+%s]",
				whatPtr(result->type->size),
				asmHex(result->offset)
			);
		}
		break;
	case BTN:
	case INC:
	case DEC:
		asmsay("\t%s %s[ebp+%s]",
			op==INC?"inc":op==NOT?"not":"dec",
			whatPtr(first->type->size),
			asmHex(first->offset)
		);
		break;
	case LEA:
		asmsay("\tlea eax,[ebp+%s]",
			asmHex(first->offset)
		);
		asmsay("\tmov [ebp+%s],%s",
			asmHex(result->offset),
			whatAReg(result->type->size)
		);
		break;
	case EQU:case NEQ:case GTR:case LES:
	case GEQ:case LEQ:{
			const char* str="je";
			//是否为无符号数
			bool usign=first->type->op<=TYPE_UNSIGNED&&second->type->op<=TYPE_UNSIGNED;
			switch(op){
			case NEQ:str="jne";break;
			case GTR:str=usign?"ja":"jg";break;
			case LES:str=usign?"jb":"jl";break;
			case GEQ:str=usign?"jae":"jge";break;
			case LEQ:str=usign?"jbe":"jle";break;
			}
			asmsay("\tmov %s,[ebp+%s]",
				whatAReg(first->type->size),
				asmHex(first->offset)
			);
			asmsay("\tcmp %s,[ebp+%s]",
				whatAReg(second->type->size),
				asmHex(second->offset)
			);
			asmsay("\tmov ebx,0");
			asmsay("\t%s $+3",str);
			asmsay("\tinc ebx");
			asmsay("\tmov [ebp+%s],%s",
				asmHex(result->offset),
				whatBReg(result->type->size)
			);
			break;
		}
	case NOT:
		asmsay("\tmov %s,[ebp+%s]",
			whatAReg(first->type->size),
			asmHex(first->offset)
		);
		asmsay("\tcmp %s,0",whatAReg(first->type->size));
		asmsay("\tmov %s,0",whatBReg(first->type->size));
		asmsay("\tjnz $+3");
		asmsay("\tinc %s",whatBReg(first->type->size));
		asmsay("\tmov [ebp+%s],%s",
			asmHex(result->offset),
			whatBReg(first->type->size)
		);
		break;
	case MOV:
		if(eax!=first){
			eax=first;
			asmsay("\tmov %s,[ebp+%s]",
				whatAReg(first->type->size),
				asmHex(first->offset)
			);
		}
		//存了之后,eax和result的内容是一样的了
		if(eax!=result){
			eax=result;
			asmsay("\tmov [ebp+%s],%s",
				asmHex(result->offset),
				whatAReg(result->type->size)
			);
		}
		break;
	}
}

//通知后端,声明了一个s符号
void tellVar(Symbol s){
	if(s->sclass>=SCLASS_CONST){
		if(s->type->op<=TYPE_ENUM)
			return;
		s->offset=constants->size;
		constants->size+=s->type->size;
	}else if(s->sclass==SCLASS_AUTO){
		if(s->temporary){
			s->offset=idt->size;
			idt->size+=8;
		}else{
			s->offset=idt->size;
			idt->size+=s->type->size;
		}
	}
}

//通知后端,声明了一个t类型
void tellType(struct type* t){
	//for(struct type*i=t->kid;i;i=i->next){
	//	if(i->sclass>=SCLASS_CONST){
	//		if(i->sclass==SCLASS_CONST)
	//			if(i->op<=TYPE_ENUM)
	//				continue;
	//		//? offset,table的offset
	//	}
	//}
}