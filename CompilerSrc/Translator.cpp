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

void primary_expression();
void translation_unit();
void external_declaration();
void declaration_specifiers();
void declarator();
void compound_statement();
void initializer();
void init_declarator();
void expression();
void type_name();
void postfix_expression();
void postfix_expression_suffix();
void argument_expression_list();
void assignment_expression();
void unary_expression();
void unary_operator();
void cast_expression();
void multiplicative_expression();
void additive_expression();
void shift_expression();
void relational_expression();
void equality_expression();
void and_expression();
void exclusive_or_expression();
void inclusive_or_expression();
void logical_and_expression();
void logical_or_expression();
void conditional_expression();
void assignment_operator();
void init_declarator_list();
void storage_class_specifier();
void type_specifier();
void type_qualifier();
void struct_or_union_specifier();
void enum_specifier();
void struct_declaration();
void specifier_qualifier_list();
void struct_declarator_list();
void enumerator_list();
void enumerator();
void direct_declarator();
void parameter_type_list();
void ID_list();
void parameter_list();
void parameter_declaration();
void direct_abstract_declarator();
void initializer_list();
void statement();
void labeled_statement();
void expression_statement();
void  selection_statement();
void jump_statement();
void iteration_statement();
void declaration();
void direct_abstract_declarator_suffix();



void translation_unit(){
	while (inFirst("external_declaration"))
	{
		external_declaration();
	}
}

void external_declaration()
{
	declaration_specifiers();
	if (t == ';')
		expect(';');
	else
	{
		declarator();
		if (inFirst("compound_statement"))
			compound_statement();
		else
		{
			if (t == '=')
			{
				expect('=');
				initializer();
			}
			while (t == ',')
			{
				expect(',');
				init_declarator();
			}
			expect(';');
		}
	}
}

void primary_expression()
{
	switch (inFirst("primary_expression"))
	{
	case 1:
		expect(ID);
		break;
	case 2:
		if (t == ICON)
			expect(ICON);
		else 
			expect(FCON);
		break;
	case 3:
		expect(SCON);
		break;
	case 4:
		expect('(');
		if (inFirst("expression"))
			expression();
		else
			type_name();
		expect(')');
		break;
	}
}

void postfix_expression()
{
	primary_expression();
	while (inFirst("postfix_expression_suffix"))

		postfix_expression_suffix();
}

void postfix_expression_suffix()
{
	switch (inFirst("postfix_expression_suffix"))
	{
	case 1:
		expect('[');
		expression();
		expect(']');
		break;
	case 2:
		expect('(');
		if (inFirst("argument_expression_list"))
			argument_expression_list();
		expect(')');
		break;
	case 3:
		expect('.');
		expect(ID);
		break;
	case 4:
		expect(PTR_OP);
		expect(ID);
		break;
	case 5:
		expect(INC_OP);
		break;
	case 6:
		expect(DEC_OP);
		break;
	}
}

void argument_expression_list()
{
	assignment_expression();
	if (t == ',')
	{
		expect(',');
		assignment_expression();
	}
}

void unary_expression()
{
	switch (inFirst("unary_expression"))
	{
	case 1:
		postfix_expression();
		break;
	case 2:
		expect(INC_OP);
		unary_expression();
		break;
	case 3:
		expect(DEC_OP);
		unary_expression();
		break;
	case 4:
		unary_operator();
		cast_expression();
		break;
	case 5:
		expect(SIZEOF);
		unary_expression();
		break;
	}
}

void unary_operator()
{
	switch (inFirst("unary_operator"))
	{
	case 1:
		expect('&');
		break;
	case 2:
		expect('*');
		break;
	case 3:
		expect('+');
		break;
	case 4:
		expect('-');
		break;
	case 5:
		expect('~');
		break;
	case 6:
		expect('!');
		break;
	}
}

void cast_expression()
{
	unary_expression();
	if (inFirst("cast_expression"))
		cast_expression();
}

void multiplicative_expression()
{
	cast_expression();
	while (t=='*'|| t == '/'|| t == '%')
	{
		if (t == '*')
			expect('*');
		else if (t == '/')
			expect('/');
		else if (t == '%')
			expect('%');
		cast_expression();
	}
}

void additive_expression()
{
	multiplicative_expression();
	while (t == '+' || t == '-')
	{
		if (t == '+')
			expect('+');
		else
			expect('-');
		multiplicative_expression();
	}
}

void shift_expression()
{
	additive_expression();
	while (t == SHL_OP || t == SHR_OP)
	{
		if (t == SHL_OP)
			expect(SHL_OP);
		else
			expect(SHR_OP);
		additive_expression();
	}
}

