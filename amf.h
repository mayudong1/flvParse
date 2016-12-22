#ifndef AMF_H
#define AMF_H

typedef enum
{ AMF_NUMBER = 0, AMF_BOOLEAN, AMF_STRING, AMF_OBJECT,
    AMF_MOVIECLIP,		/* reserved, not used */
    AMF_NULL, AMF_UNDEFINED, AMF_REFERENCE, AMF_ECMA_ARRAY, AMF_OBJECT_END,
    AMF_STRICT_ARRAY, AMF_DATE, AMF_LONG_STRING, AMF_UNSUPPORTED,
    AMF_RECORDSET,		/* reserved, not used */
    AMF_XML_DOC, AMF_TYPED_OBJECT,
    AMF_AVMPLUS,		/* switch to AMF3 */
    AMF_INVALID = 0xff
} AMFDataType;

typedef enum
{ AMF3_UNDEFINED = 0, AMF3_NULL, AMF3_FALSE, AMF3_TRUE,
    AMF3_INTEGER, AMF3_DOUBLE, AMF3_STRING, AMF3_XML_DOC, AMF3_DATE,
    AMF3_ARRAY, AMF3_OBJECT, AMF3_XML, AMF3_BYTE_ARRAY
} AMF3DataType;

typedef struct AVal
{
    char *av_val;
    int av_len;
} AVal;


unsigned short AMF_DecodeInt16(const char *data);
unsigned int AMF_DecodeInt24(const char *data);
unsigned int AMF_DecodeInt32(const char *data);
void AMF_DecodeString(const char *data, AVal * str);
void AMF_DecodeLongString(const char *data, AVal * str);
bool AMF_DecodeBoolean(const char *data);
double AMF_DecodeNumber(const char *data);

#endif // AMF_H
