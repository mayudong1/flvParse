#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
using namespace std;

#define FLV_MIN(a, b) (((a)<(b))?(a):(b))
#define FLV_MAX(a, b) (((a)>(b))?(a):(b))

namespace FLVUtils
{
int DecodeAMFString(unsigned char* pData, string& ret);
}


#endif
