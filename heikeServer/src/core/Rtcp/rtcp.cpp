/**	@rtcp.cpp
*	@note 111.
*	@brief rtcp 基类
*
*	@author		222
*	@date		2017/11/02
*
*	@note 
*
*	@warning 
*/

#include "rtcp.h"
#include "rtcpCallback.h"
#define new(x)  new(x,NPQ_NEW_FLAG)

//debug开关
#if 0
#ifdef NPQ_INFO
#undef NPQ_INFO
#define NPQ_INFO(fmt, ...)
#endif

#ifdef NPQ_DEBUG
#undef NPQ_DEBUG
#define NPQ_DEBUG(fmt, ...)
#endif
#endif

/**	@fn CRtcpPacket::CRtcpPacket
*	@brief 构造函数
*	@return 
*/
CRtcpPacket::CRtcpPacket()
:m_pData(NULL)
,m_nLen(0)
,m_nArriveTimeUs(0)
{

}

/**	@fn CRtcpPacket::~CRtcpPacket
*	@brief 析构函数
*	@return 
*/
CRtcpPacket::~CRtcpPacket()
{

}

/**	@fn CRtcpPacket::Parse
*	@brief rtcp数据包
*	@param unsigned char* pData 数据地址
*	@param unsigned int nLen 数据长度
*	@return  见错误码定义
*/
int CRtcpPacket::Parse( unsigned char* pData,unsigned int nLen)
{
	if (!pData) 
	{
		return NPQERR_DATAFORMAT;
	}

	m_pData = pData;
	m_nLen = nLen;
	return NPQ_OK;
}

/**	@fn RtcpBuffer::RtcpBuffer
*	@brief 构造函数
*	@return 
*/
RtcpBuffer::RtcpBuffer()
:m_nPos(0)
{

}

/**	@fn RtcpBuffer::~RtcpBuffer
*	@brief 析构寒素
*	@return 
*/
RtcpBuffer::~RtcpBuffer()
{

}

/**	@fn RtcpBuffer::Clear
*	@brief 清除缓冲区
*	@return void
*/
void RtcpBuffer::Clear()
{
	m_nPos = 0;
}

/**	@fn RtcpBuffer::SetPosDWord
*	@brief 设置指定位置数据
*	@param unsigned int dWord  数据
*	@param unsigned int Pos 指定位置
*	@return  数据长度
*/
int RtcpBuffer::SetPosDWord( unsigned int dWord, unsigned int Pos)
{
	if(Pos+4 > BUF_LEN)
	{	
		return 0;
	}

	unsigned int word = htonl(dWord);
	
	memcpy(m_buf+Pos, (unsigned char*)&word, 4);
	return 4;
}

/**	@fn RtcpBuffer::Enqueue
*	@brief 往缓冲区插入数据
*	@param unsigned char* pData 数据地址
*	@param int iLen 数据长度
*	@return  数据长度
*/
int RtcpBuffer::Enqueue(unsigned char* pData, int iLen )
{
	if(!pData || iLen<=0 || m_nPos+iLen > BUF_LEN)
	{
		return 0;
	}

	memcpy(m_buf+m_nPos, pData, iLen);

	m_nPos+=iLen;
	return iLen;
}

/**	@fn RtcpBuffer::EnqueueWord
*	@brief 往缓冲区插入2字节数据
*	@param  unsigned short sWord  数据
*	@return  数据长度
*/
int RtcpBuffer::EnqueueWord( unsigned short sWord )
{
	unsigned short word = htons(sWord);
	return Enqueue((unsigned char*)&word,2);
}

/**	@fn RtcpBuffer::EnqueueDWord
*	@brief 往缓冲区插入4字节数据
*	@param   unsigned int dWord 数据
*	@return  数据长度
*/
int RtcpBuffer::EnqueueDWord( unsigned int dWord )
{
	unsigned int word = htonl(dWord);
	return Enqueue((unsigned char*)&word,4);
}

