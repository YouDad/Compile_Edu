#include"CommonHead.h"
int main(){
	freopen("../unittest/LexicalAnalyzer_HXH.txt","r",stdin);
	src.file="LexicalAnalyzer_HXH.txt";
	int cnt=0;
	while(-1!=(t=getToken())){
		printf("%2d token:%d string:%s\n",cnt,t,token.c_str());
		cnt++;
	}
	return 0;
}