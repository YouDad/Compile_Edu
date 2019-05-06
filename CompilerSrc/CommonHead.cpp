#include"CommonHead.h"
#include<stdarg.h>
#include<Windows.h>
void error(const char* fmt,...){
	HANDLE hwnd=GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hwnd,4);
    va_list ap;va_start(ap,fmt);
	printf("%d,%d: ",src.x,src.y);
    vprintf(fmt,ap);va_end(ap);
	printf("\n");
    SetConsoleTextAttribute(hwnd,15);
	exit(1);
}
//printf colorful text,id means colorid
//Blue=1,Green=2,3=Blue+Green=Clay,Red=4,...
void colorPrintf(int id,const char*format,...){
    HANDLE hwnd=GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hwnd,8+id);
    va_list ap;va_start(ap,format);
    vprintf(format,ap);va_end(ap);
    SetConsoleTextAttribute(hwnd,15);
}