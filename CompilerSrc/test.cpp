#include"CommonHead.h"
int main(){
    freopen("../unittest/LexicalAnalyzer1.txt","r",stdin);
	backEndInit();
	lexAnalyzerInit();
	symInit();
	translatorInit();
	typeInit();
	return 0;
}