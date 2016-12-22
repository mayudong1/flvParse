#include "amf.h"
#include "stdlib.h"
#include "memory.h"

unsigned short AMF_DecodeInt16(const char *data)
{
    unsigned char *c = (unsigned char *) data;
    unsigned short val;
    val = (c[0] << 8) | c[1];
    return val;
}

unsigned int
AMF_DecodeInt24(const char *data)
{
    unsigned char *c = (unsigned char *) data;
    unsigned int val;
    val = (c[0] << 16) | (c[1] << 8) | c[2];
    return val;
}

unsigned int
AMF_DecodeInt32(const char *data)
{
    unsigned char *c = (unsigned char *)data;
    unsigned int val;
    val = (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
    return val;
}

void
AMF_DecodeString(const char *data, AVal *bv)
{
    bv->av_len = AMF_DecodeInt16(data);
    bv->av_val = (bv->av_len > 0) ? (char *)data + 2 : NULL;
}

void
AMF_DecodeLongString(const char *data, AVal *bv)
{
    bv->av_len = AMF_DecodeInt32(data);
    bv->av_val = (bv->av_len > 0) ? (char *)data + 4 : NULL;
}

double
AMF_DecodeNumber(const char *data)
{
    double dVal;
    unsigned char *ci, *co;
    ci = (unsigned char *)data;
    co = (unsigned char *)&dVal;
    co[0] = ci[7];
    co[1] = ci[6];
    co[2] = ci[5];
    co[3] = ci[4];
    co[4] = ci[3];
    co[5] = ci[2];
    co[6] = ci[1];
    co[7] = ci[0];
    return dVal;
}

bool
AMF_DecodeBoolean(const char *data)
{
    return *data != 0;
}
