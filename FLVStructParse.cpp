#include <assert.h>
#include <string.h>
#include "FLVStructParse.h"
#include "Util.h"
#include "amf.h"

FLVStructParse::FLVStructParse()
{
	flv = NULL;
	curIndex = 0;
}


FLVStructParse::~FLVStructParse()
{
	if (flv)
	{
		delete flv;
	}
}

bool FLVStructParse::ReadByte(char &value, FLVPosition& retPos)
{
    if(curIndex > flv->dataLen)
        return false;

	retPos.start = curIndex;
    retPos.len = 1;
    unsigned char* tmp = flv->data+curIndex;
    value = *tmp;
    curIndex++;
    return true;
}
bool FLVStructParse::ReadUint32(unsigned int &value, FLVPosition& retPos)
{
    if(curIndex+4 > flv->dataLen)
        return false;

    retPos.start = curIndex;
    retPos.len = 4;
    unsigned char* tmp = flv->data+curIndex;
    value = (tmp[0]<<24) + (tmp[1]<<16) + (tmp[2]<<8) + tmp[3];
    curIndex += 4;
    return true;
}
bool FLVStructParse::ReadUint24(unsigned int &value, FLVPosition& retPos)
{
    if(curIndex+3 > flv->dataLen)
        return false;

    retPos.start = curIndex;
    retPos.len = 3;
    unsigned char* tmp = flv->data+curIndex;
    value = (tmp[0]<<16) + (tmp[1]<<8) + tmp[2];
    curIndex += 3;
    return true;
}
bool FLVStructParse::ReadUint16(unsigned int &value, FLVPosition& retPos)
{
    if(curIndex+2 > flv->dataLen)
        return false;

    retPos.start = curIndex;
    retPos.len = 2;
    unsigned char* tmp = flv->data+curIndex;
    value = (tmp[0]<<8) + tmp[1];
    curIndex += 2;
    return true;
}
bool FLVStructParse::Seek(int len, FLVPosition& retPos)
{
    if(curIndex + len > flv->dataLen)
        return false;
	retPos.start = curIndex;	
    retPos.len = len;
    curIndex += len;
    return true;
}

int FLVStructParse::parseFlvHeader()
{
	flv->header.pos.start = curIndex;

	FLVPosition pos;	
	ReadByte(flv->header.signature.value[0], pos);
	ReadByte(flv->header.signature.value[1], pos);
	ReadByte(flv->header.signature.value[2], pos);
	if (strcmp(flv->header.signature.value, "FLV") != 0)
	{
		return -1;
	}	
	flv->header.signature.pos = pos;
		
	if (!ReadByte(flv->header.version.value, pos))
	{
		return -1;
	}	
	flv->header.version.pos = pos;

	char flag = 0;	
	if (!ReadByte(flag, pos))
	{
		return -1;
	}		
	flv->header.hasAudio.value = flag & 0x04;
	flv->header.hasAudio.pos = pos;
	flv->header.hasVideo.value = flag & 0x01;
	flv->header.hasVideo.pos = pos;

	if (!ReadUint32(flv->header.headerLen.value, pos))
	{
		return -1;
	}	
	flv->header.headerLen.pos = pos;

	flv->header.pos.len = curIndex - flv->header.pos.start;
	return 0;
}

