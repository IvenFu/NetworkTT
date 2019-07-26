
#ifndef _RTCP_CALLBACK_H_
#define _RTCP_CALLBACK_H_

#include "rtcpDefine.h"
#include "baseImpl.h"
#include "statistics.h"
#include "rtt.h"

class SenderImpl;
class ReceiverStatistics;
class RtcpCallback
{
public:
	RtcpCallback(InnerParam& m_innerParam, OuterParam& struOuterParam);
	~RtcpCallback();

	void SetSenderInterface(void* pSender);

	void SetReceiverStatisticsInterface(ReceiverStatistics* pReceiver);

	ReceiverStatistics* GetReceiverStatisticsInterface();

	int TriggerRtcpCallback(RTCP_INFO_TYPE enType, void* pInfo);

	int GetSrInfo(RTCP_SR_INFO* pInfo);

	int GetRrInfo(RTCP_RR_INFO* pInfo);

	int GetXrReferencetimeInfo(RTCP_XR_REFERENCETIME_INFO* pInfo);
    int GetFbTransportInfo( RTCP_FB_TRANSPORT_INFO* pInfo );

private:
	unsigned int CalculateRTT(unsigned int LRR,unsigned int DLRR, unsigned int* pRttMs);

	RTCP_SR_INFO m_srInfo;
	RTCP_RR_INFO m_rrInfo;
	RTCP_XR_REFERENCETIME_INFO m_xrReferencetimeInfo;
    RTCP_FB_TRANSPORT_INFO m_fbTransportInfo;

	RttStat m_rttStat;
	InnerParam& m_innerParam;

    OuterParam& m_struOuterParam;

	void* m_pSender;
	ReceiverStatistics* m_pReceiverStatistics;
};

#endif
