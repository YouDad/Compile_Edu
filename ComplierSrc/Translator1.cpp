#include"Translator.h"
//<macros>
//<data>
//<functions>
//FIRST表查找,返回数字即第几个产生式




void Start()
{
	while (1)
	{
		switch (inFirst("Start")) 
		{
		case 1:Store();
		case 2:
			Type();
			//_setvartype动作
			IDevList();
			expect(';');
			break;
		case 3:
			expect(TYPEDEF);
			if (t == ID)
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
			if (t == ID)
				expect(ID);
			//_newenumtype
			expect('{');
			IDecList();
			expect('}');
			expect(';');
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

void Sents()
{
	switch (inFirst("Sents"))
	{
		case 1:
			expect(ID);
			expect(':');
			//_addlabel
		case 2:
			Sent();
			if (inFirst("Sents"))
				Sents();
			break;

	}
}

void BSent()
{
	switch (inFirst("BSent"))
	{
	case 1:
		Sent();
		break;
	case 2:
		expect('{');
		if (inFirst("Sents"))
		{
			Sents();
		}
		expect('}');
		break;
	}
}

void BSent_bc()
{
	switch (inFirst("BSent_bc"))
	{
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

/////////
void Sent()
{
	switch (inFirst("Sent"))
	{
	case 1:Store();
	case 2:
		Type();
		//_setvartype动作
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
		if (t == ID)
		{
			expect(ID);
		}
		else
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
		if (t == ID)
		{
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
		if (t == ELSE)
		{
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

void Store()
{
	switch (inFirst("Store"))
	{
	case 1:
		expect(AUTO);
		//_setstoreclass
		break;
	case 2:
		expect(CONST);
		//_setstoreclass
		break;
	case 3:
		expect(STATIC);
		//_setstoreclass
		break;
	case 4:
		expect(EXTERN);
		//_setstoreclass
		break;
	}
}

void IDevList()             //检查
{
	while (1)
	{

		switch (inFirst("IDevList"))
		{
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
	if (t == '=')
	{
		expect('=');
		Expr();
	}
	//__setval
	if (t == ',')
	{
		expect(',');
		IDevList();
	}

}

void IDecList()
{
	expect(ID);
	if (t == '=')
	{
		expect('=');
		ConstExpr();
		//_savenum
	}
	//_addenum
	if (t == ',')
	{
		expect(',');
		IDecList();
	}
}

void Type()
{
	switch (inFirst("Type"))
	{
	case 1:
		expect(STRUCT);
		if (t == ID)
		{
			expect(ID);
			//_savesid
			if (inFirst("StructDim"))
			{
				StructDim();
			}
		}
		else
		{
			//_newunnamedsid
			StructDim();
		}
		break;
	case 2:
		expect(UNION);
		if (t == ID)
		{
			//_saveuid
			if (inFirst("StructDim"))
			{
				StructDim();
			}
			else
				//_newunnameduid
				StructDim();
		}
		break;
	case 3:
		expect(DOUBLE);
		//_setvartyp
		break;
	case 4:
		expect(FLOAT);
		//_setvartyp
		break;
	case 5:
		expect(VOID);
		expect('*');
		//_setvartyp
		break;
	case 6:
		expect(SIGNED);
		goto out;
	case 8:
		expect(UNSIGNED);
		// _setvarunsigned
		goto out;
	case 10:
		expect(ENUM);
		expect(ID);
		break;
	}
	// case 7 9 
out:if (t == INT)
		{
		expect(INT);
		}
		else if (t == LONG)
		{
		expect(LONG);
		if (t == INT)
			expect(INT);
		}
		else if (t == SHORT)
		{
		expect(SHORT);
		if (t == INT)
			expect(INT);
		}
		else if (t == CHAR)
		{
		expect(CHAR);
		}
	//_setvartype
}

void StructDim()
{
	expect('{');
	if (inFirst("SdimList"))
	{
		SdimList();
	}
	expect('}');
}

void SdimList()
{
	Field();
	expect(';');
	if (inFirst("SdimList"))
	{
		SdimList();
	}
}

void Field()
{
	switch (inFirst("Field"))
	{
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

void IDList()
{
	while(1)
	{

		switch (inFirst("IDList"))
		{
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
	if (t == ',')
	{
		expect(',');
		IDList();
	}
}

void Expr()
{
	Aexpr();
	//_popdeinc
	while (t == ',')/////////////////////////检查
	{
		expect(',');
		Aexpr();
		//_popdeinc
	}
}

void Aexpr()
{
	switch (inFirst("Aexpr"))
	{
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

void Aop()
{
	switch (inFirst("Aop"))
	{
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
		expect('+');
		//__setop
		expect('*');
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
		expect('^');
		//__setop
		expect('=');
		break;
	case 10:
		expect('|');
		//__setop
		expect('=');
		break;
	case 11:
		//__setop
		expect('=');
		break;
	}
}

void Cexpr()
{
	Bexpr();
	if (t == '?')
	{
		expect('?');
		//_popandgt
		Expr();
		expect(':');
		//_else
		Expr();
		//_ifend
	}
}

void Bexpr()
{
	Uexpr();
	while (inFirst("Bop"))
	{
		Bop();
		//_setop
		Uexpr();
		//_genqua
	}
}

void Bop()
{
	switch (inFirst("Bop"))
	{
	case 1:
		expect('|');
		expect('|');
		break;
	case 2:
		expect('&');
		expect('&');
		break;
	case 3:
		expect('|');
		break;	
	case 4:
		expect('^');
		break;
	case 5:
		expect('&');
		break;
	case 6:
		expect('=');
		expect('=');
		break;
	case 7:
		expect('!');
		expect('=');
		break;
	case 8:
		expect('<');
		break;
	case 9:
		expect('>');
		break;
	case 10:
		expect('<');
		expect('=');
		break;
	case 11:
		expect('>');
		expect('=');
		break;
	case 12:
		expect('<');
		expect('<');
		break;
	case 13:
		expect('>');
		expect('>');
		break;
	case 14:
		expect('+');
		break;
	case 15:
		expect('-');
		break;
	case 16:
		expect('*');
		break;
	case 17:
		expect('/');
		break;
	case 18:
		expect('%');
		break;
	}
}

void Uexpr()   //检查pop
{
	switch (inFirst("Uexpr")) {
	case 1:
		expect(ID);
		//_push
		while (inFirst("Pop"))
		{
			Pop();
		}
		break;
	case 2:
		expect(ICON);
		//_push
		while (inFirst("Pop"))
		{
			Pop();
		}
		break;
	case 3:
		expect(FCON);
		//_push
		while (inFirst("Pop"))
		{
			Pop();
		}
		break;
	case 4:
		expect(SCON);
		//_push
		while (inFirst("Pop"))
		{
			Pop();
		}
		break;
	case 5:
		Uop();
		Uexpr();
		break;
	case 6:
		expect(SIZEOF);
		if (t == '(') {
			expect('(');
			Type();
			expect(')');
		}
		else {
			Uexpr();
		}
		//_push
		break;
	case 7:
		expect('(');
		if (inFirst("Type")) {
			Type();
			//_pushtype
			expect(')');
			Uexpr();
		}
		else {
			Expr();
			expect(')');
			//_poptype
			//_push
			while (inFirst("Pop"))
			{
				Pop();
			}
			break;
		}
	}
}

void Uop()
{
	switch (inFirst("Uop"))
	{
	case 1:
		expect('+');
		if (t == '+')
			expect('+');
		break;
	case 2:
		expect('-');
		if (t == '-')
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

void Pop()
{
	switch (inFirst("Pop"))
	{
	case 1:
		expect('(');
		if (inFirst("Aexpr"))
		{
			Aexpr();
			//_popdeinc
			while (t == ',')
			{
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
		if (t == '-')
		{
			expect('-');
			//_pushdec
		}
		else
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






//错误期待
void expect(int token)
{
	if (t == token)
		t = getToken();
	else
	{
		
		static char str[20];
		switch (t) {
		case INT:strcpy(str, "int"); break;
		case AUTO:strcpy(str, "auto"); break;
		case BREAK:strcpy(str, "break"); break;
		case CASE:strcpy(str, "case"); break;
		case CHAR:strcpy(str, "char"); break;
		case CONST:strcpy(str, "const"); break;
		case CONTINUE:strcpy(str, "continue"); break;
		case DEFAULT:strcpy(str, "default"); break;
		case DO:strcpy(str, "do"); break;
		case DOUBLE:strcpy(str, "double"); break;
		case ELSE:strcpy(str, "else"); break;
		case ENUM:strcpy(str, "enum"); break;
		case EXTERN:strcpy(str, "extern"); break;
		case FLOAT:strcpy(str, "float"); break;
		case FOR:strcpy(str, "for"); break;
		case GOTO:strcpy(str, "goto"); break;
		case IF:strcpy(str, "if"); break;
		case LONG:strcpy(str, "long"); break;
		case RETURN:strcpy(str, "return"); break;
		case SHORT:strcpy(str, "short"); break;
		case SIGNED:strcpy(str, "signed"); break;
		case SIZEOF:strcpy(str, "sizeof"); break;
		case STATIC:strcpy(str, "static"); break;
		case STRUCT:strcpy(str, "struct"); break;
		case SWITCH:strcpy(str, "switch"); break;
		case TYPEDEF:strcpy(str, "typedef"); break;
		case UNSIGNED:strcpy(str, "unsigned"); break;
		case VOID:strcpy(str, "void"); break;
		case WHILE:strcpy(str, "while"); break;
		case REGISTER:strcpy(str, "register"); break;
		case UNION:strcpy(str, "union"); break;
		case VOLATILE:strcpy(str, "volatile"); break;
		case ID:strcpy(str, "id"); break;
		case ICON:strcpy(str, "icon"); break;
		case FCON:strcpy(str, "fcon"); break;
		case SCON:strcpy(str, "scon"); break;
		default:
			sprintf(str, "'%c'", t);
		}
		fprintf(stderr, "Error %d,%d:expected %s\n", src.x, src.y,str);
	}
}


