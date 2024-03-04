#ifndef  __PACKET__
#define  __PACKET__

class CPacket
{
public:
	enum en_PACKET
	{
		eBUFFER_DEFAULT = 1400		// 패킷의 기본 버퍼 사이즈.
	};

	CPacket();
	CPacket(int iBufferSize);
	/* 소멸자를 가상 함수로 선언하지 않으면
	이 경우 자식 클래스의 소멸자는 결코
	호출되지 않는다*/
	virtual	~CPacket();

	void Clear(void);

	/* 버퍼 사이즈 얻기 */
	int	GetBufferSize(void) { return m_iBufferSize; }

	/* 사용중인 사이즈 얻기*/
	int GetDataSize(void) { return m_iDataSize; }

	/* 버퍼 포인터 얻기*/
	char* GetBufferPtr(void) { return m_chpBuffer; }

	// 버퍼 Pos 이동. (음수이동은 안됨)
	// GetBufferPtr 함수를 이용하여 외부에서 강제로 버퍼 내용을 수정할 경우 사용. 
	// Parameters: (int) 이동 사이즈.
	// Return: (int) 이동된 사이즈.
	int		MoveWritePos(int iSize);
	int		MoveReadPos(int iSize);

	CPacket& operator= (CPacket& clSrcPacket);

	//////////////////////////////////////////////////////////////////////////
	// 넣기.	각 변수 타입마다 모두 만듬.
	//////////////////////////////////////////////////////////////////////////
	CPacket& operator<< (unsigned char byValue);
	CPacket& operator<< (char chValue);

	CPacket& operator<< (short shValue);
	CPacket& operator<< (unsigned short wValue);

	CPacket& operator<< (int iValue);
	CPacket& operator<< (unsigned int iValue);
	CPacket& operator<< (long lValue);
	CPacket& operator<<(DWORD lValue);
	CPacket& operator<< (float fValue);

	CPacket& operator<< (__int64 iValue);
	CPacket& operator<< (double dValue);


	//////////////////////////////////////////////////////////////////////////
	// 빼기.	각 변수 타입마다 모두 만듬.
	//////////////////////////////////////////////////////////////////////////
	CPacket& operator>> (BYTE& byValue);
	CPacket& operator>> (char& chValue);

	CPacket& operator>> (short& shValue);
	CPacket& operator>> (WORD& wValue);

	CPacket& operator>> (int& iValue);
	CPacket& operator>> (DWORD& dwValue);
	CPacket& operator>> (float& fValue);

	CPacket& operator>> (__int64& iValue);
	CPacket& operator>> (double& dValue);




	//////////////////////////////////////////////////////////////////////////
	// 데이타 얻기.
	//
	// Parameters: (char *)Dest 포인터. (int)Size.
	// Return: (int)복사한 사이즈.
	//////////////////////////////////////////////////////////////////////////
	int		GetData(char* chpDest, int iSize);

	//////////////////////////////////////////////////////////////////////////
	// 데이타 삽입.
	//
	// Parameters: (char *)Src 포인터. (int)SrcSize.
	// Return: (int)복사한 사이즈.
	//////////////////////////////////////////////////////////////////////////
	int		PutData(char* chpSrc, int iSrcSize);




protected:
	int	m_iBufferSize;
	int	m_iDataSize;
	char* m_chpBuffer;
	char* front;
	char* rear;

};

#endif