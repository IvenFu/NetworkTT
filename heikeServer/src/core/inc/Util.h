

#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "config.h"
#include "TPR_Tpr.h"
#include "TPR_Mutex.h"
#include "TPR_Guard.h"
#include "NPQos.h"
#include "logCtrl.h"


//支持hlog
#if defined(N_OS_WINDOWS) || defined(N_OS_WINDOWS64) || defined(N_OS_LINUX) 
//#define SUPPORT_HLOG
#endif

#ifdef SUPPORT_HLOG
#include "hlog.h"
#endif

#define NPQERR_I_OVERFLOW       0x80001000  //缓冲区满
#define NPQERR_I_REPEATE        0x80001001  //RTP包重复
#define NPQERR_I_NOTEXIST       0x80001002  //RTP包不存在
#define NPQERR_I_RTPBUFFER      0x80001003  //RTP缓冲区正在使用或者无效
#define NPQERR_I_RTPFORMAT      0x80001004  //RTP数据格式错误
#define NPQERR_I_RTCPFORMAT     0x80001005  //RTCP数据格式错误
#define NPQERR_I_ORDER          0x80001006  //RTP乱序（发送端）
#define NPQERR_I_DECODE         0x80001007  //解码出错
#define NPQERR_I_NEWFRAME       0x80001008  //插入帧缓冲数据错误（去抖动模块使用）

//要求a 是 TPR_UINT8* 类型
#define READ_BIG_ENDIAN_DWORD(a)  (((a)[0] << 24) | ((a)[1] << 16) | ((a)[2] << 8) | (a)[3])
#define READ_LITTLE_ENDIAN_DWORD(a)  (((a)[3] << 24) | ((a)[2] << 16) | ((a)[1] << 8) | (a)[0])
#define READ_BIG_ENDIAN_WORD(a)  (((a)[0] << 8) | (a)[1])
#define READ_LITTLE_ENDIAN_WORD(a)  (((a)[1] << 8) | (a)[0])

//小端转化为大端 4字节
//要求a是  TPR_UINT8* 类型， val是具体类型
inline void WriteBigEndianDWord(TPR_UINT8* a,TPR_UINT32 val)
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
inline void WriteBigEndianWord(TPR_UINT8* a,TPR_UINT16 val)
{
	do
	{    
		a[0] = (val) >> 8;
		a[1] = (val) & 0xff;
	}while(0);
}

#ifndef npq_max
#define npq_max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef npq_min
#define npq_min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


/*禁用赋值重载函数*/
#define DISALLOW_ASSIGN(TypeName) \
	void operator=(const TypeName&)

#define DISALLOW_COPY_AND_ASSIGN(TypeName)    \
	TypeName(const TypeName&);                    \
	DISALLOW_ASSIGN(TypeName)

TPR_BOOL IsNewerTimestamp(TPR_UINT32 uTimestamp, TPR_UINT32 uPrevTimestamp);
TPR_BOOL IsNewerSequenceNumber(TPR_UINT16 sSeq,TPR_UINT16 sPrevSeq) ;
TPR_BOOL IsObsoleteTimestamp(TPR_UINT32 uTimestamp,TPR_UINT32 uTimestampLimit,TPR_UINT32 uHorizonSamples);
TPR_BOOL IsContinuSequenceNumber(TPR_UINT16 sSeq,TPR_UINT16 sPrevSeq);
void* DumpData(void* pUserData,int iLen);
void NPQ_Sleep(TPR_INT32 millisecond ,TPR_BOOL& bContinue);
void * NPQ_AlignedMalloc(int size, int align);
void NPQ_AlignedFree(void * addr);
int SetLogFilePath(char* szAbsFileName);

class InitTPR
{
public:
	InitTPR(){}
	~InitTPR(){}

	static int Init()
	{
		if(!s_bInit)
		{
			s_bInit = TPR_TRUE;
			return TPR_Init();
		}
		return TPR_OK;
	}

	static int Fini()
	{
		if(s_bInit)
		{
			s_bInit = TPR_FALSE;
		}
		return TPR_OK;
	}

private:
	static TPR_BOOL s_bInit; 
};

/**	@class	 Hlog  
 *	@brief   hlog库接口封装类
 *	@note	 
 */
class Hlog
{
public:
    Hlog(){}
    ~Hlog(){}
   
    static int Init()
    {
        if(!s_bInit)
        {
            s_bInit = TPR_TRUE;
			int iRet = 0;
#ifdef SUPPORT_HLOG
            iRet = hlog_init("log4cxx.properties");
#endif
			return iRet;
        }
        return TPR_OK;
    }

    static int Fini()
    {
        if(s_bInit)
        {
			int iRet = 0;
#ifdef SUPPORT_HLOG
            iRet = hlog_fini();
#endif
            s_bInit = TPR_FALSE;
            return iRet;
        }
        return TPR_OK;
    }

private:
    #define LOG_BASE_MODULE    "NPQ"
    static TPR_BOOL s_bInit; 
};

class SequenceNumberUnwrapper 
{
public:
	SequenceNumberUnwrapper();
	~SequenceNumberUnwrapper();

	TPR_INT64 Unwrap(TPR_UINT16 sequence_number);

private:
	TPR_INT64 UnwrapWithoutUpdate(TPR_UINT16 sequence_number);

	void UpdateLast(TPR_INT64 last_sequence);

	TPR_INT64 last_seq_;
};

class TimeStampUnwrapper
{
public:
	TimeStampUnwrapper();
	~TimeStampUnwrapper();

	TPR_INT64 Unwrap(TPR_UINT32 ntimeStamp);
private:

	TPR_INT64 UnwrapWithoutUpdate(TPR_UINT32 ntimeStamp);
	void UpdateLast(TPR_INT64 llLastTimeStamp);
	TPR_INT64 m_llLastTimeStamp;
};


#ifdef SUPPORT_HLOG
//支持hlog平台
#define NPQ_ERROR(fmt, ...)            hlog_format(HLOG_LEVEL_ERROR, LOG_BASE_MODULE, "<[%d] - %s> "fmt,  __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define NPQ_WARN(fmt, ...)             hlog_format(HLOG_LEVEL_WARN, LOG_BASE_MODULE, "<[%d] - %s> "fmt,   __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define NPQ_INFO(fmt, ...)             hlog_format(HLOG_LEVEL_INFO, LOG_BASE_MODULE, "<[%d] - %s> "fmt,   __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define NPQ_DEBUG(fmt, ...)            hlog_format(HLOG_LEVEL_DEBUG, LOG_BASE_MODULE, "<[%d] - %s> "fmt,  __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define NPQ_TRACE(fmt, ...)            hlog_format(HLOG_LEVEL_TRACE, LOG_BASE_MODULE, "<[%d] - %s> "fmt,  __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
int hlogformatWarp(char* szLevel, const char* format, ...);

#define NPQ_ERROR(fmt, ...)            hlogformatWarp("ERROR","<[%d] - %s> " fmt,  __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define NPQ_WARN(fmt, ...)             hlogformatWarp("WARN","<[%d] - %s> " fmt,   __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define NPQ_INFO(fmt, ...)             hlogformatWarp("INFO","<[%d] - %s> " fmt,   __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define NPQ_DEBUG(fmt, ...)            hlogformatWarp("DEBUG","<[%d] - %s> " fmt,  __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define NPQ_TRACE(fmt, ...)            hlogformatWarp("TRACE","<[%d] - %s> " fmt,  __LINE__, __FUNCTION__, ##__VA_ARGS__)
#endif

#endif
