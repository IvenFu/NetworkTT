
#include "receiver.h"
#include "TPR_Time.h"
#define new(x)  new(x,NPQ_NEW_FLAG)

Receiver::Receiver(OuterParam& struOuterParam)
:m_pRecvVideo(NULL)
,Base(struOuterParam)
{

}

Receiver::~Receiver()
{
	ReceiverImpl::Destroy(m_pRecvVideo);
}

Base* Receiver::Create(OuterParam& struOuterParam)
{
	Receiver* pReceiver = NULL;
	
	do
	{
		pReceiver = new(std::nothrow)Receiver(struOuterParam);
		if(pReceiver == NULL)
		{
			break;
		}
		
		pReceiver->m_pRecvVideo = ReceiverImpl::Create(struOuterParam,NPQ_MAIN_VEDIO);
		if(pReceiver->m_pRecvVideo == NULL)
		{
			break;
		}

	

		return pReceiver;
	}while(0);

	SAFE_DEL(pReceiver);
	return NULL;
}


int Receiver::Start()
{
	TPR_BOOL bConfig = TPR_FALSE;

	if(m_struOuterParam.m_bVideo)
	{
		m_pRecvVideo->Start();
		bConfig = TPR_TRUE;
	}
	
	return bConfig?NPQ_OK:NPQERR_GENRAL;
}

int Receiver::Stop()
{
	m_pRecvVideo->Stop();

	

	return NPQ_OK;
}

int Receiver::InputData( int iDataType, unsigned char* pData, unsigned int nDataLen )
{
	int iRet;
	CRtpPacket rtpPacket;
	CRtcpPacket rtcpPacket;
	
	if(iDataType == NPQ_DATA_RTCP_VIDEO 
		||iDataType == NPQ_DATA_RTCP_AUDIO)
	{
		iRet = rtcpPacket.Parse(pData,nDataLen);
		if(iRet!=NPQ_OK)
		{
			return iRet;
		}

		switch (iDataType)
		{
		case NPQ_DATA_RTCP_VIDEO:
			{
				m_pRecvVideo->InputDataRtcp(rtcpPacket);
				break;
			}

		default:
			break;
		}
	}
	else
	{
		iRet = rtpPacket.Parse(pData,nDataLen,NULL);
		if(iRet!=NPQ_OK)
		{
			NPQ_ERROR("rtp parse err");
			return iRet;
		}
		rtpPacket.m_nArriveTimeUs = TPR_TimeNow();

		switch (rtpPacket.m_enType)
		{
		case RTP_VIDEO:
			{

			
				m_pRecvVideo->InputData(rtpPacket);
		

				break;
			}
	
		default:
			break;
		}
	}
	
	return NPQ_OK;
}



int Receiver::GetStat (NPQ_MAIN_TYPE enMainType,NPQ_STAT* pStat)
{
	int iRet = NPQ_OK;
	switch(enMainType)
	{
	case NPQ_MAIN_VEDIO:
		iRet = m_pRecvVideo->GetStat(pStat);
		break;


	default:
		break;
	}

	return iRet;
}



