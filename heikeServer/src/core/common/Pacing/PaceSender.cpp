/**	@PaceSender.cpp
*	@note 111.
*	@brief pacing���ʵ��
*
*	@author		333
*	@date		2017/2/7
*
*	@note 
*
*	@warning 
*/

#include "PaceSender.h"

#define new(x)  new(x,NPQ_NEW_FLAG)

#if 0
#ifdef NPQ_INFO
#undef NPQ_INFO
#define NPQ_INFO(fmt, ...)
#endif

#ifdef NPQ_DEBUG
#undef NPQ_DEBUG
#define NPQ_DEBUG(fmt, ...)
#endif
#endif


const TPR_INT64 kMaxQueueLengthMs = 2000;     //������ʱ��2�� ������ڴ�ֵ��Ӧ�ö�����֡������ Ҫ��ı����ʻ���
const float     kDefaultPaceMultiplier = 1.25f;//pacing�����ǹ������ʵ�2.5�� �����˷������� ��С���Ŷ�delay��
const TPR_INT64 kMinPacketLimitMs = 5;        // ����burst���5ms
const TPR_INT64 kMaxIntervalTimeMs = 30;      //�������30ms
#define  RTP_MINLEN                  (12)     //RTP����С����


/**	@fn  PacedSender::PacedSender
*	@brief ���캯��
*	@param  struOuterParam �ⲿ����
*	@param  TransportAddTExtension �������չͷ
*	@return void
*/
PacedSender::PacedSender(OuterParam& struOuterParam, CTrasnportAddExtension* TransportAddTExtension)
:
m_pcMediaBudget(NULL),
m_pcPaddingBudget(NULL),
m_nEstimatedBitratebps(0),
m_nPacingBitratekbps(0),
m_llTimeLastUpdateUs(OneTime::OneNowUs()),
m_pcPackets(NULL),
m_nPacketCounter(0),
m_bStart(TPR_FALSE),
m_pTimer(NULL),
m_pFec(NULL),
m_struOuterParam(struOuterParam),
m_paddExtension(TransportAddTExtension),
m_bPacketArrive(TPR_FALSE),
m_bInit(TPR_FALSE),
m_pProber(NULL)
{
    m_nLastSendTime = 0;
    UpdateBudgetWithElapsedTime(kMinPacketLimitMs);
}

/**	@fn  PacedSender::~PacedSender
*	@brief ��������
*	@param  ��
*	@return void
*/
PacedSender::~PacedSender() {}


/**	@fn  PacedSender::Create
*	@brief ����PacedSender��
*	@param  struOuterParam �ⲿ����
*	@param  TransportAddTExtension ����㷴����
*	@return �ɹ�����PacedSender* �� ʧ�ܷ���NULL
*/
PacedSender* PacedSender::Create(OuterParam& struOuterParam, CTrasnportAddExtension* TransportAddTExtension)
{
    int iRet = 0;
    PacedSender* pcPacedSender = NULL;
    pcPacedSender = new(std::nothrow)PacedSender(struOuterParam, TransportAddTExtension);
    if (NULL == pcPacedSender )
    {
        return NULL;
    }

    iRet = pcPacedSender->Init();
    if(NPQ_OK != iRet)
    {
        pcPacedSender->Fini();
        SAFE_DEL(pcPacedSender);
        return NULL;
    }
    return pcPacedSender;
}

/**	@fn  PacedSender::Destroy
*	@brief  ����PacedSender��
*	@param  pcPacedSender     PacedSender��
*	@return int ������
*/
int PacedSender::Destroy(PacedSender* pcPacedSender)
{
    if(!pcPacedSender)
    {
        return NPQERR_PARA;
    }
    pcPacedSender->Fini();

    SAFE_DEL(pcPacedSender);

    return NPQ_OK;
}

