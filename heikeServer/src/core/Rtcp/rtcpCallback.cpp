
#include "timeUtil.h"
#include "rtcpCallback.h"
#include "senderImpl.h"

RtcpCallback::RtcpCallback( InnerParam& m_innerParam, OuterParam& struOuterParam)
:m_innerParam(m_innerParam)
,m_struOuterParam(struOuterParam)
,m_pSender(NULL)
,m_pReceiverStatistics(NULL)
{
	memset(&m_srInfo,0,sizeof(RTCP_SR_INFO));
	memset(&m_rrInfo,0,sizeof(RTCP_RR_INFO));
	memset(&m_xrReferencetimeInfo,0,sizeof(RTCP_XR_REFERENCETIME_INFO));
    memset(&m_fbTransportInfo, 0, sizeof(RTCP_FB_TRANSPORT_INFO));
}

RtcpCallback::~RtcpCallback()
{

}

int RtcpCallback::TriggerRtcpCallback(RTCP_INFO_TYPE enType, void* pInfo)
{
	if(enType!=INDEX_SPEC_FB_PLI_INFO 
		&& enType!=INDEX_SPEC_FB_FIR_INFO 
		&& !pInfo)
	{
		return NPQERR_PARA;
	}

	switch(enType)
	{
	case INDEX_FB_NACK_INFO:
		{

		}
		break;
	case INDEX_SR_INFO:
		{
			RTCP_SR_INFO* pInfoR = (RTCP_SR_INFO*)pInfo;
			m_srInfo = *pInfoR;
		}
		break;
	case INDEX_RR_INFO:
		{
			RTCP_RR_INFO* pInfoR = (RTCP_RR_INFO*)pInfo;

			m_innerParam.m_cLossFraction = pInfoR->cLossFraction;

			unsigned int rttMs = 0;
			if(NPQ_OK!=CalculateRTT(pInfoR->nLsr,pInfoR->nDlsr,&rttMs))
			{
				return NPQERR_GENRAL;
			}

			m_rttStat.UpdateAvgRttMs(rttMs);
			m_innerParam.m_nRttUs = m_rttStat.RttUs();//生效rtt
			m_innerParam.m_nRealRttUs = rttMs*1000;

			//if(m_innerParam.m_enMainType == NPQ_MAIN_AUDIO)
			{
				NPQ_DEBUG("[key]RealRttUsR = %d",m_innerParam.m_nRealRttUs);
				NPQ_DEBUG("[key]filterRttUsR = %d",m_innerParam.m_nRttUs);
			}

			m_rrInfo = *pInfoR;
			
			if(m_innerParam.m_enMainType == NPQ_MAIN_VEDIO)
			{
		
			}
		}
		break;
	case INDEX_XR_DLRR_INFO:
		{
			RTCP_XR_DLRR_INFO* pInfoR = (RTCP_XR_DLRR_INFO*)pInfo;

			unsigned int rttMs = 0;
			if(NPQ_OK!=CalculateRTT(pInfoR->nLrr,pInfoR->nDlrr,&rttMs))
			{
				return NPQERR_GENRAL;
			}
			
			m_rttStat.UpdateAvgRttMs(rttMs);

			m_innerParam.m_nRttUs = m_rttStat.RttUs();//生效rtt
			m_innerParam.m_nRealRttUs = rttMs*1000;

			NPQ_DEBUG("m_innerParam.m_nRttUs = %d, m_nRealRttUs = %d",m_innerParam.m_nRttUs,m_innerParam.m_nRealRttUs);
		}
		break;
	case INDEX_XR_REFERENCETIME_INFO:
		{
			RTCP_XR_REFERENCETIME_INFO* pInfoR = (RTCP_XR_REFERENCETIME_INFO*)pInfo;
			m_xrReferencetimeInfo = *pInfoR;
		}
		break;
    case INDEX_FB_TRANSPORT_INFO:
        {
            
        }
        break;
	case INDEX_SPEC_FB_PLI_INFO:
	case INDEX_SPEC_FB_FIR_INFO:
		{
			NPQ_DEBUG("FLI or FIR trigger");
			NPQ_CMD cmd = {0};
			cmd.enInfoType = NPQ_CMD_FORCE_I_FRAME;
			m_struOuterParam.DoCallback(cmd);
		}
		break;
	default:
		break;
	}

	return NPQ_OK;
}

unsigned int RtcpCallback::CalculateRTT(unsigned int LRR,unsigned int DLRR, unsigned int* pRttMs)
{
	unsigned int nSec=0,nFrac=0;

	if(LRR == 0 || DLRR == 0)
	{
		return NPQERR_GENRAL;
	}
	
	NtpTime::NtpNow(nSec,nFrac);

	unsigned int nNowNtp = 0, nNowReal = 0;
	NtpTime::NtpShortTime(nSec,nFrac,nNowNtp);
	NtpTime::NtpNtpTimeToReal(nNowNtp,nNowReal);

	unsigned int nDelayMs = 0;
	NtpTime::NtpNtpTimeToReal(DLRR,nDelayMs);

	unsigned int LRRReal = 0;
	NtpTime::NtpNtpTimeToReal(LRR,LRRReal);
	
	int rttMs = nNowReal - LRRReal - nDelayMs;
	
	//目前的RTT计算方法存在些许误差  TBD
	if(rttMs <= 0)
	{
		NPQ_WARN("!!!! rttMs=%d, %u,%u,%u",rttMs, nNowReal,LRRReal,nDelayMs);
		rttMs = 1;
	}

	if(pRttMs)
	{
		*pRttMs = rttMs;
	}

	//NPQ_DEBUG("rttMs = %d",rttMs);
	return NPQ_OK;
}

int RtcpCallback::GetSrInfo( RTCP_SR_INFO* pInfo )
{
	if(!pInfo)
	{
		return NPQERR_PARA;
	}
	*pInfo = m_srInfo;
	return NPQ_OK;
}

int RtcpCallback::GetRrInfo( RTCP_RR_INFO* pInfo )
{
	if(!pInfo)
	{
		return NPQERR_PARA;
	}
	*pInfo = m_rrInfo;
	return NPQ_OK;
}

int RtcpCallback::GetFbTransportInfo( RTCP_FB_TRANSPORT_INFO* pInfo )
{
    if(!pInfo)
    {
        return NPQERR_PARA;
    }
    *pInfo = m_fbTransportInfo;
    return NPQ_OK;
}

int RtcpCallback::GetXrReferencetimeInfo( RTCP_XR_REFERENCETIME_INFO* pInfo )
{
	if(!pInfo)
	{
		return NPQERR_PARA;
	}
	*pInfo = m_xrReferencetimeInfo;
	return NPQ_OK;
}

void RtcpCallback::SetSenderInterface( void* pSender )
{
	m_pSender = pSender;
}

//void RtcpCallback::SetRateStatistics(RateStatistics* pRatestatics)
//{
//    m_pRateStatistics = pRatestatics;
//}


void RtcpCallback::SetReceiverStatisticsInterface( ReceiverStatistics* pReceiver )
{
	m_pReceiverStatistics = pReceiver;
}

ReceiverStatistics* RtcpCallback::GetReceiverStatisticsInterface()
{
	return m_pReceiverStatistics;
}

