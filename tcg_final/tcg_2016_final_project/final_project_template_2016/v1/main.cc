/*****************************************************************************\
 * Theory of Computer Games: Fall 2012
 * Chinese Dark Chess Search Engine Template by You-cheng Syu
 *
 * This file may not be used out of the class unless asking
 * for permission first.
 *
 * Modify by Hung-Jui Chang, December 2013
\*****************************************************************************/
#include<cstdio>
#include<cstdlib>
#include"anqi.hh"
#include"Protocol.h"
#include"ClientSocket.h"
#include<algorithm>
#include<cmath>
#include<random>
using namespace std;

#ifdef _WINDOWS
#include<windows.h>
#else
#include<ctime>
#endif

int Inf = 100000001;

const int DEFAULTTIME = 15;
typedef  int SCORE;
static const SCORE INF=1000001;
static const SCORE WIN=1000000;
SCORE SearchMax(const BOARD&,int,int);
SCORE SearchMin(const BOARD&,int,int);

#ifdef _WINDOWS
DWORD Tick;     // 開始時刻
int   TimeOut;  // 時限
#else
clock_t Tick;     // 開始時刻
clock_t TimeOut;  // 時限
#endif
MOV   BestMove; // 搜出來的最佳著法

bool TimesUp() {
#ifdef _WINDOWS
	return GetTickCount()-Tick>=TimeOut;
#else
	return clock() - Tick > TimeOut;
#endif
}


////////////////////////////////////////////////////////////////////////////
/*
typedef struct _hash{
	int m;	//value
	int depth;
	bool exact;	//if exact value or temp
	bool visit;
	int bc; //best child
}Hash;
Hash *htable;

int hashn=10;
int hashm=20;
uint64_t s[15][32];
uint64_t hcolor[2];

void Hash_Init(){
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<uint64_t> dist(llround(pow(2,hashn+hashm)), llround(pow(2,hashn+hashm+1)));
	for(int i=0;i<15;i++)
		for(int j=0;j<32;j++)
			s[i][j] = dist(gen);
	hcolor[0]=dist(gen);
	hcolor[1]=dist(gen);
}

uint64_t hash_f(const BOARD &B){
	printf("\nIn hash_f\n");
	uint64_t r=hcolor[B.who];
	for(int i=0;i<32;i++)
		if(B.fin[i]!=15)
			r ^= s[B.fin[i]][i];
	r = (r<<hashn)>>hashn;
	printf("\nOut hash_f\n");
	return r;
}

*/

int power(FIN f){
	if(f>=14) return 0;
	switch(f%7){
		case 0: return 6; //將、帥
		case 1: return 5; //士、仕
		case 2: return 4; //象、相
		case 3: return 3; //車、紅車
		case 4: return 2; //馬、碼
		case 5: return 4; //包、炮
		case 6: return 1; //卒、兵
		default: return 0;
	}
}

// 一個有加權的審局函數
SCORE Eval(const BOARD &B) {
//	printf("!!! In EVAL !!!\n");
	int cnt[2]={0,0};
	for(POS p=0;p<32;p++){
		const CLR c=GetColor(B.fin[p]);
		if(c!=-1)
			cnt[c]+=power(B.fin[p]);
	}
	for(int i=0;i<14;i++)
		cnt[GetColor(FIN(i))]+=B.cnt[i]*power(FIN(i));
//	printf("!!! OUT EVAL !!!\n");
	return cnt[B.who]-cnt[B.who^1];
}
////////////////////////////////////////////////////////////////////////////


/*
// dep=現在在第幾層
// cut=還要再走幾層
SCORE SearchMax(const BOARD &B,int dep,int cut) {
	if(B.ChkLose())return -WIN;

	MOVLST lst;
	if(cut==0||TimesUp()||B.MoveGen(lst)==0)return +Eval(B);

	SCORE ret=-INF;
	for(int i=0;i<lst.num;i++) {
		BOARD N(B);
		N.Move(lst.mov[i]);
		const SCORE tmp=SearchMin(N,dep+1,cut-1);
		if(tmp>ret){ret=tmp;if(dep==0)BestMove=lst.mov[i];}
	}
	return ret;
}

SCORE SearchMin(const BOARD &B,int dep,int cut) {
	if(B.ChkLose())return +WIN;

	MOVLST lst;
	if(cut==0||TimesUp()||B.MoveGen(lst)==0)return -Eval(B);

	SCORE ret=+INF;
	for(int i=0;i<lst.num;i++) {
		BOARD N(B);
		N.Move(lst.mov[i]);
		const SCORE tmp=SearchMax(N,dep+1,cut-1);
		if(tmp<ret){ret=tmp;}
	}
	return ret;
}
*/

