#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<algorithm>
#include<string>
#include<vector>
#include<time.h>
using namespace std;

vector<int> colhint[1050][100]; //problem: 1~1000, hint:1~10 
vector<int> rawhint[1050][100];

int size;

int max_ans_depth;
int max_depth;

typedef struct heuris{
	int M[50][50];
	int fillnum=0;
	vector<int> update;
	vector<vector<int> > col_vec[50];
	vector<vector<int> > raw_vec[50];

	int sum_col[50][50];
	int sum_raw[50][50];
	
	double prob[50][50];

}Heuristic;


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
			if(a[i][j]==1 || a[i][j]==0) printf("%d\t", int(a[i][j]));
			else printf("%.5lf\t",a[i][j]);
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
/*
int large_element(const vector<vector<int> > a[50]){
	int maxv=-1;
	int maxi=1;
	for(int i=1;i<=size;i++){
		if(a[i].size()>=2 && a[i].size()>maxv){
			maxv = a[i].size();
			maxi = i;
		}
	}
	return maxi;
}
*/
int large_element(const vector<vector<int> > a[50]){
	for(int i=1;i<=size;i++){
		if(a[i].size()>=2){
			return i;
		}
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


Heuristic count_prob(Heuristic H){
	for(int i=1;i<=size;i++){
		double com[100];
		for(int j=0;j<size;j++) com[j]=0.0;
		for(auto &p:H.col_vec[i]){
			for(int j=0;j<size;j++) com[j]+=p[j];
		}
		for(int j=0;j<size;j++) H.prob[j+1][i] = com[j]/double(H.col_vec[i].size());
	}
	for(int i=1;i<=size;i++){
		double com[100];
		for(int j=0;j<size;j++) com[j]=0.0;
		for(auto &p:H.raw_vec[i]){
			for(int j=0;j<size;j++) com[j]+=p[j];
		}
		for(int j=0;j<size;j++) H.prob[i][j+1] *= com[j]/double(H.raw_vec[i].size());
	}
	for(int i=1;i<=size;i++)
		for(int j=1;j<=size;j++)
			if(H.M[i][j]!=-1) H.prob[i][j]=H.M[i][j];
	return H;
}

Heuristic update_prob(Heuristic H){
	double maxv=-1;
	int maxi,maxj;
	for(int i=1;i<=size;i++){
		for(int j=1;j<=size;j++){
			if(H.prob[i][j]!=1 && H.prob[i][j]>maxv){
				maxv = H.prob[i][j];
				maxi = i;
				maxj = j;
			}
		}
	}

	H.M[maxi][maxj]=1;
	vector<vector<int> > tmp;
	for(auto &p:H.raw_vec[maxi])
		if(p[maxj-1]==1) tmp.push_back(p);
	H.raw_vec[maxi] = tmp;


	tmp.clear();
	for(auto &p:H.col_vec[maxj])
		if(p[maxi-1]==1) tmp.push_back(p);
	H.col_vec[maxj] = tmp;

	for(int j=0;j<size;j++) H.sum_col[maxj][j]=0;
	for(auto &p:H.col_vec[maxj])
		for(int j=0;j<size;j++) H.sum_col[maxj][j]+=p[j];

	for(int j=0;j<size;j++) H.sum_raw[maxi][j]=0;
	for(auto &p:H.raw_vec[maxi])
		for(int j=0;j<size;j++) H.sum_raw[maxi][j]+=p[j];

	return H;
}



//combine the same element and fill M_ans
Heuristic combine(Heuristic H, int ifcol, int colraw){

	H.update.clear();

	for(int i=1;i<=size;i++){
		//fill col
		if(ifcol==1){
			int col_size=H.col_vec[colraw].size();
			if(H.M[i][colraw]==-1){
				if(H.sum_col[colraw][i-1]==0){
					H.M[i][colraw]=0;
					H.fillnum++;
					H.update.push_back(i);

					//print2Darray(H.M);
					//printf("\n");
				}
				else if(H.sum_col[colraw][i-1]==col_size){
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
			int raw_size=H.raw_vec[colraw].size();
			if(H.M[colraw][i]==-1){
				if(H.sum_raw[colraw][i-1]==0){
					H.M[colraw][i]=0;
					H.fillnum++;
					H.update.push_back(i);

					//print2Darray(H.M);
					//printf("\n");
				}
				else if(H.sum_raw[colraw][i-1]==raw_size){
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
				

			for(auto &q:gar_can){
				for(int i=0;i<size;i++) H.sum_col[p][i]-=q[i];
				H.col_vec[p].erase(remove(H.col_vec[p].begin(),H.col_vec[p].end(),q), H.col_vec[p].end());
			}
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


			for(auto &q:gar_can){
				for(int i=0;i<size;i++) H.sum_raw[p][i]-=q[i];
				H.raw_vec[p].erase(remove(H.raw_vec[p].begin(),H.raw_vec[p].end(),q), H.raw_vec[p].end());
			}
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

Heuristic heuristic_recursion(Heuristic H, int depth){

	if(H.fillnum==(size*size)){
		if(depth>max_ans_depth)	max_ans_depth = depth;
		if(depth>max_depth) max_depth=depth;
		return H;
	}
	for(int i=1;i<=size;i++){
		if(H.col_vec[i].empty()){
			if(depth>max_depth) max_depth=depth;
			return H;
		}
		if(H.raw_vec[i].empty()){
			if(depth>max_depth) max_depth=depth;
			return H;
		}
	}

	int i=large_element(H.col_vec);

	for(auto &p: H.col_vec[i]){

		Heuristic H_tmp = H;
		H_tmp.col_vec[i].clear();
		H_tmp.col_vec[i].push_back(p);
		for(int j=0;j<size;j++) H_tmp.sum_col[i][j] = p[j];

		H_tmp = heuristic(H_tmp);
		H_tmp = heuristic_recursion(H_tmp, depth+1);

		if(H_tmp.fillnum==(size*size)){
			if(depth>max_ans_depth) max_ans_depth = depth;
			if(depth>max_depth) max_depth=depth;
			return H_tmp;
		}
	}
	return H;
}


Heuristic IDS(Heuristic H, int depth, int layer){

	if(H.fillnum==(size*size)) return H;
	for(int i=1;i<=size;i++){
		if(H.col_vec[i].empty()) return H;
		if(H.raw_vec[i].empty()) return H;
	}

	if(depth==layer) return H;


	int i=least_element(H.col_vec);
	//printf("Depth:%d Branch:%d\n",depth, int(H.col_vec[i].size()));
	//Heuristic H_r;
	//int maxnum=-1;
	for(auto &p: H.col_vec[i]){

		Heuristic H_tmp = H;
		H_tmp.col_vec[i].clear();
		H_tmp.col_vec[i].push_back(p);

		H_tmp = heuristic(H_tmp);
		H_tmp = IDS(H_tmp, depth+1, layer);

		if(H_tmp.fillnum==(size*size)) return H_tmp;

		/*
		if(H_tmp.fillnum>maxnum){
			maxnum=H_tmp.fillnum;
			H_r = H_tmp;
		}
		*/

	}
	return H;
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
		printf("Problem:%d\n",pbm);

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


		//count each the col/raw's possibility sum
		for(int i=1;i<=size;i++){
			for(int j=0;j<size;j++) H_ans.sum_col[i][j]=0;
			for(auto &p:H_ans.col_vec[i])
				for(int j=0;j<size;j++) H_ans.sum_col[i][j]+=p[j];
		}

		for(int i=1;i<=size;i++){
			for(int j=0;j<size;j++) H_ans.sum_raw[i][j]=0;
			for(auto &p:H_ans.raw_vec[i])
				for(int j=0;j<size;j++) H_ans.sum_raw[i][j]+=p[j];
		}

		t2=clock();
		printf("Preprocessing: %lf secs\n",(t2-t1)/(double)(CLOCKS_PER_SEC));


		/*Heuristic*/
		t1=clock();
		//Heuristic Search from 0
		H_ans = heuristic(H_ans);

		printf("%d\n",H_ans.fillnum);
		print2Darray(H_ans.M);
		printf("\n");

		t2=clock();
		printf("First Heuristic: %lf secs\n",(t2-t1)/(double)(CLOCKS_PER_SEC));
		/////
	
		
		H_ans = count_prob(H_ans);
		print2Darray_double(H_ans.prob);
		printf("\n");

		for(int i=0;i<10;i++){
			H_ans = update_prob(H_ans);
			H_ans = heuristic(H_ans);
			H_ans = count_prob(H_ans);
			print2Darray_double(H_ans.prob);
			printf("\n");
		}


		

		/*
		//DFS Heuristic Search
		if(pbm>=1 && pbm<=10){
			t1=clock();

			max_depth = -1;
			max_ans_depth = -1;

			if(H_ans.fillnum==(size*size)) print2Darray(H_ans.M);
			else{
				H_ans = heuristic_recursion(H_ans, 0);
				printf("%d\n",H_ans.fillnum);
				print2Darray(H_ans.M);
			}
			t2=clock();
			printf("max_depth: %d\n", max_depth);
			printf("max_ans_depth: %d\n", max_ans_depth);
			printf("Heuristic Recursion: %lf secs\n",(t2-t1)/(double)(CLOCKS_PER_SEC));
		}
		
		
		//DFID
		else{
			t1=clock();
			if(H_ans.fillnum==(size*size)) print2Darray(H_ans.M);
			else{
				Heuristic H_tmp;
				for(int i=1;i<=(size*size);i++){
					H_tmp = IDS(H_ans, 0, i);
					if(H_tmp.fillnum==(size*size)){
						//printf("%d\n",H_ans.fillnum);
						printf("Layer:%d\n",i);
						print2Darray(H_tmp.M);
						break;
					}
				}
			}
			t2=clock();
			printf("DFID: %lf secs\n",(t2-t1)/(double)(CLOCKS_PER_SEC));
		}
		*/
		

	}
	clock_t t3=clock();
	printf("Total: %lf secs\n",(t3-t0)/(double)(CLOCKS_PER_SEC));

	return 0;
}








