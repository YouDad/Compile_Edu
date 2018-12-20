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
Vector<bool>red(1);
std::map<String,int> stateMap;
Vector<String> state(1);
int stateid(const char*s){
    static int scnt=1;
    if(!stateMap[s]){
        stateMap[s]=scnt++;
        state.push_back(s);
        head.push_back(List<Production>());
        firstSet.push_back(std::map<Token,bool>());
        followSet.push_back(std::map<Token,bool>());
        red.push_back(false);
    }return stateMap[s];
}
std::map<String,int> actionMap;
Vector<String> action;
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
String lastI,lastS;
Token lastK;
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
		if(t.type==_K_){
			lastK=t;
		}
		if(t.type==_I_){
			lastI=token;
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
			case _K_:fprintf(stderr,"%s\n",keyWord[need.id]);break;
			case _B_:fprintf(stderr,"%s\n",boundary[need.id]);break;
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
#define 理论上能用产生式来展开 0
	assert(理论上能用产生式来展开);
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
	translatorInit("5.gmr");
    freopen("../unittest/Translator (4).txt","r",stdin);
	lexAnalyzerInit();
	run(ProductionNode(_T_,48));
	puts("Compile Over!\n\n\n");
    return 0;
}
#define nextis(str) else if(action[now.action]==str)
#define getfsr Symbol f,s,r;s=sem.top();sem.pop();f=sem.top();sem.pop();\
		r=newTemp();sem.push(r);r->type=btot((enum TYPE)TYPE_INT)