/**	@fn  PacedSender::Init
*	@brief  ��ʼ��
*	@param  ��
*	@return int ������
*/
int PacedSender::Init()
{
    int nRet = NPQ_OK;
	m_pcMediaBudget = new(std::nothrow)IntervalBudget(0);
    if (NULL == m_pcMediaBudget)
    {
        return NPQERR_SYSTEM;//�ⲿɾ��
    }

    m_pcPaddingBudget= new(std::nothrow) IntervalBudget(0);
    if (NULL == m_pcPaddingBudget)
    {
        return NPQERR_SYSTEM;
    }

    m_pcPackets = new(std::nothrow) PacketQueue();
    if (NULL == m_pcPackets)
    {
        return NPQERR_SYSTEM;
    }

    nRet = m_sendedDataBuffer.InitBuffer();
    if (NPQ_OK != nRet)
    {
        return NPQERR_SYSTEM;
    }

    //m_pTimer = new(std::nothrow)EventTimer;
    //if(!m_pTimer)
    //{
    //    return NPQERR_SYSTEM;
    //}

    m_pProber = new(std::nothrow)BitrateProber();
    if (!m_pProber)
    {
        return NPQERR_SYSTEM;
    }

	m_pAlr = new(std::nothrow)AlrDetector();
	if(!m_pAlr)
	{
		return NPQERR_SYSTEM;
	}

    m_bInit = TPR_TRUE;
    return NPQ_OK;
}

/**	@fn  PacedSender::Fini
*	@brief  ����ʼ��
*	@param  ��
*	@return int ������
*/
int PacedSender::Fini()
{
    /*if (TPR_FALSE == m_bInit)
    {
        return NPQERR_PRECONDITION;
    }*/
    SAFE_DEL(m_pcMediaBudget);
    SAFE_DEL(m_pcPaddingBudget);
    SAFE_DEL(m_pcPackets);
    SAFE_DEL(m_pTimer);
    SAFE_DEL(m_pProber);
	SAFE_DEL(m_pAlr);

    m_sendedDataBuffer.FInitBuffer();
    return NPQ_OK;
}

/**	@fn  PacedSender::Start
*	@brief  ��ʼpacing
*	@param  ��
*	@return int ������
*/
int PacedSender::Start()
{
    //m_bStart = TPR_TRUE;
    //if(NULL == m_pTimer)
    //{
    //    return NPQERR_PRECONDITION;
    //}
    //else
    //{
    //    m_pTimer->StartTimer(PACING_PROCESS_INTERVAL_MS, PacingCallback, this);
    //}

	m_hThread = TPR_Thread_Create(PacingThread, this, 0);
	if (TPR_INVALID_THREAD == m_hThread)
	{
		return NPQERR_SYSTEM;
	}
    return NPQ_OK;
}

/**	@fn  PacedSender::Stop
*	@brief  ֹͣpacing
*	@param  ��
*	@return int ������
*/
int PacedSender::Stop()
{
    if(TPR_FALSE == m_bStart)
    {
        return NPQ_OK;
    }

    m_bStart = TPR_FALSE;
	if(m_hThread!= TPR_INVALID_THREAD)
	{
		TPR_Thread_Wait(m_hThread);
		m_hThread = TPR_INVALID_THREAD;
	}

    //if (m_pTimer)
    //{
    //    m_pTimer->StopTimer();
    //}
    NPQ_DEBUG("Stop finish!");
    return NPQ_OK;
}

/**	@fn  PacedSender::Reset
*	@brief  ��������
*	@param  ��
*	@return void
*/
void PacedSender::Reset()
{
   //TBD �ⲿ����stop ��������Ҫ����?
    m_bPacketArrive = TPR_FALSE;
}

/**	@fn  PacedSender::SetEstimatedBitrate
*	@brief  ���ù��Ƶı�����
*	@param  bitrate_bps  ������
*	@return void
*/
void PacedSender::SetEstimatedBitrate(TPR_UINT32 nBitratebps) 
{
    if (0 == nBitratebps || nBitratebps == m_nEstimatedBitratebps)
    {
        //NPQ_DEBUG("PacedSender is not designed to handle 0 bitrate");
        return;
    }
    TPR_Guard guard(&m_lock);
    
	NPQ_DEBUG("[PACING]SetEstimatedBitrate=%d",nBitratebps);
    m_nEstimatedBitratebps = nBitratebps;
    if (m_pcPaddingBudget)
    {
        m_pcPaddingBudget->setTargetRateKbps(m_nEstimatedBitratebps / 1000); //����padding������
    }

    m_nPacingBitratekbps = m_nEstimatedBitratebps / 1000 * kDefaultPaceMultiplier; //����pacing������

	m_pAlr->SetEstimatedBitrate(nBitratebps); //��������
}

