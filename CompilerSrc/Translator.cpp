#pragma once
#include"Translator.h"
#define foreach(itor,obj,type,inc) for(type::iterator itor=(obj).begin();itor!=(obj).end();inc)
#define foreachR(itor,obj,type,inc) for(type::reverse_iterator itor=(obj).rbegin();itor!=(obj).rend();inc)
#define List std::list
struct ProductionNode{
    int type,id,action;
    ProductionNode(int t,int i,int a=0):type(t),id(i),action(a){}
    bool operator<(const ProductionNode other)const{
        if(type==other.type)
            return id<other.id;
        return type<other.type;
    }
    bool operator==(const ProductionNode other)const{
        return id==other.id&&type==other.type;
    }
    bool operator!=(const ProductionNode other)const{
        return !(*this==other);
    }
};
typedef List<ProductionNode> Production;
Vector<List<Production>>head(1);
Vector<std::map<Token,bool>>firstSet(1);
Vector<std::map<Token,bool>>followSet(1);
std::map<String,int> stateMap;
Vector<String> state(1);
std::map<String,int> actionMap;
Vector<String> action;
int stateid(const char*s){
    static int scnt=1;
    if(!stateMap[s]){
        stateMap[s]=scnt++;
        state.push_back(s);
        head.push_back(List<Production>());
        firstSet.push_back(std::map<Token,bool>());
        followSet.push_back(std::map<Token,bool>());
    }return stateMap[s];
}
int actionid(const char*s){
    static int acnt=0;
	if(!actionMap.count(s)){
        actionMap[s]=acnt++;
        action.push_back(s);
    }return actionMap[s];
}
bool inFirst(ProductionNode node){
	if(node.type==_T_)
		return firstSet[node.id][t];
    else if(node.id==-1)
        return node.type==t.type;
    else
        return node.type==t.type&&node.id==t.id;
}
bool inFollow(ProductionNode node){
	if(t==Invalid&&nowchar==EOF)return true;
	if(node.type==_T_)
		return followSet[node.id][t];
    else if(node.id==-1)
        return node.type==t.type;
    else
        return node.type==t.type&&node.id==t.id;
}
//语义栈
std::stack<struct symbol*> sem;
//id队列
std::stack<String> idStack;
//操作符栈
std::stack<Token>ops;
String lastS;
int charVal(){
	if(token[1]!='\\')
		return token[1];
	//对于转义字符的识别
	switch(token[2]){
	case 'a':return'\a';case 'b':return'\b';
	case 'f':return'\f';case 'n':return'\n';
	case 'r':return'\r';case 't':return'\t';
	case 'v':return'\v';
	//\xhh十六进制转义字符
	case 'x':
		if(isHex(token[3])){
			char tmp[5]={'0','x',token[3],token[4],0};
			if(!isHex(token[4]))tmp[3]=0;
			int v;sscanf(tmp,"%x",&v);
			return v;
		}else return'x';
		break;
	//\ddd八进制转义字符
	case '0':case '1':case '2':case '3':
	case '4':case '5':case '6':case '7':
		char tmp[5]={'0',token[2],token[3],token[4],0};
		if(isOct(token[3])){
			if(!isOct(token[4]))
				tmp[3]=0;
		}else
			tmp[2]=0;
		int v;
		sscanf(tmp,"%o",&v);
		return v;
	}return token[2];
}
void translatorInit(const char* filename){
	actionid("Null");
	Coordinate c={0,0};
	newSymbol("scanf",(struct type*)1,SCLASS_CONST,c);
	newSymbol("printf",(struct type*)1,SCLASS_CONST,c);
	freopen(filename,"r",stdin);
	for(int i,type,id;~scanf("%d%*c",&i);){
		char s[100],c;
		sprintf(s,"T%d",i);
		stateid(s);
		while((c=getchar())==' '){
			scanf("%d,%d",&type,&id);
			firstSet[i][Token(type,id)]=true;
		}
		while((c=getchar())==' '){
			scanf("%d,%d",&type,&id);
			followSet[i][Token(type,id)]=true;
		}
		while(1){
			Production l;
			while((c=getchar())==' '){
				scanf("%d,%d,%s",&type,&id,s);
				l.push_back(ProductionNode(type,id,actionid(s)));
			}if(c=='`')break;
			head[i].push_back(l);
		}getchar();
	}
}
void expect(ProductionNode need){
	if(t.id==need.id&&t.type==need.type){
		if(t.type==_B_&&t.id<=16){
			ops.push(t);
		}
		if(t.type==_I_){
			idStack.push(token);
		}
		if(t.type==_S_)
			lastS=token;
		if(t.type==_C_){
			tsym=new symbol();
			tsym->addressed=0;
			tsym->name=token;
			tsym->offset=0;
			tsym->sclass=SCLASS_CONST;
			tsym->scope=level;
			tsym->src=src;
			tsym->temporary=0;
			tsym->type=btot(TYPE_INT);
			tsym->u.c.i=charVal();
		}
		if(t.type==_N_){
			tsym=new symbol();
			tsym->name=token;
			tsym->offset=0;
			tsym->sclass=SCLASS_CONST;
			tsym->scope=level;
			tsym->src=src;
			tsym->temporary=0;
			bool dot=false,e=false;
			for(int i=0;i<token.size();i++)
				if(token[i]=='.')
					dot=true;
				else if((token[i]|32)=='e')
					e=true;
			if(dot||e){
				tsym->addressed=1;
				if((token.back()|32)=='f'){
					float v;sscanf(token.c_str(),"%f",&v);
					tsym->type=btot(TYPE_FLOAT);
					tsym->u.c.f=v;
				}else{
					double v;sscanf(token.c_str(),"%lf",&v);
					tsym->type=btot(TYPE_DOUBLE);
					tsym->u.c.d=v;
				}
			}else{
				tsym->addressed=0;
				tsym->type=btot(TYPE_INT);
				int v;
				if(token[0]=='0'){
					if(token.size()>1&&(token[1]=='x'||token[1]=='X'))
						sscanf(token.c_str(),"%x",&v);
					else
						sscanf(token.c_str(),"%o",&v);
				}else sscanf(token.c_str(),"%d",&v);
				tsym->u.c.i=v;
			}
		}
		t=Next();
	}else{
		fprintf(stderr,"%d,%d:Error:expected ",src.x,src.y);
		switch(need.type){
			case _I_:fprintf(stderr,"_I_\n");break;
			case _S_:fprintf(stderr,"_S_\n");break;
			case _C_:fprintf(stderr,"_C_\n");break;
			case _N_:fprintf(stderr,"_N_\n");break;
			case _K_:fprintf(stderr,"%s\n",keyWord[need.id].c_str());break;
			case _B_:fprintf(stderr,"%s\n",boundary[need.id].c_str());break;
		}
	}
}
void expand(std::stack<ProductionNode>&s,ProductionNode now){
	foreach(it,head[now.id],List<Production>,it++){
        if(it->size()==0)
            if(inFollow(now))
                return;//决定使用空产生式
            else
                continue;
		if(inFirst(it->front())){
			foreachR(rit,*it,Production,rit++)
				s.push(*rit);
			return;
		}
    }
}
void act(ProductionNode);
void run(ProductionNode now){
	std::stack<ProductionNode> s;
	expand(s,now);
	while(s.size()){
		ProductionNode now=s.top();s.pop();
		if(now.type==_T_)
			run(now);
		else{
			expect(now);
			if(now.action)
				act(now);
		}
	}
	if(now.action)
		act(now);
}
int main(){
	symInit();
	typeInit();
	backEndInit();
	translatorInit("64.gmr");
    freopen(CompileFile,"r",stdin);
	lexAnalyzerInit();
	run(ProductionNode(_T_,43));
	puts("Compile Over!\n\n\n");
    return 0;
}
#define nextis(str) else if(action[now.action]==str)
#define nexthas(str) else if(action[now.action].find(str)!=-1)
#define getfsr(Op) \
	s=sem.top();sem.pop();\
	f=sem.top();sem.pop();\
	if(isArith(f->type->op)&&isArith(s->type->op))\
		r=newTemp(compatibleType(f->type,s->type));\
	else r=newTemp(f->type);\
	sem.push(r);\
	sendOp(Op,f,s,r)
