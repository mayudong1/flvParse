#ifndef __FLVSTRUCTPARSE_H__
#define __FLVSTRUCTPARSE_H__

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string>
#include "amf.h"
using namespace std;

typedef struct FLVPosition
{
	int start;
	int len;
	FLVPosition()
	{
		start = 0;
		len = 0;
	}
}FLVPosition;

template <typename T>
class FLVObject
{
public:
	T value;
	FLVPosition pos;
};

class BaseStruct
{
public:
	FLVPosition pos;
};

class FLVHeader : public BaseStruct
{
public:	
	FLVObject<char[4]> signature;
	FLVObject<char> version;
	FLVObject<char> hasVideo;
	FLVObject<char> hasAudio;
	FLVObject<unsigned int> headerLen;

	FLVHeader()
	{
		memset(signature.value, 0, 4);
		version.value = 0;
		hasAudio.value = false;
		hasVideo.value = false;
		headerLen.value = 9;
	}
};

class FLVTagHeader : public BaseStruct
{
public:
	FLVObject<bool> encrypted;
	FLVObject<int> type;
	FLVObject<unsigned int> dataSize;
	FLVObject<unsigned int> timestamp;
	FLVObject<unsigned int> streamId;
	FLVTagHeader()
	{
		encrypted.value = false;
		type.value = 0;
		dataSize.value = 0;
		timestamp.value = 0;
		streamId.value = 0;
	}
};

typedef struct MetadataInfo
{
    string key;
    int valueType;
    bool bValue;
    double dValue;
    string strValue;
    struct MetadataInfo* next;
    MetadataInfo()
    {
        next = NULL;
    }
}MetadataInfo;

class FLVTagBody : public BaseStruct
{
public:
    FLVObject<char> amf0Type;
    FLVObject<string>amf0Data;

    FLVObject<char> amf1Type;
    FLVObject<unsigned int> amf1Count;
    MetadataInfo* metaArray;
    FLVTagBody()
    {
        amf0Type.value = 0;
        amf1Type.value = 0;
        amf1Count.value = 0;
        metaArray = NULL;
    }
    ~FLVTagBody()
    {
        MetadataInfo* p = metaArray;
        while(p != NULL)
        {
            MetadataInfo* tmp = p;
            p = p->next;
            delete tmp;
        }
    }
};

class FLVTag : public BaseStruct
{
public:
	FLVTagHeader header;	
	FLVTagBody data;
	FLVObject<unsigned int> preTagSize;
    FLVTag* next;
	FLVTag()
	{	
		preTagSize.value = 0;
		next = NULL;
	}
};

class FLVStruct : public BaseStruct
{
public:
	FLVHeader header;
	FLVObject<unsigned int> firstTagSize;
	FLVTag* tagList;
    int dataLen;
	unsigned char* data;
	FLVStruct()
	{
		dataLen = 0;
		tagList = NULL;
		data = NULL;
	}
	~FLVStruct()
	{
		if (data != NULL)
		{
			delete[] data;
			data = NULL;
		}
		if (tagList != NULL)
		{
			FLVTag* tag = tagList;
			while (tag)
			{
				FLVTag* cur = tag;
				tag = tag->next;
				delete cur;
			}
		}
	}
};

class FLVStructParse
{
public:
	FLVStructParse();
	~FLVStructParse();

public:
	FLVStruct* parseFile(const char* fileName);

private:
	int LoadFile(const char* fileName);
	int parseFlvHeader();
	int parseFlvTags();
    int parseMetadata(FLVTagBody* meta);

private:
	bool ReadByte(char &value, FLVPosition& retPos);
	bool ReadUint32(unsigned int &value, FLVPosition& retPos);
	bool ReadUint24(unsigned int &value, FLVPosition& retPos);
    bool ReadUint16(unsigned int &value, FLVPosition& retPos);
	bool Seek(int len, FLVPosition& retPos);
    bool DecodeAMFString(string& ret, FLVPosition& retPos);
private:
	FLVStruct* flv;
	int curIndex;
};

#endif
