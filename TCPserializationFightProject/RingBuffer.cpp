#include "RingBuffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

//static int front_counting = 1;
//static int rear_counting = 1;

/*
크기 얻기, 인큐, 디큐, peek까지

-틀어지면 해당 시점을 봐야함 if문으로 문자열 제대로빠지는지 확인
-디큐인큐 횟수카운터를 해두고 해도됨
-100byte 내외로 테스트할 것
-링버퍼의 크키도 다양하게 해서 테스트할 것

*/

/*roop 돌고나서 Front == RearPos처리*/

CRingBuffer::CRingBuffer()
{
	Data = (char*)malloc(DEFAULFBUFFER);
	Front = Data + 1;
	RearPos = Data;
}

CRingBuffer::CRingBuffer(int iBufferSize)
{
	Data = (char*)malloc(iBufferSize);
	Front = Data + 1;
	RearPos = Data;
}

CRingBuffer::~CRingBuffer()
{
	free(Data);
}

void CRingBuffer::Resize(int size)
{

}

int CRingBuffer::GetBufferSize()
{
	/* 실제 사용 데이터는 -1 rear값은 못씀*/
	int heapSize = _msize(Data) - 1;
	return heapSize;
}

int CRingBuffer::Enqueue(const char* chpData, int iSize)
{
	/*빈 공간 보다 큰 수를 요구하면 ..*/
	int FreeSize = GetFreeSize();
	if (FreeSize <= iSize)
	{
		iSize = FreeSize;
		if (iSize == 0)
			return 0;
	}

	/*front가 roop 돌고 0번째 값 안넣었을 경우 Rear와 겹치게됨.
		사실 RearPos는 -1을 가르키고 있어야 한다. 그러나 그럴 순 없으니 예외처리*/
	if (Front == RearPos)
	{
		if (Front == Data)
		{

		}
		else
		{
			return 0;
		}
	}
	/* buffer 크기를 넘는지 확인 변수 */


	/* 받은 데이터 const 분리 */
	char* inputData = (char*)chpData;

	/* 실제로 넣은 크기, 반환 값*/
	int inputSize = 0;

	// 크기 초과 시 분할
	int _firstMove = 0;
	int _secondMove = 0;
	bool _division = false;

	// 버퍼 사이즈 저장
	int _bufferSize = GetBufferSize();

	/* 버퍼 사이즈보다 큰게 들어온다면.. */
	if (iSize > _bufferSize)
		return 0;

	/* 순서 2분할 */
	if (front_counting + iSize > _bufferSize)
	{
		_firstMove = _bufferSize - front_counting + 1;
		_secondMove = iSize - _firstMove;
		_division = true;
		/* 안전장치? */
		if (_firstMove + _secondMove != iSize)
		{
			printf("분할 잘못 됨\n");
			return 0;
		}
	}

	/* rear에 걸치는지 확인*/
	char* frontCheck = Front;
	int real_range = 0;
	bool frontCheckBreak = false;

	if (!_division)
	{
		for (int i = 0; i < iSize; ++i)
		{
			if (frontCheck == RearPos)
			{
				if (frontCheck == Data)
				{

				}
				else
				{
					break;
				}
			}
			++frontCheck;

			++real_range;
		}
	}

	else
	{
		for (int i = 0; i < _firstMove; ++i)
		{
			if (frontCheck == RearPos)
			{
				/* 애초에 여기서 break가 걸리면 더 확인할 필요 없음*/
				frontCheckBreak = true;
				break;
			}
			++frontCheck;

			++real_range;
		}

		if (!frontCheckBreak)
		{
			frontCheck = Data;

			for (int i = 0; i < _secondMove; ++i)
			{
				if (frontCheck == RearPos)
				{
					break;
				}
				++frontCheck;

				++real_range;
			}
		}
	}

	/* 데이터 input */
	if (!_division)
	{
		for (int i = 0; i < real_range; ++i)
		{
			*Front = *inputData;
			++Front;
			++inputData;

			++inputSize;
			++front_counting;
		}
	}
	else
	{
		/* _firstMove, _secondMove 재설정 */
		// real 크기가 더 크면 first는 그대로 가고 second까지 확인하고 그래도 real이 더크면 그대로 간다
		bool noInit = true;
		if (_firstMove <= real_range)
		{
			if (_secondMove + _firstMove <= real_range)
			{

			}
			else
			{
				_secondMove = real_range - _firstMove;
			}
		}
		else
		{
			_firstMove = real_range;
			_secondMove = 0;
			noInit = false;
		}


		for (int i = 0; i < _firstMove; ++i)
		{
			*Front = *inputData;
			++Front;
			++inputData;

			++inputSize;
			++front_counting;
		}

		if (noInit)
		{
			Front = Data;
			if (front_counting > _bufferSize)
				front_counting = 0;
		}
		for (int i = 0; i < _secondMove; ++i)
		{
			*Front = *inputData;
			++Front;
			++inputData;

			++inputSize;
			++front_counting;
		}
	}



	return inputSize;
}


