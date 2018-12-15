#include"CommonHead.h"
void error(const char* fmt,...){
	//fprintf(stderr,"%s:%d:%d: ",src.file,src.x,src.y);
	fprintf(stderr,"%d:%d: ",src.x,src.y);
	va_list ap;
	va_start(ap,fmt);
	vfprintf(stderr,fmt,ap);
	va_end(ap);
	fprintf(stderr,"\n");
	exit(1);
}