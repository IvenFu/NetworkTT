

#include "audio.h"



TPR_BOOL InitTPR::s_bInit = TPR_FALSE;


Audio::Audio()
:
m_netLostRTT(NULL)
, m_netUpBw(NULL)
, m_netDownBw(NULL)
, m_qosDownBw(NULL)
, m_qosLostRTT(NULL)
{
	InitTPR::Init();
}

Audio::~Audio()
{
	InitTPR::Fini();
}

int Audio::SimpleParsePacket(unsigned char* pData, unsigned int nDataLen)
{
	if(!pData || nDataLen == 0)
	{
		return PACKET_ERROR;
	}

	unsigned int iRtcpHdr = ntohl(*(unsigned int*)pData);

	unsigned int pt = (iRtcpHdr>>16)&0xFF;

	if(pt >= 200 && pt <=207)
	{
		//解析RTCP包类型
		if (!pData || nDataLen<4)
		{
			printf("RTCP packet length %d\n", nDataLen);
			return PACKET_ERROR;
		}
		return PACKET_RTCPVIDEO;
	}

	return PACKET_RTP;
}

int Audio::Start()
{
	m_qosLostRTT = new Qos();
	m_qosDownBw = new Qos();


	m_config.m_sServerPort = 5678;
	m_netLostRTT = new Net(m_config);

	m_config.m_sServerPort = 5679;
	m_netUpBw = new Net(m_config);

	m_config.m_sServerPort = 5678;
	m_netDownBw = new NetSender(m_config);


	m_qosLostRTT->Open(QosLostHandle, NULL,this, NPQ_QOS_RECEIVER);
	//m_qosDownBw->Open(QosHandle, NULL, this, NPQ_QOS_RECEIVER);

	m_netLostRTT->Open(NetLosteHandle,this);

	m_netUpBw->Open(NetUpBwHandle, this);


	//m_netDownBw->Open(NetReceHandle, this);



#if 0
	while(1)
	{
		TPR_Sleep(1*1000);

		unsigned int iRtt = 0;
		unsigned char cLossRat = 0;
		unsigned int iBitrate = 0;

		if (m_config.bClient)
		{
			m_qosS.GetStat(&iRtt, &cLossRat, &iBitrate);
		}
		else
		{
			m_qosR.GetStat(&iRtt, &cLossRat, &iBitrate);
		}
			
		printf("rtt = %d, lostrate=%d,iBitrate=%d\n", iRtt, cLossRat, iBitrate);
		printf("iBitrate=%d\n", m_rate.Rate(TPR_TimeNow() / 1000));
	}
#endif
	return 0;
}

int Audio::Stop()
{
#if 0
	printf("Stop 1\n");

	m_collector.Close();
	m_qosS.Close();
	m_netS.Close();
	
	if(m_config.m_bUseQos)
	{
		m_bStart = TPR_FALSE;
		if(m_hThread!= TPR_INVALID_THREAD)
		{
			TPR_Thread_Wait(m_hThread);
			m_hThread = TPR_INVALID_THREAD;
		}
	}
	m_netR.Close();
	m_file.Close();
	m_qosR.Close();
	m_player.Close();
	
	printf("Stop 2\n");
#endif

	return 0;
}

void Audio::NetLosteHandle( unsigned char* pData, unsigned int nDataLen, void* pUser )
{
	Audio* p = (Audio*)pUser;

	if (NULL == p)
	{
		return;
	}

	p->NetLosteHandleRel(pData,nDataLen);
}

int Audio::NetLosteHandleRel(unsigned char* pData, unsigned int nDataLen)
{
	int iPacketType;
	TPR_BOOL bData;
	iPacketType = SimpleParsePacket(pData,nDataLen);
	
	if(iPacketType == PACKET_ERROR)
	{
		return -1;
	}
	else if(iPacketType == PACKET_RTP)
	{
		bData = TPR_TRUE;
	}
	else
	{
		bData = TPR_FALSE;
	}
	
	//printf("m_qosR inputdata nDataLen=%d\n", nDataLen);
	m_qosLostRTT->InputData(bData,pData,nDataLen);
	//NPQ_DEMO("m_qosR inputdata end");

	return 0;
}

void Audio::NetUpBwHandle(unsigned char* pData, unsigned int nDataLen, void* pUser)
{
	Audio* p = (Audio*)pUser;

	if (NULL == p)
	{
		return;
	}

	p->NetUpBwHandleRel(pData, nDataLen);
}

int Audio::NetUpBwHandleRel(unsigned char* pData, unsigned int nDataLen)
{
	int iPacketType;
	TPR_BOOL bData;
	iPacketType = SimpleParsePacket(pData, nDataLen);

	if (iPacketType == PACKET_ERROR)
	{
		return -1;
	}
	else if (iPacketType == PACKET_RTP)
	{
		bData = TPR_TRUE;
	}
	else
	{
		bData = TPR_FALSE;
	}

	if (bData)
	{
		m_rateUp.Update(nDataLen, TPR_TimeNow() / 1000);
	}

	return 0;
}


void Audio::QosLostHandle(TPR_BOOL bReceiver,TPR_BOOL bData, unsigned char* pData, unsigned int uDataLen, void* pUser )
{
	Audio* p = (Audio*)pUser;

	if (NULL == p)
	{
		return;
	}

	p->QosLostHandleRel(bReceiver,bData,pData,uDataLen);
}


int Audio::QosLostHandleRel(TPR_BOOL bReceiver,TPR_BOOL bData, unsigned char* pData, unsigned int uDataLen)
{
	if (bData)
	{
		//printf("!!!!!!!!uDataLen = %d\n",uDataLen);
		//m_player.InputData(pData,uDataLen);
	}
	else
	{
		m_netLostRTT->InputData(pData,uDataLen);
	}

	return 0;
}