int CRingBuffer::Dequeue(char* chpDest, int iSize)
{
	/*검증용 첫 포인터*/
	char* stchpDest = chpDest;


	int UseSize = GetUseSize();
	int BufferSize = GetBufferSize() + 1; // 100의 크기

	/* 빼고자 하는 공간이 안의 있는 공간 보다 클 경우*/
	if (UseSize < iSize)
	{
		iSize = UseSize;
		if (iSize == 0)
			return 0;
	}

	/* 뺄 공간 크기 + 진행위치 가 Buffer보다 크다면 분할이 필요함*/
	int part_frt = 0;
	int part_scd = 0;
	bool dvs = false;

	/* 맨 처음만 다르게 처리*/
	if (onecheck == true)
	{
		if (rear_counting + iSize > BufferSize)
		{
			part_frt = BufferSize - rear_counting;
			part_scd = iSize - part_frt;
			onecheck = false;
			dvs = true;
		}
	}
	else
	{
		if (rear_counting + iSize >= BufferSize)
		{
			part_frt = BufferSize - rear_counting;
			part_scd = iSize - part_frt;
			dvs = true;
		}
	}

	/* 실제로 output한 횟수 */
	int outpt = 0;
	/* NO 분할 데이터 처리*/
	if (!dvs)
	{
		/*roop 돌고 나서는 0번째 부터 처리 해야함.*/
		if (rear_counting == 0)
		{
			if (iSize == 0)
				return 0;
			*chpDest = *RearPos;
			*RearPos = 0xFF;
			++chpDest;

			++rear_counting;
			++outpt;

			for (int i = 0; i < iSize - 1; ++i)
			{
				++RearPos;
				*chpDest = *RearPos;
				*RearPos = 0xFF;
				++chpDest;

				++rear_counting;
				++outpt;
			}
		}
		/* 아니라면 ..*/
		else
		{
			for (int i = 0; i < iSize; ++i)
			{
				++RearPos;
				*chpDest = *RearPos;
				*RearPos = 0xFF;
				++chpDest;

				++rear_counting;
				++outpt;
			}
		}
	}
	/* YES 분할 데이터 처리*/
	else
	{
		/* 분할 앞 부분 처리 ..*/
		for (int i = 0; i < part_frt; ++i)
		{
			++RearPos;
			*chpDest = *RearPos;
			*RearPos = 0xFF;
			++chpDest;

			++rear_counting;
			++outpt;
		}

		/* 분할이 되었다는 것은 초기로 돌아간다는 것*/
		rear_counting = 0;
		RearPos = Data;

		/* 초기는 ++하고 값을 빼면 안됨 */

		if (part_scd != 0)
		{
			*chpDest = *RearPos;
			*RearPos = 0xFF;
			++chpDest;

			++rear_counting;
			++outpt;

			/* 분할 뒷 부분 처리 ..*/
			part_scd = part_scd - 1;
			for (int i = 0; i < part_scd; ++i)
			{
				++RearPos;
				*chpDest = *RearPos;
				*RearPos = 0xFF;
				++chpDest;

				++rear_counting;
				++outpt;
			}
		}
	}

	return outpt;
}

int CRingBuffer::GetUseSize()
{
	if (front_counting > rear_counting)
	{

		return front_counting - rear_counting;
	}
	else if (front_counting < rear_counting)
	{
		int buffersize = GetBufferSize() + 1;
		int partition = buffersize - rear_counting;
		return front_counting + partition;
	}
	else
	{
		return 0;
	}

}

int CRingBuffer::GetFreeSize()
{
	int buffersize = GetBufferSize(); // 여기 1 안붙히는 이유는 usesize에서 1 붙혀서 계산했으므로
	int useSize = GetUseSize();

	return buffersize - useSize;
}