/**	@fn  PacedSender::InsertPacket
*	@brief  �������ݰ�, ����ԭʼ����Ƶ���ݡ��ش�����FEC
*	@param  rtpPacket  rtp��
*	@return int ������
*/
int PacedSender::InputData(CRtpPacket& rtpPacket)
{
    if (!m_bInit || !m_bStart)
    {
        return NPQERR_PRECONDITION;
    }

	//��FEC��ԭʼ���� Ҳ������һ��pacing ��ʱֱ�ӷ���
	if(rtpPacket.m_bHaveCallback)
	{
		//NPQ_DEBUG("m_bHaveCallback");
		return NPQ_OK;
	}

	//FEC����pacing���У���֤FEC�����ȼ�
	if(rtpPacket.IsFecPacket())
	{
		return OutputData(rtpPacket);
	}

    TPR_Guard guard(&m_lock);
    //TPR_INT64 nStart = OneTime::OneNowUs();
    if (TPR_FALSE == m_bPacketArrive)
    {
        m_bPacketArrive = TPR_TRUE;
        m_llTimeLastUpdateUs = OneTime::OneNowUs();
    }

    if(m_pProber)
    {
        m_pProber->OnIncomingPacket(rtpPacket.m_nLen);
    }

    Priority pri = kLowPriority;
    if(rtpPacket.m_enType == RTP_VIDEO)
    {
        pri = kLowPriority;
    }
    else if(rtpPacket.m_enType == RTP_AUDIO)
    {
        pri = kHigTPRiority;
    }

    PacedPacket* pPacedPacket = new(std::nothrow) PacedPacket;
    if (NULL == pPacedPacket)
    {
        return NPQERR_SYSTEM;
    }
    memset(pPacedPacket->pBuffer, 0, RTP_PACKET_LEN);

    pPacedPacket->priority        = pri;
    pPacedPacket->capture_time_us = rtpPacket.m_nArriveTimeUs;
    pPacedPacket->enqueue_time_ms = OneTime::OneNowMs();
    pPacedPacket->bytes           = rtpPacket.m_nLen;//����RTP���ĳ���
    pPacedPacket->retransmission  = rtpPacket.IsRetransPacket();
    pPacedPacket->enqueue_order   = m_nPacketCounter++;
    if (rtpPacket.m_nLen > RTP_PACKET_LEN)
    {
        NPQ_ERROR("rtp Packet len err=%d",rtpPacket.m_nLen);
        SAFE_DEL(pPacedPacket);
        return NPQERR_GENRAL;
    }

    memcpy(pPacedPacket->pBuffer, rtpPacket.m_pData, rtpPacket.m_nLen);
    pPacedPacket->nDataLen = rtpPacket.m_nLen;
    if (NULL == m_pcPackets)
    {
        SAFE_DEL(pPacedPacket);
        return NPQERR_GENRAL;
    }
    m_pcPackets->Push(pPacedPacket);//���ȼ�����Ƶ�� ���ش��� ����Ƶ�� ͬһ�����ȼ��ıȽϲɼ�ʱ��  �ɼ�ʱ����ͬ�� �Ƚϼ�����е��Ⱥ�˳��
	
	//debug ��ӡ����֡���
	if(rtpPacket.m_struHeader.bMarkerBit && rtpPacket.m_enType == RTP_VIDEO && !rtpPacket.IsRetransPacket())
	{
		//static TPR_TIME_T t1 = 0;
		//NPQ_WARN("[key]intputDiffS=%lld",TPR_TimeNow()-t1);
		//t1 = TPR_TimeNow();
	}
    return NPQ_OK;
}

