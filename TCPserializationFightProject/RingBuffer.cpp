#include "RingBuffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

//static int front_counting = 1;
//static int rear_counting = 1;

/*
ũ�� ���, ��ť, ��ť, peek����

-Ʋ������ �ش� ������ ������ if������ ���ڿ� ����κ������� Ȯ��
-��ť��ť Ƚ��ī���͸� �صΰ� �ص���
-100byte ���ܷ� �׽�Ʈ�� ��
-�������� ũŰ�� �پ��ϰ� �ؼ� �׽�Ʈ�� ��

*/

/*roop ������ Front == RearPosó��*/

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
	/* ���� ��� �����ʹ� -1 rear���� ����*/
	int heapSize = _msize(Data) - 1;
	return heapSize;
}

int CRingBuffer::Enqueue(const char* chpData, int iSize)
{
	/*�� ���� ���� ū ���� �䱸�ϸ� ..*/
	int FreeSize = GetFreeSize();
	if (FreeSize <= iSize)
	{
		iSize = FreeSize;
		if (iSize == 0)
			return 0;
	}

	/*front�� roop ���� 0��° �� �ȳ־��� ��� Rear�� ��ġ�Ե�.
		��� RearPos�� -1�� ����Ű�� �־�� �Ѵ�. �׷��� �׷� �� ������ ����ó��*/
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
	/* buffer ũ�⸦ �Ѵ��� Ȯ�� ���� */


	/* ���� ������ const �и� */
	char* inputData = (char*)chpData;

	/* ������ ���� ũ��, ��ȯ ��*/
	int inputSize = 0;

	// ũ�� �ʰ� �� ����
	int _firstMove = 0;
	int _secondMove = 0;
	bool _division = false;

	// ���� ������ ����
	int _bufferSize = GetBufferSize();

	/* ���� ������� ū�� ���´ٸ�.. */
	if (iSize > _bufferSize)
		return 0;

	/* ���� 2���� */
	if (front_counting + iSize > _bufferSize)
	{
		_firstMove = _bufferSize - front_counting + 1;
		_secondMove = iSize - _firstMove;
		_division = true;
		/* ������ġ? */
		if (_firstMove + _secondMove != iSize)
		{
			printf("���� �߸� ��\n");
			return 0;
		}
	}

	/* rear�� ��ġ���� Ȯ��*/
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
				/* ���ʿ� ���⼭ break�� �ɸ��� �� Ȯ���� �ʿ� ����*/
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

	/* ������ input */
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
		/* _firstMove, _secondMove �缳�� */
		// real ũ�Ⱑ �� ũ�� first�� �״�� ���� second���� Ȯ���ϰ� �׷��� real�� ��ũ�� �״�� ����
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
	/*������ ù ������*/
	char* stchpDest = chpDest;


	int UseSize = GetUseSize();
	int BufferSize = GetBufferSize() + 1; // 100�� ũ��

	/* ������ �ϴ� ������ ���� �ִ� ���� ���� Ŭ ���*/
	if (UseSize < iSize)
	{
		iSize = UseSize;
		if (iSize == 0)
			return 0;
	}

	/* �� ���� ũ�� + ������ġ �� Buffer���� ũ�ٸ� ������ �ʿ���*/
	int part_frt = 0;
	int part_scd = 0;
	bool dvs = false;

	/* �� ó���� �ٸ��� ó��*/
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

	/* ������ output�� Ƚ�� */
	int outpt = 0;
	/* NO ���� ������ ó��*/
	if (!dvs)
	{
		/*roop ���� ������ 0��° ���� ó�� �ؾ���.*/
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
		/* �ƴ϶�� ..*/
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
	/* YES ���� ������ ó��*/
	else
	{
		/* ���� �� �κ� ó�� ..*/
		for (int i = 0; i < part_frt; ++i)
		{
			++RearPos;
			*chpDest = *RearPos;
			*RearPos = 0xFF;
			++chpDest;

			++rear_counting;
			++outpt;
		}

		/* ������ �Ǿ��ٴ� ���� �ʱ�� ���ư��ٴ� ��*/
		rear_counting = 0;
		RearPos = Data;

		/* �ʱ�� ++�ϰ� ���� ���� �ȵ� */

		if (part_scd != 0)
		{
			*chpDest = *RearPos;
			*RearPos = 0xFF;
			++chpDest;

			++rear_counting;
			++outpt;

			/* ���� �� �κ� ó�� ..*/
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
	int buffersize = GetBufferSize(); // ���� 1 �Ⱥ����� ������ usesize���� 1 ������ ��������Ƿ�
	int useSize = GetUseSize();

	return buffersize - useSize;
}

int CRingBuffer::Peek(char* chpDest, int iSize)
{
	/* ���ĺ���, ReadPos���� ������ �о��*/
	int UseSize = GetUseSize();
	int BufferSize = GetBufferSize() + 1; // 100�� ũ��

	/* ������ �ϴ� ������ ���� �ִ� ���� ���� Ŭ ���*/
	if (UseSize < iSize)
	{
		iSize = UseSize;
		if (iSize == 0)
			return 0;
	}

	/* �� ���� ũ�� + ������ġ �� Buffer���� ũ�ٸ� ������ �ʿ���*/
	int part_frt = 0;
	int part_scd = 0;
	bool dvs = false;

	/* �� ó���� �ٸ��� ó��*/
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

	/* ������ output�� Ƚ�� */
	int outpt = 0;

	/* ��¥ RearPos, rear_counting */
	char* fake_RearPos = RearPos;
	int fake_rear_counting = rear_counting;

	/* NO ���� ������ ó��*/
	if (!dvs)
	{
		/*roop ���� ������ 0��° ���� ó�� �ؾ���.*/
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
		/* �ƴ϶�� ..*/
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
	/* YES ���� ������ ó��*/
	else
	{
		/* ���� �� �κ� ó�� ..*/
		for (int i = 0; i < part_frt; ++i)
		{
			++fake_RearPos;
			*chpDest = *fake_RearPos;
			++chpDest;

			++fake_rear_counting;
			++outpt;
		}

		/* ������ �Ǿ��ٴ� ���� �ʱ�� ���ư��ٴ� ��*/
		fake_rear_counting = 0;
		fake_RearPos = Data;

		/* �ʱ�� ++�ϰ� ���� ���� �ȵ� */

		if (part_scd != 0)
		{
			*chpDest = *fake_RearPos;
			++chpDest;

			++fake_rear_counting;
			++outpt;

			/* ���� �� �κ� ó�� ..*/
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
	/* ���������� �����ϴ°� �� ����Ʈ ���Ҵ��� �ܼ��� ��길 */
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
	int BufferSize = GetBufferSize() + 1; // 100�� ũ��

	/* ������ �ϴ� ������ ���� �ִ� ���� ���� Ŭ ���*/
	if (UseSize < iSize)
	{
		iSize = UseSize;
		if (iSize == 0)
			return 0;
	}

	/* �� ���� ũ�� + ������ġ �� Buffer���� ũ�ٸ� ������ �ʿ���*/
	int part_frt = 0;
	int part_scd = 0;
	bool dvs = false;

	/* �� ó���� �ٸ��� ó��*/
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

	/* ������ output�� Ƚ�� */
	int outpt = 0;
	/* NO ���� ������ ó��*/
	if (!dvs)
	{
		/*roop ���� ������ 0��° ���� ó�� �ؾ���.*/
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
		/* �ƴ϶�� ..*/
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
	/* YES ���� ������ ó��*/
	else
	{
		/* ���� �� �κ� ó�� ..*/
		for (int i = 0; i < part_frt; ++i)
		{
			++RearPos;

			*RearPos = 0xFF;


			++rear_counting;
			++outpt;
		}

		/* ������ �Ǿ��ٴ� ���� �ʱ�� ���ư��ٴ� ��*/
		rear_counting = 0;
		RearPos = Data;

		/* �ʱ�� ++�ϰ� ���� ���� �ȵ� */

		if (part_scd != 0)
		{

			*RearPos = 0xFF;


			++rear_counting;
			++outpt;

			/* ���� �� �κ� ó�� ..*/
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