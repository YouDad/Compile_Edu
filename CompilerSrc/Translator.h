#pragma once
#include"CommonHead.h"
//<exported macros>
//<exported types>
//<exported typedefs>
//<exported functions>
void translatorInit();
void Start();
//<exported data>
//语义栈
extern std::stack<struct symbol*> sem;
//关键字的字符串表示
extern const char* keyword[];