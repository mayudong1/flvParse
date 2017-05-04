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

bool FLVStructParse::DecodeAMFString(string& ret, FLVPosition& retPos)
{
    int len = FLVUtils::DecodeAMFString(flv->data+curIndex, ret);
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
    flv->header.hasAudio.value = (flag>>2) & 0x01;
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
    ReadByte(meta->amf0Type.value, meta->amf0Type.pos);

    DecodeAMFString(meta->amf0Data.value, meta->amf0Data.pos);

    ReadByte(meta->amf1Type.value, meta->amf1Type.pos);
    meta->metaArray = new MetadataInfo;
    MetadataInfo* pEnd = meta->metaArray;
    if(meta->amf1Type.value == AMF_ECMA_ARRAY)
    {        
        ReadUint32(meta->amf1Count.value, meta->amf1Count.pos);
        for(int i=0;i<meta->amf1Count.value;i++)
        {
            MetadataInfo* pMetaData = new MetadataInfo;
            FLVPosition pos;            
            DecodeAMFString(pMetaData->key, pos);

            char valueType = 0;
            ReadByte(valueType, pos);
            pMetaData->valueType = valueType;
            if(valueType == AMF_NUMBER)
            {
                pMetaData->dValue = AMF_DecodeNumber((const char*)flv->data+curIndex);
                curIndex += 8;
            }
            else if(valueType == AMF_STRING)
            {
               DecodeAMFString(pMetaData->strValue, pos);
            }
            else if(valueType == AMF_BOOLEAN)
            {
                pMetaData->bValue = AMF_DecodeBoolean((const char*)flv->data+curIndex);
                curIndex += 1;
            }
            pEnd->next = pMetaData;
            pEnd = pEnd->next;
        }
    }
    else if(meta->amf1Type.value == AMF_OBJECT)
    {
        int metaItemCount = 0;
        while(1)
        {
            MetadataInfo* pMetaData = new MetadataInfo;
            FLVPosition pos;
            DecodeAMFString(pMetaData->key, pos);

            char valueType = 0;
            ReadByte(valueType, pos);
            pMetaData->valueType = valueType;
            if(valueType == AMF_NUMBER)
            {
                pMetaData->dValue = AMF_DecodeNumber((const char*)flv->data+curIndex);
                curIndex += 8;
            }
            else if(valueType == AMF_STRING)
            {
               DecodeAMFString(pMetaData->strValue, pos);
            }
            else if(valueType == AMF_BOOLEAN)
            {
                pMetaData->bValue = AMF_DecodeBoolean((const char*)flv->data+curIndex);
                curIndex += 1;
            }
            else if(valueType == AMF_OBJECT_END)
            {
                curIndex += 2;
                break;
            }

            pEnd->next = pMetaData;
            pEnd = pEnd->next;
            metaItemCount++;
        }
        meta->amf1Count.value = metaItemCount;
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
