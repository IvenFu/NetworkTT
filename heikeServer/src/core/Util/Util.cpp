
#include "TPR_Thread.h"
#include "TPR_Time.h"
#include "Util.h"
#include "TPR_Utils.h"
#define new(x)  new(x,NPQ_NEW_FLAG)

#ifdef N_OS_ANDROID
#include <android/log.h>

#define  LOG_TAG    "NPQ"
#define  LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#endif

//判断X是否是2的指数次方
#define IS_2_POW_N(X)   (((X)&(X-1)) == 0)

TPR_BOOL Hlog::s_bInit = TPR_FALSE; 
TPR_BOOL InitTPR::s_bInit = TPR_FALSE;
static FILE* g_hLogFile = NULL;


TPR_BOOL IsNewerTimestamp(TPR_UINT32 uTimestamp, TPR_UINT32 uPrevTimestamp) 
{
	if ((TPR_UINT32)(uTimestamp - uPrevTimestamp) == 0x80000000) 
	{
		return uTimestamp > uPrevTimestamp;
	}
	return uTimestamp != uPrevTimestamp && (TPR_UINT32)(uTimestamp - uPrevTimestamp) < 0x80000000;
}

TPR_BOOL IsNewerSequenceNumber(TPR_UINT16 sSeq,TPR_UINT16 sPrevSeq) 
{
	if ((TPR_UINT16)(sSeq - sPrevSeq) == 0x8000) 
	{
		return sSeq > sPrevSeq;
	}

	/*考虑序号的循环，两数差的中点0x8000为界限判断大小*/
	return sSeq != sPrevSeq && (TPR_UINT16)(sSeq - sPrevSeq) < 0x8000;
}

//  uTimestampLimit-uHorizonSamples < uTimestamp  < uTimestampLimit  返回true
TPR_BOOL IsObsoleteTimestamp(TPR_UINT32 uTimestamp,TPR_UINT32 uTimestampLimit,TPR_UINT32 uHorizonSamples) 
{
	return IsNewerTimestamp(uTimestampLimit, uTimestamp) &&
		(uHorizonSamples == 0 ||IsNewerTimestamp(uTimestamp, uTimestampLimit - uHorizonSamples));
}

TPR_BOOL IsContinuSequenceNumber(TPR_UINT16 sSeq,TPR_UINT16 sPrevSeq) 
{
	return (TPR_UINT16)(sSeq - sPrevSeq) == 1;
}

void* DumpData(void* pUserData,int iLen)
{
	void* p = NULL;

	if(!pUserData || iLen<=0)
	{
		return NULL;
	}

	p = (TPR_VOIDPTR)new (std::nothrow)char[iLen];
	if(!p)
	{
		return NULL;
	}

	memcpy(p,pUserData,iLen);
	return p;
}

void NPQ_Sleep(TPR_INT32 millisecond ,TPR_BOOL& bContinue)
{
	TPR_INT32 once = 0;
	TPR_INT32 mLeft = millisecond;
	const int ONCE = 1000;

	while(mLeft > 0 && bContinue)
	{
		once = npq_min(ONCE,mLeft);
		mLeft -= once;
		TPR_Sleep(once);
	}
}

void *aligned_malloc(unsigned int size, unsigned int alignment)
{
	unsigned char *p_base, *p_use;

	if (!IS_2_POW_N(alignment))
	{
		return NULL;
	}

	p_base = (unsigned char *)malloc(size + alignment + sizeof(unsigned char *));
	if(p_base == NULL)
	{
		return NULL;
	}

	p_use = p_base + sizeof(unsigned char *);
	while((unsigned long)p_use & ((unsigned long)alignment - 1))
	{
		p_use ++;
	}

	*(unsigned char**)(p_use - sizeof(unsigned char *)) = p_base;

	return p_use;
}

void aligned_free(void *p)
{
	unsigned char *p_base, *p_use;

	if(p != NULL)
	{
		p_use = (unsigned char *)p;
		p_base = *(unsigned char **)(p_use - sizeof(void*));

		free(p_base);
	}
}

void * NPQ_AlignedMalloc(int size, int align) 
{
	//TBD  对齐分配内存
#if defined(N_OS_WINDOWS) || defined(N_OS_WINDOWS64)
	return _aligned_malloc(size,align);
#else
	return aligned_malloc(size,align);
#endif
}

void NPQ_AlignedFree(void * addr) 
{
#if defined(N_OS_WINDOWS) || defined(N_OS_WINDOWS64)
    _aligned_free(addr);
#else
	aligned_free(addr);
#endif
}

int SetLogFilePath(char* szAbsFileName)
{
	if(!szAbsFileName)
	{
		return NPQERR_PARA;
	}

	g_hLogFile = fopen(szAbsFileName,"wb+");
	if(!g_hLogFile)
	{
		NPQ_ERROR("open logfile err szAbsFileName =%s",szAbsFileName);
		return NPQERR_GENRAL;
	}

	return NPQ_OK;
}