void relational_expression()
{
	shift_expression();
	while (t == '<' || t == '>' || t == LE_OP || t == GE_OP)
	{
		if (t == '<')
			expect('<');
		else if (t == '>')
			expect('>');
		else if (t == LE_OP)
			expect(LE_OP);
		else if (t == GE_OP)
			expect(GE_OP);
		shift_expression();
	}
}

void equality_expression()
{
	relational_expression();
	while (t == EQ_OP || t == NEQ_OP)
	{
		if (t == EQ_OP)
			expect(EQ_OP);
		else
			expect(NEQ_OP);
		relational_expression();
	}
}

void and_expression()
{
	equality_expression();
	while (t == '&')
	{
		expect('&');
		equality_expression();
	}
}

void exclusive_or_expression()
{
	and_expression();
	while (t == '^')
	{
		expect('^');
		and_expression();
	}
}

void inclusive_or_expression()
{
	exclusive_or_expression();
	while (t == '|')
	{
		expect('|');
		exclusive_or_expression();
	}
}

void logical_and_expression()
{
	inclusive_or_expression();
	while (t == AND_OP)
	{
		expect(AND_OP);
		inclusive_or_expression();
	}
}

void logical_or_expression()
{
	logical_and_expression();
	while (t == OR_OP)
	{
		expect(OR_OP);
		logical_and_expression();
	}
}

void conditional_expression()
{
	logical_or_expression();
	if (t == '?')
	{
		expect('?');
		expression();
		expect(':');
		conditional_expression();
	}
}

void assignment_expression()
{
	unary_expression();
	if (inFirst("assignment_operator"))
	{
		assignment_operator();
		assignment_expression();
	}
	else
	{
		if (inFirst("cast_expression"))
			cast_expression();
		while (t == '*' || t == '/' || t == '%')
		{
			if (t == '*')
				expect('*');
			else if (t == '/')
				expect('/');
			else if (t == '%')
				expect('%');
			cast_expression();
		}
		while (t == '*' || t == '/')
		{
			if (t == '+')
				expect('+');
			else if (t == '-')
				expect('-');
			multiplicative_expression();
		}
		while (t == SHL_OP || t == SHR_OP)
		{
			if (t == SHL_OP)
				expect(SHL_OP);
			else
				expect(SHR_OP);
			additive_expression();
		}
		while (t == '<' || t == '>' || t == LE_OP || t == GE_OP)
		{
			if (t == '<')
				expect('<');
			else if (t == '>')
				expect('>');
			else if (t == LE_OP)
				expect(LE_OP);
			else if (t == GE_OP)
				expect(GE_OP);
			shift_expression();
		}
		while (t == EQ_OP || t == NEQ_OP)
		{
			if (t == EQ_OP)
				expect(EQ_OP);
			else
				expect(NEQ_OP);
			relational_expression();
		}
		while (t == '&')
		{
			expect('&');
			equality_expression();
		}
		while (t == '^')
		{
			expect('^');
			and_expression();
		}
		while (t == '|')
		{
			expect('|');
			exclusive_or_expression();
		}
		while (t == AND_OP)
		{
			expect(AND_OP);
			inclusive_or_expression();
		}
		while (t == OR_OP)
		{
			expect(OR_OP);
			logical_and_expression();
		}
		if (t == '?')
		{
			expect('?');
			expression();
			expect(':');
			conditional_expression();
		}
	}
}

void assignment_operator()
{
	switch (inFirst("assignment_operator"))
	{
	case 1:
		expect('=');
		break;
	case 2:
		expect(MUL_ASSIGN);
		break;
	case 3:
		expect(DIV_ASSIGN);
		break;
	case 4:
		expect(MOD_ASSIGN);
		break;
	case 5:
		expect(ADD_ASSIGN);
		break;
	case 6:
		expect(SUB_ASSIGN);
		break;
	case 7:
		expect(SHL_ASSIGN);
		break;
	case 8:
		expect(SHR_ASSIGN);
		break;
	case 9:
		expect(AND_ASSIGN);
		break;
	case 10:
		expect(XOR_ASSIGN);
		break;
	case 11:
		expect(OR_ASSIGN);
		break;

	}
}

void expression()
{
	assignment_expression();
	while (t == ',')
	{
		expect(',');
		assignment_expression();
	}
}

void declaration()
{
	declaration_specifiers();
	if (inFirst("init_declarator_list"))
		init_declarator_list();
	expect(';');
}

void declaration_specifiers()
{
	switch (inFirst("declaration_specifiers"))
	{
	case 1:
		storage_class_specifier();
		if (inFirst("declaration_specifiers"))
			declaration_specifiers();
		break;
	case 2:
		type_specifier();
		if (inFirst("declaration_specifiers"))
			declaration_specifiers();
		break;
	case 3:
		type_qualifier();
		if (inFirst("declaration_specifiers"))
			declaration_specifiers();
		break;
	}
}

