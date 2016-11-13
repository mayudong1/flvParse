#ifndef __UTIL_H__
#define __UTIL_H__

namespace Util
{

#define MYD_MIN(a, b) (((a)<(b))?(a):(b))
#define MYD_MAX(a, b) (((a)>(b))?(a):(b))

inline unsigned int convert32(unsigned char* pData)
{
	return (pData[0] << 24) | (pData[1] << 16) | (pData[2] << 8) | pData[3];
}

inline unsigned int convert24(unsigned char* pData)
{
	return (pData[0] << 16) | (pData[1] << 8) | pData[2];
}


bool ReadByte(char &value, unsigned char* pData, int dataLen, int &curIndex);
bool ReadUint32(unsigned int &value, unsigned char* pData, int dataLen, int &curIndex);
bool ReadUint24(unsigned int &value, unsigned char* pData, int dataLen, int &curIndex);
bool Seek(int len, unsigned char* pData, int dataLen, int &curIndex);
}
#endif