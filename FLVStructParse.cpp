#include <assert.h>
#include <string.h>
#include "FLVStructParse.h"
#include "Util.h"

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

int FLVStructParse::parseFlvHeader()
{
	flv->header.pos.start = curIndex;

	FLVPosition pos;
	pos.start = curIndex;
	ReadByte(flv->header.signature.value[0], flv->data, flv->dataLen, curIndex);
	ReadByte(flv->header.signature.value[1], flv->data, flv->dataLen, curIndex);
	ReadByte(flv->header.signature.value[2], flv->data, flv->dataLen, curIndex);
	if (strcmp(flv->header.signature.value, "FLV") != 0)
	{
		return -1;
	}
	pos.len = curIndex - pos.start;
	flv->header.signature.pos = pos;
	
	pos.start = curIndex;
	if (!ReadByte(flv->header.version.value, flv->data, flv->dataLen, curIndex))
	{
		return -1;
	}
	pos.len = curIndex - pos.start;
	flv->header.version.pos = pos;

	char flag = 0;
	pos.start = curIndex;
	if (!ReadByte(flag, flv->data, flv->dataLen, curIndex))
	{
		return -1;
	}	
	pos.len = curIndex - pos.start;
	flv->header.hasAudio.value = flag & 0x04;
	flv->header.hasAudio.pos = pos;
	flv->header.hasVideo.value = flag & 0x01;
	flv->header.hasVideo.pos = pos;

	pos.start = curIndex;
	if (!ReadUint32(flv->header.headerLen.value, flv->data, flv->dataLen, curIndex))
	{
		return -1;
	}
	pos.len = curIndex - pos.start;
	flv->header.headerLen.pos = pos;

	flv->header.pos.len = curIndex - flv->header.pos.start;
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
		char type = 0;
		if (!ReadByte(type, flv->data, flv->dataLen, curIndex))
		{
			break;
		}
		tag->encrypted.value = (type >> 5) & 0x01;
		tag->type.value = type & 0x1f;
		if (!ReadUint24(tag->dataSize.value, flv->data, flv->dataLen, curIndex))
		{
			break;
		}
		unsigned int timestamp = 0;
		if (!ReadUint24(timestamp, flv->data, flv->dataLen, curIndex))
		{
			break;
		}
		char timestampEx = 0;
		if (!ReadByte(timestampEx, flv->data, flv->dataLen, curIndex))
		{
			break;
		}
		tag->timestamp.value = timestampEx << 24 | timestamp;
		Seek(3, flv->data, flv->dataLen, curIndex);//streamid always 0
		Seek(tag->dataSize.value, flv->data, flv->dataLen, curIndex);
		if (!ReadUint32(tag->tagSize.value, flv->data, flv->dataLen, curIndex))
		{
			break;
		}
		tag->pos.len = curIndex - tag->pos.start;
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

	if (!Seek(4, flv->data, flv->dataLen, curIndex))
		return NULL;

	if (parseFlvTags() != 0)
		return NULL;
	
	return flv;
}