void init_declarator_list()
{
	init_declarator();
	while (t == ',')
	{
		expect(',');
		init_declarator();
	}
}

void init_declarator()
{
	declarator();
	while (t == '=')
	{
		expect('=');
		initializer();
	}
}

void storage_class_specifier()
{
	switch (inFirst("storage_class_specifier"))
	{
	case 1:
		expect(TYPEDEF);
		break;
	case 2:
		expect(EXTERN);
		break;
	case 3:
		expect(STATIC);
		break;
	case 4:
		expect(AUTO);
		break;
	case 5:
		expect(REGISTER);
		break;
	}
}

void type_specifier()
{
	switch (inFirst("type_specifier"))
	{
	case 1:
		expect(VOID);
		break;
	case 2:
		expect(CHAR);
		break;
	case 3:
		expect(SHORT);
		break;
	case 4:
		expect(INT);
		break;
	case 5:
		expect(LONG);
		break;
	case 6:
		expect(FLOAT);
		break;
	case 7:
		expect(DOUBLE);
		break;
	case 8:
		expect(SIGNED);
		break;
	case 9:
		expect(UNSIGNED);
		break;
	case 10:
		struct_or_union_specifier();
		break;
	case 11:
		enum_specifier();
		break;
	case 12:
		expect(ID);
		break;
	}
}

void struct_or_union_specifier()
{
	if (t == STRUCT)
		expect(STRUCT);
	else if (t == UNION)
		expect(UNION);
	if (t == ID)
	{
		expect(ID);
		if (t == '{')
		{
			expect('{');
			while (inFirst("struct_declaration"))
				struct_declaration();
			expect('}');
		}
	}
	else if (t == '{')
	{
		expect('{');
		while (inFirst("struct_declaration"))
			struct_declaration();
		expect('}');
	}

}

void struct_declaration()
{
	specifier_qualifier_list();
	struct_declarator_list();
	expect(';');
}

void specifier_qualifier_list()
{
	switch (inFirst("specifier_qualifier_list"))
	{
	case 1:
		type_specifier();
		if(inFirst("specifier_qualifier_list"))
			specifier_qualifier_list();
		break;
	case 2:
		type_qualifier();
		if (inFirst("specifier_qualifier_list"))
			specifier_qualifier_list();
		break;
	}
}

void struct_declarator_list()
{
	declarator();
	while (t == ',')
	{
		expect(',');
		declarator();
	}
}

void enum_specifier()
{
	expect(ENUM);
	if (t == ID)
	{
		expect(ID);
		if (t == '{')
		{
			expect('{');
			enumerator_list();
			expect('}');
		}
	}
	else if (t == '{')
	{
		expect('{');
		enumerator_list();
		expect('}');
	}
}

void enumerator_list()
{
	enumerator();
	while (t == ',')
	{
		expect(',');
		enumerator();
	}
}

void enumerator()
{
	expect(ID);
	if (t == '=')
	{
		expect('=');
		conditional_expression();
	}
}

void type_qualifier()
{
	switch (inFirst("type_qualifier"))
	{
	case 1:
		expect(CONST);
		break;
	case 2:
		expect(VOLATILE);
		break;
	}
}

void declarator()
{
	while (t == '*')
	{
		expect('*');
		while (inFirst("type_qualifier"))
			type_qualifier();
	}
	direct_declarator();
}

void direct_declarator()
{
	switch (inFirst("direct_declarator"))
	{
	case 1:
		expect(ID);
		break;
	case 2:
		expect('(');
		declarator();
		expect(')');
		break;
	}
}

void direct_declarator_suffix()
{
	switch (inFirst("direct_declarator"))
	{
	case 1:
		expect('[');
		if (inFirst("conditional_expression"))
			conditional_expression();
		expect(']');
		break;
	case 2:
		expect('(');
		if (inFirst("parameter_type_list") || inFirst("ID_list"))
		{
			if (inFirst("parameter_type_list"))
				parameter_type_list();
			else 
				ID_list();
		}
		expect(')');
		break;
	}
}

void parameter_type_list()
{
	parameter_list();
	if (t == ',')
	{
		expect(',');
		expect(ELLIPSIS);
	}
}

void parameter_list()
{
	parameter_declaration();
	while (t == ',')
	{
		expect(',');
		parameter_declaration();
	}
}

void parameter_declaration()
{
	declaration_specifiers();
	if (inFirst("declarator"))
		declarator();
	else 
	{
		if (t == '*')
		{
			while (t == '*')
			{
				expect('*');
				while (inFirst("type_qualifier"))
					type_qualifier();
			}
			if (inFirst("direct_abstract_declarator"))
				direct_abstract_declarator();
		}
		else if (inFirst("direct_abstract_declarator"))
			direct_abstract_declarator();
	}
}

