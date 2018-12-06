#include"CommonHead.h"
int main(){
	freopen("../unittest/LexicalAnalyzer_HXM.txt","r",stdin);
	src.file="LexicalAnalyzer_HXM.txt";
	while(-1!=(t=getToken())){
		printf("token:%d string:%s\n",t,token.c_str());
	}
	return 0;
}