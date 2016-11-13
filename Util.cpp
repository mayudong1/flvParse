#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Util.h"

bool ReadByte(char &value, unsigned char* pData, int dataLen, int &curIndex)
{
	assert(pData && dataLen>0 && curIndex>=0);

	if (curIndex + 1 > dataLen)
		return false;

	value = pData[curIndex++];

	return true;
}

bool ReadUint32(unsigned int &value, unsigned char* pData, int dataLen, int &curIndex)
{
	assert(pData && dataLen>0 && curIndex >= 0);

	if (curIndex + 4 > dataLen)
		return false;

	unsigned char* pTmp = pData + curIndex;
	value = convert32(pTmp);
	curIndex += 4;
	return true;
}
bool ReadUint24(unsigned int &value, unsigned char* pData, int dataLen, int &curIndex)
{
	assert(pData && dataLen>0 && curIndex >= 0);

	if (curIndex + 3 > dataLen)
		return false;

	unsigned char* pTmp = pData + curIndex;
	value = convert24(pTmp);
	curIndex += 3;
	return true;
}

bool Seek(int len, unsigned char* pData, int dataLen, int &curIndex)
{
	assert(pData && dataLen>0 && curIndex >= 0);

	if (curIndex + len > dataLen)
		return false;
	curIndex += len;
	return true;
}