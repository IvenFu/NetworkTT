/**	@receiverBase.cpp
*	@note 111.
*	@brief receiverBase实现
*
*	@author		222
*	@date		2017/8/28
*
*	@note 
*
*	@warning 
*/

#include "TPR_Guard.h"
#include "TPR_Thread.h"
#include "TPR_Utils.h"
#include "TPR_Time.h"
#include "receiverImpl.h"
#include "receiverBase.h"
#define new(x)  new(x,NPQ_NEW_FLAG)

/**	@fn ReceiverBase::ReceiverBase
*	@brief 构造函数
*	@param OuterParam& struOuterParam 外部参数
*	@param NPQ_QOS_MAIN_TYPE enMainType Qos主类型
*	@return 
*/
ReceiverBase::ReceiverBase(OuterParam& struOuterParam, NPQ_QOS_MAIN_TYPE enMainType)
:m_bStartBase(TPR_FALSE)
,m_innerParam(enMainType)
,m_pRtcp(NULL)
,m_pRtcpCallback(NULL)
,m_struOuterParam(struOuterParam)
,m_pStat(NULL)
,m_pBitrateStatistics(NULL)
{
}

/**	@fn ReceiverBase::~ReceiverBase
*	@brief 析构函数
*	@return 
*/
ReceiverBase::~ReceiverBase()
{

}

/**	@fn ReceiverBase::Start
*	@brief 开启功能
*	@return int  见错误码定义
*/
int ReceiverBase::Start()
{
	m_bStartBase = TPR_TRUE;
	
	m_pRtcp->Start();
	
	//第一次，触发FIR
	if(m_innerParam.m_enMainType == NPQ_MAIN_VEDIO)
	{
		m_pRtcp->ReportFIR();
	}
	return NPQ_OK;
}

/**	@fn ReceiverBase::Stop
*	@brief 停止功能
*	@return int  见错误码定义
*/
int ReceiverBase::Stop()
{
	if(m_bStartBase == TPR_FALSE)
	{
		return NPQ_OK;
	}

	m_bStartBase = TPR_FALSE;
	
	m_pRtcp->Stop();

	return NPQ_OK;
}

/**	@fn ReceiverBase::Init
*	@brief 初始化
*	@return int  见错误码定义
*/
int ReceiverBase::Init()
{
	int iRet;

	m_pRtcpCallback = new(std::nothrow)RtcpCallback(m_innerParam, m_struOuterParam);
	if(!m_pRtcpCallback)
	{
		goto ERR;
	}

	m_pRtcp = new(std::nothrow)RtcpReceiver(m_struOuterParam,m_innerParam,*m_pRtcpCallback);
	if(!m_pRtcp)
	{
		goto ERR;
	}

	m_pStat = new(std::nothrow)ReceiverStatistics();
	if(!m_pStat)
	{
		goto ERR;
	}
	
	m_pRtcpCallback->SetReceiverStatisticsInterface(m_pStat);

	m_pBitrateStatistics = new(std::nothrow)RateStatistics();
	if(!m_pBitrateStatistics)
	{
		goto ERR;
	}

	return NPQ_OK;
ERR:
	Fini();
	return NPQERR_SYSTEM;
}

/**	@fn ReceiverBase::Fini
*	@brief 反初始化
*	@return int  见错误码定义
*/
int ReceiverBase::Fini()
{	
	//注意销毁的顺序
	SAFE_DEL(m_pBitrateStatistics);
	SAFE_DEL(m_pStat);
	SAFE_DEL(m_pRtcp);
	SAFE_DEL(m_pRtcpCallback);
	return NPQ_OK;
}

/**	@fn ReceiverBase::InputData
*	@brief 输入数据
*	@param CRtpPacket& RtpPacket  RTP包
*	@return  见错误码定义
*/
int ReceiverBase::InputData( CRtpPacket& RtpPacket )
{
	if(!m_bStartBase)
	{
		NPQ_ERROR("impl do not start");
		return NPQERR_PRECONDITION;
	}
	
	//添加ssrc，非FEC即媒体ssrc
	m_pRtcp->AddSsrc(RtpPacket.m_struHeader.nSsrc, !RtpPacket.IsFecPacket());

	m_pStat->NotePacketStat(RtpPacket,(TPR_INT64)RtpPacket.m_nArriveTimeUs,TPR_TRUE);
	m_pBitrateStatistics->Update(RtpPacket.m_nLen,OneTime::OneNowMs());
	return NPQ_OK;
}

/**	@fn ReceiverBase::InputDataRtcp
*	@brief 输入rtcp数据
*	@param CRtcpPacket& RtpPacket  rtcp包
*	@return  见错误码定义
*/
int ReceiverBase::InputDataRtcp(CRtcpPacket& RtpPacket)
{
	if(!m_bStartBase)
	{
		NPQ_WARN("impl do not start");
		return NPQERR_PRECONDITION;
	}

	return m_pRtcp->InputData(RtpPacket.m_pData,RtpPacket.m_nLen);
}

/**	@fn ReceiverBase::GetStat
*	@brief 获取状态
*	@param NPQ_STAT* pStat  状态
*	@return  见错误码定义
*/
int ReceiverBase::GetStat( NPQ_STAT* pStat )
{
	if(!pStat)
	{
		return NPQERR_PARA;
	}
	
	memset(pStat,0,sizeof(NPQ_STAT));
	pStat->cLossFraction = m_innerParam.m_cLossFraction;
	pStat->nRttUs = m_innerParam.m_nRttUs;
	pStat->nRealRttUs = m_innerParam.m_nRealRttUs;
	pStat->nBitRate = m_pBitrateStatistics->Rate(OneTime::OneNowMs());
	
	if(m_innerParam.m_enMainType == NPQ_MAIN_VEDIO)
	{
		pStat->cLossFraction2 = m_struOuterParam.GetVideoLossRate();
		
		NPQ_DEBUG("[key]RealRttUsR = %d",pStat->nRealRttUs);
		NPQ_DEBUG("[key]filterRttUsR = %d",pStat->nRttUs);
		NPQ_DEBUG("[key]lossRate1R = %d",pStat->cLossFraction);
		NPQ_DEBUG("[key]lossRate2R = %d",pStat->cLossFraction2);
		NPQ_DEBUG("[key]video nBitRate = %d",pStat->nBitRate);
		NPQ_DEBUG("[key]video Bitrate Recovery = %d",m_struOuterParam.GetVideoBitrate());
	}

	return NPQ_OK;
}

/**	@fn ReceiverBase::GetSrInfo
*	@brief 获取sr报告信息
*	@param RTCP_SR_INFO* pInfo  sr报告信息
*	@return  见错误码定义
*/
int ReceiverBase::GetSrInfo( RTCP_SR_INFO* pInfo)
{
	if (m_pRtcpCallback)
	{
        m_pRtcpCallback->GetSrInfo(pInfo);
		return NPQ_OK;
	}

    return NPQERR_GENRAL;
}

