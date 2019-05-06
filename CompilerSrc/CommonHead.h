#pragma once
#include<algorithm>
#include<string.h>
#include<stdarg.h>
#include<assert.h>
#include<hash_map>
#include<stdio.h>
#include<string>
#include<vector>
#include<stack>
#include<queue>
#include<list>
#include<map>

#define Vector std::vector
typedef std::string String;
void error(const char* fmt,...);
void colorPrintf(int id,const char*format,...);

#define WordOutput true
#define ActionOutput true
#define FileOutput true
#define CompileFile "../unittest/Translator (4).txt"

#include"BackEnd.h"
#include"types.h"
#include"sym.h"
#include"LexicalAnalyzer.h"
#include"Translator.h"



//<macros>
//<data>
//<functions>

//<exported macros>
//<exported types>
//<exported typedefs>
//<exported functions>
//<exported data>