/**	@fn  PacedSender::PacingThread
*	@brief  pacing�߳�
*	@param  pUserData �û�����ָ��
*	@return NULL
*/
void PacedSender::PacingCallback( TPR_VOIDPTR pUserData )
{
    PacedSender* pPacedSender = (PacedSender*)pUserData;

    if (NULL == pPacedSender)
    {
        return;
    }

    pPacedSender->Process();
}

TPR_INT64 PacedSender::TimeUntilNextProcess() 
{
	TPR_Guard guard(&m_lock);
	
	//��һ�����ݰ���û�����̼߳��5ms����
	if(!m_bPacketArrive)
	{
		return kMinPacketLimitMs;
	}

	TPR_INT64 llNowUs = OneTime::OneNowUs();

	TPR_INT64 llElapsedTimeUs = llNowUs - m_llTimeLastUpdateUs;
	TPR_INT64 llElapsedTimeMs = (llElapsedTimeUs + 500) / 1000;

	if (m_pProber && m_pProber->IsProbing()) 
	{
		TPR_INT64 llRet = m_pProber->TimeUntilNextProbe(OneTime::OneNowMs());
		if (llRet >= 0)
		{
			return llRet;
		}
	}

	return npq_max(kMinPacketLimitMs - llElapsedTimeMs, 0); //��������� 5ms ����һ������
}

/**	@fn  PacedSender::PacingThreadRel
*	@brief  pacing�߳�ʵ�ʴ�����
*	@param  ��
*	@return int ������
*/
void PacedSender::PacingThreadRel()
{
	m_bStart = TPR_TRUE;

	while(m_bStart)
	{
		TPR_INT64 llWaitMs = TimeUntilNextProcess();
		//NPQ_DEBUG("llWaitMs = %lld",llWaitMs);
		if(llWaitMs>0)
		{
			TPR_Sleep(llWaitMs);
		}
	
		Process();
	}
}

TPR_VOIDPTR __stdcall PacedSender::PacingThread( TPR_VOIDPTR pUserData)
{
	PacedSender* pPacedSender = (PacedSender*)pUserData;

	if (NULL == pPacedSender)
	{
		return NULL;
	}

	pPacedSender->PacingThreadRel();
	return NULL;
}

