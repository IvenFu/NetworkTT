
/**	@sendImpl.cpp
*	@note 111.
*	@brief ���Ͷ�ʵ���� ���ʵ��
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
*	@brief ���Ͷ�ʵ���๹�캯��
*	@param OuterParam& struOuterParam  �ⲿ������Ϣ
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
*	@brief ���Ͷ�ʵ������������
*	@param 
*	@return 
*/
SenderBase::~SenderBase()
{

}


/**	@fn SenderImpl::Init()
*	@brief  ��ʼ������
*	@param SenderImpl* pSenderImpl ���Ͷ�ʵ����
*	@return int ������
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
*	@brief  ����ʼ������
*	@param 
*	@return int ������
*/
int SenderBase::Fini()
{	
	SAFE_DEL(m_pRtcp);
	SAFE_DEL(m_pRtcpCallback);

    return NPQ_OK;
}

/**	@fn SenderImpl::Start()
*	@brief ��ʼRTCP
*	@param 
*	@return int ������
*/
int SenderBase::Start()
{
    m_pRtcp->Start();
	
    return NPQ_OK;
}

/**	@fn SenderImpl::Stop()
*	@brief ֹͣRTCP
*	@param 
*	@return int ������
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
*	@brief ����rtcp����
*	@param CRtcpPacket& RtcpPacket rtcp���ݰ�
*	@return int ������
*/
int SenderBase::InputDataRtcp(CRtcpPacket& RtcpPacket)
{
    return m_pRtcp->InputData(RtcpPacket.m_pData,RtcpPacket.m_nLen);//����RTCP������ ����RTCP��
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



