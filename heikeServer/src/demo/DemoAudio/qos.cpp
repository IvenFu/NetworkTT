
#include "qos.h"

Qos::Qos(Config& config)
:m_nNPQId(-1)
,m_pCallback(NULL)
,m_pUser(NULL)
,m_role(NPQ_QOS_RECEIVER)
,m_config(config)
{
}

Qos::~Qos()
{
	if(m_nNPQId>0)
	{
		Close();
	}
}


int Qos::Open(CallbackFunc2 pCallback, CallbackFuncCmd pCallbackCmd, void* pUser,NPQ_QOS_ROLE role)
{
	int nRet;
	int nNPQId;

	NPQ_SetLogFile("npp.log");

	nNPQId = NPQ_Create(role);
	if (nNPQId < 0)
	{
		printf("NPQ_Create error! nRet =%x\n", nNPQId);
		return -1;
	};

	m_role = role;

	NPQ_PARAM struNpqParam={0};
	
	struNpqParam.m_type = QOS_TYPE_BW  ;


	struNpqParam.bVideo = 1;
	struNpqParam.bAudio = 0;
	struNpqParam.enValueType = NPQ_PARAM_SDP;

#if 0
	struNpqParam.pValue = 
		"v=0\n\
o=jyw-app 1726716695 1 IN IP4 10.0.0.41\n\
s=softwareConference\n\
i=P2P\n\
c=IN IP4 10.0.0.41\n\
t=0 0\n\
m=audio 20396 RTP/AVP 0 8 9 104\n\
a=rtpmap:0 PCMU/8000\n\
a=rtpmap:8 PCMA/8000\n\
a=rtpmap:9 G722/16000\n\
a=rtpmap:104 MPEG4-GENERIC/16000\n\
a=fmtp:104 streamtype=5;profile-level-id=1;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3\n\
m=video 20496 RTP/AVP 96\n\
a=rtpmap:96 H264/90000\n\
a=fmtp:96 profile-level-id=408055; max-mbps=486000; max-fs=8192;\n";
#endif

	
	struNpqParam.pValue = (void*)"v=0\r\n\
o=- 4257326727277154302 2 IN IP4 127.0.0.1\r\n\
s=-\r\n\
t=0 0\r\n\
c=IN IP4 10.20.134.56/127\r\n\
a=group:BUNDLE audio video\r\n\
m=audio 9 RTP/AVP 0 8 11 14 98 102 100 104 115 127\r\n\
a=mid:audio\r\n\
a=rtcp-mux\r\n\
a=rtpmap:11 PCM/8000\r\n\
a=rtpmap:0 PCMU/8000\r\n\
a=rtpmap:8 PCMA/8000\r\n\
a=rtpmap:14 MPA/90000\r\n\
a=rtpmap:98 G7221/16000\r\n\
a=rtpmap:102 G726-32/8000\r\n\
a=rtpmap:104 mpeg4-generic/16000\r\n\
a=rtpmap:100 MP4A-LATM/16000\
a=fmtp:100 bitrate=64000;profile-level-id=25;object=23\r\n\
a=rtpmap:115 opus/48000/2\r\n\
a=fmtp:115 minptime=10; useinbandfec=1\r\n\
a=rtpmap:100 MP4A-LATM/16000\r\n\
a=fmtp:100 bitrate=32000;profile-level-id=25;object=23\r\n\
a=rtpmap:127 red/8000\r\n\
a=ssrc:xxxxxxx1 cname:PC-222\r\n\
m=video 9 RTP/AVP 117 96\r\n\
a=mid:video\r\n\
a=rtcp-mux\r\n\
a=rtpmap:117 ulpfec/90000\r\n\
a=rtpmap:96 H264/90000\r\n\
a=ssrc:xxxxxxx2 cname:PC-222\r\n\
a=ssrc:xxxxxxx3 cname:PC-222\r\n\
a=ssrc-group:FEC-FR xxxxxxx2 xxxxxxx3\r\n\
a=hik-rtx";

	NPQ_SetParam(nNPQId, &struNpqParam);

	nRet = NPQ_Start(nNPQId);
	if (NPQ_OK != nRet)
	{
		printf("NPQ_Start error! nRet =%x\n", nRet);
		return -1;
	}

	nRet = NPQ_RegisterDataCallBack(nNPQId, QosCallback, this);
	if (NPQ_OK != nRet)
	{
		printf("NPQ_RegisterDataCallBack error! nRet =%x\n", nRet);
		return -1;
	}

	m_nNPQId = nNPQId;
	m_pCallback = pCallback;
	m_pCmdCallback = pCallbackCmd;
	m_pUser = pUser;
	return 0;
}


void __stdcall Qos::QosCallback( int id , int iDataType, unsigned char* pData, unsigned int nDataLen, void* pUser )
{
	Qos* p = (Qos*)pUser;
	if (NULL == p)
	{
		return;
	}

	p->QosCallbackRel(iDataType,pData,nDataLen);
}

int Qos::QosCallbackRel(int iDataType, unsigned char* pData, unsigned int nDataLen)
{
	if (iDataType == NPQ_DATA_CMD)
	{
		NPQ_CMD* pCmd = NULL;
		pCmd = (NPQ_CMD*)pData;
		if (pCmd->enInfoType == NPQ_CMD_ENCODE)
		{
			if (m_pCmdCallback)
			{
				m_pCmdCallback((void*)&pCmd->unCmd.struEncode.iMaxBitRate, m_pUser);
			}
		}

		return 0;
	}

	TPR_BOOL bData = (iDataType == NPQ_DATA_RTP_VIDEO? TPR_TRUE:TPR_FALSE);
	TPR_BOOL bReceiver = (m_role == NPQ_QOS_RECEIVER? TPR_TRUE:TPR_FALSE);

	m_pCallback(bReceiver,bData,pData,nDataLen,m_pUser);
	return 0;
}

int Qos::InputData(TPR_BOOL bData, unsigned char* pData, int iDataLen )
{
	int iRet;
	if(m_nNPQId != -1)
	{
		iRet = NPQ_InputData(m_nNPQId, 
			bData? NPQ_DATA_RTP_VIDEO : NPQ_DATA_RTCP_VIDEO,
			pData, iDataLen);

		if (NPQ_OK != iRet)
		{
			printf("NPQ_InputData error! iRet =%x,bData=%d,iDataLen=%d\n", iRet, bData, iDataLen);
			return -1;
		}
		return 0;
	}
	return -1;
}

int Qos::OutPutData(unsigned char* pData, unsigned int* pDataLen)
{
	return 0;
}

int Qos::Close()
{
	if(m_nNPQId == -1)
	{
		return -1;
	}

	NPQ_Stop(m_nNPQId);
	NPQ_Destroy(m_nNPQId);
	m_nNPQId = -1;
	return 0;
}

int Qos::GetStat( unsigned int* pRtt, unsigned char* pLossRate ,unsigned int* pBitrate)
{
	if(m_nNPQId == -1)
	{
		return -1;
	}

	if(!pRtt || !pLossRate)
	{	
		return -1;
	}
	
	NPQ_STAT stat={0};
	NPQ_GetStat(m_nNPQId,NPQ_MAIN_VEDIO,&stat);

	*pRtt = stat.nRttUs;
	*pLossRate = stat.cLossFraction;

	if (pBitrate)
	{
		*pBitrate = stat.nBitRate;
	}
	return 0;
}
