#include"types.h"
//<macros>
//<data>
	//用hash技术可以降低复杂度
	struct entry{
		struct type type;
		struct entry* next;
	}* typetable[128];
//<functions>