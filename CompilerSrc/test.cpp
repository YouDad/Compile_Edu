#include"CommonHead.h"
int main(){
	int a=1;
	int b=1;
	int c=a+++b;
	typedef int x;
	const int d=sizeof (1+2);
    freopen("../unittest/Translator (3).txt","r",stdin);
	backEndInit();
	lexAnalyzerInit();
	symInit();
	translatorInit();
	typeInit();
	Start();
	return 0;
}