void ID_list()
{
	expect(ID);
	while (t == ',')
	{
		expect(',');
		expect(ID);
	}
}

void type_name()
{
	specifier_qualifier_list();
	if (t == '*'||inFirst("direct_abstract_declarator"))
	{
		if (inFirst("direct_abstract_declarator"))
			direct_abstract_declarator();
		else
			while (t == '*')
			{
				expect('*');
				while (inFirst("type_qualifier"))
					type_qualifier();
			}
		if (inFirst("direct_abstract_declarator"))
			direct_abstract_declarator();
	}
}

void direct_abstract_declarator()
{
	switch (inFirst("direct_abstract_declarator"))
	{
	case 1:
		expect('(');
		if (t == '*' || inFirst("parameter_type_list") || inFirst("direct_abstract_declarator"))
		{
			if (inFirst("parameter_type_list"))
				parameter_type_list();
			else if (t == '*')
			{
				while (t == '*')
				{
					expect('*');
					while (inFirst("type_qualifier"))
						type_qualifier();
				}
				if (inFirst("direct_abstract_declarator"))
					direct_abstract_declarator();
			}
			else
				direct_abstract_declarator();
		}
		expect(')');
		while(inFirst("direct_abstract_declarator_suffix"))
			direct_abstract_declarator_suffix();
		break;
	case 2:
		expect('[');
		if (inFirst("conditional_expression"))
			conditional_expression();
		expect(']');
		while (inFirst("direct_abstract_declarator_suffix"))
			direct_abstract_declarator_suffix();
		break;
	}
}

void direct_abstract_declarator_suffix()
{
	switch (inFirst("direct_abstract_declarator_suffix"))
	{
	case 1:
		expect('[');
		if (inFirst("conditional_expression"))
			conditional_expression();
		expect(']');
		break;
	case 2:
		expect('(');
		if (inFirst("parameter_type_list"))
			parameter_type_list();
		expect(')');
		break;
	}
}

void initializer()
{
	switch (inFirst("initializer"))
	{
	case 1:
			assignment_expression();
		break;
	case 2:
		expect('{');
		initializer_list();
		if (t == ',')
			expect(',');
		expect('}');
	}
}

void initializer_list()
{
	initializer();
	while (t == ',')
	{
		expect(',');
		initializer();
	}
}

void statement()
{
	switch (inFirst("statement"))
	{
	case 1:
		labeled_statement();
		break;
	case 2:
		compound_statement();
		break;
	case 3:
		expression_statement();
		break;
	case 4:
		selection_statement();
		break;
	case 5:
		iteration_statement();
		break;
	case 6:
		jump_statement();
		break;
	}
}

void labeled_statement()
{
	switch (inFirst("labeled_statement"))
	{
	case 1:
		expect(ID);
		expect(':');
		statement();
		break;
	case 2:
		expect(CASE);
		conditional_expression();
		expect(':');
		statement();
		break;
	case 3:
		expect(DEFAULT);
		expect(':');
		statement();
		break;
	}
}

void compound_statement()
{
	expect('{');
	while (inFirst("declaration"))
		declaration();
	while (inFirst("statement"))
		statement();
	expect('}');
}

void expression_statement()
{
	if (inFirst("expression"))
		expression();
	expect(';');
}

void selection_statement()
{
	switch (inFirst("selection_statement"))
	{
	case 1:
		expect(IF);
		expect('(');
		expression();
		expect(')');
		statement();
		if (t == ELSE)
		{
			expect(ELSE);
			statement();
		}
		break;
	case 2:
		expect(SWITCH);
		expect('(');
		expression();
		expect(')');
		statement();
		break;
	}
}

void iteration_statement()
{
	switch (inFirst("iteration_statement"))
	{
	case 1:
		expect(WHILE);
		expect('(');
		expression();
		expect(')');
		statement();
		break;
	case 2:
		expect(DO);
		statement();
		expect(WHILE);
		expect('(');
		expression();
		expect(')');
		expect(';');
		break;
	case 3:
		expect(FOR);
		expect('(');
		expression_statement();
		expression_statement();
		if (inFirst("expression"))
			expression();
		expect(')');
		statement();
		break;
	}
}

void jump_statement()
{
	switch (inFirst("jump_statement"))
	{
	case 1:
		expect(GOTO);
		expect(ID);
		expect(';');
		break;
	case 2:
		expect(CONTINUE);
		expect(';');
		break;
	case 3:
		expect(BREAK);
		expect(';');
		break;
	case 4:
		expect(RETURN);
		if (inFirst("expression"))
			expression();
		expect(';');
		break;
	}
}
