#include"board.h"
#include<random>
#ifdef _WIN32
#include<chrono>
#endif
#include<cstring>
#include<string>
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
class OTP{
    board B;
    history H[128],*HED;
    //initialize in do_init
    void do_init(){
        B = board();
        HED = H;
    }
    //choose the best move in do_genmove
    int do_genmove(){
        /*
          todo: use Monte-Carlo to choose move
        */
        int ML[64],*MLED(B.get_valid_move(ML));
        return MLED==ML?64:*random_choice(ML,MLED);
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
                sprintf(out,"name Random");
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
