#pragma comment(lib,"ws2_32")
#pragma comment(lib,"Winmm.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include "RingBuffer.h"
#include <WS2tcpip.h>
#include "LinkedList.h"
#include "CPacket.h"
#include "main.h"


/* listen sock*/
SOCKET listen_sock;

/* Player [List / array ]*/
CList<Player*> PlayerList;

int main()
{
	timeBeginPeriod(1);

	if (NetworkInit() == false)
		return 0;


	LARGE_INTEGER start, end;
	float DeltaTime;
	float TimeRoutine = 0;
	float second_Frame = 0;
	QueryPerformanceFrequency(&timer);

	while (1)
	{
		QueryPerformanceCounter(&start);

		NetWorking();

		/* 20ms����.. */
		if (TimeRoutine > 0.02f)
		{
			Logic();
			QueryPerformanceCounter(&start);
			TimeRoutine = 0;
			++fpsCount;
		}

		QueryPerformanceCounter(&end);
		DeltaTime = (end.QuadPart - start.QuadPart) / (float)timer.QuadPart;
		TimeRoutine += DeltaTime;
		second_Frame += DeltaTime;

		if (second_Frame > 1.0f)
		{
			wprintf(L"FPS : %d\n", fpsCount);
			//wprintf(L"Accept : %d\n", acceptCount);
			wprintf(L"User : %d\n", playerCount);
			//wprintf(L"Attacker : %d\n", AttackerCount);
			//wprintf(L"MoveUser : %d\n", MoveCount);
			second_Frame = 0;
			fpsCount = 0;
			acceptCount = 0;
			AttackerCount = 0;
		}

	}
	timeEndPeriod(1);
}


