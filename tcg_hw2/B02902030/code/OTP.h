#include"board.h"
#include<random>
#include<chrono>
#include<cstring>
#include<string>
#include<math.h>
constexpr char m_tolower(char c){
    return c+('A'<=c&&c<='Z')*('a'-'A');
}
constexpr unsigned my_hash(const char*s,unsigned long long int hv=0){
    return *s&&*s!=' '?my_hash(s+1,(hv*('a'+1)+m_tolower(*s))%0X3FFFFFFFU):hv;
}
struct history{
    int x,y,pass,tiles_to_flip[27],*ed;
};
template<class RIT>RIT random_choice(RIT st,RIT ed){
#ifdef _WIN32
    //std::random_device is deterministic with MinGW gcc 4.9.2 on Windows
    static std::mt19937 local_rand(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
#else
    static std::mt19937 local_rand(std::random_device{}());
#endif
    return st+std::uniform_int_distribution<int>(0,ed-st-1)(local_rand);
}

typedef struct _node{
	int tile;
	int move;
	board B;

	int win;
	int Ni;
	double ucb;

	struct _node *prev;
	struct _node **next;
	int nn; // number of next

	int pruned;
	double mean;
	double sd;

	int depth;
}Node;

typedef std::chrono::system_clock my_clock;
typedef std::chrono::microseconds my_tunit;

class OTP{
    board B;
    history H[128],*HED;
    //initialize in do_init
    void do_init(){
        B = board();
        HED = H;
    }
    //choose the best move in do_genmove
	int random_genmove(board B){
        int ML[64],*MLED(B.get_valid_move(ML));
        return MLED==ML?64:*random_choice(ML,MLED);
	}

	void update_ucb(Node * leaf, int *N_all, double ucbc){
		//update UCB
		leaf->win = 0;
		leaf->Ni = 0;
		for(int i=0;i<leaf->nn;i++){
			if(leaf->next[i]->pruned==1) continue;
			leaf->win += leaf->next[i]->win;
			leaf->Ni += leaf->next[i]->Ni;
		}
		leaf->ucb = double(leaf->win)/double(leaf->Ni) + ucbc * sqrt( log(*N_all) / double(leaf->Ni) );
		return;
	}

	Node* Selection(Node *root, int roottile, int *N_all, double ucbc){
		//leaf return
		if(root->nn==0) return root;

		Node *leaf;
		//Max node
		if(root->tile == roottile){
			int maxi = 0;
			double maxucb = -100000.0;
			update_ucb(root, N_all, ucbc);
			for(int i=0;i<root->nn;i++){
				if(root->next[i]->pruned==1) continue;
				if(root->next[i]->ucb > maxucb){
					maxucb = root->next[i]->ucb;
					maxi = i;
				}
			}
			leaf = Selection(root->next[maxi], roottile, N_all, ucbc);
		}
		//Min node
		else{
			int mini = 0;
			double minucb = 100000.0;
			update_ucb(root, N_all, ucbc);
			for(int i=0;i<root->nn;i++){
				if(root->next[i]->pruned==1) continue;
				if(root->next[i]->ucb < minucb){
					minucb = root->next[i]->ucb;
					mini = i;
				}
			}
			leaf = Selection(root->next[mini], roottile, N_all, ucbc);
		}

		return leaf;
	}

	void Expansion(Node * leaf){
		board B = leaf->B;
        int ML[64],*MLED(B.get_valid_move(ML)), ttf[27];
		int nc = MLED-ML;
		leaf->nn = nc;
		if(nc>0){
			leaf->next = (Node**)malloc(nc * sizeof(Node*));
			for(int i=0;i<nc;i++){
				leaf->next[i] = (Node*)malloc(sizeof(Node));
				leaf->next[i]->nn = 0;
				leaf->next[i]->tile = -(leaf->tile)+3;
				leaf->next[i]->move = ML[i];
				B = leaf->B;
				B.update(ML[i]/8, ML[i]%8, ttf);
				leaf->next[i]->B = B;
				leaf->next[i]->win = 0;
				leaf->next[i]->Ni = 0;
				leaf->next[i]->pruned = 0;
				leaf->next[i]->prev = leaf;
				leaf->next[i]->depth = leaf->depth+1;
			}
		}
		else{ //nc==0, add passnode
			leaf->nn = 1;
			leaf->next = (Node**)malloc(nc * sizeof(Node*));
			leaf->next[0] = (Node*)malloc(sizeof(Node));
			leaf->next[0]->nn = 0;
			leaf->next[0]->tile = -(leaf->tile)+3;
			leaf->next[0]->move = 64;
			B = leaf->B;
			B.update(8, 0, ttf);
			leaf->next[0]->B = B;
			leaf->next[0]->win = 0;
			leaf->next[0]->Ni = 0;
			leaf->next[0]->pruned = 0;
			leaf->next[0]->prev = leaf;
			leaf->next[0]->depth = leaf->depth+1;
		}
		return;
	}
	 
	void Simulation(Node *leaf, int *N_all, double ucbc, int mytile){
		int sim_num = 500;
		int ttf[27], move;
		board B;
		for(int i=0;i<leaf->nn;i++){
			int sum1 = 0.0;
			int sum2 = 0.0;
			for(int k=0;k<sim_num;k++){
				B = leaf->next[i]->B;
				while(1){
					move = random_genmove(B);
					B.update(move/8, move%8, ttf);
					if(B.is_game_over()) break;
				}
				if(B.is_win(mytile)) leaf->next[i]->win++;
				leaf->next[i]->Ni++;
				(*N_all)++;
				int tmp = B.get_my_score(mytile);
				sum1+=tmp;
				sum2+=tmp*tmp;
			}
			double mean = double(sum1)/double(sim_num);
			leaf->next[i]->mean = mean;
			leaf->next[i]->sd = sqrt( (sum2-2*sum1*mean)/double(sim_num) + mean*mean);
			//printf("mean:%lf\n", leaf->next[i]->mean);
			//printf("sd:%lf\n", leaf->next[i]->sd);
		}
		//update leaf children UCB
		for(int i=0;i<leaf->nn;i++){
			leaf->next[i]->ucb = double(leaf->next[i]->win)/double(leaf->next[i]->Ni) + ucbc * sqrt( log(*N_all) / double(leaf->next[i]->Ni) );
		}

		return;
	}

	void Propagation(Node *leaf, int *N_all, double ucbc){
		if(leaf==NULL) return; //root->prev==NULL

		update_ucb(leaf, N_all, ucbc);
		Propagation(leaf->prev, N_all, ucbc);
		return;
	}

	void PP(Node *leaf, double rd, double sigmae, int *prun_cnt, int *N_all, double ucbc){
		int cut=0;
		for(int i=0;i<leaf->nn;i++){
			if(leaf->next[i]->sd >= sigmae) continue;
			double mr = leaf->next[i]->mean + rd*leaf->next[i]->sd;
			for(int j=0;j<leaf->nn;j++){
				if(j==i) continue;
				if(leaf->next[j]->sd >= sigmae) continue;
				double ml = leaf->next[j]->mean - rd*leaf->next[j]->sd;
				if(mr<ml){//statistically inferior, cut i
					leaf->next[i]->pruned = 1;
					(*prun_cnt)++;
					cut++;
					(*N_all)-=500;
					break;
				}
			}
		}
		//since N_all may change, non-pruned sibling has to update ucb
		for(int i=0;i<leaf->nn;i++){
			if(leaf->next[i]->pruned==1) continue;
			leaf->next[i]->ucb = double(leaf->next[i]->win)/double(leaf->next[i]->Ni) + ucbc * sqrt( log(*N_all) / double(leaf->next[i]->Ni) );
		}

		if(cut==leaf->nn) printf("NO CHILDREN!!!!!!!!!\nWRONG!!!!!!!!!!!\n");
		return;
	}

	void delete_tree(Node *root){
		if(root->nn==0){
			free(root);
			return;
		}
		for(int i=0;i<root->nn;i++)
			delete_tree(root->next[i]);
		free(root);
		return;
	}

    int do_genmove(){
		
        int ML[64],*MLED(B.get_valid_move(ML));
		if(MLED==ML) return 64;
		if((MLED-ML)==1) return ML[0];

		std::chrono::time_point<my_clock> st = my_clock::now();
		int remain_time = 1000000 * 9;
		int nc = MLED-ML; 
		printf("number of legal moves:%d\n",nc);
		int max_depth = -1;
		int N_all = 0;
		double ucbc = 0.05;
		double rd = 1.5;
		double sigmae = 20;
		int prun_cnt = 0;

		Node *root;
		root = (Node*)malloc(sizeof(Node));
		root->tile = B.get_my_tile();
		root->move = 64;
		root->B = B;
		root->win = 0;
		root->Ni = 0;
		root->prev = NULL;
		root->depth = 0;
		root->nn = 0;
		board B_tmp = B;

        int wait_time = std::chrono::duration_cast<my_tunit>(my_clock::now() - st).count();
		remain_time -= wait_time;
		while(remain_time>=0){
			st = my_clock::now();
			B_tmp = B;
			Node *leaf = Selection(root, root->tile, &N_all, ucbc);
			Expansion(leaf);
			if(leaf->next[0]->depth > max_depth) max_depth = leaf->next[0]->depth;
			Simulation(leaf, &N_all, ucbc, root->tile);
			if(leaf->nn > 1) PP(leaf, rd, sigmae, &prun_cnt, &N_all, ucbc);
			Propagation(leaf, &N_all, ucbc);

			wait_time = std::chrono::duration_cast<my_tunit>(my_clock::now() - st).count();
			remain_time -= wait_time;
		}
        
		printf("max depth: %d\n",max_depth);
		printf("N_all: %d\n",N_all);
		printf("prun_cnt:%d\n", prun_cnt);
		double maxucb = -10000000.0;
		int maxi = 0;
		for(int i=0;i<root->nn;i++){
			if(root->next[i]->ucb > maxucb){
				maxucb = root->next[i]->ucb;
				maxi = i;
			}
		}

		delete_tree(root);
		return ML[maxi];
    }
    //update board and history in do_play
    void do_play(int x,int y){
        if(HED!=std::end(H)&&B.is_game_over()==0&&B.is_valid_move(x,y)){
            HED->x = x;
            HED->y = y;
            HED->pass = B.get_pass();
            HED->ed = B.update(x,y,HED->tiles_to_flip);
            ++HED;
        }else{
            fputs("wrong play.\n",stderr);
        }
    }
    //undo board and history in do_undo
    void do_undo(){
        if(HED!=H){
            --HED;
            B.undo(HED->x,HED->y,HED->pass,HED->tiles_to_flip,HED->ed);
        }else{
            fputs("wrong undo.\n",stderr);
        }
    }
public:
    OTP():B(),HED(H){
        do_init();
    }
    bool do_op(const char*cmd,char*out,FILE*myerr){
        switch(my_hash(cmd)){
            case my_hash("name"):
                sprintf(out,"name B02902030");
                return true;
            case my_hash("clear_board"):
                do_init();
                B.show_board(myerr);
                sprintf(out,"clear_board");
                return true;
            case my_hash("showboard"):
                B.show_board(myerr);
                sprintf(out,"showboard");
                return true;
            case my_hash("play"):{
                int x,y;
                sscanf(cmd,"%*s %d %d",&x,&y);
                do_play(x,y);
                B.show_board(myerr);
                sprintf(out,"play");
                return true;
            }
            case my_hash("genmove"):{
                int xy = do_genmove();
                int x = xy/8, y = xy%8;
                do_play(x,y);
                B.show_board(myerr);
                sprintf(out,"genmove %d %d",x,y);
                return true;
            }
            case my_hash("undo"):
                do_undo();
                sprintf(out,"undo");
                return true;
            case my_hash("final_score"):
                sprintf(out,"final_score %d",B.get_score());
                return true;
            case my_hash("quit"):
                sprintf(out,"quit");
                return false;
            //commmands used in simple_http_UI.cpp
            case my_hash("playgen"):{
                int x,y;
                sscanf(cmd,"%*s %d %d",&x,&y);
                do_play(x,y);
                if(B.is_game_over()==0){
                    int xy = do_genmove();
                    x = xy/8, y = xy%8;
                    do_play(x,y);
                }
                B.show_board(myerr);
                sprintf(out,"playgen %d %d",x,y);
                return true;
            }
            case my_hash("undoundo"):{
                do_undo();
                do_undo();
                sprintf(out,"undoundo");
                return true;
            }
            case my_hash("code"):
                do_init();
                B = board(cmd+5,cmd+strlen(cmd));
                B.show_board(myerr);
                sprintf(out,"code");
                return true;
            default:
                sprintf(out,"unknown command");
                return true;
        }
    }
    std::string get_html(unsigned,unsigned)const;
};
