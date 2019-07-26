
/**	@sendImpl.cpp
*	@note 111.
*	@brief 发送端实现类 相关实现
*
*	@author		333
*	@date		2016/10/08
*
*	@note 
*
*	@warning 
*/
#include "senderBase.h"
#define new(x)  new(x,NPQ_NEW_FLAG)

/**	@fn senderBase::senderBase
*	@brief 发送端实现类构造函数
*	@param OuterParam& struOuterParam  外部参数信息
*	@return 
*/
SenderBase::SenderBase(OuterParam& struOuterParam,NPQ_QOS_MAIN_TYPE enMainType)
:
m_struOuterParam(struOuterParam)
,m_innerParam(enMainType)
,m_pRtcp(NULL)
,m_pRtcpCallback(NULL)
{
}

/**	@fn senderBase::~senderBase
*	@brief 发送端实现类析构函数
*	@param 
*	@return 
*/
SenderBase::~SenderBase()
{

}


/**	@fn SenderImpl::Init()
*	@brief  初始化函数
*	@param SenderImpl* pSenderImpl 发送端实现类
*	@return int 错误码
*/
int SenderBase::Init()
{
    int iRet = NPQ_OK;
	
	m_pRtcpCallback = new(std::nothrow)RtcpCallback(m_innerParam, m_struOuterParam);
	if(!m_pRtcpCallback)
	{
		goto ERR;
	}

	m_pRtcp = new(std::nothrow)RtcpSender(m_struOuterParam,m_innerParam,*m_pRtcpCallback);
	if(!m_pRtcp)
	{
		goto ERR;
	}
	
	return NPQ_OK;
ERR:
	SAFE_DEL(m_pRtcp);
	SAFE_DEL(m_pRtcpCallback);
    return NPQERR_SYSTEM;
}

/**	@fn SenderImpl::Fini()
*	@brief  反初始化函数
*	@param 
*	@return int 错误码
*/
int SenderBase::Fini()
{	
	SAFE_DEL(m_pRtcp);
	SAFE_DEL(m_pRtcpCallback);

    return NPQ_OK;
}

/**	@fn SenderImpl::Start()
*	@brief 开始RTCP
*	@param 
*	@return int 错误码
*/
int SenderBase::Start()
{
    m_pRtcp->Start();
	
    return NPQ_OK;
}

/**	@fn SenderImpl::Stop()
*	@brief 停止RTCP
*	@param 
*	@return int 错误码
*/
int SenderBase::Stop()
{
    m_pRtcp->Stop();

    return NPQ_OK;
}

int SenderBase::InputData( CRtpPacket& rtpPacket )
{
	//if(rtpPacket.m_struHeader.bMarkerBit)
	{
		m_pRtcp->UpdateMediaStamp(rtpPacket.m_struHeader.nTimestamp, rtpPacket.m_nArriveTimeUs,rtpPacket.m_struHeader.iPayLoadFrequence);
	}
	
	return NPQ_OK;
}

/**	@fn SenderImpl::InputData(CRtcpPacket& RtcpPacket)
*	@brief 输入rtcp数据
*	@param CRtcpPacket& RtcpPacket rtcp数据包
*	@return int 错误码
*/
int SenderBase::InputDataRtcp(CRtcpPacket& RtcpPacket)
{
    return m_pRtcp->InputData(RtcpPacket.m_pData,RtcpPacket.m_nLen);//根据RTCP的类型 传入RTCP类
}

int SenderBase::GetStat( NPQ_STAT* pStat )
{
	if(!pStat)
	{
		return NPQERR_PARA;
	}
	
	memset(pStat,0,sizeof(NPQ_STAT));
	pStat->cLossFraction = m_innerParam.m_cLossFraction;
	pStat->nRttUs = m_innerParam.m_nRttUs;
	pStat->nRealRttUs = m_innerParam.m_nRealRttUs;

	NPQ_DEBUG("memroy used =%lld",MemoryUsed());

	NPQ_DEBUG("[key]RealRttUsS = %d",pStat->nRealRttUs);
	NPQ_DEBUG("[key]filterRttUsS = %d",pStat->nRttUs);
	
	TPR_UINT32 nVideobps=0, nNackbps=0,nFecbps=0,nPaddingbps=0;
	m_struOuterParam.GetRate(&nVideobps,&nNackbps,&nFecbps,&nPaddingbps,OneTime::OneNowMs());

	NPQ_DEBUG("[BW]Videobps = %d",nVideobps);
	NPQ_DEBUG("[BW]Nackbp = %d",nNackbps);
	NPQ_DEBUG("[BW]Fecbps = %d",nFecbps);
	NPQ_DEBUG("[BW]Padding = %d",nPaddingbps);
	NPQ_DEBUG("[BW]Total = %d",nVideobps+nNackbps+nFecbps+nPaddingbps);
	pStat->nBitRate = nVideobps + nNackbps + nFecbps + nPaddingbps;
	return NPQ_OK;
}