/**	@fn  PacedSender::Process
*	@brief  pacing�̴߳�����
*	@param  ��
*	@return void
*/
void PacedSender::Process() 
{
	TPR_INT64 nAvgtimeLeftMs = 0;
	int nMinBitrateNeededKbps = 0;

    if (!m_bPacketArrive)
    {
        return;
    }

    TPR_Guard guard(&m_lock);


	TPR_INT64 llNowUs = OneTime::OneNowUs();
    TPR_INT64 llElapsedTimeMs = (llNowUs - m_llTimeLastUpdateUs + 500) / 1000;//�������� ��һ��Ԥ����5ms���ϴ�ʱ��
    m_llTimeLastUpdateUs = llNowUs;
    int nTargetBitrateKbps = m_nPacingBitratekbps;
    if (NULL == m_pcPackets)
    {
        return;
    }

    if (llElapsedTimeMs > 0) 
    {
        int nQueueSizeBytes = m_pcPackets->SizeInBytes();
        if (nQueueSizeBytes > 0) 
        {
            m_pcPackets->UpdateQueueTime(OneTime::OneNowMs());

            nAvgtimeLeftMs = npq_max(1, kMaxQueueLengthMs - m_pcPackets->AverageQueueTimeMs());

#if 0
			//��� ͨ�������������С�������ǲ��ǳ���nTargetBitrateKbps��������е��� 
			//����ʵ����������ԶԶ����nTargetBitrateKbps������������
            nMinBitrateNeededKbps = (int)(nQueueSizeBytes * 8 / nAvgtimeLeftMs); 
            if (nMinBitrateNeededKbps > nTargetBitrateKbps)
            {                                              
                nTargetBitrateKbps = nMinBitrateNeededKbps;
                NPQ_DEBUG("[PACING]pacing adjust bitrate=%d", nTargetBitrateKbps);
            }
#endif
        }
		
		//���� media�����ʣ����ڿ��ܶ�̬��������������������
        if (m_pcMediaBudget)
        {
            m_pcMediaBudget->setTargetRateKbps(nTargetBitrateKbps);
        }

        llElapsedTimeMs = min(kMaxIntervalTimeMs, llElapsedTimeMs);

		//����padding��media��Ԥ��
        UpdateBudgetWithElapsedTime(llElapsedTimeMs);
    }

    TPR_BOOL bIsProbing = TPR_FALSE;
    int nBytesSent = 0;
    int nRecommendedProbeSize = 0;
    int iProbeClusterId = PacketInfo::kNotAProbe;

    if (m_pProber)
    {
        bIsProbing = m_pProber->IsProbing();
		//NPQ_DEBUG("[PACING]bIsProbing ? %d ",bIsProbing);
        if (bIsProbing)
        {
            iProbeClusterId = m_pProber->CurrentClusterId();
            nRecommendedProbeSize = m_pProber->RecommendedMinProbeSize();
        }
    }

    while (m_bStart && !m_pcPackets->Empty())
    {
        PacedPacket* packet = m_pcPackets->BeginPop();
        if (SendPacket(packet, iProbeClusterId)) 
        {
            nBytesSent += packet->bytes;
  
            m_pcPackets->FinalizePop(packet);
            if (bIsProbing && nBytesSent > nRecommendedProbeSize)
            {
				//nBytesSent���ܴ���nRecommendedProbeSize����������̽��Ŀ�����ʽϵ�ʱ�����ݴ��ڳ�����Ӱ��Ӧ��Ҳ����̫��
                break;
            }
        }
        else 
        {
            break;
        }
    }

    //m_pcPacketsΪ�գ�����ý�����ݻ���ʣ��˵��pacingԤ���Ѿ����꣬��ʱ�϶�����Ҫpadding
    if (m_pcPackets->Empty() && m_bStart)
    {
        if (m_nPacketCounter > 0) 
        {
			//Paddingͳ�Ƶ�Ԥ������ʣ��
            int nPaddingNeeded = bIsProbing ? (nRecommendedProbeSize - nBytesSent) : m_pcPaddingBudget->bytesRemaining();
            if (nPaddingNeeded > 0)
            {
                nBytesSent += SendPadding(nPaddingNeeded, iProbeClusterId);
            }
        }
    }

    if (bIsProbing && nBytesSent > 0)
    {
        m_pProber->ProbeSent(OneTime::OneNowMs(), nBytesSent);
    }

	m_pAlr->OnBytesSent(nBytesSent, llElapsedTimeMs);

	//NPQ_DEBUG("[PACING]run_time=%lld",TPR_TimeNow()-t2);
}
//FILE* pFpRTMP = fopen("e:/paddingData","wb+");
/**	@fn  PacedSender::SendPadding
*	@brief  ����padding
*	@param  padding_needed  padding�����ֽ���
*	@return int ���͵��ֽ���
*/
int PacedSender::SendPadding(int nPaddingNeeded, int iProbeClusterId) 
{
    int nRet = 0;
    if (RTP_MINLEN > nPaddingNeeded)
    {
        return 0;
    }
 
    int nBytesSent = 0;
    int nBytesLeft = nPaddingNeeded;
    //TBD ����padding
    while (nBytesLeft > RTP_MINLEN) 
    {
        unsigned char* pPacedPkt = NULL;
        unsigned int nDatalen = 0;
        nRet = m_sendedDataBuffer.FindFittingPacket(&pPacedPkt, &nDatalen, nBytesLeft);
        if (NPQ_OK != nRet)
        {
            NPQ_ERROR("FindFittingPacket err=%d", nRet);
            return 0;
        }

        //����RTP
        CRtpPacket RtpPacket;
        SdpInfo* pSdp = NULL;
        if(m_struOuterParam.m_sdpInfo.m_bValid)
        {
            pSdp = &m_struOuterParam.m_sdpInfo;
        }

        nRet = RtpPacket.Parse(pPacedPkt, nDatalen, pSdp);
        if(NPQ_OK != nRet)
        {
            NPQ_ERROR("parse RTPpacket error %x", nRet);
            return nRet;
        }
        RtpPacket.m_nArriveTimeUs = TPR_TimeNow();

        //�ش������Ӷ���padding����������,�����ڴ�ָ��padding
        CRetransPadding padding;
        nRet = padding.RetransPadding(RtpPacket);
        if(NPQ_OK != nRet)
        {
            NPQ_ERROR("RetransPadding err nRet=%#x",nRet);
            return 0;
        }

        //���봫�����չͷ�ٻص���ȥ
        if (NULL == m_paddExtension)
        {
            return 0;
        }
        nRet = m_paddExtension->AddTransExtension(RtpPacket, iProbeClusterId);
        if(NPQ_OK != nRet)
        {
            NPQ_ERROR("AddTransExtension err iRet=%#x",nRet);
            return 0;
        }

        nBytesLeft -= RtpPacket.m_nLen;//����Ϊ����
        //NPQ_DEBUG("nBytesLeft=%d, RtpPacket.m_nLen=%d RtpPacket.pt=%d",nBytesLeft,RtpPacket.m_nLen, RtpPacket.m_struHeader.iPayloadType);
        RtpPacket.m_bPaddingPacket = TPR_TRUE;
        if(m_struOuterParam.pCallback)
        {
            m_struOuterParam.DoCallback(RtpPacket);
        }
        //�����Ҫ���ͷ��ڴ�
        RtpPacket.FreeBuffer();
    }

    nBytesSent = nPaddingNeeded - nBytesLeft;
    if (nBytesSent > 0) 
    {
        UpdateBudgetWithBytesSent(nBytesSent);//����Ԥ�������
    }
    return nBytesSent;
}

