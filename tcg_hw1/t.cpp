#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<vector>
#include<algorithm>

using namespace std;

typedef struct heu{
	vector<int> a[10];
}H;


void foo(vector<int> *b){
	(*b).push_back(1234);
	return;
}

int main(){

	H h;
	h.a[1].push_back(1);
	h.a[1].clear();
	h.a[0].clear();


	return 0;
}
