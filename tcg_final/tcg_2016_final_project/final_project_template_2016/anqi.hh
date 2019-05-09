#ifndef ANQI
#define ANQI

// (color)
//  0 = ���� (�j�g�r��)
//  1 = �¤� (�p�g�r��)
// -1 = �����O
typedef int CLR;

// (level)
enum LVL {
	LVL_K=0, // �ӱN King
	LVL_G=1, // �K�h Guard
	LVL_M=2, // �۶H Minister
	LVL_R=3, // �Ϩ� Rook     // BIG5 �S���H������
	LVL_N=4, // �X�� kNight
	LVL_C=5, // ���� Cannon
	LVL_P=6  // �L�� Pawn
};

enum FIN {
	FIN_K= 0 /* 'K' �� */ , FIN_k= 7 /* 'k' �N */ , FIN_X=14 /* 'X' ��½ */ ,
	FIN_G= 1 /* 'G' �K */ , FIN_g= 8 /* 'g' �h */ , FIN_E=15 /* '-' �Ů� */ ,
	FIN_M= 2 /* 'M' �� */ , FIN_m= 9 /* 'm' �H */ ,
	FIN_R= 3 /* 'R' �� */ , FIN_r=10 /* 'r' �� */ ,
	FIN_N= 4 /* 'N' �X */ , FIN_n=11 /* 'n' �� */ ,
	FIN_C= 5 /* 'C' �� */ , FIN_c=12 /* 'c' �� */ ,
	FIN_P= 6 /* 'P' �L */ , FIN_p=13 /* 'p' �� */
};

// (position)
//  0  1  2  3
//  4  5  6  7
//  8  9 10 11
// 12 13 14 15
// 16 17 18 19
// 20 21 22 23
// 24 25 26 27
// 28 29 30 31
typedef int POS;

struct MOV {
	POS st; // �_�I
	POS ed; // ���I // �Y ed==st ��ܬO½�l

	MOV() {}
	MOV(POS s,POS e):st(s),ed(e) {}

	bool operator==(const MOV &x) const {return st==x.st&&ed==x.ed;}
	MOV operator=(const MOV &x) {st=x.st;ed=x.ed;return MOV(x.st, x.ed);}
};

struct MOVLST {
	int num;     // ���k��(����+�Y�l,���]�A½�l)
	MOV mov[68];
};

struct BOARD {
	CLR who;     // �{�b���쨺�@��U
	FIN fin[32]; // �U�Ӧ�m�W���\�Fԣ
	int cnt[14]; // �U�شѤl����½�}�ƶq

	void NewGame();              // �}�s�C��
	int  LoadGame(const char*);  // ���J�C���öǦ^�ɭ�(���:��)
	void Display() const;        // ��ܨ� stderr �W
	int  MoveGen(MOVLST&) const; // �C�X�Ҧ����k(���l+�Y�l,���]�A½�l)
	                             // �^�Ǩ��k�ƶq
	bool ChkLose() const;        // �ˬd��e���a(who)�O�_��F
	bool ChkValid(MOV) const;    // �ˬd�O�_���X�k���k
	void Flip(POS,FIN=FIN_X);    // ½�l
	void Move(MOV);              // ���� or �Y�l
	void DoMove(MOV m, FIN f) ;
	//void Init(int Board[32], int Piece[14], int Color);
	void Init(char Board[32], int Piece[14], int Color);
};

CLR  GetColor(FIN);    // ��X�Ѥl���C��
LVL  GetLevel(FIN);    // ��X�Ѥl������
bool ChkEats(FIN,FIN); // �P�_�Ĥ@�ӴѤl��_�Y�ĤG�ӴѤl
void Output (MOV);     // �N���׶ǵ� GUI

#endif
