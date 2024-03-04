#include <iostream>
#include <Windows.h>
#include "CPacket.h"

CPacket::CPacket()
{
	m_chpBuffer = (char*)malloc(eBUFFER_DEFAULT);
	m_iDataSize = 0;
	m_iBufferSize = eBUFFER_DEFAULT;
	front = m_chpBuffer;
	rear = m_chpBuffer;
}

CPacket::CPacket(int iBufferSize)
{
	m_chpBuffer = (char*)malloc(iBufferSize);
	m_iDataSize = 0;
	m_iBufferSize = iBufferSize;
	front = m_chpBuffer;
	rear = m_chpBuffer;
}

CPacket::~CPacket()
{
	free(m_chpBuffer);
}

int CPacket::MoveWritePos(int iSize)
{
	int moveSize = 0;

	for (int i = 0; i < iSize; ++i)
	{
		++front;
		++moveSize;
	}

	return moveSize;
}

int CPacket::MoveReadPos(int iSize)
{
	int moveSize = 0;

	for (int i = 0; i < iSize; ++i)
	{
		++rear;
		++moveSize;
	}

	return moveSize;
}

/* 데이터 가져오기 */
int CPacket::GetData(char* chpDest, int iSize)
{
	int retValue = 0;

	/* GetData를 많이 할텐데 rear 쓰면 다시 GetData할때 cd cd 복사한다 임시 포인터를 사용하자*/
	char* m_rear = rear;

	for (int i = 0; i < iSize; ++i)
	{
		*chpDest = *m_rear;

		/* 값 증가 */
		++m_rear;
		++chpDest;
		++retValue;
	}

	return retValue;
}

/* 데이터 넣기*/
int CPacket::PutData(char* chpSrc, int iSrcSize)
{
	int retValue = 0;
	{
		/* 84 순회
	char* buffer = m_chpBuffer + m_iDataSize;
	for (int i = 0; i < iSrcSize; ++i)
	{
		*buffer = *chpSrc;
		++buffer;
		++chpSrc;
		++retValue;
		++m_iDataSize;
	}
	*/
	}
	/* 89회 순회 */
	for (int i = 0; i < iSrcSize; ++i)
	{
		*front = *chpSrc;
		++front;
		++chpSrc;
		++retValue;
		++m_iDataSize;
	}

	return retValue;
}


void CPacket::Clear()
{
	front = m_chpBuffer;
	rear = m_chpBuffer;
	m_iDataSize = 0;
}

/* operator = */
/* 얕은 복사 하지 말 것*/
CPacket& CPacket::operator=(CPacket& clSrcPacket)
{
	return *this;
}

/* operator << */
CPacket& CPacket::operator<<(unsigned char byValue)
{
	unsigned char* BufferPtr = (unsigned char*)m_chpBuffer + m_iDataSize;
	*BufferPtr = byValue;
	m_iDataSize += sizeof(char);

	return *this;
}

CPacket& CPacket::operator<<(char chValue)
{
	char* BufferPtr = m_chpBuffer + m_iDataSize;
	*BufferPtr = chValue;
	m_iDataSize += sizeof(char);

	return *this;
}

CPacket& CPacket::operator<<(short shValue)
{
	char* BufferPtr = m_chpBuffer + m_iDataSize;
	short* BufferPtr2 = (short*)BufferPtr;

	*BufferPtr2 = shValue;
	m_iDataSize += sizeof(short);

	return *this;
}

CPacket& CPacket::operator<<(unsigned short wValue)
{
	char* BufferPtr = m_chpBuffer + m_iDataSize;
	unsigned short* BufferPtr2 = (unsigned short*)BufferPtr;

	*BufferPtr2 = wValue;
	m_iDataSize += sizeof(short);

	return *this;
}

CPacket& CPacket::operator<<(int iValue)
{
	char* BufferPtr = m_chpBuffer + m_iDataSize;
	int* BufferPtr2 = (int*)BufferPtr;

	*BufferPtr2 = iValue;
	m_iDataSize += sizeof(int);

	return *this;
}

CPacket& CPacket::operator<<(unsigned int iValue)
{
	char* BufferPtr = m_chpBuffer + m_iDataSize;
	unsigned int* BufferPtr2 = (unsigned int*)BufferPtr;

	*BufferPtr2 = iValue;
	m_iDataSize += sizeof(int);

	return *this;
}

CPacket& CPacket::operator<<(long lValue)
{
	char* BufferPtr = m_chpBuffer + m_iDataSize;
	long* BufferPtr2 = (long*)BufferPtr;

	*BufferPtr2 = lValue;
	m_iDataSize += sizeof(long);

	return *this;
}

CPacket& CPacket::operator<<(DWORD lValue)
{
	char* BufferPtr = m_chpBuffer + m_iDataSize;
	DWORD* BufferPtr2 = (DWORD*)BufferPtr;

	*BufferPtr2 = lValue;
	m_iDataSize += sizeof(DWORD);

	return *this;
}

CPacket& CPacket::operator<<(float fValue)
{
	char* BufferPtr = m_chpBuffer + m_iDataSize;
	float* BufferPtr2 = (float*)BufferPtr;

	*BufferPtr2 = fValue;
	m_iDataSize += sizeof(float);

	return *this;
}

CPacket& CPacket::operator<<(__int64 iValue)
{
	char* BufferPtr = m_chpBuffer + m_iDataSize;
	__int64* BufferPtr2 = (__int64*)BufferPtr;

	*BufferPtr2 = iValue;
	m_iDataSize += sizeof(__int64);

	return *this;
}

CPacket& CPacket::operator<<(double dValue)
{
	char* BufferPtr = m_chpBuffer + m_iDataSize;
	double* BufferPtr2 = (double*)BufferPtr;

	*BufferPtr2 = dValue;
	m_iDataSize += sizeof(double);

	return *this;
}

/* operator >> */
CPacket& CPacket::operator>>(BYTE& byValue)
{
	BYTE* BufferPtr = (BYTE*)rear;

	byValue = *BufferPtr;
	rear += 1;

	return *this;
}

CPacket& CPacket::operator>>(char& chValue)
{
	char* BufferPtr = (char*)rear;

	chValue = *BufferPtr;
	rear += 1;

	return *this;
}

CPacket& CPacket::operator>>(short& shValue)
{
	short* BufferPtr = (short*)rear;

	shValue = *BufferPtr;
	rear += 2;

	return *this;
}

CPacket& CPacket::operator>>(WORD& wValue)
{
	WORD* BufferPtr = (WORD*)rear;

	wValue = *BufferPtr;
	rear += 2;

	return *this;
}

CPacket& CPacket::operator>>(int& iValue)
{
	int* BufferPtr = (int*)rear;

	iValue = *BufferPtr;
	rear += 4;

	return *this;
}

CPacket& CPacket::operator>>(DWORD& dwValue)
{
	DWORD* BufferPtr = (DWORD*)rear;

	dwValue = *BufferPtr;
	rear += 4;

	return *this;
}

CPacket& CPacket::operator>>(float& fValue)
{
	float* BufferPtr = (float*)rear;

	fValue = *BufferPtr;
	rear += sizeof(float);

	return *this;
}

CPacket& CPacket::operator>>(__int64& iValue)
{
	__int64* BufferPtr = (__int64*)rear;

	iValue = *BufferPtr;
	rear += 8;

	return *this;
}

CPacket& CPacket::operator>>(double& dValue)
{
	double* BufferPtr = (double*)rear;

	dValue = *BufferPtr;
	rear += 8;

	return *this;
}

