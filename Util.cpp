#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Util.h"

namespace FLVUtils
{
int DecodeAMFString(unsigned char* pData, string& ret)
{
    int len = (pData[0]<<8) + pData[1];
    char* str = new char[len+1];
    memset(str, 0, len+1);
    memcpy(str, pData+2, len);
    ret = str;
    delete str;
    return len+2;
}
}