bool NetworkInit()
{
	/* ���� �ʱ�ȭ */
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;

	/* socket() */
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		return false;

	/* bind() */
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	InetPton(AF_INET, L"180.69.30.237", &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	bind_ret = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (bind_ret == SOCKET_ERROR)
	{
		bind_ret = WSAGetLastError();
		printf("bind error ��ȣ %d", bind_ret);
		return false;
	}

	printf("bind() ����\n");

	/* listen */
	listen_ret = listen(listen_sock, SOMAXCONN);
	if (listen_ret == SOCKET_ERROR)
	{
		listen_ret = WSAGetLastError();
		printf("listen error ��ȣ %d", listen_ret);
		return false;
	}

	printf("listen() ����\n");

	// nonblocking()
	u_long on = 1;
	nblock_ret = ioctlsocket(listen_sock, FIONBIO, &on);
	if (nblock_ret == SOCKET_ERROR)
	{
		nblock_ret = WSAGetLastError();
		printf("non blocking error ��ȣ %d ", nblock_ret);
		return false;
	}

	printf("non-block() ��ȯ\n");

	// linger() [SRT]
	LINGER lingerSt;
	lingerSt.l_onoff = 1;
	lingerSt.l_linger = 0;
	setsockopt(listen_sock, SOL_SOCKET, SO_LINGER, (char*)&lingerSt, sizeof(lingerSt));

	printf("Linger() ���� \n");

	//timeval 0.00020000 - usec 20000
	sel_time.tv_sec = 0;
	sel_time.tv_usec = 0;

	return true;
}

void NetWorking()
{
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_SET(listen_sock, &rset);

	for (CList<Player*>::iterator iter = PlayerList.begin(); iter != PlayerList.end(); ++iter)
	{
		if (PlayerList.empty() == true)
			break;

		FD_SET((*iter)->sock, &rset);
		if ((*iter)->sendRingBuffer.GetUseSize() > 0)
		{
			FD_SET((*iter)->sock, &wset);
		}
	}

	sel_ret = select(0, &rset, &wset, 0, &sel_time);
	if (sel_ret > 0)
	{
		if (FD_ISSET(listen_sock, &rset))
		{
			AcceptProc();
		}

		/* ����Ʈ ���� �� ��ŭ �ݺ��ϸ�, recv�Ұ� �ִ��� send�Ұ� �ִ��� �Ǵ�*/
		for (CList<Player*>::iterator iter = PlayerList.begin(); iter != PlayerList.end(); ++iter)
		{
			if (FD_ISSET((*iter)->sock, &rset))
			{
				RewProc(*iter);
			}

			if (FD_ISSET((*iter)->sock, &wset))
			{
				SendProc(*iter);
			}
		}

	}
	else if (sel_ret == SOCKET_ERROR)
	{
		printf("select �Լ� ���� ���� code : %d ", sel_ret);
	}

	/* �ϰ� List ����*/
	ListDelete();

	/* Logic �ݺ� Ƚ�� */
	++acceptCount;
}

void Logic()
{
	if (PlayerList.empty() == true)
		return;

	/*�ϴ� .. �� List���� ���¸� ���� Ȯ���ϸ鼭..
		Move���¶�� Move���ְ�, Attack ���¶�� Attack�ϰ� */
	for (CList<Player*>::iterator iter = PlayerList.begin(); iter != PlayerList.end(); ++iter)
	{
		if ((*iter)->StopCheck == true)
		{
			switch ((*iter)->Direction)
			{
			case dfPACKET_MOVE_DIR_LL:
			{
				//printf("# gameeRun:LL # PlayerID: %d / X:%d / Y:%d\n", (*iter)->ID, (*iter)->xpos, (*iter)->ypos);
				if (validation((*iter)->xpos, (*iter)->ypos))
				{
					(*iter)->xpos -= 3;
				}
				break;
			}
			case dfPACKET_MOVE_DIR_LU:
			{
				//printf("# gameeRun:LU # PlayerID: %d / X:%d / Y:%d\n", (*iter)->ID, (*iter)->xpos, (*iter)->ypos);
				if (validation((*iter)->xpos, (*iter)->ypos))
				{
					(*iter)->xpos -= 3;
					(*iter)->ypos -= 2;
				}
				break;

			}
			case dfPACKET_MOVE_DIR_UU:
			{
				//printf("# gameeRun:UU # PlayerID: %d / X:%d / Y:%d\n", (*iter)->ID, (*iter)->xpos, (*iter)->ypos);
				if (validation((*iter)->xpos, (*iter)->ypos))
				{
					(*iter)->ypos -= 2;
				}
				break;
			}
			case dfPACKET_MOVE_DIR_RU:
			{
				//printf("# gameeRun:RU # PlayerID: %d / X:%d / Y:%d\n", (*iter)->ID, (*iter)->xpos, (*iter)->ypos);
				if (validation((*iter)->xpos, (*iter)->ypos))
				{
					(*iter)->xpos += 3;
					(*iter)->ypos -= 2;
				}
				break;

			}
			case dfPACKET_MOVE_DIR_RR:
			{
				//printf("# gameeRun:RR # PlayerID: %d / X:%d / Y:%d\n", (*iter)->ID, (*iter)->xpos, (*iter)->ypos);
				if (validation((*iter)->xpos, (*iter)->ypos))
				{
					(*iter)->xpos += 3;
				}
				break;
			}
			case dfPACKET_MOVE_DIR_RD:
			{
				//printf("# gameeRun:RD # PlayerID: %d / X:%d / Y:%d\n", (*iter)->ID, (*iter)->xpos, (*iter)->ypos);
				if (validation((*iter)->xpos, (*iter)->ypos))
				{
					(*iter)->xpos += 3;
					(*iter)->ypos += 2;
				}
				break;
			}
			case dfPACKET_MOVE_DIR_DD:
			{
				//printf("# gameeRun:DD # PlayerID: %d / X:%d / Y:%d\n", (*iter)->ID, (*iter)->xpos, (*iter)->ypos);
				if (validation((*iter)->xpos, (*iter)->ypos))
				{
					(*iter)->ypos += 2;
				}
				break;
			}
			case dfPACKET_MOVE_DIR_LD:
			{
				//printf("# gameeRun:LD # PlayerID: %d / X:%d / Y:%d\n", (*iter)->ID, (*iter)->xpos, (*iter)->ypos);
				if (validation((*iter)->xpos, (*iter)->ypos))
				{
					(*iter)->xpos -= 3;
					(*iter)->ypos += 2;
				}
				break;
			}
			default:
				break;
			}
		}
	}

}

/* ��ǥ ��ȿ �˻� */
bool validation(unsigned short x, unsigned short y)
{
	if (x < dfRANGE_MOVE_LEFT)
		return false;

	if (x > dfRANGE_MOVE_RIGHT)
		return false;

	if (y < dfRANGE_MOVE_TOP)
		return false;

	if (y > dfRANGE_MOVE_BOTTOM)
		return false;

	return true;
}

void AcceptProc()
{
	int addrlen;
	SOCKET client_sock;
	SOCKADDR_IN client_addr;

	addrlen = sizeof(client_addr);
	client_sock = accept(listen_sock, (SOCKADDR*)&client_addr, &addrlen);
	if (client_sock == INVALID_SOCKET)
	{
		printf("���� ���� �ȵ�! \n");
	}

	printf("���� �Ǿ����ϴ�.\n");

	Player* player = new Player();
	player->sock = client_sock;
	playerInit(player);

	/*List�� �־� ����*/
	PlayerList.push_back(player);
	++playerCount;
	printf("Create Character # Player ID: %d	X:%d	Y:%d\n", player->ID, player->xpos, player->ypos);


	/* ���� (0) ��Ŷ*/
	CPacket clpPacketCreate;
	mpCreateCharacter(&clpPacketCreate, player->ID, player->Direction, player->xpos, player->ypos, player->HP);
	SendUniPacket(player, &clpPacketCreate);


	printf("����(0) ��Ŷ ���½��ϴ�\n");

	/* �ٸ� ������ ���� �� ���� �˸��� (1) ��Ŷ*/
	CPacket clpPacketOther;
	mpCreateOther(&clpPacketOther, player->ID, player->Direction, player->xpos, player->ypos, player->HP);
	SendBoardPacket(player, &clpPacketOther);

	printf("�ٸ� �������� (1) �˸� ��Ŷ ���½��ϴ�\n");

	/* ���� �ϰ� �ִ� ���� ���� �ޱ�, �� playerList ���� Ȯ�� �� Unisend */
	CPacket clpConnectClients;
	for (CList<Player*>::iterator iter = PlayerList.begin(); iter != PlayerList.end(); ++iter)
	{
		if (player->ID == (*iter)->ID)
			continue;
		
		clpConnectClients.Clear();
		mpCreateOther(&clpConnectClients, (*iter)->ID, (*iter)->Direction, (*iter)->xpos, (*iter)->ypos, (*iter)->HP);
		SendUniPacket(player, &clpConnectClients);
	}
	printf("���ӵǾ� �ִ� ��� Ŭ�� ���� �޾ҽ��ϴ�. \n");
}

void playerInit(Player* player)
{
	player->ID = ID++;
	player->Direction = dfPACKET_MOVE_DIR_LL;
	player->HP = 100;
	player->xpos = rand() % 600 + 10;
	player->ypos = rand() % 400 + 50;
	/*	���ϴ� ��ȿ ������ x 10~630 y 50~470 */
}

Header CreateHeader(BYTE type)
{
	Header header;
	switch (type)
	{
	case dfPACKET_SC_CREATE_MY_CHARACTER: /* 0�� */
	{
		header.byCode = 0x89;
		header.bySize = 10;
		header.byType = dfPACKET_SC_CREATE_MY_CHARACTER;
		break;
	}
	case dfPACKET_SC_CREATE_OTHER_CHARACTER: /* 1�� */
	{
		header.byCode = 0x89;
		header.bySize = 10;
		header.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;
		break;
	}
	case dfPACKET_SC_DELETE_CHARACTER: /* 2�� */
	{
		header.byCode = 0x89;
		header.bySize = 4;
		header.byType = dfPACKET_SC_DELETE_CHARACTER;
		break;
	}
	case dfPACKET_SC_MOVE_START: /* 11�� */
	{
		header.byCode = 0x89;
		header.bySize = 9;
		header.byType = dfPACKET_SC_MOVE_START;
		break;
	}
	case dfPACKET_SC_MOVE_STOP: /* 13�� */
	{
		header.byCode = 0x89;
		header.bySize = 9;
		header.byType = dfPACKET_SC_MOVE_STOP;
		break;
	}
	case dfPACKET_SC_ATTACK1: /* 21�� */
	{
		header.byCode = 0x89;
		header.bySize = 9;
		header.byType = dfPACKET_SC_ATTACK1;
		break;
	}
	case dfPACKET_SC_ATTACK2: /* 23�� */
	{
		header.byCode = 0x89;
		header.bySize = 9;
		header.byType = dfPACKET_SC_ATTACK2;
		break;
	}
	case dfPACKET_SC_ATTACK3: /* 25�� */
	{
		header.byCode = 0x89;
		header.bySize = 9;
		header.byType = dfPACKET_SC_ATTACK3;
		break;
	}
	case dfPACKET_SC_DAMAGE: /* 30�� */
	{
		header.byCode = 0x89;
		header.bySize = 9;
		header.byType = dfPACKET_SC_DAMAGE;
		break;
	}
	default:
		printf("tpye ���� �߸� ����ϴ�. %d type code ", type);
		break;
	}

	return header;
}

/* RewProc */
void RewProc(Player* player)
{
	/* 1000 byte �̾Ƽ� Enqueue */
	char buffer[1000];
	memset(buffer, 0, sizeof(buffer));

	recv_ret = recv(player->sock, buffer, sizeof(buffer), 0);
	if (recv_ret == SOCKET_ERROR)
	{
		recv_ret = WSAGetLastError();
		if (recv_ret != WSAEWOULDBLOCK)
		{
			/* SRT */
			if (recv_ret == WSAECONNRESET)
			{
				printf("SRT 10054 Disconnet Player ID: %d\n", player->ID);
				Disconnect(player);
				return;
			}
		}

	}
	/* FIN */
	if (recv_ret == 0)
	{
		printf("FIN Disconnet Player ID: %d\n", player->ID);
		Disconnect(player);
		return;
	}

	/* recv Print */
	printf("*****************[Player ��ü ID : %d recv %d BYTE�޾ҽ��ϴ�.]****************\n", player->ID, recv_ret);

	/* ���� �� ����[recv]���ۿ� �ױ� */
	recv_Enqueue = player->recvRingBuffer.Enqueue(buffer, recv_ret);
	if (recv_Enqueue != recv_ret)
	{
		printf("���� ī�� ����\n");
	}

	/* ���� ���� ��� ó�� */
	while (player->recvRingBuffer.GetUseSize() > 0)
	{
		char HeaderMessage[100];
		memset(HeaderMessage, 0, sizeof(HeaderMessage));
		recv_Dequeue = player->recvRingBuffer.Dequeue(HeaderMessage, sizeof(Header));
		if (recv_Dequeue != sizeof(Header))
		{
			printf("RingBuffer���� Header ��ŭ ���� ����!! ���� Disconnet Player ID: %d\n", player->ID);
			Disconnect(player);
			return;
		}

		Header* header = (Header*)HeaderMessage;
		if (header->byCode != 0x89)
		{
			printf("byCode�� 0x89 �� �ƴ�!! ���� Code %c Disconnet Player ID: %d\n", header->byCode, player->ID);
			Disconnect(player);
			return;
		}

		char RealMessage[100];
		memset(RealMessage, 0, sizeof(RealMessage));
		recv_Dequeue = player->recvRingBuffer.Dequeue(RealMessage, header->bySize);
		if (recv_Dequeue != header->bySize)
		{
			printf("HeaderSize ��ŭ ���� ���� ���� Size %d / Disconnet Player ID: %d\n", recv_Dequeue, player->ID);
			Disconnect(player);
			return;
		}

		printf("*****************[���� ��Ŷ Type %d]********************\n", header->byType);

		CPacket clpPacket;
		clpPacket.PutData(RealMessage, header->bySize);

		PacketProc(player, header->byType, &clpPacket);
	}
}

bool PacketProc(Player* player, BYTE byPacketType, CPacket* pPacket)
{
	switch (byPacketType)
	{
	case dfPACKET_CS_MOVE_START:
	{
		return netPacketProc_MoveStart(player, pPacket);
		break;
	}
	case dfPACKET_CS_MOVE_STOP:
	{
		return netPacketProc_MoveStop(player, pPacket);
		break;
	}
	case dfPACKET_CS_ATTACK1:
	{
		return netPacketProc_Attack1(player, pPacket);
		break;
	}
	case dfPACKET_CS_ATTACK2:
	{
		return netPacketProc_Attack2(player, pPacket);
		break;
	}
	case dfPACKET_CS_ATTACK3:
		return netPacketProc_Attack3(player, pPacket);
		break;
	}

	return true;
}

bool netPacketProc_MoveStart(Player* player, CPacket* pPacket)
{
	/* �̵� �޼����� ����, �̵� ���·� ������ְ� �������� ó���� ����
				�ٸ� Ŭ���̾�Ʈ�鿡�� BoardCast�ؾ��� */
	BYTE Direction;
	unsigned short x;
	unsigned short y;

	*pPacket >> Direction;
	*pPacket >> x;
	*pPacket >> y;

	player->Direction = Direction;
	player->xpos = x;
	player->ypos = y;
	player->StopCheck = true;

	printf("# PACKET_MOVESTART # SessionID:%d / Direction:%d / X:%d / Y:%d\n", player->ID, Direction, x,y);

	CPacket MoveStart;
	mpMoveStart(&MoveStart,player->ID, player->Direction, player->xpos, player->ypos);
	SendBoardPacket(player, &MoveStart);

	return true;
}

bool netPacketProc_MoveStop(Player* player, CPacket* pPacket)
{
	/*			1. ��ǥ ��ȿ �˻�
				2. ��ǥ ���� ����
				3. �ش� Player�� ����ٴ� ���� BoardCast
				*/
	BYTE Direction;
	unsigned short x;
	unsigned short y;

	*pPacket >> Direction;
	*pPacket >> x;
	*pPacket >> y;


	if (x > player->xpos + dfERROR_RANGE ||x < player->xpos - dfERROR_RANGE)
	{
		printf("���� x�� ��ǥ�� 50 ���� ���� �ʰ���. player�� x %d / ��Ŷ�� x %d\n", player->xpos, x);
		Disconnect(player);
		return false;
	}

	if (y > player->ypos + dfERROR_RANGE ||y < player->ypos - dfERROR_RANGE)
	{
		printf("���� y�� ��ǥ�� 50 ���� ���� �ʰ���. player�� y %d / ��Ŷ�� y %d\n", player->ypos,y);
		Disconnect(player);
		return false;
	}

	player->Direction = Direction;
	player->StopCheck = false;
	player->xpos = x;
	player->ypos = y;
	

	printf("# PACKET_MOVESTOP # SessionID:%d / Direction:%d / X:%d / Y:%d\n", player->ID, Direction,x, y);

	CPacket clpPacket;
	mpMoveStop(&clpPacket, player->ID, player->Direction, player->xpos, player->ypos);
	SendBoardPacket(player, &clpPacket);

	return true;
}

bool netPacketProc_Attack1(Player* player, CPacket* pPacket)
{
	/*	0. ������ �� ���� �����ߴٴ� ���.
				1. ��ġ�� ������ �˻�
				2. �ٸ� ��ο��� BoardCast , Header�����ؼ� ����� ���� ��
				3. ����Ʈ ��ȯ �� �ش��ϴ� ��ġ�� ���� �ִ��� �Ǻ�, LL/ RR���� Ȯ��
				4. ���� ���ٸ� Braek
				4. ���� �ִٸ� �ش� ���� HP�� ���ҽ�Ų �� BoardCast */

	if (continuityAttack1(player->ID, player->Player_DeltaTime, player->AttackMotion))
	{
		return true;
	}
	QueryPerformanceCounter(&(player->Player_DeltaTime));
	player->AttackMotion = ATTACK1;

	BYTE Direction;
	unsigned short x;
	unsigned short y;

	*pPacket >> Direction;
	*pPacket >> x;
	*pPacket >> y;


	availablePos = player->xpos - x;
	if (availablePos > dfERROR_RANGE || availablePos < -dfERROR_RANGE)
	{
		printf("x�� ��ǥ�� 50 ���� ���� �ʰ��� player�� x %d/ ��Ŷ�� x %d \n", player->xpos, x);
		Disconnect(player);
		return false;
	}
	/* Ŭ�� �ϰ� ��ǥ �缳�� */
	player->xpos =x;
	player->ypos =y;

	CPacket clpPacket;
	mpAttack1(&clpPacket, player->ID, player->Direction, player->xpos, player->ypos);
	SendBoardPacket(player, &clpPacket);

	AttackPriority object;
	/* ���ʺ��� �����ߴٸ� */
	if (Direction == dfPACKET_MOVE_DIR_LL)
	{
		availablePos_x = player->xpos - dfATTACK1_RANGE_X;
		availablePos_y_up = player->ypos - dfATTACK1_RANGE_Y;
		availablePos_y_down = player->ypos + dfATTACK1_RANGE_Y;

		/* ��ȸ ���� */
		for (CList<Player*>::iterator iter = PlayerList.begin(); iter != PlayerList.end(); ++iter)
		{
			if ((*iter)->ID == player->ID)
				continue;
			/* x ���� ���� �ְ� */
			if ((*iter)->xpos > availablePos_x && (*iter)->xpos < player->xpos)
			{
				/* y ���� ������ �ִٸ� */
				if ((*iter)->ypos > availablePos_y_up && (*iter)->ypos < availablePos_y_down)
				{
					/* ������ �÷��̾� �Ѹ��� ����*/
					if (object.x > player->xpos - (*iter)->xpos)
					{
						object.x = player->xpos - (*iter)->xpos;
						object.priority = (*iter);
					}
				}
			}
		}
		/* ��ȸ �� */
	}
	/* ������ ���� �����ߴٸ� */
	else if (Direction == dfPACKET_MOVE_DIR_RR)
	{
		availablePos_x = player->xpos + dfATTACK1_RANGE_X;
		availablePos_y_up = player->ypos - dfATTACK1_RANGE_Y;
		availablePos_y_down = player->ypos + dfATTACK1_RANGE_Y;

		/* ��ȸ ����*/
		for (CList<Player*>::iterator iter = PlayerList.begin(); iter != PlayerList.end(); ++iter)
		{
			if ((*iter)->ID == player->ID)
				continue;
			/* x ���� ���� �ְ� */
			if ((*iter)->xpos < availablePos_x && (*iter)->xpos > player->xpos)
			{
				/* y ���� ������ �ִٸ� */
				if ((*iter)->ypos > availablePos_y_up && (*iter)->ypos < availablePos_y_down)
				{
					/* ������ �÷��̾� �Ѹ��� ����*/
					if (object.x > (*iter)->xpos - player->xpos)
					{
						object.x = (*iter)->xpos - player->xpos;
						object.priority = (*iter);
					}
				}
			}
		}
		/* ��ȸ �� */
	}
	if (object.x != USHRT_MAX)
	{
		object.priority->HP -= dfATTACK1_DAMGE;

		CPacket clpPacketDamage;
		mpDamage(&clpPacketDamage, player->ID, object.priority->ID, object.priority->HP);
		SendUniPacket(player, &clpPacketDamage);
		SendBoardPacket(player, &clpPacketDamage);

		if (object.priority->HP <= 0 || object.priority->HP > 100)
		{
			printf("��ü�� HP�� 0�� �Ǿ� Disconnect�� ��ü ID : %d \n", object.priority->ID);
			Disconnect(object.priority);
		}
	}
	return true;
}

bool netPacketProc_Attack2(Player* player, CPacket* pPacket)
{
	if (continuityAttack2(player->ID, player->Player_DeltaTime, player->AttackMotion))
	{
		return true;
	}
	QueryPerformanceCounter(&(player->Player_DeltaTime));
	player->AttackMotion = ATTACK2;

	BYTE Direction;
	unsigned short x;
	unsigned short y;

	*pPacket >> Direction;
	*pPacket >> x;
	*pPacket >> y;

	availablePos = player->xpos - x;
	if (availablePos > dfERROR_RANGE || availablePos < -dfERROR_RANGE)
	{
		printf("x�� ��ǥ�� 50 ���� ���� �ʰ��� player�� x %d/ ��Ŷ�� x %d \n", player->xpos,x);
		Disconnect(player);
		return false;
	}
	/* Ŭ�� �ϰ� ��ǥ �缳�� */
	player->xpos = x;
	player->ypos = y;

	CPacket clpPacket;
	mpAttack2(&clpPacket, player->ID, player->Direction, player->xpos, player->ypos);
	SendBoardPacket(player, &clpPacket);

	AttackPriority object;
	/* ���ʺ��� �����ߴٸ� */
	if (Direction == dfPACKET_MOVE_DIR_LL)
	{
		availablePos_x = player->xpos - dfATTACK2_RANGE_X;
		availablePos_y_up = player->ypos - dfATTACK2_RANGE_Y;
		availablePos_y_down = player->ypos + dfATTACK2_RANGE_Y;


		/* ��ȸ ���� */
		for (CList<Player*>::iterator iter = PlayerList.begin(); iter != PlayerList.end(); ++iter)
		{
			if ((*iter)->ID == player->ID)
				continue;
			/* x ���� ���� �ְ� */
			if ((*iter)->xpos > availablePos_x && (*iter)->xpos < player->xpos)
			{
				/* y ���� ������ �ִٸ� */
				if ((*iter)->ypos > availablePos_y_up && (*iter)->ypos < availablePos_y_down)
				{
					/* ������ �÷��̾� �Ѹ��� ����*/
					if (object.x > player->xpos - (*iter)->xpos)
					{
						object.x = player->xpos - (*iter)->xpos;
						object.priority = (*iter);
					}
				}
			}
		}
		/* ��ȸ �� */

	}
	/* ������ ���� �����ߴٸ� ���߿� else�� �ٲ� �� */
	else if (Direction == dfPACKET_MOVE_DIR_RR)
	{
		availablePos_x = player->xpos + dfATTACK2_RANGE_X;
		availablePos_y_up = player->ypos - dfATTACK2_RANGE_Y;
		availablePos_y_down = player->ypos + dfATTACK2_RANGE_Y;


		/* ��ȸ ����*/
		for (CList<Player*>::iterator iter = PlayerList.begin(); iter != PlayerList.end(); ++iter)
		{
			if ((*iter)->ID == player->ID)
				continue;
			/* x ���� ���� �ְ� */
			if ((*iter)->xpos < availablePos_x && (*iter)->xpos > player->xpos)
			{
				/* y ���� ������ �ִٸ� */
				if ((*iter)->ypos > availablePos_y_up && (*iter)->ypos < availablePos_y_down)
				{
					/* ������ �÷��̾� �Ѹ��� ����*/
					if (object.x > (*iter)->xpos - player->xpos)
					{
						object.x = (*iter)->xpos - player->xpos;
						object.priority = (*iter);
					}
				}
			}
		}
		/* ��ȸ �� */
	}

	if (object.x != USHRT_MAX)
	{
		object.priority->HP -= dfATTACK2_DAMGE;
	
		CPacket clpPacketDamage;
		mpDamage(&clpPacketDamage, player->ID, object.priority->ID, object.priority->HP);
		SendUniPacket(player, &clpPacketDamage);
		SendBoardPacket(player, &clpPacketDamage);

		if (object.priority->HP <= 0 || object.priority->HP > 100)
		{
			printf("��ü�� HP�� 0�� �Ǿ� Disconnect�� ��ü ID : %d \n", object.priority->ID);
			Disconnect(object.priority);
		}
	}

	return true;
}

bool netPacketProc_Attack3(Player* player, CPacket* pPacket)
{
	if (continuityAttack3(player->ID, player->Player_DeltaTime, player->AttackMotion))
	{
		return true;
	}
	QueryPerformanceCounter(&(player->Player_DeltaTime));
	player->AttackMotion = ATTACK3;

	BYTE Direction;
	unsigned short x;
	unsigned short y;

	*pPacket >> Direction;
	*pPacket >> x;
	*pPacket >> y;

	availablePos = player->xpos - x;
	if (availablePos > dfERROR_RANGE || availablePos < -dfERROR_RANGE)
	{
		printf("x�� ��ǥ�� 50 ���� ���� �ʰ��� player�� x %d/ ��Ŷ�� x %d \n", player->xpos,x);
		Disconnect(player);
		return false;
	}

	/* Ŭ�� �ϰ� ��ǥ �缳�� */
	player->xpos = x;
	player->ypos = y;

	CPacket clpPacket;
	mpAttack3(&clpPacket, player->ID, player->Direction, player->xpos, player->ypos);
	SendBoardPacket(player, &clpPacket);

	AttackPriority object;
	/* ���ʺ��� �����ߴٸ� */
	if (Direction == dfPACKET_MOVE_DIR_LL)
	{
		availablePos_x = player->xpos - dfATTACK3_RANGE_X;
		availablePos_y_up = player->ypos - dfATTACK3_RANGE_Y;
		availablePos_y_down = player->ypos + dfATTACK3_RANGE_Y;


		/* ��ȸ ���� */
		for (CList<Player*>::iterator iter = PlayerList.begin(); iter != PlayerList.end(); ++iter)
		{
			if ((*iter)->ID == player->ID)
				continue;
			/* x ���� ���� �ְ� */
			if ((*iter)->xpos > availablePos_x && (*iter)->xpos < player->xpos)
			{
				/* y ���� ������ �ִٸ� */
				if ((*iter)->ypos > availablePos_y_up && (*iter)->ypos < availablePos_y_down)
				{
					/* ������ �÷��̾� �Ѹ��� ����*/
					if (object.x > player->xpos - (*iter)->xpos)
					{
						object.x = player->xpos - (*iter)->xpos;
						object.priority = (*iter);
					}
				}
			}
		}
		/* ��ȸ �� */

	}
	/* ������ ���� �����ߴٸ� ���߿� else�� �ٲ� �� */
	else if (Direction == dfPACKET_MOVE_DIR_RR)
	{
		availablePos_x = player->xpos + dfATTACK3_RANGE_X;
		availablePos_y_up = player->ypos - dfATTACK3_RANGE_Y;
		availablePos_y_down = player->ypos + dfATTACK3_RANGE_Y;


		/* ��ȸ ����*/
		for (CList<Player*>::iterator iter = PlayerList.begin(); iter != PlayerList.end(); ++iter)
		{
			if ((*iter)->ID == player->ID)
				continue;
			/* x ���� ���� �ְ� */
			if ((*iter)->xpos < availablePos_x && (*iter)->xpos > player->xpos)
			{
				/* y ���� ������ �ִٸ� */
				if ((*iter)->ypos > availablePos_y_up && (*iter)->ypos < availablePos_y_down)
				{
					/* ������ �÷��̾� �Ѹ��� ����*/
					if (object.x > (*iter)->xpos - player->xpos)
					{
						object.x = (*iter)->xpos - player->xpos;
						object.priority = (*iter);
					}
				}
			}
		}
		/* ��ȸ �� */
	}

	if (object.x != USHRT_MAX)
	{
		object.priority->HP -= dfATTACK3_DAMGE;

		CPacket clpPacketDamage;
		mpDamage(&clpPacketDamage, player->ID, object.priority->ID, object.priority->HP);
		SendUniPacket(player, &clpPacketDamage);
		SendBoardPacket(player, &clpPacketDamage);

		if (object.priority->HP <= 0 || object.priority->HP > 100)
		{
			printf("��ü�� HP�� 0�� �Ǿ� Disconnect�� ��ü ID : %d \n", object.priority->ID);
			Disconnect(object.priority);
		}
	}

	return true;
}

/* attack 1 2 3 �ð��� ��ȿó�� */
bool continuityAttack1(unsigned int ID, LARGE_INTEGER player_delta, BYTE AttackMotion)
{
	QueryPerformanceCounter(&current_time);
	timerCheck = (current_time.QuadPart - player_delta.QuadPart) / (float)timer.QuadPart;

	if (unionAttack(ID, AttackMotion))
	{
		return true;
	}
	return false;
}

bool continuityAttack2(unsigned int ID, LARGE_INTEGER player_delta, BYTE AttackMotion)
{
	QueryPerformanceCounter(&current_time);
	timerCheck = (current_time.QuadPart - player_delta.QuadPart) / (float)timer.QuadPart;

	if (unionAttack(ID, AttackMotion))
	{
		return true;
	}
	return false;
}

bool continuityAttack3(unsigned int ID, LARGE_INTEGER player_delta, BYTE AttackMotion)
{
	QueryPerformanceCounter(&current_time);
	timerCheck = (current_time.QuadPart - player_delta.QuadPart) / (float)timer.QuadPart;

	if (unionAttack(ID, AttackMotion))
	{
		return true;
	}

	return false;
}

bool unionAttack(unsigned int ID, BYTE AttackMotion)
{
	switch (AttackMotion)
	{
	case NOATTACK:
		break;
	case ATTACK1:
		if (timerCheck < 0.2f)
		{
			printf("�ش� Player ID : %d �� ���ӵ� ��Ŷ ������. timerCheck %f \n", ID, timerCheck);
			return true;
		}
		break;
	case ATTACK2:
		if (timerCheck < 0.3f)
		{
			printf("�ش� Player ID : %d �� ���ӵ� ��Ŷ ������. timerCheck %f \n", ID, timerCheck);
			return true;
		}
		break;
	case ATTACK3:
		if (timerCheck < 0.4f)
		{
			printf("�ش� Player ID : %d �� ���ӵ� ��Ŷ ������. timerCheck %f \n", ID, timerCheck);
			return true;
		}
		break;
	}

	return false;
}

/* msg Init */
void mpCreateCharacter(CPacket* clpPacket, unsigned int ID, BYTE Direction, unsigned short x, unsigned short y, BYTE HP)
{
	Header stPacketHeader;
	stPacketHeader = CreateHeader(dfPACKET_SC_CREATE_MY_CHARACTER);

	mp_create_ret = clpPacket->PutData((char*)&stPacketHeader, sizeof(Header));
	*clpPacket << ID;
	*clpPacket << Direction;
	*clpPacket << x;
	*clpPacket << y;
	*clpPacket << HP;
}

void mpCreateOther(CPacket* clpPacket, unsigned int ID, BYTE Direction, unsigned short x, unsigned short y, BYTE HP)
{
	Header stPacketHeader;
	stPacketHeader = CreateHeader(dfPACKET_SC_CREATE_OTHER_CHARACTER);

	mp_other_ret = clpPacket->PutData((char*)&stPacketHeader, sizeof(Header));
	*clpPacket << ID;
	*clpPacket << Direction;
	*clpPacket << x;
	*clpPacket << y;
	*clpPacket << HP;
}

void mpMoveStart(CPacket* clpPacket, unsigned int ID, BYTE byDirection, unsigned short shX, unsigned short shY)
{
	Header stPacketHeader;
	stPacketHeader = CreateHeader(dfPACKET_SC_MOVE_START);

	mp_move_ret = clpPacket->PutData((char*)&stPacketHeader, sizeof(Header));

	*clpPacket << ID;
	*clpPacket << byDirection;
	*clpPacket << shX;
	*clpPacket << shY;
}

void mpMoveStop(CPacket* clpPacket, unsigned int ID, BYTE Direction, unsigned short x, unsigned short y)
{
	Header stPacketHeader;
	stPacketHeader = CreateHeader(dfPACKET_SC_MOVE_STOP);

	mp_stop_ret = clpPacket->PutData((char*)&stPacketHeader, sizeof(Header));
	*clpPacket << ID;
	*clpPacket << Direction;
	*clpPacket << x;
	*clpPacket << y;
}

void mpAttack1(CPacket* clpPacket, unsigned int ID, BYTE Direction, unsigned short xpos, unsigned short ypos)
{
	Header stPacketHeader;
	stPacketHeader = CreateHeader(dfPACKET_SC_ATTACK1);

	mp_Attack1_ret = clpPacket->PutData((char*)&stPacketHeader, sizeof(Header));
	*clpPacket << ID;
	*clpPacket << Direction;
	*clpPacket << xpos;
	*clpPacket << ypos;
}

void mpAttack2(CPacket* clpPacket, unsigned int ID, BYTE Direction, unsigned short xpos, unsigned short ypos)
{
	Header stPacketHeader;
	stPacketHeader = CreateHeader(dfPACKET_SC_ATTACK2);

	mp_Attack2_ret = clpPacket->PutData((char*)&stPacketHeader, sizeof(Header));
	*clpPacket << ID;
	*clpPacket << Direction;
	*clpPacket << xpos;
	*clpPacket << ypos;
}

void mpAttack3(CPacket* clpPacket, unsigned int ID, BYTE Direction, unsigned short xpos, unsigned short ypos)
{
	Header stPacketHeader;
	stPacketHeader = CreateHeader(dfPACKET_SC_ATTACK3);

	mp_Attack3_ret = clpPacket->PutData((char*)&stPacketHeader, sizeof(Header));
	*clpPacket << ID;
	*clpPacket << Direction;
	*clpPacket << xpos;
	*clpPacket << ypos;
}

void mpDamage(CPacket* clpPacket, unsigned int AttackerID, unsigned int VictimID, BYTE VictimHP)
{
	Header stPacketHeader;
	stPacketHeader = CreateHeader(dfPACKET_SC_DAMAGE);

	mp_Damage_ret = clpPacket->PutData((char*)&stPacketHeader, sizeof(Header));
	*clpPacket << AttackerID;
	*clpPacket << VictimID;
	*clpPacket << VictimHP;
}

void mpDelete(CPacket* clpPacket, unsigned int ID)
{
	Header stPacketHeader;
	stPacketHeader = CreateHeader(dfPACKET_SC_DELETE_CHARACTER);

	mp_Del_ret = clpPacket->PutData((char*)&stPacketHeader, sizeof(Header));
	*clpPacket << ID;
}

/* SendProc */
void SendProc(Player* player)
{
	char buffer[1000];

	send_Size = player->sendRingBuffer.Dequeue(buffer, sizeof(buffer));
	if (send_Size == 0)
	{
		int* a = nullptr;
		*a = 123;
	}

	send_ProcRet = send(player->sock, buffer, send_Size, 0);
	if (send_ProcRet != send_Size)
	{
		send_ProcRet = WSAGetLastError();
		if (send_ProcRet == WSAECONNRESET)
			return;
		printf("send_ProcRet error code %d \n", send_ProcRet);
		Disconnect(player);
	}

	/* send Print*/
	printf("**************************************[Player ��ü ID : %d send %d BYTE���½��ϴ�.]\n ", player->ID, send_ProcRet);

}

/* Uni, Board, Disconnect */
void Disconnect(Player* player)
{
	CList<Player*>::iterator iter;
	for (iter = PlayerList.begin(); iter != PlayerList.end(); ++iter)
	{
		if ((*iter)->ID == player->ID)
		{
			player->DeleteCheck = true;
			break;
		}
	}

	CPacket clpPacket;
	mpDelete(&clpPacket, player->ID);
	SendBoardPacket(player, &clpPacket);
}

void ListDelete()
{
	if (PlayerList.empty() == true)
		return;

	for (CList<Player*>::iterator iter = PlayerList.begin(); iter != PlayerList.end(); ++iter)
	{
		if ((*iter)->DeleteCheck == true)
		{
			--playerCount;
			closesocket((*iter)->sock);
			iter = PlayerList.erase(*iter);
			if (iter == PlayerList.end())
				break;
		}
	}
}

void SendUniPacket(Player* player, CPacket* clpPacket)
{
	int Uni_send_ret;
	int Uni_ret;

	char Data[1000];
	memset(Data, 0, sizeof(Data));
	Uni_ret = clpPacket->GetData(Data, clpPacket->GetDataSize());

	Uni_send_ret = player->sendRingBuffer.Enqueue(Data, Uni_ret);
	if (Uni_send_ret != Uni_ret)
	{
		printf(" len ���� %d / Enqueue ���� %d �� �ٸ��ϴ�\n", Uni_ret, Uni_send_ret);
		Disconnect(player);
	}
}

void SendBoardPacket(Player* player,CPacket* clpPacket)
{
	CList<Player*>::iterator iter;
	int sendBoard_ret;

	char Data[1000];
	memset(Data, 0, sizeof(Data));
	sendBoard_ret = clpPacket->GetData(Data, clpPacket->GetDataSize());

	for (iter = PlayerList.begin(); iter != PlayerList.end(); ++iter)
	{
		if ((*iter)->ID == player->ID)
			continue;
		
		board_ret = (*iter)->sendRingBuffer.Enqueue(Data, sendBoard_ret);
		if (board_ret != sendBoard_ret)
		{
			printf(" len ���� %d / Enqueue ���� %d �� �ٸ��ϴ�\n", sendBoard_ret, board_ret);
			Disconnect(*iter);
		}
	}
}