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
	
    int do_genmove(){
		
        int ML[64],*MLED(B.get_valid_move(ML));
		if(MLED==ML) return 64;
		if((MLED-ML)==1) return ML[0];
		/*
		int *p = ML;
		while(p!=MLED){
			if(*p==0 || *p==7 || *p==56 || *p==63) return *p;
			p++;
		}
		*/

		std::chrono::time_point<my_clock> st = my_clock::now();
		int remain_time = 1000000;

		int nc = MLED-ML; 
		printf("number of legal moves:%d\n",nc);
		float win[64]={0}, Ni[64]={0}, N=0;
		double UCB[64]={0};
		double ucbc=0.9;
		int ttf[27];
		int mytile = B.get_my_tile();
		board B_tmp, B_tmptmp;
		int move;
		//initialize, every child runs 100 times
		for(int i=0;i<nc;i++){
			B_tmp = B;
			move = ML[i];
			B_tmp.update(move/8, move%8, ttf);
			for(int t=0;t<10;t++){
				B_tmptmp = B_tmp;
				while(1){
					move = random_genmove(B_tmptmp);
					B_tmptmp.update(move/8, move%8, ttf);
					if(B_tmptmp.is_game_over()) break;
				}
				if(B_tmptmp.is_win(mytile)) win[i]++;
				Ni[i]++;
				N++;
			}
		}

		for(int i=0;i<nc;i++){
			UCB[i] = win[i]/Ni[i] + ucbc * sqrt(log(N) / Ni[i]);
		}
		
		for(int i=0;i<nc;i++) printf("%lf ",win[i]);
		printf("\n");
		for(int i=0;i<nc;i++) printf("%lf ",Ni[i]);
		printf("\n");
		for(int i=0;i<nc;i++) printf("%lf ",UCB[i]);
		printf("\n");
		
					
		//int sample_num = 500;
		int sample_cnt = 0;
        int wait_time = std::chrono::duration_cast<my_tunit>(my_clock::now() - st).count();
		remain_time -= wait_time;
		while(remain_time>=0){
			st = my_clock::now();
			//find largest UCB
			double maxucb = -100;
			int maxi=0;
			for(int i=0;i<nc;i++){
				if(UCB[i]>maxucb){
					maxucb = UCB[i];
					maxi = i;
				}
			}

			B_tmp = B;
			move = ML[maxi];
			B_tmp.update(move/8, move%8, ttf);
			for(int i=0;i<10;i++){
				B_tmptmp = B_tmp;
				while(1){
					move = random_genmove(B_tmptmp);
					B_tmptmp.update(move/8, move%8, ttf);
					if(B_tmptmp.is_game_over()) break;
				}
				if(B_tmptmp.is_win(mytile)) win[maxi]++;
				Ni[maxi]++;
				N++;
			}

			//update UCB
			UCB[maxi] = win[maxi]/Ni[maxi] + ucbc * sqrt(log(N) / Ni[maxi]);

			wait_time = std::chrono::duration_cast<my_tunit>(my_clock::now() - st).count();
			remain_time -= wait_time;
			sample_cnt++;

		}
		
		double maxwr = -100;
		int maxi = 0;
		for(int i=0;i<nc;i++){
			if(win[i]/Ni[i] > maxwr){
				maxwr = win[i]/Ni[i];
				maxi = i;
			}
		}


        //wait_time = std::chrono::duration_cast<my_tunit>(my_clock::now() - st).count();
		//printf("wait time: %dus\n",wait_time);
		printf("sample cnt = %d\n",sample_cnt);
        return ML[maxi];
        //return *random_choice(ML,MLED);
		
		


		/*
        int ML[64],*MLED(B.get_valid_move(ML));
		if(MLED!=ML){
			//do UCB
			int *it = ML;
			while(it!=MLED){
				int cnt = 1000;
				int win = 0;
				for(int i=0;i<cnt;i++){
					Board B_tmp = B;
					history H_tmp[128],*HED_tmp;
					memcpy(H_tmp, H, sizeof(H));
					HED_tmp = (HED - H) + H_tmp;
				}
			}

			int *it = ML;
			while(it!=MLED){
				//printf("ML:%d\n",*it);
				if(*it==0||*it==7||*it==56||*it==63) return *it;
				it++;
			}
			return *random_choice(ML,MLED);
		}
		else return 64;
		*/
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