#define getfrr(op) \
	f=sem.top();sem.pop();\
	r=sem.top();sem.pop();\
	sem.push(r);sendOp(op,f,r,r)
#define funcdef(name) goto name##end;name:
#define funcend(name) __asm{ret};name##end:
#define funcall(name) __asm{call name}
//执行now后面的动作
void act(ProductionNode now){
	if(ActionOutput)
		colorPrintf(-2,"动作:%s\n",action[now.action].c_str());
	enum{INT=1<<0,LONG=1<<1,SHORT=1<<2,FLOAT=1<<3,DOUBLE=1<<4,
		VOID=1<<5,CHAR=1<<6,SIGNED=1<<7,UNSIGNED=1<<8,TYPE=1<<9};
	static bool STATIC=false,STRUCT=false,UNION=false;
	static std::stack<int>dimTypeStack;
	static std::stack<int>forid;
	static int pointer=0;
	static std::queue<std::pair<Symbol,int>>indec;
	static std::stack<struct type*>castTypeStack;

	if(0);
	nexthas("send"){
		Symbol f,s,r;
		if(0);
		nextis("send/"){getfsr(_DIV);}
		nextis("send%"){getfsr(_MOD);}
		nextis("send>"){getfsr(_GTR);}
		nextis("send<"){getfsr(_LES);}
		nextis("send|"){getfsr(_BTO);}
		nextis("send^"){getfsr(_BTX);}
		nextis("send>>"){getfsr(_SHR);}
		nextis("send<<"){getfsr(_SHL);}
		nextis("send>="){getfsr(_GEQ);}
		nextis("send<="){getfsr(_LEQ);}
		nextis("send=="){getfsr(_EQU);}
		nextis("send!="){getfsr(_NEQ);}
		nextis("send&&"){getfsr(_AND);}
		nextis("send||"){getfsr(_OR );}
		else{
			Token top=ops.top();ops.pop();
			if(0);
			nextis("sendop"){
				if(boundary[top.id].size()==1)
					switch(boundary[top.id][0]){
					case '+':assert(0);break;
					case '&':r=newTemp(ptr(btot(TYPE_INT)));
						sendOp(_LEA,sem.top(),0,r);
						sem.pop();sem.push(r);break;
					case '*':sem.top()->needebx=1;break;
					case '-':r=newTemp(sem.top()->type);
						sendOp(_NEG,sem.top(),0,r);
						sem.pop();sem.push(r);break;
					case '~':r=newTemp(sem.top()->type);
						sendOp(_NOT,sem.top(),0,r);
						sem.pop();sem.push(r);break;
					case '!':r=newTemp(sem.top()->type);
						sendOp(_BTN,sem.top(),0,r);
						sem.pop();sem.push(r);break;
					case '=':getfrr(_MOV);break;
					}
				else
					switch(boundary[ops.top().id][0]){
					case '+':getfrr(_ADD);break;
					case '-':getfrr(_SUB);break;
					case '*':getfrr(_MUL);break;
					case '/':getfrr(_DIV);break;
					case '%':getfrr(_MOD);break;
					case '>':getfrr(_SHR);break;
					case '<':getfrr(_SHL);break;
					case '^':getfrr(_BTX);break;
					case '|':getfrr(_BTO);break;
					case '&':getfrr(_BTA);break;
					}
			}
			nextis("send="){getfrr(_MOV);}
			nextis("send*"){getfsr(_MUL);}
			nextis("send&"){getfsr(_BTA);}
			nextis("send+"){getfsr(_ADD);}
			nextis("send-"){getfsr(_SUB);}
		}
	}
	nexthas("func"){
		static Symbol function;
		static std::stack<Symbol> ss;
		if(0);
		nextis("retfunc"){
			sendRet(newIntConst(0));
		}
		nextis("retefunc"){
			sendRet(sem.top());sem.pop();
		}
		nextis("argfunc"){
			ss.push(sem.top());sem.pop();
		}
		nextis("callfunc"){
			ss.push(sem.top());sem.pop();
			sendFunc(FUNC_CALL,ss);
			sem.push(FRET);
			while(ss.size())ss.pop();
		}
		nextis("newfunc"){
			funcall(intToType);
			struct type*tmp=newFunc(level,(struct type*)dimTypeStack.top());
			dimTypeStack.pop();
			dimTypeStack.push((int)tmp);
			function=newSymbol(idStack.top(),0,SCLASS_CONST,src);
			enterScope();
		}
		nextis("paramfunc"){
			funcall(intToType);
			struct type*kid=(struct type*)dimTypeStack.top();dimTypeStack.pop();
			struct type*fun=(struct type*)dimTypeStack.top();dimTypeStack.pop();
			addField(fun,kid);
			dimTypeStack.push((int)fun);
			dimTypeStack.push(0);
			fun=fun->kid;
			while(fun->next)
				fun=fun->next;
			String varName=idStack.top();
			fun->name=varName;
			Symbol s=findSymbol(varName);
			if(s&&s->scope==level)
				error("redifined %s",varName.c_str());
			else{
				Symbol s=newSymbol(varName,kid,SCLASS_AUTO,src);
				tellVar(s);
			}
			idStack.pop();
		}
		nextis("addfunc"){
			if(dimTypeStack.size()>1)
				dimTypeStack.pop();
			struct type*tmp=(struct type*)dimTypeStack.top();
			dimTypeStack.pop();
			dimTypeStack.push((int)saveFunc(tmp));
			function->type=(struct type*)dimTypeStack.top();
			dimTypeStack.pop();
			std::stack<Symbol>ss;
			ss.push(function);
			sendFunc(FUNC_DEFINE,ss);
		}
		nextis("endfunc"){
			std::stack<Symbol>ss;
			ss.push(function);
			sendFunc(FUNC_END,ss);
			idStack.pop();
			exitScope();
		}
	}
	nexthas("push"){
		if(0);
		nextis("pushOperand"){
			if(lastS==""&&tsym==0){
				String name=idStack.top();idStack.pop();
				Symbol s=findSymbol(name);
				if(s)sem.push(s);
				else error("undefined %s",name.c_str());
			}
			if(lastS!=""){
				Symbol s=findConstSymbol(lastS);
				if(s)sem.push(s);
				else sem.push(s=newStringConst(lastS,src));
				tellVar(s);lastS="";
			}
			if(tsym!=0){
				if(tsym->addressed)
					sem.push(newFloatConst(tsym,src));
				else sem.push(newIntConst(tsym->u.c.i));
				tsym=NULL;
			}
		}
		nextis("pushinc"){
			indec.push(std::make_pair(sem.top(),1));
		}
		nextis("pushdec"){
			indec.push(std::make_pair(sem.top(),-1));
		}
		nextis("pushop");
		nextis("pushStruct"){
			if(UNION||STRUCT)
				error("Wrong type");
			STRUCT=true;
		}
		nextis("pushUnion"){
			if(UNION||STRUCT)
				error("Wrong type");
			UNION=true;
		}
		nextis("pushType"){
			if(STRUCT||UNION){
				struct type* tmp=findType(idStack.top());
				idStack.pop();
				dimTypeStack.push((int)tmp);
				STRUCT=UNION=false;
			}else if(dimTypeStack.size()){
				int type=dimTypeStack.top();
				if(type<TYPE)
					dimTypeStack.pop();
				else
					type=0;

#define xx(str,op) else if((*id2string[now.type])[now.id]==str)\
						dimTypeStack.push(type|op)

				switch(type){
				case VOID:case DOUBLE:case FLOAT:
				case SIGNED|CHAR:case UNSIGNED|CHAR:
				case SIGNED|SHORT|INT:case UNSIGNED|SHORT|INT:
				case SIGNED|LONG|INT:case UNSIGNED|LONG|INT:
					error("invalid type");break;
				case INT:
					if(0);xx("long",LONG);xx("short",SHORT);xx("signed",SIGNED);
					xx("unsigned",UNSIGNED);else error("invalid type");break;
				case LONG:case SHORT:
					if(0);xx("int",INT);xx("signed",SIGNED);
					xx("unsigned",UNSIGNED);else error("invalid type");break;
				case CHAR:case INT|LONG:case INT|SHORT:
					if(0);xx("signed",SIGNED);xx("unsigned",UNSIGNED);
					else error("invalid type");break;
				case SIGNED:case UNSIGNED:
					if(0);xx("long",LONG);xx("short",SHORT);xx("int",INT);
					xx("char",CHAR);else error("invalid type");break;
				case SIGNED|INT:case UNSIGNED|INT:
					if(0);xx("long",LONG);xx("short",SHORT);
					else error("invalid type");break;
				case SIGNED|SHORT:case SIGNED|LONG:
				case UNSIGNED|SHORT:case UNSIGNED|LONG:
					if((*id2string[now.type])[now.id]=="int")
						dimTypeStack.push(type|INT);
					else error("invalid type");break;
				default:
#undef xx
#define xx(str,op) else if((*id2string[now.type])[now.id]==str)dimTypeStack.push(op)

					if(0);xx("int",INT);xx("void",VOID);xx("long",LONG);
					xx("short",SHORT);xx("char",CHAR);xx("double",DOUBLE);
					xx("float",FLOAT);xx("signed",SIGNED);xx("unsigned",UNSIGNED);
					else assert(0);
				}
			}else{
				if(0);xx("int",INT);xx("void",VOID);xx("long",LONG);
				xx("short",SHORT);xx("char",CHAR);xx("double",DOUBLE);
				xx("float",FLOAT);xx("signed",SIGNED);xx("unsigned",UNSIGNED);
				else assert(0);
#undef xx
			}
		}
	}
	nexthas("pop"){
		if(0);
		nextis("popOperand"){
			if(boundary[now.id]==","||!forid.size())
				sem.pop();
			while(indec.size()){
				Symbol s=indec.front().first;
				sendOp(_ADD,s,newIntConst(indec.front().second),s);
				indec.pop();
			}
		}
		nextis("popType"){
			dimTypeStack.pop();
		}
		nextis("popType&Operand"){
			sem.pop();
			dimTypeStack.pop();
		}
	}
	nexthas("if"){
		static std::stack<int>ifid;
		static int ifId=1;
		if(0);
		nextis("if1"){
			ifid.push(ifId++);
			sendIf(ifid.top(),IF_THEN);
		}
		nextis("if2"){
			sendIf(ifid.top(),IF_ELSE);
		}
		nextis("if3"){
			sendIf(ifid.top(),IF_END);
			ifid.pop();
		}
		nextis("ife1"){
			ifid.push(ifId++);
			sendIf(ifid.top(),IF_THEN);
		}
		nextis("ife2"){
			sendCON(sem.top());
			sem.pop();
			sendIf(ifid.top(),IF_ELSE);
		}
		nextis("ife3"){
			sendCON(sem.top());sem.pop();
			sendIf(ifid.top(),IF_END);ifid.pop();
			Symbol r=newTemp(btot(TYPE_INT));
			sem.push(r);
			sendOp(_MOV,CON,0,r);
		}
	}
	nexthas("for"){
		static int forId=1;
		if(0);
		nextis("for1"){
			if(forid.size())
				sem.pop();
			forid.push(forId++);
			sendFor(forid.top(),FOR_CMP);
		}
		nextis("for2"){
			sendFor(forid.top(),FOR_INC);
		}
		nextis("for3"){
			while(indec.size()){
				Symbol s=indec.front().first;
				sendOp(_ADD,s,newIntConst(indec.front().second),s);
				indec.pop();
			}
			sendFor(forid.top(),FOR_BLOCK);
		}
		nextis("for2for3"){
			sendFor(forid.top(),FOR_INC);
			sem.push(0);
			sendFor(forid.top(),FOR_BLOCK);
		}
		nextis("for4"){
			sendFor(forid.top(),FOR_END);
			forid.pop();
		}
		nextis("for5"){
			sendFor(forid.top(),FOR_BREAK);
		}
		nextis("for6"){
			sendFor(forid.top(),FOR_CONTINUE);
		}
	}
	nexthas("new"){
		if(0);
		nextis("newArray"){
			int sz=0;
			if(sem.size()&&sem.top()->addressed==0){
				sz=sem.top()->u.c.i;sem.pop();
			}
			funcall(intToType);
			struct type*tmp=(struct type*)dimTypeStack.top();
			dimTypeStack.pop();
			tmp=newArray(tmp,sz);
			dimTypeStack.push((int)tmp);
		}
		nextis("newNamedSU"){
			String name=idStack.top();idStack.pop();
			struct type* tmp;
			if(STRUCT)
				tmp=newStruct(level,name);
			else
				tmp=newUnion(level,name);
			STRUCT=UNION=false;
			dimTypeStack.push((int)tmp);
		}
		nextis("newUnamedSU"){
			struct type* tmp;
			if(STRUCT)
				tmp=newStruct(level);
			else
				tmp=newUnion(level);
			STRUCT=UNION=false;
			dimTypeStack.push((int)tmp);
		}
	}
	nextis("find"){
		Symbol s=sem.top();sem.pop();
		Symbol f=sem.top();sem.pop();
		Symbol r=newTemp(f->type);
		sem.push(r);sendOp(_ADD,f,s,r);
		sem.top()->needebx=1;
	}
	nextis("addField"){
		funcall(intToType);
		struct type* fieldType=(struct type*)dimTypeStack.top();dimTypeStack.pop();
		struct type* suType=(struct type*)dimTypeStack.top();dimTypeStack.pop();
		String tmp=fieldType->name;
		fieldType->name=idStack.top();idStack.pop();
		addField(suType,fieldType);
		fieldType->name=tmp;
		dimTypeStack.push((int)suType);
		dimTypeStack.push(0);
	}
	nextis(".member"){
		Symbol s=sem.top();sem.pop();
		String memberName=idStack.top();idStack.pop();
		Symbol sameSym;
		for(struct type*i=s->type->kid
				;i;i=i->next){
			if(i->name==memberName){
				sameSym=newTemp(i,false);
				sameSym->scope=s->scope;
				sameSym->addressed=1;
				sameSym->temporary=0;
				sameSym->offset=i->offset+s->offset;
				sameSym->src=src;
			}
		}
		sem.push(sameSym);
	}
	nextis("inc"){
		Symbol f=sem.top();
		Symbol t=newTemp(f->type);
		sem.pop();sem.push(t);
		sendOp(_INC,f,NULL,t);
	}
	nextis("dec"){
		Symbol f=sem.top();
		Symbol t=newTemp(f->type);
		sem.pop();sem.push(t);
		sendOp(_DEC,f,NULL,t);
	}
	nextis("sizeof"){
		Symbol f=sem.top();sem.pop();
		sem.push(newIntConst(f->type->size));
	}
	nextis("checkConst"){
		if(sem.top()->sclass!=SCLASS_CONST){
			error("Array length must is const integer.");
		}
	}
	nextis("ptr"){
		pointer++;
		ops.pop();
	}
	nextis("tellVar"){
		funcall(intToType);
		struct type*tmp=(struct type*)dimTypeStack.top();
		while(pointer>0)
			tmp=ptr(tmp),
			pointer--;
		Symbol finds=findSymbol(idStack.top());
		if(finds&&finds->scope==level)
			error("Redefined '%s'",idStack.top().c_str());
		Symbol s=newSymbol(idStack.top(),tmp,SCLASS_AUTO,src);
		s->addressed=1;
		tellVar(s);idStack.pop();
		sem.push(s);
	}
	nextis("enterscope"){
		enterScope();
		enterScopeB();
	}
	nextis("exitscope"){
		exitScope();
		exitScopeB();
		remoteType(level);
	}
	nextis("end"){
		sendEnd();
	}


	funcdef(intToType){
		int type=dimTypeStack.top();
		dimTypeStack.pop();
		struct type*tmp;
		if(type<TYPE){
			switch(type){
			case VOID:tmp=btot(TYPE_VOID);break;
			case FLOAT:tmp=btot(TYPE_FLOAT);break;
			case DOUBLE:tmp=btot(TYPE_DOUBLE);break;
			case CHAR:case SIGNED|CHAR:
				tmp=btot(TYPE_CHAR);break;
			case UNSIGNED|CHAR:
				tmp=btot(TYPE_UCHAR);break;
			case INT:case LONG|INT:case LONG:
			case SIGNED|LONG|INT:
			case SIGNED|LONG:
			case SIGNED|INT:
				tmp=btot(TYPE_INT);break;
			case SHORT|INT:case SHORT:
			case SIGNED|SHORT|INT:
			case SIGNED|SHORT:
				tmp=btot(TYPE_SHORT);break;
			case UNSIGNED|INT:
			case UNSIGNED|LONG|INT:
			case UNSIGNED|LONG:
				tmp=btot(TYPE_UINT);break;
			case UNSIGNED|SHORT|INT:
			case UNSIGNED|SHORT:
				tmp=btot(TYPE_USHORT);break;
			default:
				assert(0);tmp=0;break;
			}
		}else tmp=(struct type*)type;
		dimTypeStack.push((int)tmp);
	}
	funcend(intToType);
}