SequenceNumberUnwrapper::SequenceNumberUnwrapper()
 : last_seq_(-1)
{

}

SequenceNumberUnwrapper::~SequenceNumberUnwrapper()
{

}

TPR_INT64 SequenceNumberUnwrapper::Unwrap(TPR_UINT16 sequence_number)
{	
	TPR_INT64 unwrapped = UnwrapWithoutUpdate(sequence_number);

	UpdateLast(unwrapped);

	return unwrapped;
}

TPR_INT64 SequenceNumberUnwrapper::UnwrapWithoutUpdate(TPR_UINT16 sequence_number)
{
	if (-1 == last_seq_)
    {
        return sequence_number;
    }

	TPR_UINT16 cropped_last = (TPR_UINT16)(last_seq_);
	TPR_INT64 delta = sequence_number - cropped_last;
	if (IsNewerSequenceNumber(sequence_number, cropped_last)) 
	{
		if (delta < 0)
		{
			delta += (1 << 16);
		}
	} 
	else if (delta > 0 && (last_seq_ + delta - (1 << 16)) >= 0) 
	{
		delta -= (1 << 16);
	}

	return last_seq_ + delta;
}

void SequenceNumberUnwrapper::UpdateLast(TPR_INT64 last_sequence)
{
	last_seq_ = last_sequence;
}


TimeStampUnwrapper::TimeStampUnwrapper()
: m_llLastTimeStamp(-1)
{

}

TimeStampUnwrapper::~TimeStampUnwrapper()
{

}

TPR_INT64 TimeStampUnwrapper::Unwrap(TPR_UINT32 ntimeStamp)
{	
	TPR_INT64 unwrapped = UnwrapWithoutUpdate(ntimeStamp);

	UpdateLast(unwrapped);

	return unwrapped;
}

TPR_INT64 TimeStampUnwrapper::UnwrapWithoutUpdate(TPR_UINT32 ntimeStamp)
{
	if (m_llLastTimeStamp == -1)
	{
		return ntimeStamp;
	}

	TPR_UINT32 cropped_last = (TPR_UINT32)(m_llLastTimeStamp);
	TPR_INT64 delta = ntimeStamp - cropped_last;
	if (IsNewerTimestamp(ntimeStamp, cropped_last)) 
	{
		if (delta < 0)
		{
			delta += (1 << 32);
		}
	} 
	else if (delta > 0 && (m_llLastTimeStamp + delta - (1 << 32)) >= 0) 
	{
		delta -= (1 << 32);
	}

	return m_llLastTimeStamp + delta;
}

void TimeStampUnwrapper::UpdateLast(TPR_INT64 llLastTimeStamp)
{
	m_llLastTimeStamp = llLastTimeStamp;
}

int hlogformatWarp(char* szLevel, const char* format, ...)
{
	int iPos = 0 ;
	char szTemp[256] = {0};
	TPR_TIME_EXP_T struTime;
	TPR_TIME_T nTime = TPR_TimeNow();

	if(!szLevel)
	{
		return 0;
	}

	if(1)
	{
		if(1)
		{
			//时间
			memset(&struTime,0,sizeof(TPR_TIME_EXP_T));

			//UTC/GMT时间
			TPR_ExpTimeFromTime(nTime,  0, &struTime);

			iPos+=TPR_Snprintf(szTemp+iPos,sizeof(szTemp)-iPos,"[%04d-%02d-%02d %04d.%03d]",
				struTime.tm_year+1900,struTime.tm_mon+1,struTime.tm_mday,
				60*struTime.tm_min+struTime.tm_sec,
				struTime.tm_usec/1000);
			
			//线程ID
			iPos+=TPR_Snprintf(szTemp+iPos,sizeof(szTemp)-iPos,"[0x%08x]",TPR_Thread_GetSelfId());

			//日志类型
			iPos+=TPR_Snprintf(szTemp+iPos,sizeof(szTemp)-iPos,"[%05s]",szLevel);
		}
		
		int iRet;

		va_list al;
		va_start(al, format);
		iRet = TPR_Vsnprintf(szTemp+iPos, sizeof(szTemp)-iPos, format, al);
		va_end(al);
		
		//被截取
		if(iRet != -1)
		{
			iPos+= iRet;
			iPos+=TPR_Snprintf(szTemp+iPos, sizeof(szTemp)-iPos, "\n");
		}
		else
		{
			szTemp[255] = '\n'; 
		}

		if(g_hLogFile)
		{
			fprintf(g_hLogFile, "%s", szTemp);
			fflush(g_hLogFile);
			return 0;
		}

#if defined(N_OS_ANDROID)
		//__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s", szTemp);
		LOGD(szTemp);
#elif defined(N_OS_IOS)
		printf(szTemp);
#endif
	}
	return 0;
}