int NegaScout(const BOARD &B, int alpha, int beta, int depth, int cut){
	
	//printf("\n~ In NegaScout ~\n");

	if(B.ChkLose()) return (depth&1)? -WIN:WIN;

	//printf("\n~ In NegaScout -2 ~\n");

	//int m;
	//check if Hash Hit
	//uint64_t tmpi=hash_f(B);

	//printf("\n~ In NegaScout -3 ~\n");

/*	if(htable[tmpi].visit==true){

		printf("\n~ In NegaScout -4 ~\n");

		if(htable[tmpi].exact==true){
			if(depth<=htable[tmpi].depth) return htable[tmpi].m;
			else m=htable[tmpi].m;
		}
		else{
			m=htable[tmpi].m;
		}
	}
	else{
		printf("\n in m=-inf!? \n");
		m=-Inf;
	}

*/

	//m=-Inf;

	//printf("\n~~ after m ~~\n");

	MOVLST lst;
	int b=B.MoveGen(lst);
	if(b==0 || cut==0) return (depth&1)?(-Eval(B)):Eval(B);

	int m=-Inf;
	int n=beta;
	int bestc=0;
	BOARD _B;
	for(int i=0;i<b;i++){
		_B = B;
		_B.DoMove(lst.mov[i], FIN(15));
		int t = -NegaScout(_B, -n, -max(alpha,m), depth+1, cut-1);
		if(t>m){
			if(n==beta || cut<3 || t>=beta) m=t;
			else m = -NegaScout(_B, -beta, -t, depth+1, cut-1); //re-search
			if(depth==0){
				BestMove = lst.mov[i];
				bestc=i;
			}
		}
		if(m>=beta){ //cut-off
			/*
			htable[tmpi].m = m;
			htable[tmpi].depth = depth;
			htable[tmpi].exact = false;
			htable[tmpi].visit = true;
			htable[tmpi].bc = i;
			*/
			return m;
		}
		n = max(alpha,m)+1; //set-up a null window
	}
	
	//update
	/*
	htable[tmpi].m = m;
	htable[tmpi].depth = depth;
	htable[tmpi].exact = true;
	htable[tmpi].visit = true;
	htable[tmpi].bc = bestc;
	*/
	return m;
}

MOV Play(const BOARD &B){
	printf("~~~~~~~~~~~~~~~~~~~~~~~\n");
/*#ifdef _WINDOWS
	Tick=GetTickCount();
	TimeOut = (DEFAULTTIME-3)*1000;*/
//#else
	Tick=clock();
	//TimeOut = 15*CLOCKS_PER_SEC;
//#endif
	POS p; int c=0;

	//New Game
	if(B.who==-1) return MOV(25,25);

	int depth_limit = 5;
	if(NegaScout(B,-Inf, Inf, 0, depth_limit)>Eval(B)) return BestMove;

	for(p=0;p<32;p++)if(B.fin[p]==FIN_X)c++;
	if(c==0)return BestMove;
	c=rand()%c;
	for(p=0;p<32;p++)if(B.fin[p]==FIN_X&&--c<0)break;
	return MOV(p,p);

	//printf("~TIME: %lf\n", (double)(clock()-Tick)/CLOCKS_PER_SEC);





	/*	
	// 新遊戲？隨機翻子
	if(B.who==-1){p=rand()%32;printf("%d\n",p);return MOV(p,p);}

	// 若搜出來的結果會比現在好就用搜出來的走法
	if(SearchMax(B,0,5)>Eval(B))return BestMove;

	// 否則隨便翻一個地方 但小心可能已經沒地方翻了
	for(p=0;p<32;p++)if(B.fin[p]==FIN_X)c++;
	if(c==0)return BestMove;
	c=rand()%c;
	for(p=0;p<32;p++)if(B.fin[p]==FIN_X&&--c<0)break;
	return MOV(p,p);
	*/
	
}