/**	@fn  PacedSender::SendPacket
*	@brief  �������ݰ�
*	@param  pPacedPacket ���ݰ�
*	@return bool �����Ƿ�ɹ�
*/
TPR_BOOL PacedSender::SendPacket(PacedPacket* pPacedPacket, int iProbeClusterId) 
{
    int nRet = NPQ_OK;

	if(!pPacedPacket)
	{
		return TPR_FALSE;
	}

    if (kHigTPRiority != pPacedPacket->priority) 
     {
         //Ԥ����ʣ��
         if (m_pcMediaBudget->bytesRemaining() == 0 && iProbeClusterId == PacketInfo::kNotAProbe) 
         {
             return TPR_FALSE;
         }
     }

    //����RTP
    CRtpPacket RtpPacket;
    SdpInfo* pSdp = NULL;
    if(m_struOuterParam.m_sdpInfo.m_bValid)
    {
        pSdp = &m_struOuterParam.m_sdpInfo;
    }
   
    nRet = RtpPacket.Parse(pPacedPacket->pBuffer, pPacedPacket->nDataLen, pSdp);
    if(NPQ_OK != nRet)
    {
        NPQ_ERROR("parse RTPpacket error %x", nRet);
        return TPR_FALSE;
    }
    RtpPacket.m_nArriveTimeUs = TPR_TimeNow();

     if (m_struOuterParam.pCallback)
     {
         //Ҫ���͵������ȼ���sendedBuffer  ���뵽�Ѿ����͵����ݻ����� ֻ����ԭʼ����Ƶ��
         if (!pPacedPacket->retransmission && (kHigTPRiority != pPacedPacket->priority))
         {
             nRet = m_sendedDataBuffer.InsertPacket(pPacedPacket->pBuffer, pPacedPacket->nDataLen);
             if (NPQ_OK != nRet)
             {
                 NPQ_ERROR("insert to SendedBuffer=%x", nRet);
                 return TPR_FALSE;
             }
         }
         
         //���봫�����չͷ�ٻص���ȥ Video ����FECģ��
         if (NULL == m_paddExtension)
         {
             return TPR_FALSE;
         }

		 //��Ƶ����˽������(��������)����Ҫ����չͷ�� TBD
		 if(RtpPacket.m_enType == RTP_VIDEO)
		 {
			 nRet = m_paddExtension->AddTransExtension(RtpPacket, iProbeClusterId);
			 if(NPQ_OK != nRet)
			 {
				 NPQ_ERROR("AddTransExtension err iRet=%#x",nRet);
				 return TPR_FALSE;
			 }
		 }

         //���ݻص������
         m_struOuterParam.DoCallback(RtpPacket);

         if (0 == m_nLastSendTime || TPR_TRUE == RtpPacket.m_struHeader.bMarkerBit)
         {
             if ((!RtpPacket.IsRetransPacket()) && (RTP_VIDEO == RtpPacket.m_enType))
             {
                 //NPQ_WARN("[key]outputDiffS=%d",RtpPacket.m_nArriveTimeUs - m_nLastSendTime);
                 m_nLastSendTime = RtpPacket.m_nArriveTimeUs;
             }
         }

         //��Ƶ��FEC���
         if (RTP_VIDEO == RtpPacket.m_enType)
         {
             //��չͷ����FECһ�����
             if(m_struOuterParam.m_enQosConfig & QOS_TYPE_FEC)
             {
                 if (NULL == m_pFec)
                 {
                      return TPR_FALSE;
                 }
                 nRet = m_pFec->InputData(RtpPacket);//�ش�����FECģ���ֱ�ӻص���ȥ
                 if(NPQ_OK != nRet)
                 {
                     NPQ_ERROR("m_fec InputData err");
                     RtpPacket.FreeBuffer();
                     return TPR_FALSE;
                 }
             }
         }
     }

     //�����Ҫ���ͷ��ڴ�
     RtpPacket.FreeBuffer();
     //�������������Ƶ��Ӧ�ÿ��Ǹ����ȼ��İ�������
     if (kHigTPRiority != pPacedPacket->priority)
     {
         // Ԥ���м�ȥ��Ӧ���ֽ�
         UpdateBudgetWithBytesSent(RtpPacket.m_nLen);
     }

     return TPR_TRUE;
}

