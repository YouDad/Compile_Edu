#include"CommonHead.h"
void error(const char* fmt,...){
	//fprintf(stderr,"%s:%d:%d: ",src.file,src.x,src.y);
	fprintf(stderr,"%s:%d: ",src.file,src.x);
	va_list ap;
	va_start(ap,fmt);
	vfprintf(stderr,fmt,ap);
	va_end(ap);
	fprintf(stderr,"\n");
	exit(1);
}
#include<stdarg.h>
#include<Windows.h>
//printf colorful text,id means colorid
//Blue=1,Green=2,3=Blue+Green=Clay,Red=4,...
void colorPrintf(int id,const char*format,...){
    HANDLE hwnd=GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hwnd,8+id);
    va_list ap;va_start(ap,format);
    vprintf(format,ap);va_end(ap);
    SetConsoleTextAttribute(hwnd,7);
}