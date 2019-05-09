#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<vector>

using namespace std;

void printvec(vector<int> a){
	for(int i=0;i<10;i++) printf("%d",a[i]);
	printf("\n");
	return;
}


vector<int> unfoldvec(vector<int> a){
	vector<int> b;
	for(int i=0;i<a.size();i++){
		for(int j=0;j<a[i];j++) b.push_back(1);
		b.push_back(0);
	}
	b.pop_back();
	return b;
}

void listall(vector<int> a, vector<int> b,int freed){
	if(a.empty()){
		for(int i=1;i<=freed;i++) b.push_back(0);
		printvec(unfoldvec(b));
		return;
	}

	for(int i=0;i<=freed;i++){
		vector<int> tmpb=b;
		for(int j=0;j<i;j++) tmpb.push_back(0);
		tmpb.push_back(a[0]);
		vector<int> tmp=a;
		tmp.erase(tmp.begin());
		listall(tmp,tmpb,freed-i);
	}




}


int main(){
	vector<int> a;
	a.push_back(3);
	a.push_back(1);
	a.push_back(2);

	int freed=2;
	vector<int> b;
	listall(a,b,freed);

	return 0;
}
