
#include "Util.h"
#include "callback.h"
#include "outerCtrl.h"

#define new(x)  new(x,NPQ_NEW_FLAG)

OuterCtrlReceiver::OuterCtrlReceiver(OuterParam& struOuterParam)
:m_pStatistics(NULL)
,m_struOuterParam(struOuterParam)
{

}

OuterCtrlReceiver::~OuterCtrlReceiver()
{
	SAFE_DEL(m_pStatistics);
}

unsigned char OuterCtrlReceiver::GetVideoLossRate()
{
	if(!m_pStatistics)
	{
		return 0;
	}

	return m_pStatistics->GetLossRate();
}


void OuterCtrlReceiver::UpdateVideoStatistics( CRtpPacket& rtpPacket )
{
	//只统计视频
	if(rtpPacket.m_enType != RTP_VIDEO)
	{
		return;
	}

	if(!m_pStatistics)
	{
		m_pStatistics = new(std::nothrow)ReceiverStatisticsWrap();
		if(!m_pStatistics)
		{
			return;
		}
	}
	
	//从NPQ输出的数据参数统计
	m_pStatistics->NotePacketStat(rtpPacket);
}

TPR_UINT32 OuterCtrlReceiver::GetVideoBitrate()
{
	if(!m_pStatistics)
	{
		return 0;
	}

	NPQ_DEBUG("[key]GetFrameRate=%d",m_pStatistics->GetFrameRate());

	return m_pStatistics->GetBitrate();
}


OuterCtrlSender::OuterCtrlSender(OuterParam& struOuterParam)
:m_pRetrans(NULL)
,m_pFec(NULL)
,m_pVideo(NULL)
,m_pPadding(NULL)
,m_bInit(TPR_FALSE)
,m_struOuterParam(struOuterParam)
{

}

OuterCtrlSender::~OuterCtrlSender()
{
	SAFE_DEL(m_pRetrans);
	SAFE_DEL(m_pFec);
	SAFE_DEL(m_pVideo);
	SAFE_DEL(m_pPadding);
}

void OuterCtrlSender::GetRate( TPR_UINT32* pVideo, TPR_UINT32* pTetrans,TPR_UINT32* pFec,TPR_UINT32* pPadding,TPR_INT64 nowMs )
{
	if(pVideo)
	{
		*pVideo = m_pVideo->Rate(nowMs);
	}

	if(pTetrans)
	{
		*pTetrans = m_pRetrans->Rate(nowMs);
	}

	if(pFec)
	{
		*pFec = m_pFec->Rate(nowMs);
	}

	if(pPadding)
	{
		*pPadding =  m_pPadding->Rate(nowMs);
	}
}

void OuterCtrlSender::UpdateRateStatistics(CRtpPacket& rtpPacket,TPR_INT64 nowMs)
{
    //只统计视频
	if(rtpPacket.m_enType != RTP_VIDEO)
	{
		return;
	}

	if(rtpPacket.IsPaddingPacket())
	{
		m_pPadding->Update(rtpPacket.m_nLen, nowMs);
	}
	else if(rtpPacket.IsRetransPacket())
	{
		//NPQ_DEBUG("RetransPacket update");
		m_pRetrans->Update(rtpPacket.m_nLen, nowMs);
	}
	else if(rtpPacket.IsFecPacket())
	{
		//NPQ_DEBUG("FecPacket update");
		m_pFec->Update(rtpPacket.m_nLen, nowMs);
	}
	else
	{
		//NPQ_DEBUG("Video update");
        if (m_pVideo)
        {
            m_pVideo->Update(rtpPacket.m_nLen, nowMs);
        }
		
	}
}

int OuterCtrlSender::Init()
{
	if(m_bInit)
	{
		return NPQERR_GENRAL;
	}

	m_pRetrans = new(std::nothrow)RateStatistics();
	if(!m_pRetrans)
	{
		goto ERR;
	}

	m_pFec = new(std::nothrow)RateStatistics();
	if(!m_pFec)
	{
		goto ERR;
	}

	m_pVideo = new(std::nothrow)RateStatistics();
	if(!m_pVideo)
	{
		goto ERR;
	}

	m_pPadding = new(std::nothrow)RateStatistics();
	if(!m_pPadding)
	{
		goto ERR;
	}

	m_bInit = TPR_TRUE;
	return NPQ_OK;

ERR:
	SAFE_DEL(m_pRetrans);
	SAFE_DEL(m_pFec);
	SAFE_DEL(m_pVideo);
	SAFE_DEL(m_pPadding);
	return NPQERR_SYSTEM;
}

