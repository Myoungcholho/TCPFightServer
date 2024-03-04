/* define*/
#define SERVERPORT 5000

#define dfPACKET_MOVE_DIR_LL					0
#define dfPACKET_MOVE_DIR_LU					1
#define dfPACKET_MOVE_DIR_UU					2
#define dfPACKET_MOVE_DIR_RU					3
#define dfPACKET_MOVE_DIR_RR					4
#define dfPACKET_MOVE_DIR_RD					5
#define dfPACKET_MOVE_DIR_DD					6
#define dfPACKET_MOVE_DIR_LD					7


/* 화면 이동영역*/

#define dfRANGE_MOVE_TOP	50
#define dfRANGE_MOVE_LEFT	10
#define dfRANGE_MOVE_RIGHT	630
#define dfRANGE_MOVE_BOTTOM	470
/*
		50
10				630

		470
*/

/* 공격 범위 */
#define dfATTACK1_RANGE_X		80
#define dfATTACK2_RANGE_X		90
#define dfATTACK3_RANGE_X		100
#define dfATTACK1_RANGE_Y		10
#define dfATTACK2_RANGE_Y		10
#define dfATTACK3_RANGE_Y		20

/* 데미지 */
#define dfATTACK1_DAMGE			10
#define dfATTACK2_DAMGE			15
#define dfATTACK3_DAMGE			20

/* x,y 유효성 검사*/
#define dfERROR_RANGE		50


/* Player */
#pragma pack(1)
struct Player
{
	/* 세션 */
	SOCKET sock;
	unsigned int ID;
	CRingBuffer sendRingBuffer;
	CRingBuffer recvRingBuffer;

	/* player */
	unsigned short xpos;
	unsigned short ypos;
	BYTE HP;
	BYTE Direction;
	bool StopCheck = false;
	bool DeleteCheck = false;
	LARGE_INTEGER Player_DeltaTime;
	BYTE AttackMotion;
};

/* 구조체*/
struct Header
{
	BYTE byCode;
	BYTE bySize;
	BYTE byType;
};

#define	dfPACKET_SC_CREATE_MY_CHARACTER 0 
#define	dfPACKET_SC_CREATE_OTHER_CHARACTER 1
#define	dfPACKET_SC_DELETE_CHARACTER 2
#define	dfPACKET_CS_MOVE_START 10 /* CS */
#define	dfPACKET_SC_MOVE_START 11 // 서버가 클라들에게 주는 것[해당 클라는 제외]
#define	dfPACKET_CS_MOVE_STOP 12 /* CS */
#define	dfPACKET_SC_MOVE_STOP 13 // 서버가 클라들에게 주는 것[해당 클라는 제외]
#define	dfPACKET_CS_ATTACK1	 20 /* CS */
#define	dfPACKET_SC_ATTACK1 21 // 서버가 클라들에게 주는 것, 다른 클라들에게 행동 모션을 보여주기 위함
#define	dfPACKET_CS_ATTACK2 22 /* CS */
#define	dfPACKET_SC_ATTACK2	23
#define	dfPACKET_CS_ATTACK3	24 /* CS */
#define	dfPACKET_SC_ATTACK3 25
#define	dfPACKET_SC_DAMAGE	30


struct AttackPriority
{
	unsigned short x = USHRT_MAX;
	Player* priority;
};

/* 메소드 */
bool NetworkInit();
void NetWorking();
void Logic();
void AcceptProc();
void playerInit(Player* player);
void RewProc(Player* player);
void SendProc(Player* player);


/* 메소드 */

Header CreateHeader(BYTE type);
void mpCreateCharacter(CPacket* clpPacket, unsigned int ID, BYTE Direction, unsigned short x, unsigned short y, BYTE HP);
void mpCreateOther(CPacket* clpPacket, unsigned int ID, BYTE Direction, unsigned short x, unsigned short y, BYTE HP);
void mpMoveStart(CPacket* clpPacket, unsigned int ID, BYTE byDirection, unsigned short shX, unsigned short shY);
void mpMoveStop(CPacket* clpPacket, unsigned int ID, BYTE Direction, unsigned short x, unsigned short y);
void mpAttack1(CPacket* clpPacket, unsigned int ID, BYTE Direction, unsigned short xpos, unsigned short ypos);
void mpAttack2(CPacket* clpPacket, unsigned int ID, BYTE Direction, unsigned short xpos, unsigned short ypos);
void mpAttack3(CPacket* clpPacket, unsigned int ID, BYTE Direction, unsigned short xpos, unsigned short ypos);
void mpDamage(CPacket* clpPacket, unsigned int AttackerID, unsigned int VictimID, BYTE VictimHP);

void mpDelete(CPacket* clpPacket, unsigned int ID);

/* 메소드 */
bool PacketProc(Player* player, BYTE byPacketType, CPacket* cPacket);
bool netPacketProc_MoveStart(Player* player, CPacket* pPacket);
bool netPacketProc_MoveStop(Player* player, CPacket* pPacket);
bool netPacketProc_Attack1(Player* player, CPacket* pPacket);
bool netPacketProc_Attack2(Player* player, CPacket* pPacket);
bool netPacketProc_Attack3(Player* player, CPacket* pPacket);

/* UniCast, BarodCast, Disconnect*/
void SendUniPacket(Player* player, CPacket* clpPacket);
void SendBoardPacket(Player* player, CPacket* clpPacket);
void Disconnect(Player* player);
void ListDelete();

/* 오류 저장 값*/
int bind_ret;
int listen_ret;
int nblock_ret;
int sel_ret;
int board_ret;
int recv_ret;
int recv_Enqueue;
int recv_Dequeue;
int send_Size;
int send_ProcRet;

/* clp mp 오류 저장값 */
int mp_create_ret;
int mp_other_ret;
int mp_move_ret;
int mp_stop_ret;
int mp_Attack1_ret;
int mp_Attack2_ret;
int mp_Attack3_ret;
int mp_Damage_ret;
int mp_Del_ret;

/* 데이터 통신에 사용할 변수 */
FD_SET rset, wset;
timeval sel_time;
int ID;

/*전역변수*/
short availablePos;
short availablePos_x;
short availablePos_y_up;
short availablePos_y_down;

/*로그 저장 값*/
int acceptCount;
int fpsCount;
int playerCount; /* AcceptProc() / ListDelete()*/
int AttackerCount; /* 패킷오면 1증가 1초마다 0 초기화 */
int MoveCount;  /* MoveStart() / MoveStop() */

/* 로직 */
bool validation(unsigned short x, unsigned short y);

/* 연속 패킷 처리 */
#define NOATTACK 0
#define ATTACK1 1
#define ATTACK2 2
#define ATTACK3 3
LARGE_INTEGER timer;
float timerCheck;
LARGE_INTEGER current_time;
bool continuityAttack1(unsigned int ID, LARGE_INTEGER player_delta, BYTE AttackMotion);
bool continuityAttack2(unsigned int ID, LARGE_INTEGER player_delta, BYTE AttackMotion);
bool continuityAttack3(unsigned int ID, LARGE_INTEGER player_delta, BYTE AttackMotion);
bool unionAttack(unsigned int ID, BYTE AttackMotion);