/**	@fn  PacedSender::UpdateBudgetWithElapsedTime
*	@brief  ����Ԥ��
*	@param  delta_time_ms Ԥ��ʱ��  
*	@return void
*/
void PacedSender::UpdateBudgetWithElapsedTime(TPR_INT64 nDeltaTimeMs) 
{
    if (m_pcMediaBudget)
    {
        m_pcMediaBudget->IncreaseBudget(nDeltaTimeMs);
    }

    if (m_pcPaddingBudget)
    {
        m_pcPaddingBudget->IncreaseBudget(nDeltaTimeMs);
    }
    
}

/**	@fn  PacedSender::UpdateBudgetWithBytesSent
*	@brief  ��ȥԤ��
*	@param  bytes_sent �Ѿ����͵��ֽ���  
*	@return void
*/
void PacedSender::UpdateBudgetWithBytesSent(int nBytesSent) 
{
    if (m_pcMediaBudget)
    {
        m_pcMediaBudget->UseBudget(nBytesSent);
    }
    
    if (m_pcPaddingBudget)
    {
        m_pcPaddingBudget->UseBudget(nBytesSent);
    }
}

void PacedSender::SetFecInterface(Fec* pFec)
{
    m_pFec = pFec;
}

int PacedSender::OutputData(CRtpPacket& rtpPacket)
{
    int nRet = NPQ_OK;
    if (NULL == m_paddExtension)
    {
        return NPQERR_GENRAL;
    }
    nRet = m_paddExtension->AddTransExtension(rtpPacket,  -1);//TBD?
    if(NPQ_OK != nRet)
    {
        NPQ_ERROR("FEC AddTransExtension err iRet=%#x",nRet);
        return nRet;
    }

    UpdateBudgetWithBytesSent(rtpPacket.m_nLen);

    m_struOuterParam.DoCallback(rtpPacket);
    rtpPacket.FreeBuffer();
    return NPQ_OK;
}

void PacedSender::CreateProbeCluster(TPR_INT64 llBitratebps) 
{
    TPR_Guard guard(&m_lock);
    if (m_pProber)
    {
		TPR_INT64 llNowMs = OneTime::OneNowMs();
        m_pProber->CreateProbeCluster(llBitratebps, llNowMs);
    }
}

