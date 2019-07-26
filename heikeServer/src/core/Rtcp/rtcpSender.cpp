/**	@rtcpSender.cpp
*	@note 111.
*	@brief rtcp 发送端派生类
*
*	@author		222
*	@date		2017/11/02
*
*	@note 
*
*	@warning 
*/
#include "rtcpSender.h"

/**	@fn RtcpSender::RtcpSender
*	@brief 构造函数
*	@param  OuterParam& struOuterParam 外部参数
*	@param   InnerParam& innerParam 内部参数
*	@param   RtcpCallback& callback rtcp回调
*	@return  
*/
RtcpSender::RtcpSender(OuterParam& struOuterParam,InnerParam& innerParam,RtcpCallback& callback)
:RTCP(struOuterParam,innerParam,callback)
,m_bRttFlag(TPR_FALSE)
,m_nMediaTimeStamp(0)
,m_nMediaCaptureTimeUs(0)
,m_iPayLoadFrequence(0)
{

}

/**	@fn RtcpSender::~RtcpSender
*	@brief 析构函数
*	@return  
*/
RtcpSender::~RtcpSender()
{

}

/**	@fn RtcpSender::UpdateMediaStamp
*	@brief 更新数据包相关参数
*	@param  TPR_UINT32 nTimeStamp 时间戳
*	@param TPR_TIME_T nArriveTimeUs 到达时间
*	@param  TPR_INT32 iPayLoadFrequence 时钟频率
*	@return  void
*/
void RtcpSender::UpdateMediaStamp( TPR_UINT32 nTimeStamp, TPR_TIME_T nArriveTimeUs,TPR_INT32 iPayLoadFrequence)
{
	m_nMediaTimeStamp = nTimeStamp;
	m_nMediaCaptureTimeUs = nArriveTimeUs;
	m_iPayLoadFrequence = iPayLoadFrequence;
}

/**	@fn RtcpSender::GetRttFlag
*	@brief 获取是否产生第一个SR报告标志
*	@return  标志
*/
TPR_BOOL RtcpSender::GetRttFlag()
{
	return m_bRttFlag;
}

/**	@fn RtcpSender::BuildRtcpBasic
*	@brief 构建RTCP报文
*	@param  RTCP_OUTPUT_TYPE enOutputType  需要构建的报文类型
*	@param void* pBuildPara 构建参数
*	@return  NPQ_OK
*/
int RtcpSender::BuildRtcpBasic( RTCP_OUTPUT_TYPE enOutputType,void* pBuildParam)
{
	if(enOutputType == OUTPUT_BASIC)
	{
		AddSR();
		AddSDES();
		AddXRDLRR();
	}
	return NPQ_OK;
}

/**	@fn RtcpSender::AddSR
*	@brief 构建SR报文
*	@return  NPQ_OK
*/
int RtcpSender::AddSR()
{
	AddCommonHead(RTCP_PT_SR, 6, 0);//28字节  应该填充 28/4 -1 = 6 
	AddSendInfo();
	return NPQ_OK;
}

/**	@fn RtcpSender::AddSDES
*	@brief 构建SDES报文
*	@return  NPQ_OK
*/
int RtcpSender::AddSDES()
{
	AddCommonHead(RTCP_PT_SDES, 9 , 1);//40字节  应该填充 40/4 -1 = 9 
	AddSDESItem();
	return NPQ_OK;
}

/**	@fn RtcpSender::AddSendInfo
*	@brief 构建SR body报文
*	@return  NPQ_OK
*/
int RtcpSender::AddSendInfo()
{
	//NTP timestamp
	unsigned int nSec;
	unsigned int nFrac;
	NtpTime::NtpNow(nSec,nFrac);

	m_buffer.EnqueueDWord(nSec);
	m_buffer.EnqueueDWord(nFrac);
	//RTPtimestamp 0

	TPR_TIME_T nowUs = OneTime::OneNowUs();
	TPR_UINT32 nTimeStamp = 0;

	if(nowUs>=m_nMediaCaptureTimeUs)
	{
		int iRate = m_iPayLoadFrequence/1000;
		nTimeStamp = m_nMediaTimeStamp + ((nowUs - m_nMediaCaptureTimeUs)/1000)*iRate;
	}

	m_buffer.EnqueueDWord(nTimeStamp);

	//sender packet count 0
	m_buffer.EnqueueDWord(0);//TBD

	//sender octet count 0
	m_buffer.EnqueueDWord(0);//TBD
	return NPQ_OK;
}

