#include"CommonHead.h"
int main(){
	freopen("../unittest/LexicalAnalyzer_LWH.txt","r",stdin);
	src.file="LexicalAnalyzer_LWH.txt";
	while(-1!=(t=getToken())){
		printf("token:%d\n",t);
	}
	return 0;
}