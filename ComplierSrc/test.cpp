#include"CommonHead.h"
int main(){
	freopen("../unittest/LexicalAnalyzer.txt","r",stdin);
	//src.file="LexicalAnalyzer.txt";
	int cnt=0;
	while(-1!=(t=getToken())){
		printf("%2d token:%d string:%s\n",cnt,t,token.c_str());
		cnt++;
	}
	return 0;
}