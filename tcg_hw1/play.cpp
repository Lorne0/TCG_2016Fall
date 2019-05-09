#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<algorithm>
#include<string>
#include<vector>
#include<time.h>
using namespace std;


vector<int> colhint[100][100]; //problem: 1~1000, hint:1~10 
vector<int> rawhint[100][100];
vector<vector<int> > col_all_vec[100][100];
vector<vector<int> > raw_all_vec[100][100];
int **M_ans,**M_tmp;
int finish_flag=0;
int fill_num=0;

void printvec(vector<int> a){
	for(int i=0;i<a.size();i++) printf("%d",a[i]);
	printf("\n");
	return;
}

void printtmp(int size){
	for(int i=1;i<=size;i++){
		for(int j=1;j<=size;j++) printf("%d ",M_tmp[i][j]);
		printf("\n");
	}
}

void printans(int size){
	for(int i=1;i<=size;i++){
		for(int j=1;j<=size;j++) printf("%d\t",M_ans[i][j]);
		printf("\n");
	}
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

int count_freed(int size, vector<int> a){
	if(a.empty()) return size;
	int sum=a.size()-1;
	for(auto &p:a) sum+=p;
	return size-sum;
}

void listall_col(vector<int> a, vector<int> b,int freed,int pbm,int col){
	if(a.empty()){
		for(int i=1;i<=freed;i++) b.push_back(0);
		col_all_vec[pbm][col].push_back(unfoldvec(b));
		return;
	}

	for(int i=0;i<=freed;i++){
		vector<int> tmpb=b;
		for(int j=0;j<i;j++) tmpb.push_back(0);
		tmpb.push_back(a[0]);
		vector<int> tmp=a;
		tmp.erase(tmp.begin());
		listall_col(tmp,tmpb,freed-i,pbm,col);
	}
	return;
}

void listall_raw(vector<int> a, vector<int> b,int freed,int pbm,int col){
	if(a.empty()){
		for(int i=1;i<=freed;i++) b.push_back(0);
		raw_all_vec[pbm][col].push_back(unfoldvec(b));
		return;
	}

	for(int i=0;i<=freed;i++){
		vector<int> tmpb=b;
		for(int j=0;j<i;j++) tmpb.push_back(0);
		tmpb.push_back(a[0]);
		vector<int> tmp=a;
		tmp.erase(tmp.begin());
		listall_raw(tmp,tmpb,freed-i,pbm,col);
	}
	return;
}


//combine the same element and fill M_ans
vector<int> combine(vector<vector<int> > a, int size, int ifcol, int colraw, int pbm){

	vector<int> update;

	int k=a.size();


	int *com;
	com = (int *)malloc(size*sizeof(int));
	for(int i=0;i<size;i++) com[i]=0;
	for(auto &p:a){
		for(int i=0;i<size;i++) com[i]+=p[i];
	}
	for(int i=1;i<=size;i++){
		//fill M_ans's col
		if(ifcol==1){
			if(M_ans[i][colraw]==-1){
				if(com[i-1]==0){
					M_ans[i][colraw]=0;
					fill_num++;
					update.push_back(i);
				}
				else if(com[i-1]==k){
					M_ans[i][colraw]=1;
					fill_num++;
					update.push_back(i);
				}
			}
		}
		//fill M_ans's raw
		else if(ifcol==0){
			if(M_ans[colraw][i]==-1){
				if(com[i-1]==0){
					M_ans[colraw][i]=0;
					fill_num++;
					update.push_back(i);
				}
				else if(com[i-1]==k){
					M_ans[colraw][i]=1;
					fill_num++;
					update.push_back(i);
				}
			}
		}
	}
	return update;
}

void delete_q(int pbm, int p, vector<int> q, int ifcol){
	//col
	if(ifcol==1){
		col_all_vec[pbm][p].erase(remove(col_all_vec[pbm][p].begin(), col_all_vec[pbm][p].end(), q), col_all_vec[pbm][p].end() );
	}
	//raw
	else{
		raw_all_vec[pbm][p].erase(remove(raw_all_vec[pbm][p].begin(), raw_all_vec[pbm][p].end(), q), raw_all_vec[pbm][p].end() );
	}
}

void updaterc(vector<int> update, int size, int ifcol, int pbm){
	//update col
	if(ifcol==1){
		//the pth col has to update
		for(auto &p:update){
			vector<vector<int> > gar_can;
			for(auto &q:col_all_vec[pbm][p]){
				for(int i=1;i<=size;i++){
					if(M_ans[i][p]==-1) continue;
					if(q[i-1]!=M_ans[i][p]){
						gar_can.push_back(q);
						break;
					}
				}
			}
			for(auto &q:gar_can) delete_q(pbm,p,q,1);
		}
	}
	//update raw
	else{
		//the pth row has to update
		for(auto &p:update){
			vector<vector<int> > gar_can;
			for(auto &q:raw_all_vec[pbm][p]){
				//q: ex 1101110110
				for(int i=1;i<=size;i++){
					if(M_ans[p][i]==-1) continue;
					if(q[i-1]!=M_ans[p][i]){
						gar_can.push_back(q);
						break;
					}
				}
			}
			for(auto &q:gar_can) delete_q(pbm,p,q,0);
		}
	}
}



void heuristic(int size, int pbm){
	int tmp_fill_num;
	//combine the same element and fill M_ans
	do{
		tmp_fill_num=fill_num;
		//combine the ith col
		for(int i=1;i<=size;i++){
			vector<int> update;
			update=combine(col_all_vec[pbm][i], size, 1, i, pbm);
			//if col filled the M_ans, update raw_all_vec
			if(!update.empty()) updaterc(update, size, 0, pbm);

		}

		//combine the ith raw
		for(int i=1;i<=size;i++){
			vector<int> update;
			update=combine(raw_all_vec[pbm][i], size, 0, i, pbm);

			//if raw filled the M_ans, update col_all_vec
			if(!update.empty()) updaterc(update, size, 1, pbm);
		}

	}while(tmp_fill_num != fill_num);

	return;
}






int verify(int size, int pbm){
	for(int i=1;i<=size;i++){
		vector<int> tmpv;
		int sum=0;
		for(int j=1;j<=size;j++){
			if(M_tmp[i][j]==1) sum++;
			else if(M_tmp[i][j]==-1) return 0;
			else if(M_tmp[i][j]==0 && sum>0){
				tmpv.push_back(sum);
				sum=0;
			}
		}
		if(sum>0) tmpv.push_back(sum);
		
		if(rawhint[pbm][i].empty()){
			if(!tmpv.empty()) return 0;
		}
		else if(!equal(tmpv.begin(),tmpv.end(),rawhint[pbm][i].begin())) return 0;
	}
	return 1;
}


int check(int size, int pbm, int col){
	//the ith raw
	for(int i=1;i<=size;i++){
		//collect the blocks
		vector<int> tmpv;
		int sum=0;
		for(int j=1;j<=col;j++){
			if(M_tmp[i][j]==1) sum++;
			else if(M_tmp[i][j]==-1) return 0;
			else if(M_tmp[i][j]==0 && sum>0){
				tmpv.push_back(sum);
				sum=0;
			}
		}
		if(sum>0) tmpv.push_back(sum);
		
		//raw empty -> collected needs to be empty too
		if(rawhint[pbm][i].empty()){
			if(!tmpv.empty()) return 0;
		}
		
		//the # of collected block has to be <= the raw blocks
		else if(tmpv.size()>rawhint[pbm][i].size()){
			return 0;
		}

		//if collected empty: col <= freed
		else if(tmpv.empty()){
			if(col>count_freed(size,rawhint[pbm][i])) return 0;
		}

		//every block has to be the same # except the last one can be <=
		else{
			for(int j=1;j<tmpv.size();j++){
				if(tmpv[j-1]!=rawhint[pbm][i][j-1]) return 0;
			}
			if(tmpv[tmpv.size()-1]>rawhint[pbm][i][tmpv.size()-1]) return 0;
		}
	}
	return 1;
}

void fill_table(int pbm, int col, int size){
	if(col>size){
		if(verify(size,pbm)==1) finish_flag=1;
		return;
	}
	for(auto &p:col_all_vec[pbm][col]){
		if(finish_flag==1) break;
		for(int i=1;i<=size;i++) M_tmp[i][col]=p[i-1];

		if(check(size,pbm,col)==0) continue;

		fill_table(pbm, col+1, size);
	}
	return;
}

int main(int argc, char* argv[]){

	int size = atoi(argv[1]);

	char str[100];
	FILE *fp;
	int problem_cnt=1;
	int hint_cnt=1;
	fp=fopen("test.txt","r");
	while(fgets(str,100,fp)!=NULL){
		if(str[0]=='$'){
			str[0]='0';
			problem_cnt = atoi(str);
			//printf("Number of problem:%d\n",problem_cnt);
			hint_cnt=1;
		}
		else{
			str[strlen(str)-1]='\0';
			char *p=strtok(str,"\t");
			while(p!=NULL){
				if(hint_cnt<=size){
					colhint[problem_cnt][hint_cnt].push_back(atoi(p));
				}
				else if(hint_cnt>size){
					rawhint[problem_cnt][hint_cnt-size].push_back(atoi(p));
				}
				p=strtok(NULL,"\t");
			}
			hint_cnt++;	
		}
	}

	printf("Number of problem:%d\n",problem_cnt);

	clock_t t0;
	t0=clock();
	for(int pbm=1;pbm<=problem_cnt;pbm++){
		printf("Problem:%d\n",pbm);

		clock_t t1,t2;
		t1=clock();

		M_ans = (int**)malloc((size+2)*sizeof(int *));
		for(int i=0;i<(size+2);i++) M_ans[i]=(int*)malloc((size+2)*sizeof(int));
		M_tmp = (int**)malloc((size+2)*sizeof(int *));
		for(int i=0;i<(size+2);i++) M_tmp[i]=(int*)malloc((size+2)*sizeof(int));
		for(int i=0;i<(size+2);i++)
			for(int j=0;j<(size+2);j++){
				M_ans[i][j]=-1;
				M_tmp[i][j]=-1;
			}

		/*preprocess:store all the states*/
		//printf("Preprocessing\n");
		for(int i=1;i<=size;i++){
			int freed=count_freed(size,colhint[pbm][i]);
			vector<int> b;
			listall_col(colhint[pbm][i],b,freed,pbm,i);
		}
		for(int i=1;i<=size;i++){
			int freed=count_freed(size,rawhint[pbm][i]);
			vector<int> b;
			listall_raw(rawhint[pbm][i],b,freed,pbm,i);
		}

		t2=clock();
		printf("Preprocessing: %lf secs\n",(t2-t1)/(double)(CLOCKS_PER_SEC));


		/*Heuristic*/
		t1=clock();

		fill_num=0;
		heuristic(size, pbm);
		printf("%d\n",fill_num);

		t2=clock();
		printf("Heuristic: %lf secs\n",(t2-t1)/(double)(CLOCKS_PER_SEC));
		/////
		
		//printans(size);

		for(int i=1;i<=size;i++)
			for(int j=1;j<=size;j++) M_tmp[i][j]=M_ans[i][j];

		//Search
		t1=clock();
		
		finish_flag=0;
		fill_table(pbm,1,size);
		printtmp(size);

		t2=clock();
		printf("Search: %lf secs\n",(t2-t1)/(double)(CLOCKS_PER_SEC));
		printf("\n");
		

	}
	//for(int i=0;i<all_vec[2][4].size();i++)
	//	printvec(all_vec[2][4][i]);
	clock_t t3=clock();
	printf("Total: %lf secs\n",(t3-t0)/(double)(CLOCKS_PER_SEC));

	return 0;
}