/**	@fn RtcpBuffer::GetValidData
*	@brief 获取有效数据地址和长度
*	@param   unsigned char** ppData  数据地址
*	@param   int* pLen  长度
*	@return  见错误码定义
*/
int RtcpBuffer::GetValidData(unsigned char** ppData, int* pLen )
{
	if(!ppData || !pLen)
	{
		return NPQERR_PARA;
	}

	*ppData = m_buf;
	*pLen = m_nPos;
	return NPQ_OK;
}

/**	@fn RTCP::RTCP
*	@brief 构造函数
*	@param  OuterParam& struOuterParam 外部参数
*	@param   InnerParam& innerParam 内部参数
*	@param   RtcpCallback& callback rtcp回调
*	@return  
*/
RTCP::RTCP(OuterParam& struOuterParam,InnerParam& innerParam,RtcpCallback& callback)
:
m_cOurSsrc(innerParam.m_enMainType == NPQ_MAIN_AUDIO ? RTCP_AUDIO_SSRC:RTCP_VIDEO_SSRC)
,m_rtcpCallback(callback)
,m_innerParam(innerParam)
,m_bStart(TPR_FALSE)
,m_hThread(TPR_INVALID_HANDLE)
,m_struOuterParam(struOuterParam)
#if defined(N_OS_WINDOWS) || defined(N_OS_WINDOWS64)||defined(N_OS_IOS)
,m_hSem(TPR_INVALID_HANDLE)
#endif
,m_pRtcpParser(NULL)
{

}

/**	@fn RTCP::~RTCP
*	@brief 析构函数
*	@return  
*/
RTCP::~RTCP()
{
	SAFE_DEL(m_pRtcpParser);
}

/**	@fn RTCP::InputData
*	@brief 输入数据
*	@param  unsigned char* pData 数据地址
*	@param   unsigned int uDataLen 数据长度
*	@return  见错误码定义
*/
int RTCP::InputData(unsigned char* pData, unsigned int uDataLen)
{
	int iRet = NPQ_OK;
	unsigned char* pNext=NULL;
	unsigned int nNextLen=0;

	if(!m_bStart)
	{
		return NPQERR_PRECONDITION;
	}

	if(!m_pRtcpParser)
	{
		m_pRtcpParser = new(std::nothrow)RtcpParser(m_rtcpCallback);
		if(!m_pRtcpParser)
		{
			NPQ_ERROR("new rtcp parser failed");
			return NPQERR_SYSTEM;
		}
	}

	while(1)
	{
		iRet = m_pRtcpParser->InputRTCP(pData,uDataLen,&pNext,&nNextLen);
		if(iRet!=NPQ_OK)
		{
			break;
		}

		if(nNextLen==0)
		{
			break;
		}
		pData = pNext;
		uDataLen = nNextLen;
	}
	
	return iRet;
}

/**	@fn RTCP::Start
*	@brief 开启内部线程运作
*	@return  见错误码定义
*/
int RTCP::Start()
{
	int iRet;
	iRet = TPR_SemCreate(&m_hSem,0);
	if(iRet!= TPR_OK)
	{
		return NPQERR_GENRAL;
	}

	m_hThread = TPR_Thread_Create(RtcpThread, this, 0,0);
	if (TPR_INVALID_THREAD == m_hThread)
	{
		NPQ_ERROR("RtcpThread error");
		TPR_SemDestroy(&m_hSem);
		return NPQERR_SYSTEM;
	}

	return NPQ_OK;
}

