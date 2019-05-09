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

int size;
int pbm_now;

int max_depth;
int max_ans_depth;

typedef struct heuris{
	int M[50][50];
	int fillnum=0;
	vector<int> update;
	vector<vector<int> > col_vec[50];
	vector<vector<int> > raw_vec[50];

	double prob[50][50];

}Heuristic;

Heuristic H_final;

void printvec(vector<int> a){
	for(int i=0;i<a.size();i++) printf("%d",a[i]);
	printf("\n");
	return;
}

void print2Darray(int a[50][50]){
	for(int i=1;i<=size;i++){
		for(int j=1;j<=size;j++) printf("%d ",a[i][j]);
		printf("\n");
	}
	return;
}

void print2Darray_double(double a[50][50]){
	for(int i=1;i<=size;i++){
		for(int j=1;j<=size;j++){
			if(a[i][j]==0 || a[i][j]==1) printf("%d\t\t",int(a[i][j]));
			else printf("%lf\t",a[i][j]);
		}
		printf("\n");
	}
	return;
}

int least_element(const vector<vector<int> > a[50]){
	int minv=100000000;
	int mini=1;
	for(int i=1;i<=size;i++){
		if(a[i].size()>=2 && a[i].size()<minv){
			minv = a[i].size();
			mini = i;
		}
	}
	return mini;
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

int count_freed(vector<int> a){
	if(a.empty()) return size+1;
	int sum=a.size()-1;
	for(auto &p:a) sum+=p;
	return size-sum;
}


void listall(vector<int> a, vector<int> b, int freed, int colraw, Heuristic *H, int ifcol){
	if(a.empty()){
		for(int i=1;i<=freed;i++) b.push_back(0);
		if(ifcol==1) H->col_vec[colraw].push_back(unfoldvec(b));
		else H->raw_vec[colraw].push_back(unfoldvec(b));
		return;
	}

	for(int i=0;i<=freed;i++){
		vector<int> tmpb=b;
		for(int j=0;j<i;j++) tmpb.push_back(0);
		tmpb.push_back(a[0]);
		vector<int> tmp=a;
		tmp.erase(tmp.begin());
		listall(tmp, tmpb, freed-i, colraw, H, ifcol);
	}
	return;
}




//combine the same element and fill M_ans
Heuristic combine(Heuristic H, int ifcol, int colraw){

	H.update.clear();

	vector<vector<int> > a;
	if(ifcol==1) a=H.col_vec[colraw];
	else a=H.raw_vec[colraw];
	int k=a.size();

	int com[100];
	for(int i=0;i<size;i++) com[i]=0;

	for(auto &p:a){
		//printvec(p);
		for(int i=0;i<size;i++) com[i]+=p[i];
	}

	for(int i=1;i<=size;i++){
		//fill col
		if(ifcol==1){
			if(H.M[i][colraw]==-1){
				if(com[i-1]==0){
					H.M[i][colraw]=0;
					H.fillnum++;
					H.update.push_back(i);

					//print2Darray(H.M);
					//printf("\n");
				}
				else if(com[i-1]==k){
					H.M[i][colraw]=1;
					H.fillnum++;
					H.update.push_back(i);

					//print2Darray(H.M);
					//printf("\n");
				}
			}
		}
		//fill raw
		else if(ifcol==0){
			if(H.M[colraw][i]==-1){
				if(com[i-1]==0){
					H.M[colraw][i]=0;
					H.fillnum++;
					H.update.push_back(i);

					//print2Darray(H.M);
					//printf("\n");
				}
				else if(com[i-1]==k){
					H.M[colraw][i]=1;
					H.fillnum++;
					H.update.push_back(i);

					//print2Darray(H.M);
					//printf("\n");
				}
			}
		}
	}
	return H;
}

Heuristic updaterc(Heuristic H, int ifcol){
	//update col
	if(ifcol==1){
		//the pth col has to update
		for(auto &p: H.update){
			vector<vector<int> > gar_can;
			for(auto &q:H.col_vec[p]){
				//q: ex 1101110110
				for(int i=1;i<=size;i++){
					if(H.M[i][p]==-1) continue;
					if(q[i-1] != H.M[i][p]){
						gar_can.push_back(q);
						break;
					}
				}
			}
				
			if(H.col_vec[p].empty()) printf("WWWTTTFFF\n");

			for(auto &q:gar_can) H.col_vec[p].erase(remove(H.col_vec[p].begin(),H.col_vec[p].end(),q), H.col_vec[p].end());
		}
	}
	//update raw
	else{
		//the pth row has to update
		for(auto &p: H.update){
			vector<vector<int> > gar_can;
			for(auto &q:H.raw_vec[p]){
				//q: ex 1101110110
				for(int i=1;i<=size;i++){
					if(H.M[p][i]==-1) continue;
					if(q[i-1] != H.M[p][i]){
						gar_can.push_back(q);
						break;
					}
				}
			}

			if(H.raw_vec[p].empty()) printf("WWWTTTFFF\n");

			for(auto &q:gar_can) H.raw_vec[p].erase(remove(H.raw_vec[p].begin(),H.raw_vec[p].end(),q), H.raw_vec[p].end());
		}
	}
	return H;
}



Heuristic heuristic(Heuristic H){
	int tmp_fillnum;
	//combine the same element and fill M_ans
	do{
		tmp_fillnum = H.fillnum;
		//combine the ith col
		for(int i=1;i<=size;i++){
			H = combine(H, 1, i);
			//if col filled the M_ans, update raw_all_vec
			if(!H.update.empty()) H = updaterc(H, 0);
			
			for(int j=1;j<=size;j++)
				if(H.raw_vec[j].empty()) return H;

		}

		//combine the ith raw
		for(int i=1;i<=size;i++){
			H = combine(H, 0, i);
			//if raw filled the M_ans, update col_all_vec
			if(!H.update.empty()) H = updaterc(H, 1);

			for(int j=1;j<=size;j++)
				if(H.col_vec[j].empty()) return H;

		}

	}while(tmp_fillnum != H.fillnum);
	return H;
}

Heuristic heuristic_recursion(Heuristic H){

	if(H.fillnum==(size*size)) return H;
	for(int i=1;i<=size;i++){
		if(H.col_vec[i].empty()) return H;
		if(H.raw_vec[i].empty()) return H;
	}

	int i=least_element(H.col_vec);

	for(auto &p: H.col_vec[i]){

		//printf("111\n");
		//printvec(p);

		Heuristic H_tmp = H;
		H_tmp.col_vec[i].clear();
		H_tmp.col_vec[i].push_back(p);

		//printf("111\n");
		H_tmp = heuristic(H_tmp);
		//printf("222\n");
		H_tmp = heuristic_recursion(H_tmp);
		//printf("333\n");

		if(H_tmp.fillnum==(size*size)) return H_tmp;
	}

	return H;
}

Heuristic count_prob(Heuristic H){
	for(int i=1;i<=size;i++){
		int size2=H.col_vec[i].size();
		double sum_array[100];
		for(int j=0;j<size;j++) sum_array[j]=0;
		for(auto &p:H.col_vec[i])
			for(int j=0;j<size;j++) sum_array[j] += p[j];
		double sum=0;
		for(int j=0;j<size;j++)
			if(sum_array[j]!=size2) sum+=sum_array[j];
		for(int j=1;j<=size;j++){
			if(sum_array[j-1]==0) H.prob[j][i]=0;
			else if(sum_array[j-1]==size2) H.prob[j][i]=1;
			else H.prob[j][i]=sum_array[j-1]/sum;
		}
	}

	for(int i=1;i<=size;i++){
		int size2=H.raw_vec[i].size();
		double sum_array[100];
		for(int j=0;j<size;j++) sum_array[j]=0;
		for(auto &p:H.raw_vec[i])
			for(int j=0;j<size;j++) sum_array[j] += p[j];
		double sum=0;
		for(int j=0;j<size;j++)
			if(sum_array[j]!=size2) sum+=sum_array[j];
		for(int j=1;j<=size;j++){
			if(sum_array[j-1]==0) H.prob[i][j]=0;
			else if(sum_array[j-1]==size2) H.prob[i][j]=1;
			//else H.prob[i][j] *= sum_array[j-1]/sum;
			//else H.prob[i][j] = (H.prob[i][j]+sum_array[j-1]/sum)/2;
			else{
				double x=H.prob[i][j];
				double y=sum_array[j-1]/sum;
				H.prob[i][j] = x + x*y + y;
			}
			/*
			else{
				double x=H.prob[i][j];
				double y=sum_array[j-1]/sum;
				if(H.raw_vec[i].size()>H.col_vec[j].size())	H.prob[i][j] = 2*x + x*y + y;
				else if(H.raw_vec[i].size()<H.col_vec[j].size()) H.prob[i][j] = x + x*y + y*2;
				else H.prob[i][j] = x + x*y + y;
			}
			*/
		}
	}

	return H;
}

Heuristic prob_updaterc(Heuristic H, int maxi, int maxj, int v){
	//update col
	vector<vector<int> > tmp;
	for(auto &p:H.col_vec[maxj])
		if(p[maxi-1]==v) tmp.push_back(p);
	H.col_vec[maxj] = tmp;
	//update raw
	tmp.clear();
	for(auto &p:H.raw_vec[maxi])
		if(p[maxj-1]==v) tmp.push_back(p);
	H.raw_vec[maxi] = tmp;

	return H;
}

int check_empty(Heuristic H){
	for(int i=1;i<=size;i++){
		if(H.col_vec[i].empty()) return 0;
		if(H.raw_vec[i].empty()) return 0;
	}
	return 1;
}

int check(Heuristic H){
	for(int i=1;i<=size;i++){
		vector<int> tmpv;
		int sum=0;
		for(int j=1;j<=size;j++){
			if(H.M[i][j]==1) sum++;
			else if(H.M[i][j]==-1) return 0;
			else if(H.M[i][j]==0 && sum>0){
				tmpv.push_back(sum);
				sum=0;
			}
		}
		if(sum>0) tmpv.push_back(sum);
		
		if(rawhint[pbm_now][i].empty()){
			if(!tmpv.empty()) return 0;
		}
		else if(tmpv.empty()) return 0;
		else if(!equal(tmpv.begin(),tmpv.end(),rawhint[pbm_now][i].begin())) return 0;
	}
	for(int i=1;i<=size;i++){
		vector<int> tmpv;
		int sum=0;
		for(int j=1;j<=size;j++){
			if(H.M[j][i]==1) sum++;
			else if(H.M[j][i]==-1) return 0;
			else if(H.M[j][i]==0 && sum>0){
				tmpv.push_back(sum);
				sum=0;
			}
		}
		if(sum>0) tmpv.push_back(sum);
		
		if(colhint[pbm_now][i].empty()){
			if(!tmpv.empty()) return 0;
		}
		else if(tmpv.empty()) return 0;
		else if(!equal(tmpv.begin(),tmpv.end(),colhint[pbm_now][i].begin())) return 0;
	}
	return 1;
}

int update_prob(Heuristic H, int depth){
	//printf("in update\n");

	//termination condition
	if(H.fillnum==(size*size)){
		if(check(H)==1){
			H_final = H;
			if(depth > max_ans_depth) max_ans_depth = depth;
			if(depth > max_depth) max_depth = depth;
			return 1;
		}
		else{
			if(depth > max_depth) max_depth = depth;
			return 0;
		}
	}

	/*
	for(int i=1;i<=size;i++){
		if(H.col_vec[i].empty()) return H;
		if(H.raw_vec[i].empty()) return H;
	}
	*/


	//find max value position
	double maxv=-1;
	int maxi=1, maxj=1;
	for(int i=1;i<=size;i++){
		for(int j=1;j<=size;j++){
			if(H.prob[i][j]!=1 && H.prob[i][j]>maxv){
				maxv = H.prob[i][j];
				maxi = i;
				maxj = j;
			}
		}
	}

	// fill 1
	Heuristic H_tmp = H;
	H_tmp.prob[maxi][maxj] = 1;
	H_tmp.M[maxi][maxj] = 1;
	H_tmp.fillnum++;

	while(1){
		H_tmp = prob_updaterc(H_tmp, maxi, maxj, 1);
		if(check_empty(H_tmp)==0) break;

		H_tmp = heuristic(H_tmp);
		if(check_empty(H_tmp)==0) break;

		H_tmp = count_prob(H_tmp);
		//printf("Before enter\n");
		int r1 = update_prob(H_tmp, depth+1);
		//printf("Done r1.\n");
		if(r1==1) return 1;
		break;
	}

	// fill 0
	H_tmp = H;
	H_tmp.prob[maxi][maxj] = 0;
	H_tmp.M[maxi][maxj] = 0;
	H_tmp.fillnum++;

	H_tmp = prob_updaterc(H_tmp, maxi, maxj, 0);
	if(check_empty(H_tmp)==0) return 0;

	H_tmp = heuristic(H_tmp);
	if(check_empty(H_tmp)==0) return 0;

	H_tmp = count_prob(H_tmp);
	int r2 = update_prob(H_tmp, depth+1);
	//printf("Done r2.\n");
	if(r2==1) return 1;

	return 0;

}



int main(int argc, char* argv[]){

	size = atoi(argv[1]);

	char str[100];
	FILE *fp;
	int problem_cnt=1;
	int hint_cnt=1;
	fp=fopen("tcga2016-question.txt","r");
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
	fclose(fp);

	printf("Number of problem:%d\n",problem_cnt);

	clock_t t0;
	t0=clock();
	for(int pbm=1;pbm<=problem_cnt;pbm++){
		pbm_now = pbm;
		printf("Problem:%d\n",pbm);

		max_ans_depth = -1;
		max_depth = -1;

		Heuristic H_ans;
		for(int i=0;i<size+2;i++)
			for(int j=0;j<size+2;j++) H_ans.M[i][j]=-1;

		clock_t t1,t2;
		t1=clock();
		//preprocessing: store all the states
		for(int i=1;i<=size;i++){
			int freed=count_freed(colhint[pbm][i]);
			vector<int> b;
			listall(colhint[pbm][i], b, freed, i, &H_ans, 1);
		}
		for(int i=1;i<=size;i++){
			int freed=count_freed(rawhint[pbm][i]);
			vector<int> b;
			listall(rawhint[pbm][i], b, freed, i, &H_ans, 0);
		}

		t2=clock();
		printf("Preprocessing: %lf secs\n",(t2-t1)/(double)(CLOCKS_PER_SEC));


		/*Heuristic*/
		t1=clock();
		//Heuristic Search from 0
		H_ans = heuristic(H_ans);

		printf("%d\n",H_ans.fillnum);
		//print2Darray(H_ans.M);

		t2=clock();
		printf("First Heuristic: %lf secs\n",(t2-t1)/(double)(CLOCKS_PER_SEC));
		/////

		H_ans = count_prob(H_ans);
		//printf("count_prob done.\n");
		//print2Darray_double(H_ans.prob);
		//printf("\n");

		int r = update_prob(H_ans, 0);
		printf("r:%d\n",r);
		//print2Darray_double(H_ans.prob);
		print2Darray(H_final.M);
		printf("\n");

		/*
		//DFS Heuristic Search
		t1=clock();
		if(H_ans.fillnum==(size*size)) print2Darray(H_ans.M);
		else{
			H_ans = heuristic_recursion(H_ans);
			printf("%d\n",H_ans.fillnum);
			print2Darray(H_ans.M);
		}
		*/

		printf("max_depth:%d\n",max_depth);
		printf("max_ans_depth:%d\n",max_ans_depth);


		t2=clock();
		printf("Heuristic Recursion: %lf secs\n",(t2-t1)/(double)(CLOCKS_PER_SEC));

		

	}
	clock_t t3=clock();
	printf("Total: %lf secs\n",(t3-t0)/(double)(CLOCKS_PER_SEC));

	return 0;
}








