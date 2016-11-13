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

bool FLVStructParse::ReadByte(char &value, FLVPosition& retPos)
{
	retPos.start = curIndex;
	bool ret = Util::ReadByte(value, flv->data, flv->dataLen, curIndex);
	retPos.len = curIndex - retPos.start;
	return ret;
}
bool FLVStructParse::ReadUint32(unsigned int &value, FLVPosition& retPos)
{
	retPos.start = curIndex;
	bool ret = Util::ReadUint32(value, flv->data, flv->dataLen, curIndex);
	retPos.len = curIndex - retPos.start;
	return ret;
}
bool FLVStructParse::ReadUint24(unsigned int &value, FLVPosition& retPos)
{
	retPos.start = curIndex;
	bool ret = Util::ReadUint24(value, flv->data, flv->dataLen, curIndex);
	retPos.len = curIndex - retPos.start;
	return ret;
}
bool FLVStructParse::Seek(int len, FLVPosition& retPos)
{
	retPos.start = curIndex;
	bool ret = Util::Seek(len, flv->data, flv->dataLen, curIndex);
	retPos.len = curIndex - retPos.start;
	return ret;
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

int FLVStructParse::parseFlvTags()
{
	flv->tagList = new FLVTag;
	FLVTag* p = flv->tagList;
	while (1)
	{
		FLVTag* tag = new FLVTag;
		tag->pos.start = curIndex;

		FLVPosition pos;
		char type = 0;
		if (!ReadByte(type, pos))
		{
			break;
		}
		tag->encrypted.value = (type >> 5) & 0x01;
		tag->encrypted.pos = pos;
		tag->type.value = type & 0x1f;
		tag->type.pos = pos;

		if (!ReadUint24(tag->dataSize.value, pos))
		{
			break;
		}
		tag->dataSize.pos = pos;

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
		tag->timestamp.value = timestampEx << 24 | timestamp;
		pos.start -= 3;
		pos.len += 3;
		tag->timestamp.pos = pos;
		
		if (!ReadUint24(tag->streamId.value, pos))
		{
			break;
		}
		tag->streamId.pos = pos;

		Seek(tag->dataSize.value, pos);

		if (!ReadUint32(tag->preTagSize.value, pos))
		{
			break;
		}
		tag->preTagSize.pos = pos;
		if (tag->dataSize.pos.len == 0)
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

	FLVPosition pos;
	if (!ReadUint32(flv->firstTagSize.value, pos))
		return NULL;
	flv->firstTagSize.pos = pos;

	if (parseFlvTags() != 0)
		return NULL;
	
	return flv;
}
