#define DEFAULFBUFFER 2000

class CRingBuffer
{
public:
	CRingBuffer();
	CRingBuffer(int iBufferSize);
	~CRingBuffer();

public:
	/* 크기 재 설정 */
	void Resize(int size);
	/* 버퍼 크기 얻기 */
	int GetBufferSize();

	/* 현재 사용중인 용량 얻기*/
	int GetUseSize();

	/* 현재 버퍼에 남은 용량 얻기*/
	int GetFreeSize();

	/* 포인터로 한번에 읽을 수 있는 길이*/
	int DirectEnqueueSize();
	int DirectDequeueSize();

	/* 데이터 넣음*/
	int Enqueue(const char* chpData, int iSize);

	/* 데이터 가져옴*/
	int Dequeue(char* chpDest, int iSize);

	/* 특정 구역 데이터 읽어옴 ReadPos 고정*/
	int Peek(char* chpDest, int iSize);

	/* 원하는 길이만큼 읽기위치에서 삭제/쓰기 위치 이동*/
	int MoveRear(int iSize);
	int MoveFront(int iSize);

	/* 버퍼의 모든 데이터 삭제*/
	void ClearBuffer();

	/* 버퍼의 Front 포인터 얻음*/
	char* GetFrontBufferPtr();

	/* 버퍼의 RearPos 포인터 얻음*/
	char* GetRearBufferPtr();

public:
	char* Data;
	char* Front;
	char* RearPos;

	int front_counting = 1;
	int rear_counting = 1;

	bool onecheck = true;
};