FIN type2fin(int type) {
    switch(type) {
	case  1: return FIN_K;
	case  2: return FIN_G;
	case  3: return FIN_M;
	case  4: return FIN_R;
	case  5: return FIN_N;
	case  6: return FIN_C;
	case  7: return FIN_P;
	case  9: return FIN_k;
	case 10: return FIN_g;
	case 11: return FIN_m;
	case 12: return FIN_r;
	case 13: return FIN_n;
	case 14: return FIN_c;
	case 15: return FIN_p;
	default: return FIN_E;
    }
}
FIN chess2fin(char chess) {
    switch (chess) {
	case 'K': return FIN_K;
	case 'G': return FIN_G;
	case 'M': return FIN_M;
	case 'R': return FIN_R;
	case 'N': return FIN_N;
	case 'C': return FIN_C;
	case 'P': return FIN_P;
	case 'k': return FIN_k;
	case 'g': return FIN_g;
	case 'm': return FIN_m;
	case 'r': return FIN_r;
	case 'n': return FIN_n;
	case 'c': return FIN_c;
	case 'p': return FIN_p;
	default: return FIN_E;
    }
}

int main(int argc, char* argv[]) {

#ifdef _WINDOWS
	srand(Tick=GetTickCount());
#else
	srand(Tick=time(NULL));
#endif


////////////////////////////////////////////////////////////////////////////
//init hash table
/*
	Tick = clock();
	Hash_Init();
	uint64_t tablesize = (uint64_t)(1<<hashn);
	htable = (Hash*)malloc(tablesize*sizeof(Hash));
	for(int i=0;i<tablesize;i++) htable[i].visit=false;
	printf("Initial TIME: %lf\n", (double)(clock()-Tick)/CLOCKS_PER_SEC);
*/
////////////////////////////////////////////////////////////////////////////


	BOARD B;
	if (argc!=3) {
	    TimeOut=(B.LoadGame("board.txt")-3)*1000;
	    if(!B.ChkLose())Output(Play(B));
	    return 0;
	}
	Protocol *protocol;
	protocol = new Protocol();
	protocol->init_protocol(argv[1],atoi(argv[2]));
	int iPieceCount[14];
	char iCurrentPosition[32];
	int type, remain_time;
	bool turn;
	PROTO_CLR color;

	char src[3], dst[3], mov[6];
	History moveRecord;
	protocol->init_board(iPieceCount, iCurrentPosition, moveRecord, remain_time);
	protocol->get_turn(turn,color);

	TimeOut = (DEFAULTTIME-3)*1000;

	B.Init(iCurrentPosition, iPieceCount, (color==2)?(-1):(int)color);

	MOV m;
	if(turn) // 我先
	{
	    m = Play(B);
	    sprintf(src, "%c%c",(m.st%4)+'a', m.st/4+'1');
	    sprintf(dst, "%c%c",(m.ed%4)+'a', m.ed/4+'1');
	    protocol->send(src, dst);
	    protocol->recv(mov, remain_time);
	    if( color == 2)
		color = protocol->get_color(mov);
	    B.who = color;
	    B.DoMove(m, chess2fin(mov[3]));
	    protocol->recv(mov, remain_time);
	    m.st = mov[0] - 'a' + (mov[1] - '1')*4;
	    m.ed = (mov[2]=='(')?m.st:(mov[3] - 'a' + (mov[4] - '1')*4);
	    B.DoMove(m, chess2fin(mov[3]));
	}
	else // 對方先
	{
	    protocol->recv(mov, remain_time);
	    if( color == 2)
	    {
		color = protocol->get_color(mov);
		B.who = color;
	    }
	    else {
		B.who = color;
		B.who^=1;
	    }
	    m.st = mov[0] - 'a' + (mov[1] - '1')*4;
	    m.ed = (mov[2]=='(')?m.st:(mov[3] - 'a' + (mov[4] - '1')*4);
	    B.DoMove(m, chess2fin(mov[3]));
	}
	B.Display();

	while(1)
	{
	    m = Play(B);
	    sprintf(src, "%c%c",(m.st%4)+'a', m.st/4+'1');
	    sprintf(dst, "%c%c",(m.ed%4)+'a', m.ed/4+'1');
	    protocol->send(src, dst);
	    protocol->recv(mov, remain_time);
	    m.st = mov[0] - 'a' + (mov[1] - '1')*4;
	    m.ed = (mov[2]=='(')?m.st:(mov[3] - 'a' + (mov[4] - '1')*4);
	    B.DoMove(m, chess2fin(mov[3]));
	    B.Display();

	    protocol->recv(mov, remain_time);
	    m.st = mov[0] - 'a' + (mov[1] - '1')*4;
	    m.ed = (mov[2]=='(')?m.st:(mov[3] - 'a' + (mov[4] - '1')*4);
	    B.DoMove(m, chess2fin(mov[3]));
	    B.Display();
	}

	return 0;
}