#define funcdef(name) goto name##end;name:
#define funcend(name) __asm{ret};name##end:
#define funcall(name) __asm{call name}
//执行now后面的动作
void act(ProductionNode now){
	printf("!%s\n",action[now.action].c_str());
	//return;
	enum{INT=1<<0,LONG=1<<1,SHORT=1<<2,FLOAT=1<<3,DOUBLE=1<<4,
		VOID=1<<5,CHAR=1<<6,SIGNED=1<<7,UNSIGNED=1<<8,TYPE=1<<9};

	static bool STATIC=false;

	static std::stack<int>ifid;
	static std::stack<int>forid;
	static int forId=1;
	static int ifId=1;
	static std::stack<int>dimTypeStack;
	static int pointer=0;
	static enum SCLASS sclass=SCLASS_AUTO;


	if(action[now.action].substr(0,4)=="send"&&
		action[now.action].size()==5){
		enum OP op;
		switch(action[now.action][4]){
		case '*':op=_MUL;goto send;
		case '/':op=_DIV;goto send;
		case '%':op=_MOD;goto send;
		case '+':op=_ADD;goto send;
		case '-':op=_SUB;goto send;
		case '>':op=_GTR;goto send;
		case '<':op=_LES;goto send;
		case '&':op=_BTA;goto send;
		case '|':op=_BTO;goto send;
		case '^':op=_BTX;goto send;
send:		getfsr;
			sendOp(op,f,s,r);
		}return;
	}
	if(false);
	nextis("find"){
		//`postfix_expression [ expression ] _ApopFindPush
	}
	nextis("pushOperand"){
		//`_I_ _ApushOperand
		//`_C_ _ApushOperand
		//`_N_ _ApushOperand
		//`_S_ _ApushOperand
		if(lastS!=""){
			sem.push(findConstSymbol(lastS));
			lastS="";
		}
		if(tsym!=0){
			if(tsym->addressed)
				sem.push(newFloatConst(tsym,src));
			else sem.push(newIntConst(tsym->u.c.i));
			tsym=NULL;
		}
	}
	nextis("call"){
		//`postfix_expression ( ) _Acall
		//`postfix_expression ( argument_expression_list ) _Acall
	}
	nextis("arg"){
		//`argument_expression_list , assignment_expression _Aarg
		//`assignment_expression _Aarg
		//把语义栈栈顶放到参数队列里
	}
	nextis(".member"){
		//`postfix_expression . _I_ _A.member
	}
	nextis("->member"){
		//`postfix_expression -> _I_ _A->member
	}
	nextis("pushinc"){
		//`postfix_expression ++ _Apushinc
	}
	nextis("pushdec"){
		//`postfix_expression -- _Apushdec
	}
	nextis("inc"){
		//`++ unary_expression _Ainc
		Symbol f=sem.top();
		Symbol t=newTemp();
		sem.pop();sem.push(t);
		sendOp(_INC,f,NULL,t);
	}
	nextis("dec"){
		//`-- unary_expression _Adec
		Symbol f=sem.top();
		Symbol t=newTemp();
		sem.pop();sem.push(t);
		sendOp(_DEC,f,NULL,t);
	}
	nextis("sizeof"){
		//`sizeof unary_expression _Asizeof
		Symbol f=sem.top();sem.pop();
		sem.push(newIntConst(f->type->size));
	}
	nextis("pushop"){
		//`unary_operator _Apushop unary_expression _Asendop

	}
	nextis("sendop"){
		//`unary_operator _Apushop unary_expression _Asendop

	}
	nextis("pushCastType"){
		//`:: type_specifier _ApushCastType : cast_expression _ApopCastType
	}
	nextis("popCastType"){
		//`:: type_specifier _ApushCastType : cast_expression _ApopCastType

	}
	nextis("send="){
		//`declarator _AtellVar = assignment_expression _Asend=
		Symbol f=sem.top();
		Symbol t=newTemp();
		sem.pop();sem.push(t);
		sendOp(_MOV,f,NULL,t);
	}
	nextis("send>>"){getfsr;sendOp(_SHR,f,s,r);}
	nextis("send<<"){getfsr;sendOp(_SHL,f,s,r);}
	nextis("send>="){getfsr;sendOp(_GEQ,f,s,r);}
	nextis("send<="){getfsr;sendOp(_LEQ,f,s,r);}
	nextis("send=="){getfsr;sendOp(_EQU,f,s,r);}
	nextis("send!="){getfsr;sendOp(_NEQ,f,s,r);}
	nextis("send&&"){getfsr;sendOp(_AND,f,s,r);}
	nextis("send||"){getfsr;sendOp(_OR ,f,s,r);}
	nextis("popOperand"){
		//`expression , _ApopOperand assignment_expression
		//`expression ; _ApopOperand
		if(boundary[now.id]==","||!forid.size())
			sem.pop();
	}
	nextis("checkConst"){
		//`conditional_expression _Acheckconst
		if(sem.top()->sclass!=SCLASS_CONST){
			error("Array length must is const integer.");
		}
	}
	nextis("saveSclass"){
		//`static _AsaveSclass
		//`auto _AsaveSclass
		if((*id2string[lastK.type])[lastK.id]=="static"){
			sclass=SCLASS_STATIC;
		}else{
			sclass=SCLASS_AUTO;
		}
	}
	nextis("pushStruct"){
		//`struct _ApushStruct
	}
	nextis("pushUnion"){
		//`union _ApushUnion
	}
	nextis("newSU"){

	}
	nextis("addField"){
		//`declarator _AaddField
		//`struct_declarator_list , declarator _AaddField
	}
	nextis("pushType"){
		//`specifier_qualifier_list _ApushType struct_declarator_list ; _ApopType
		if(dimTypeStack.size()){
			int type=dimTypeStack.top();
			if(type<TYPE)
				dimTypeStack.pop();
			else
				type=0;
			switch(type){
			case VOID:case DOUBLE:case FLOAT:
			case SIGNED|CHAR:case UNSIGNED|CHAR:
			case SIGNED|SHORT|INT:case UNSIGNED|SHORT|INT:
			case SIGNED|LONG|INT:case UNSIGNED|LONG|INT:
				error("invalid type");break;
			case INT:
				if((*id2string[now.type])[now.id]=="long")dimTypeStack.push(type|LONG);
				else if((*id2string[now.type])[now.id]=="short")dimTypeStack.push(type|SHORT);
				else if((*id2string[now.type])[now.id]=="signed")dimTypeStack.push(type|SIGNED);
				else if((*id2string[now.type])[now.id]=="unsigned")dimTypeStack.push(type|UNSIGNED);
				else error("invalid type");break;
			case LONG:case SHORT:
				if((*id2string[now.type])[now.id]=="int")dimTypeStack.push(type|INT);
				else if((*id2string[now.type])[now.id]=="signed")dimTypeStack.push(type|SIGNED);
				else if((*id2string[now.type])[now.id]=="unsigned")dimTypeStack.push(type|UNSIGNED);
				else error("invalid type");break;
			case CHAR:case INT|LONG:case INT|SHORT:
				if((*id2string[now.type])[now.id]=="signed")dimTypeStack.push(type|SIGNED);
				else if((*id2string[now.type])[now.id]=="unsigned")dimTypeStack.push(type|UNSIGNED);
				else error("invalid type");break;
			case SIGNED:case UNSIGNED:
				if((*id2string[now.type])[now.id]=="long")dimTypeStack.push(type|LONG);
				else if((*id2string[now.type])[now.id]=="int")dimTypeStack.push(type|INT);
				else if((*id2string[now.type])[now.id]=="short")dimTypeStack.push(type|SHORT);
				else if((*id2string[now.type])[now.id]=="char")dimTypeStack.push(type|CHAR);
				else error("invalid type");break;
			case SIGNED|INT:case UNSIGNED|INT:
				if((*id2string[now.type])[now.id]=="long")dimTypeStack.push(type|LONG);
				else if((*id2string[now.type])[now.id]=="short")dimTypeStack.push(type|SHORT);
				else error("invalid type");break;
			case SIGNED|SHORT:case SIGNED|LONG:
			case UNSIGNED|SHORT:case UNSIGNED|LONG:
				if((*id2string[now.type])[now.id]=="int")dimTypeStack.push(type|INT);
				else error("invalid type");break;
			default:
				if((*id2string[now.type])[now.id]=="int")dimTypeStack.push(INT);
				else if((*id2string[now.type])[now.id]=="void")dimTypeStack.push(VOID);
				else if((*id2string[now.type])[now.id]=="long")dimTypeStack.push(LONG);
				else if((*id2string[now.type])[now.id]=="short")dimTypeStack.push(SHORT);
				else if((*id2string[now.type])[now.id]=="char")dimTypeStack.push(CHAR);
				else if((*id2string[now.type])[now.id]=="double")dimTypeStack.push(DOUBLE);
				else if((*id2string[now.type])[now.id]=="float")dimTypeStack.push(FLOAT);
				else if((*id2string[now.type])[now.id]=="signed")dimTypeStack.push(SIGNED);
				else if((*id2string[now.type])[now.id]=="unsigned")dimTypeStack.push(UNSIGNED);
				else{
					assert(0);
				}
			}
		}else{
			if((*id2string[now.type])[now.id]=="int")dimTypeStack.push(INT);
			else if((*id2string[now.type])[now.id]=="void")dimTypeStack.push(VOID);
			else if((*id2string[now.type])[now.id]=="long")dimTypeStack.push(LONG);
			else if((*id2string[now.type])[now.id]=="short")dimTypeStack.push(SHORT);
			else if((*id2string[now.type])[now.id]=="char")dimTypeStack.push(CHAR);
			else if((*id2string[now.type])[now.id]=="double")dimTypeStack.push(DOUBLE);
			else if((*id2string[now.type])[now.id]=="float")dimTypeStack.push(FLOAT);
			else if((*id2string[now.type])[now.id]=="signed")dimTypeStack.push(SIGNED);
			else if((*id2string[now.type])[now.id]=="unsigned")dimTypeStack.push(UNSIGNED);
			else{
				assert(0);
			}
		}
	}
	nextis("popType"){
		//`specifier_qualifier_list _ApushType struct_declarator_list ; _ApopType
		dimTypeStack.pop();
	}
	nextis("addEnum"){
		//`_I_ _AaddEnum
		//`_I_ _AaddEnum = constant_expression _Aaddval
	}
	nextis("addVal"){
		//`_I_ _AaddEnum = constant_expression _Aaddval
	}
	nextis("newEnum"){
		//`enum _I_ { _AnewEnum _Aenterscope enumerator_list } _Aexitscope _ApushType
		//`enum { _AnewEnum _Aenterscope enumerator_list } _Aexitscope _ApushType
		//Type=newEnum(level);
	}
	nextis("enum"){
		//`enum _I_ _Aenum _ApushType
	}
	nextis("const"){
		//`const _Aconst
	}
	nextis("ptr"){
		//`* _Aptr
		pointer++;
	}
	nextis("newArray"){
		//`direct_declarator [ constant_expression _AcheckConst ] _AnewArray
		//`direct_declarator [ ] _AnewArray
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
	nextis("tellVar"){
		//`declarator _AtellVar
		//`declarator _AtellVar = assignment_expression _Asend=
		funcall(intToType);
		struct type*tmp=(struct type*)dimTypeStack.top();
		while(pointer>0)
			tmp=ptr(tmp),
			pointer--;
		Symbol s=newSymbol(idStack.top(),tmp,sclass,src);
		tellVar(s);idStack.pop();
	}
	nextis("if1"){
		//`logical_or_expression ? _Aif1 expression : _Aif2 conditional_expression _Aif3
		//`if ( expression ) _Aif1 statement _Aif2 else statement _Aif3
		ifid.push(ifId++);
		sendIf(ifid.top(),IF_THEN);
	}
	nextis("if2"){
		//`logical_or_expression ? _Aif1 expression : _Aif2 conditional_expression _Aif3
		//`if ( expression ) _Aif1 statement _Aif2 else statement _Aif3
		sendIf(ifid.top(),IF_ELSE);
	}
	nextis("if3"){
		//`logical_or_expression ? _Aif1 expression : _Aif2 conditional_expression _Aif3
		//`if ( expression ) _Aif1 statement _Aif2 else statement _Aif3
		sendIf(ifid.top(),IF_END);
		ifid.pop();
	}
	/*
		`for ( expression_statement _Afor1 expression_statement _Afor2 ) _Afor3 statement _Afor4
		`for ( expression_statement _Afor1 expression_statement _Afor2 expression ) _Afor3 statement _Afor4
		`continue _Afor6 ;
		`break _Afor5 ;
	*/
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
	nextis("ret"){
		//`return ; _Aret
		sendRet(newIntConst(0));
	}
	nextis("rete"){
		//`return expression ; _Arete
		Symbol s=sem.top();
		sem.pop();
		sendRet(s);
	}
	nextis("enterscope"){
		//`{ _Aenterscope declaration_list } _Aexitscope
		//`{ _Aenterscope statement_list } _Aexitscope
		//`{ _Aenterscope declaration_list statement_list } _Aexitscope
		enterScope();
	}
	nextis("exitscope"){
		//`{ _Aenterscope declaration_list } _Aexitscope
		//`{ _Aenterscope statement_list } _Aexitscope
		//`{ _Aenterscope declaration_list statement_list } _Aexitscope
		exitScope();
	}
	nextis("newfunc"){
		//`: type_specifier _I_ ( _Anewfunc parameter_list ) _Aaddfunc compound_statement
		//`: type_specifier _I_ ( _Anewfunc ) _Aaddfunc compound_statement
		funcall(intToType);
		struct type*tmp=newFunc(level,(struct type*)dimTypeStack.top());
		dimTypeStack.pop();
		dimTypeStack.push((int)tmp);
	}
	nextis("addfunc"){
		//`: type_specifier _I_ ( _Anewfunc parameter_list ) _Aaddfunc compound_statement
		//`: type_specifier _I_ ( _Anewfunc ) _Aaddfunc compound_statement
		dimTypeStack.pop();
		String name=idStack.top();idStack.pop();
		Symbol s=newSymbol(name,(struct type*)dimTypeStack.top(),SCLASS_CONST,src);
		dimTypeStack.pop();
		std::queue<Symbol>sq;
		sq.push(s);
		sendFunc(FUNC_DEFINE,sq);
	}
	nextis("funcAddKidWithName"){
		//`declaration_specifiers declarator _AfuncAddKidWithName
		funcall(intToType);
		struct type*kid=(struct type*)dimTypeStack.top();dimTypeStack.pop();
		struct type*fun=(struct type*)dimTypeStack.top();dimTypeStack.pop();
		addField(fun,kid);
		dimTypeStack.push((int)fun);
		dimTypeStack.push(0);
		fun=fun->kid;
		while(fun->next)
			fun=fun->next;
		fun->name=idStack.top();
		idStack.pop();
	}
	nextis("end"){
		//`translation_unit _Aend
		sendEnd();
	}
	nextis("newEnumenterscope"){

	}
	nextis("enumpushType"){

	}
	nextis("newSUenterscope"){

	}
	nextis("exitscopepushType"){

	}
	else{
#define 不会吧，有没捕捉的动作？？？ 0
		assert(不会吧，有没捕捉的动作？？？);
	}
	funcdef(intToType){
		int type=dimTypeStack.top();
		dimTypeStack.pop();
		struct type*tmp;
		if(type<TYPE){
			switch(type){
			case VOID:tmp=btot(TYPE_VOID);break;
			case FLOAT:tmp=btot(TYPE_VOID);break;
			case DOUBLE:tmp=btot(TYPE_VOID);break;
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