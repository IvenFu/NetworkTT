
#include "sender.h"
#include "TPR_Time.h"
#define new(x)  new(x,NPQ_NEW_FLAG)

//#define  SUPPRT_RAWDATA_API 


Sender::Sender(OuterParam& struOuterParam)
:
m_pSendVideo(NULL)
,m_bFirst(TPR_TRUE)
,Base(struOuterParam)
{

}

Sender::~Sender()
{
    SenderImpl::Destroy(m_pSendVideo);


}

Base* Sender::Create(OuterParam& struOuterParam)
{
    Sender* pSender = NULL;
    pSender = new (std::nothrow)Sender(struOuterParam);
    if(NULL == pSender)
    {
        return NULL;
    }




    pSender->m_pSendVideo = SenderImpl::Create(struOuterParam, NPQ_MAIN_VEDIO);
    if (NULL == pSender->m_pSendVideo)
    {
        SAFE_DEL(pSender);
        return NULL;
    }


    return pSender;
}

int Sender::Start()
{
	if(m_struOuterParam.m_bVideo)
	{
		NPQ_DEBUG("m_bVideo Start()");
		m_pSendVideo->Start();

		
	}


	return NPQ_OK;
}


int Sender::Stop()
{
    m_pSendVideo->Stop();

	return NPQ_OK;
}

//static FILE* g_debugSFile = fopen("video_len_s.rtp","wb+");

int Sender::InputData(int iDataType,unsigned char* pData ,unsigned int nDataLen)
{
	//fwrite(pData,1,nDataLen,pFile);

    int nRet = NPQERR_PARA;
    CRtpPacket rtpPacket;
	CRtcpPacket rtcpPacket;

	if(iDataType == NPQ_DATA_RTCP_VIDEO 
		||iDataType == NPQ_DATA_RTCP_AUDIO)
	{
		nRet = rtcpPacket.Parse(pData,nDataLen);
		if(nRet!=NPQ_OK)
		{
			return nRet;
		}

		switch (iDataType)
		{
		case NPQ_DATA_RTCP_VIDEO:
			{
				m_pSendVideo->InputDataRtcp(rtcpPacket);
				break;
			}
		case NPQ_DATA_RTCP_AUDIO:
			{

			}
		default:
			break;
		}
	}
	else
	{
		nRet = rtpPacket.Parse(pData, nDataLen, NULL);
		if(NPQ_OK != nRet)
		{
			NPQ_ERROR("parse RTPpacket error %x", nRet);
			return nRet;
		}

        //unsigned int time1 = TPR_TimeNow();
		rtpPacket.m_nArriveTimeUs = TPR_TimeNow();
		if (RTP_VIDEO == rtpPacket.m_enType)
		{
			//fwrite(&rtpPacket.m_nLen,1,4,g_debugSFile);
			//fwrite(rtpPacket.m_pData,1,rtpPacket.m_nLen,g_debugSFile);

			if(m_struOuterParam.m_bVideo)
			{
				m_pSendVideo->InputData(rtpPacket);
			}
			else
			{
				m_struOuterParam.DoCallback(rtpPacket);
			}
		}
		else if(RTP_AUDIO == rtpPacket.m_enType)
		{

		}
		else if(RTP_PRIVATE == rtpPacket.m_enType)
		{
			
		}
	}
 
    return nRet;
}

int Sender::GetStat( NPQ_MAIN_TYPE enMainType,NPQ_STAT* pStat )
{
	int iRet = NPQ_OK;
	switch(enMainType)
	{
	case NPQ_MAIN_VEDIO:
		iRet = m_pSendVideo->GetStat(pStat);
		break;


	default:
		break;
	}

	return iRet;
}