/**	@fn RTCP::Stop
*	@brief 停止内部线程运作
*	@return  见错误码定义
*/
int RTCP::Stop()
{
	if(m_bStart == TPR_FALSE)
	{
		return NPQ_OK;
	}

	m_bStart = TPR_FALSE;
	TPR_SemPost(&m_hSem);
	if(m_hThread!= TPR_INVALID_THREAD)
	{
		TPR_Thread_Wait(m_hThread);
		m_hThread = TPR_INVALID_THREAD;
	}

	TPR_SemDestroy(&m_hSem);
	#if defined(N_OS_WINDOWS) || defined(N_OS_WINDOWS64) || defined(N_OS_IOS)
	m_hSem = TPR_INVALID_HANDLE;
	#endif
	return NPQ_OK;
}

/**	@fn RTCP::RtcpThread
*	@brief 线程函数
*	@param  TPR_VOIDPTR pUserData 线程参数
*	@return  void
*/
TPR_VOIDPTR __stdcall RTCP::RtcpThread( TPR_VOIDPTR pUserData )
{
	RTCP* p = (RTCP*)pUserData;

	if (NULL == p)
	{
		return NULL;
	}

	p->RtcpThreadRel();
	return NULL;
}

/**	@fn RTCP::RtcpThreadRel
*	@brief 线程函数
*	@return  NPQ_OK
*/
int RTCP::RtcpThreadRel()
{
	m_bStart = TPR_TRUE;

	while(m_bStart)
	{
		TPR_SemTimedWait(&m_hSem,500);
		OutputRtcp(OUTPUT_BASIC,NULL);

		NPQ_DEBUG("[key]!!!video Bitrate Recovery = %d", m_struOuterParam.GetVideoBitrate());
	}
	return NPQ_OK;
}

/**	@fn RTCP::BuildRtcpBasic
*	@brief 构建基础报文（基类虚函数）
*	@param RTCP_OUTPUT_TYPE enOutputType 类型
*	@param void* pBuildPara 参数
*	@return  0
*/
int RTCP::BuildRtcpBasic(RTCP_OUTPUT_TYPE enOutputType,void* pBuildParam)
{
	return 0;
}

/**	@fn RTCP::OutputRtcp
*	@brief 输出RTCP报文
*	@param RTCP_OUTPUT_TYPE enOutputType 类型
*	@param void* pBuildPara 参数
*	@return  NPQ_OK
*/
int RTCP::OutputRtcp(RTCP_OUTPUT_TYPE enOutputType,void* pBuildParam)
{
	TPR_Guard guard(&m_lock);

	BuildRtcpBasic(enOutputType,pBuildParam);
	
	Flush();
	return NPQ_OK;
}

/**	@fn RTCP::AddCommonHead
*	@brief 构建基础报文头
*	@param RTCP_PT_TYPE enType  pt值
*	@param int nPacketLength  数据包长度
*	@param  int numReportingSources 报告的源个数
*	@return  NPQ_OK
*/
int RTCP::AddCommonHead(RTCP_PT_TYPE enType, int nPacketLength, int numReportingSources)
{
	unsigned int iRtcpHdr = 0x80000000; // version 2, no padding
	unsigned int SSRC = m_cOurSsrc; 
	
	if(numReportingSources!=0)
	{
		iRtcpHdr |= (numReportingSources<<24);//rc
	}

	iRtcpHdr |= (enType<<16);   //PT
	iRtcpHdr |= nPacketLength;  //length   不同类型不一样

	m_buffer.EnqueueDWord(iRtcpHdr);
	m_buffer.EnqueueDWord(SSRC);

	return NPQ_OK;
}

/**	@fn RTCP::Flush
*	@brief 输出RTCP
*	@return  
*/
void RTCP::Flush()
{
	unsigned char* pData = NULL;
	int iLen = 0;

	m_buffer.GetValidData(&pData,&iLen);
	
	if(pData && iLen > 0 )
	{
		int iDataType = (m_innerParam.m_enMainType == NPQ_MAIN_AUDIO ? NPQ_DATA_RTCP_AUDIO: NPQ_DATA_RTCP_VIDEO);
		m_struOuterParam.DoCallback(iDataType,pData,iLen);
	}
	m_buffer.Clear();
}