int FLVStructParse::parseMetadata(FLVTagBody* meta)
{
    ReadByte(meta->amf0_type.value, meta->amf0_type.pos);
    ReadUint16(meta->amf0_len.value, meta->amf0_len.pos);
    meta->amf0_data.value = new char[meta->amf0_len.value+1];
    memset(meta->amf0_data.value, 0, meta->amf0_len.value+1);
    memcpy(meta->amf0_data.value, flv->data+curIndex, meta->amf0_len.value);
    Seek(meta->amf0_len.value, meta->amf0_data.pos);

    ReadByte(meta->amf1_type.value, meta->amf1_type.pos);
    if(meta->amf1_type.value == AMF_ECMA_ARRAY)
    {
        ReadUint32(meta->amf1_count.value, meta->amf1_count.pos);
        meta->values = new KeyValue[meta->amf1_count.value];
        for(int i=0;i<meta->amf1_count.value;i++)
        {
            FLVPosition pos;
            unsigned int keyLen = 0;
            ReadUint16(keyLen, pos);
            meta->values[i].key = new char[keyLen+1];
            memset(meta->values[i].key, 0, keyLen+1);
            memcpy(meta->values[i].key, flv->data+curIndex, keyLen);
            Seek(keyLen, pos);

            char valueType = 0;
            ReadByte(valueType, pos);
            if(valueType == AMF_NUMBER)
            {
//                meta->values->value = AMF_DecodeNumber((const char*)flv->data+curIndex);
                curIndex += 8;
            }
            else if(valueType == AMF_STRING)
            {
                unsigned int len = 0;
                ReadUint16(len, pos);
                Seek(len, pos);
            }
            else if(valueType == AMF_BOOLEAN)
            {
                curIndex += 1;
            }
        }
    }

    return 0;
}

int FLVStructParse::parseFlvTags()
{
	flv->tagList = new FLVTag;
	FLVTag* p = flv->tagList;
	while (1)
	{
		FLVTag* tag = new FLVTag;
		tag->pos.start = curIndex;		
		tag->header.pos.start = curIndex;
		FLVPosition pos;
		char type = 0;
		if (!ReadByte(type, pos))
		{
			break;
		}
		tag->header.encrypted.value = (type >> 5) & 0x01;
		tag->header.encrypted.pos = pos;
		tag->header.type.value = type & 0x1f;
		tag->header.type.pos = pos;

		if (!ReadUint24(tag->header.dataSize.value, pos))
		{
			break;
		}
		tag->header.dataSize.pos = pos;

		unsigned int timestamp = 0;
		if (!ReadUint24(timestamp, pos))
		{
			break;
		}
		char timestampEx = 0;
		if (!ReadByte(timestampEx, pos))
		{
			break;
		}
		tag->header.timestamp.value = timestampEx << 24 | timestamp;
		pos.start -= 3;
		pos.len += 3;
		tag->header.timestamp.pos = pos;
		
		if (!ReadUint24(tag->header.streamId.value, pos))
		{
			break;
		}
		tag->header.streamId.pos = pos;
		tag->header.pos.len = curIndex - tag->header.pos.start;

        if(tag->header.type.value == 0x12)
        {
            int tmp = curIndex;
            parseMetadata(&tag->data);
            curIndex = tmp;
        }
		Seek(tag->header.dataSize.value, pos);
		tag->data.pos = pos;
		tag->pos.len = curIndex - tag->pos.start;

		if (!ReadUint32(tag->preTagSize.value, pos))
		{
			break;
		}
		tag->preTagSize.pos = pos;		
		p->next = tag;
		p = p->next;

		if (curIndex >= flv->dataLen)
		{
			break;
		}
	}
	return 0;
}

int FLVStructParse::LoadFile(const char* fileName)
{
	if (flv == NULL)
		return -1;

	FILE* pFile = fopen(fileName, "rb");
	if (pFile == NULL)
		return -1;

	fseek(pFile, 0, SEEK_END);
	flv->dataLen = ftell(pFile);
	if (flv->dataLen < 0)
	{
		fclose(pFile);
		return -1;
	}
		
	fseek(pFile, 0, SEEK_SET);
	if (flv->data != NULL)
		delete[] flv->data;
	flv->data = new unsigned char[flv->dataLen];
	fread(flv->data, flv->dataLen, 1, pFile);
	fclose(pFile);

	return 0;
}

FLVStruct* FLVStructParse::parseFile(const char* fileName)
{
	if (flv != NULL)
	{
		delete flv;		
	}

	flv = new FLVStruct();
	curIndex = 0;

	if(LoadFile(fileName) != 0)	
		return NULL;

	if (parseFlvHeader() != 0)
		return NULL;	

	FLVPosition pos;
	if (!ReadUint32(flv->firstTagSize.value, pos))
		return NULL;
	flv->firstTagSize.pos = pos;

	if (parseFlvTags() != 0)
		return NULL;
	
	return flv;
}