/**	@fn RtcpSender::AddSDESItem
*	@brief 构建SR SEDS具体报文
*	@return  NPQ_OK
*/
int RtcpSender::AddSDESItem()
{
	int CName = 0x01;
	m_buffer.Enqueue((unsigned char*)&CName, 1);

	int nLen = strlen(SDES_TEXT);//TBD
	m_buffer.Enqueue((unsigned char*)&nLen, 1);

	unsigned char* pTmp = (unsigned char*)SDES_TEXT;
	m_buffer.Enqueue(pTmp, nLen);

	int nType = SDES_ITEM_TYPE;
	pTmp = (unsigned char*)&nType;
	m_buffer.Enqueue(pTmp, 1);

	nLen = strlen(SDES_TEXT_1);
	m_buffer.Enqueue((unsigned char*)&nLen, 1);

	pTmp = (unsigned char*)SDES_TEXT_1;
	m_buffer.Enqueue(pTmp, nLen);

	nLen = 0;
	m_buffer.Enqueue((unsigned char*)&nLen, 2);
	return NPQ_OK;
}

/**	@fn RtcpSender::AddXRDLRR
*	@brief 构建XR DLRR报文
*	@return  NPQ_OK
*/
int RtcpSender::AddXRDLRR()
{
	AddCommonHead(RTCP_PT_XR, 5, 0); //DLRR长度24字节 故填充24/4 -1 = 5
	AddXRBlock(RTCP_XR_BT_DLRR);

	return NPQ_OK;
}

/**	@fn RtcpSender::AddXRBlock
*	@brief 构建XR 块信息
*	@param RTCP_XR_BT_TYPE enXRBT  XR类型
*	@return  NPQ_OK
*/
int RtcpSender::AddXRBlock(RTCP_XR_BT_TYPE enXRBT)
{
	unsigned int nFir = 0;


	RTCP_XR_REFERENCETIME_INFO info = {0};
	m_rtcpCallback.GetXrReferencetimeInfo(&info);

	nFir |= enXRBT<<24;  //BT
	nFir |= (3);  //block length

	m_buffer.EnqueueDWord(nFir);
	//SSRC
	m_buffer.EnqueueDWord(m_cOurSsrc);//TBD 确认ssrc
	//LRR
	unsigned int NTPmsw =info.nLastXRNTPmsw;  //单位秒
	unsigned int NTPlsw =info.nLastXRNTPlsw;  //单位232ps    1000 000 000 000 /2^32  = 232

	unsigned int LRR = 0;
	NtpTime::NtpShortTime(info.nLastXRNTPmsw,info.nLastXRNTPlsw,LRR);//截取NTP时间，防止NTP到2036年无法使用

	m_buffer.EnqueueDWord(LRR);
	//DLRR
	TPR_TIME_T now = TPR_TimeNow();
	TPR_TIME_T delay = 0;
	unsigned int delayS = 0;
	unsigned int delayUs = 0;

	if(info.nlastReceiveXRTime>0 && now>info.nlastReceiveXRTime)
	{
		delay = now-info.nlastReceiveXRTime;// 算delay 是相对时间，不用NTP时间
		delayS = delay/1000000;
		delayUs = delay%1000000;
	}

	unsigned int DLRR=0;
	if (LRR == 0) 
	{
		DLRR = 0;
	} 
	else 
	{
		NtpTime::NtpRealToNtpTime(delayS,delayUs,DLRR);
		m_bRttFlag = TPR_TRUE;
	}
	m_buffer.EnqueueDWord(DLRR);

	return NPQ_OK;
}



