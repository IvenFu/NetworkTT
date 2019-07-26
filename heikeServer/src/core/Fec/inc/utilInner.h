#ifndef _UTILINNER_H_
#define _UTILINNER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "Util.h"

typedef signed char		HPR_INT8;
typedef unsigned char	HPR_UINT8;
typedef signed short	HPR_INT16;
typedef unsigned short	HPR_UINT16;
typedef signed int		HPR_INT32;
typedef unsigned int	HPR_UINT32;
typedef void*			HPR_VOIDPTR;
typedef signed long		HPR_LONG;
typedef unsigned long	HPR_ULONG;

#ifndef  HPR_BOOL
#define HPR_BOOL HPR_INT32
#define HPR_TRUE  1
#define HPR_FALSE 0
#endif

#ifndef SAFE_DEL
#define SAFE_DEL(ptr)               \
	if(ptr != NULL)                 \
{                               \
	delete ptr;                 \
	ptr = NULL;                 \
}
#endif

#ifndef SAFE_ARR_DEL
#define SAFE_ARR_DEL(ptr)           \
	if (ptr != NULL)                \
{                               \
	delete[] ptr;               \
	ptr = NULL;                 \
}
#endif

#ifndef SAFE_ARR_FREE
#define SAFE_ARR_FREE(ptr)           \
	if (ptr != NULL)                \
{                               \
	free(ptr);               \
	ptr = NULL;                 \
}
#endif

#ifndef npq_max
#define npq_max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef npq_min
#define npq_min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


//要求a 是 HPR_UINT8* 类型
#ifndef READ_BIG_ENDIAN_DWORD
#define READ_BIG_ENDIAN_DWORD(a)  (((a)[0] << 24) | ((a)[1] << 16) | ((a)[2] << 8) | (a)[3])
#endif

#ifndef READ_LITTLE_ENDIAN_DWORD
#define READ_LITTLE_ENDIAN_DWORD(a)  (((a)[3] << 24) | ((a)[2] << 16) | ((a)[1] << 8) | (a)[0])
#endif

#ifndef READ_BIG_ENDIAN_WORD
#define READ_BIG_ENDIAN_WORD(a)  (((a)[0] << 8) | (a)[1])
#endif

#ifndef READ_LITTLE_ENDIAN_WORD
#define READ_LITTLE_ENDIAN_WORD(a)  (((a)[1] << 8) | (a)[0])
#endif

//小端转化为大端 4字节
//要求a是  HPR_UINT8* 类型， val是具体类型
inline void WriteBigEndianDWordIn(HPR_UINT8* a,HPR_UINT32 val)
{
	do
	{ 
		a[0] = (val) >> 24;
		a[1] = (val) >> 16;
		a[2] = (val) >> 8;
		a[3] = (val);
	}while(0);
}

//小端转化为大端 2字节
inline void WriteBigEndianWordIn(HPR_UINT8* a,HPR_UINT16 val)
{
	do
	{    
		a[0] = (val) >> 8;
		a[1] = (val) & 0xff;
	}while(0);
}

//库内部，关掉所有日志
#if 0
#ifndef NPQ_ERROR
#define NPQ_ERROR(fmt, ...)   printf("<"fmt">\n", ##__VA_ARGS__);
#endif
#ifndef NPQ_WARN
#define NPQ_WARN(fmt, ...)    printf("<"fmt">\n", ##__VA_ARGS__);
#endif
#ifndef NPQ_INFO
#define NPQ_INFO(fmt, ...)   printf("<"fmt">\n", ##__VA_ARGS__);
#endif
#ifndef NPQ_DEBUG
#define NPQ_DEBUG(fmt, ...) printf("<"fmt">\n", ##__VA_ARGS__);
#endif
#ifndef NPQ_TRACE
#define NPQ_TRACE(fmt, ...)   printf("<"fmt">\n", ##__VA_ARGS__);
#endif

#else
#ifndef NPQ_ERROR
#define NPQ_ERROR(fmt, ...) 
#endif
#ifndef NPQ_WARN
#define NPQ_WARN(fmt, ...)
#endif
#ifndef NPQ_INFO
#define NPQ_INFO(fmt, ...) 
#endif
#ifndef NPQ_DEBUG
#define NPQ_DEBUG(fmt, ...)
#endif
#ifndef NPQ_TRACE
#define NPQ_TRACE(fmt, ...)
#endif
#endif

inline HPR_BOOL IsNewerSequenceNumberIn(HPR_UINT16 sSeq,HPR_UINT16 sPrevSeq) 
{
	if ((HPR_UINT16)(sSeq - sPrevSeq) == 0x8000) 
	{
		return sSeq > sPrevSeq;
	}

	/*考虑序号的循环，两数差的中点0x8000为界限判断大小*/
	return sSeq != sPrevSeq && (HPR_UINT16)(sSeq - sPrevSeq) < 0x8000;
}

inline HPR_BOOL IsContinuSequenceNumberIn(HPR_UINT16 sSeq,HPR_UINT16 sPrevSeq) 
{
	return (HPR_UINT16)(sSeq - sPrevSeq) == 1;
}
#endif


