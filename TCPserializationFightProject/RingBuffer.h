#define DEFAULFBUFFER 2000

class CRingBuffer
{
public:
	CRingBuffer();
	CRingBuffer(int iBufferSize);
	~CRingBuffer();

public:
	/* ũ�� �� ���� */
	void Resize(int size);
	/* ���� ũ�� ��� */
	int GetBufferSize();

	/* ���� ������� �뷮 ���*/
	int GetUseSize();

	/* ���� ���ۿ� ���� �뷮 ���*/
	int GetFreeSize();

	/* �����ͷ� �ѹ��� ���� �� �ִ� ����*/
	int DirectEnqueueSize();
	int DirectDequeueSize();

	/* ������ ����*/
	int Enqueue(const char* chpData, int iSize);

	/* ������ ������*/
	int Dequeue(char* chpDest, int iSize);

	/* Ư�� ���� ������ �о�� ReadPos ����*/
	int Peek(char* chpDest, int iSize);

	/* ���ϴ� ���̸�ŭ �б���ġ���� ����/���� ��ġ �̵�*/
	int MoveRear(int iSize);
	int MoveFront(int iSize);

	/* ������ ��� ������ ����*/
	void ClearBuffer();

	/* ������ Front ������ ����*/
	char* GetFrontBufferPtr();

	/* ������ RearPos ������ ����*/
	char* GetRearBufferPtr();

public:
	char* Data;
	char* Front;
	char* RearPos;

	int front_counting = 1;
	int rear_counting = 1;

	bool onecheck = true;
};