int CRingBuffer::Peek(char* chpDest, int iSize)
{
	/* 훔쳐보기, ReadPos에서 데이터 읽어옴*/
	int UseSize = GetUseSize();
	int BufferSize = GetBufferSize() + 1; // 100의 크기

	/* 빼고자 하는 공간이 안의 있는 공간 보다 클 경우*/
	if (UseSize < iSize)
	{
		iSize = UseSize;
		if (iSize == 0)
			return 0;
	}

	/* 뺄 공간 크기 + 진행위치 가 Buffer보다 크다면 분할이 필요함*/
	int part_frt = 0;
	int part_scd = 0;
	bool dvs = false;

	/* 맨 처음만 다르게 처리*/
	if (onecheck == true)
	{
		if (rear_counting + iSize > BufferSize)
		{
			part_frt = BufferSize - rear_counting;
			part_scd = iSize - part_frt;
			onecheck = false;
			dvs = true;
		}
	}
	else
	{
		if (rear_counting + iSize >= BufferSize)
		{
			part_frt = BufferSize - rear_counting;
			part_scd = iSize - part_frt;
			dvs = true;
		}
	}

	/* 실제로 output한 횟수 */
	int outpt = 0;

	/* 가짜 RearPos, rear_counting */
	char* fake_RearPos = RearPos;
	int fake_rear_counting = rear_counting;

	/* NO 분할 데이터 처리*/
	if (!dvs)
	{
		/*roop 돌고 나서는 0번째 부터 처리 해야함.*/
		if (fake_rear_counting == 0)
		{
			if (iSize == 0)
				return 0;
			*chpDest = *fake_RearPos;
			++chpDest;

			++fake_rear_counting;
			++outpt;

			for (int i = 0; i < iSize - 1; ++i)
			{
				++fake_RearPos;
				*chpDest = *fake_RearPos;
				++chpDest;

				++fake_rear_counting;
				++outpt;
			}
		}
		/* 아니라면 ..*/
		else
		{
			for (int i = 0; i < iSize; ++i)
			{
				++fake_RearPos;
				*chpDest = *fake_RearPos;
				++chpDest;

				++fake_rear_counting;
				++outpt;
			}
		}
	}
	/* YES 분할 데이터 처리*/
	else
	{
		/* 분할 앞 부분 처리 ..*/
		for (int i = 0; i < part_frt; ++i)
		{
			++fake_RearPos;
			*chpDest = *fake_RearPos;
			++chpDest;

			++fake_rear_counting;
			++outpt;
		}

		/* 분할이 되었다는 것은 초기로 돌아간다는 것*/
		fake_rear_counting = 0;
		fake_RearPos = Data;

		/* 초기는 ++하고 값을 빼면 안됨 */

		if (part_scd != 0)
		{
			*chpDest = *fake_RearPos;
			++chpDest;

			++fake_rear_counting;
			++outpt;

			/* 분할 뒷 부분 처리 ..*/
			part_scd = part_scd - 1;
			for (int i = 0; i < part_scd; ++i)
			{
				++fake_RearPos;
				*chpDest = *fake_RearPos;
				++chpDest;

				++fake_rear_counting;
				++outpt;
			}
		}
	}
	return outpt;
}

int CRingBuffer::DirectEnqueueSize()
{
	return 0;
}

int CRingBuffer::DirectDequeueSize()
{
	/* 마지막까지 도달하는게 몇 포인트 남았는지 단순히 계산만 */
	int BufferSize = GetBufferSize() + 1;
	int fake_rear = rear_counting;
	int counting = 0;

	while (fake_rear < BufferSize)
	{
		++fake_rear;
		++counting;
	}

	return counting;
}

int CRingBuffer::MoveRear(int iSize)
{
	int UseSize = GetUseSize();
	int BufferSize = GetBufferSize() + 1; // 100의 크기

	/* 빼고자 하는 공간이 안의 있는 공간 보다 클 경우*/
	if (UseSize < iSize)
	{
		iSize = UseSize;
		if (iSize == 0)
			return 0;
	}

	/* 뺄 공간 크기 + 진행위치 가 Buffer보다 크다면 분할이 필요함*/
	int part_frt = 0;
	int part_scd = 0;
	bool dvs = false;

	/* 맨 처음만 다르게 처리*/
	if (onecheck == true)
	{
		if (rear_counting + iSize > BufferSize)
		{
			part_frt = BufferSize - rear_counting;
			part_scd = iSize - part_frt;
			onecheck = false;
			dvs = true;
		}
	}
	else
	{
		if (rear_counting + iSize >= BufferSize)
		{
			part_frt = BufferSize - rear_counting;
			part_scd = iSize - part_frt;
			dvs = true;
		}
	}

	/* 실제로 output한 횟수 */
	int outpt = 0;
	/* NO 분할 데이터 처리*/
	if (!dvs)
	{
		/*roop 돌고 나서는 0번째 부터 처리 해야함.*/
		if (rear_counting == 0)
		{
			if (iSize == 0)
				return 0;

			*RearPos = 0xFF;


			++rear_counting;
			++outpt;

			for (int i = 0; i < iSize - 1; ++i)
			{
				++RearPos;

				*RearPos = 0xFF;


				++rear_counting;
				++outpt;
			}
		}
		/* 아니라면 ..*/
		else
		{
			for (int i = 0; i < iSize; ++i)
			{
				++RearPos;

				*RearPos = 0xFF;


				++rear_counting;
				++outpt;
			}
		}
	}
	/* YES 분할 데이터 처리*/
	else
	{
		/* 분할 앞 부분 처리 ..*/
		for (int i = 0; i < part_frt; ++i)
		{
			++RearPos;

			*RearPos = 0xFF;


			++rear_counting;
			++outpt;
		}

		/* 분할이 되었다는 것은 초기로 돌아간다는 것*/
		rear_counting = 0;
		RearPos = Data;

		/* 초기는 ++하고 값을 빼면 안됨 */

		if (part_scd != 0)
		{

			*RearPos = 0xFF;


			++rear_counting;
			++outpt;

			/* 분할 뒷 부분 처리 ..*/
			part_scd = part_scd - 1;
			for (int i = 0; i < part_scd; ++i)
			{
				++RearPos;

				*RearPos = 0xFF;


				++rear_counting;
				++outpt;
			}
		}
	}

	return outpt;
}