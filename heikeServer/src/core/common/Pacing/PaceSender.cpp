/**	@PaceSender.cpp
*	@note 111.
*	@brief pacing类的实现
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


const TPR_INT64 kMaxQueueLengthMs = 2000;     //最大队列时长2秒 如果大于此值，应该丢掉该帧不编码 要求的比特率会变大
const float     kDefaultPaceMultiplier = 1.25f;//pacing码率是估计码率的2.5倍 增加了发送码率 减小了排队delay？
const TPR_INT64 kMinPacketLimitMs = 5;        // 包的burst间隔5ms
const TPR_INT64 kMaxIntervalTimeMs = 30;      //最大处理间隔30ms
#define  RTP_MINLEN                  (12)     //RTP包最小长度


/**	@fn  PacedSender::PacedSender
*	@brief 构造函数
*	@param  struOuterParam 外部参数
*	@param  TransportAddTExtension 传输层扩展头
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
*	@brief 析构函数
*	@param  无
*	@return void
*/
PacedSender::~PacedSender() {}


/**	@fn  PacedSender::Create
*	@brief 创建PacedSender类
*	@param  struOuterParam 外部参数
*	@param  TransportAddTExtension 传输层反馈类
*	@return 成功返回PacedSender* 类 失败返回NULL
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
*	@brief  销毁PacedSender类
*	@param  pcPacedSender     PacedSender类
*	@return int 错误码
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
*	@brief  初始化
*	@param  无
*	@return int 错误码
*/
int PacedSender::Init()
{
    int nRet = NPQ_OK;
	m_pcMediaBudget = new(std::nothrow)IntervalBudget(0);
    if (NULL == m_pcMediaBudget)
    {
        return NPQERR_SYSTEM;//外部删除
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
*	@brief  反初始化
*	@param  无
*	@return int 错误码
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
*	@brief  开始pacing
*	@param  无
*	@return int 错误码
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
*	@brief  停止pacing
*	@param  无
*	@return int 错误码
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
*	@brief  参数重置
*	@param  无
*	@return void
*/
void PacedSender::Reset()
{
   //TBD 外部调了stop 参数否需要重置?
    m_bPacketArrive = TPR_FALSE;
}

/**	@fn  PacedSender::SetEstimatedBitrate
*	@brief  设置估计的比特率
*	@param  bitrate_bps  比特率
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
        m_pcPaddingBudget->setTargetRateKbps(m_nEstimatedBitratebps / 1000); //设置padding的码率
    }

    m_nPacingBitratekbps = m_nEstimatedBitratebps / 1000 * kDefaultPaceMultiplier; //设置pacing的码率

	m_pAlr->SetEstimatedBitrate(nBitratebps); //正常码率
}

/**	@fn  PacedSender::InsertPacket
*	@brief  塞入数据包, 包含原始音视频数据、重传包、FEC
*	@param  rtpPacket  rtp包
*	@return int 错误码
*/
int PacedSender::InputData(CRtpPacket& rtpPacket)
{
    if (!m_bInit || !m_bStart)
    {
        return NPQERR_PRECONDITION;
    }

	//送FEC的原始数据 也会再送一次pacing 此时直接返回
	if(rtpPacket.m_bHaveCallback)
	{
		//NPQ_DEBUG("m_bHaveCallback");
		return NPQ_OK;
	}

	//FEC不过pacing队列，保证FEC的优先级
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
    pPacedPacket->bytes           = rtpPacket.m_nLen;//整个RTP包的长度
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
    m_pcPackets->Push(pPacedPacket);//优先级：音频包 》重传包 》视频包 同一个优先级的比较采集时间  采集时间相同的 比较加入队列的先后顺序
	
	//debug 打印输入帧间隔
	if(rtpPacket.m_struHeader.bMarkerBit && rtpPacket.m_enType == RTP_VIDEO && !rtpPacket.IsRetransPacket())
	{
		//static TPR_TIME_T t1 = 0;
		//NPQ_WARN("[key]intputDiffS=%lld",TPR_TimeNow()-t1);
		//t1 = TPR_TimeNow();
	}
    return NPQ_OK;
}

/**	@fn  PacedSender::PacingThread
*	@brief  pacing线程
*	@param  pUserData 用户数据指针
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
	
	//第一个数据包还没来，线程间隔5ms运行
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

	return npq_max(kMinPacketLimitMs - llElapsedTimeMs, 0); //正常情况下 5ms 发送一波数据
}

/**	@fn  PacedSender::PacingThreadRel
*	@brief  pacing线程实际处理函数
*	@param  无
*	@return int 错误码
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
*	@brief  pacing线程处理函数
*	@param  无
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
    TPR_INT64 llElapsedTimeMs = (llNowUs - m_llTimeLastUpdateUs + 500) / 1000;//四舍五入 第一次预算是5ms加上此时间
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
			//检查 通过队列算出的最小比特率是不是超过nTargetBitrateKbps，否则进行调整 
			//比如实际数据码率远远大于nTargetBitrateKbps会出现这种情况
            nMinBitrateNeededKbps = (int)(nQueueSizeBytes * 8 / nAvgtimeLeftMs); 
            if (nMinBitrateNeededKbps > nTargetBitrateKbps)
            {                                              
                nTargetBitrateKbps = nMinBitrateNeededKbps;
                NPQ_DEBUG("[PACING]pacing adjust bitrate=%d", nTargetBitrateKbps);
            }
#endif
        }
		
		//设置 media的码率，由于可能动态调整，所以在这里设置
        if (m_pcMediaBudget)
        {
            m_pcMediaBudget->setTargetRateKbps(nTargetBitrateKbps);
        }

        llElapsedTimeMs = min(kMaxIntervalTimeMs, llElapsedTimeMs);

		//增加padding和media的预算
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
				//nBytesSent可能大于nRecommendedProbeSize，尤其是在探测目标码率较低时，数据存在超发（影响应该也不会太大）
                break;
            }
        }
        else 
        {
            break;
        }
    }

    //m_pcPackets为空，否则媒体数据还有剩余说明pacing预算已经用完，此时肯定不需要padding
    if (m_pcPackets->Empty() && m_bStart)
    {
        if (m_nPacketCounter > 0) 
        {
			//Padding统计的预算尚有剩余
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
*	@brief  发送padding
*	@param  padding_needed  padding所需字节数
*	@return int 发送的字节数
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
    //TBD 发送padding
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

        //解析RTP
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

        //重传包增加额外padding，用于区分,数据内存指向padding
        CRetransPadding padding;
        nRet = padding.RetransPadding(RtpPacket);
        if(NPQ_OK != nRet)
        {
            NPQ_ERROR("RetransPadding err nRet=%#x",nRet);
            return 0;
        }

        //加入传输层扩展头再回调出去
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

        nBytesLeft -= RtpPacket.m_nLen;//可以为负数
        //NPQ_DEBUG("nBytesLeft=%d, RtpPacket.m_nLen=%d RtpPacket.pt=%d",nBytesLeft,RtpPacket.m_nLen, RtpPacket.m_struHeader.iPayloadType);
        RtpPacket.m_bPaddingPacket = TPR_TRUE;
        if(m_struOuterParam.pCallback)
        {
            m_struOuterParam.DoCallback(RtpPacket);
        }
        //如果需要，释放内存
        RtpPacket.FreeBuffer();
    }

    nBytesSent = nPaddingNeeded - nBytesLeft;
    if (nBytesSent > 0) 
    {
        UpdateBudgetWithBytesSent(nBytesSent);//更新预算的数据
    }
    return nBytesSent;
}

/**	@fn  PacedSender::SendPacket
*	@brief  发送数据包
*	@param  pPacedPacket 数据包
*	@return bool 发送是否成功
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
         //预算无剩余
         if (m_pcMediaBudget->bytesRemaining() == 0 && iProbeClusterId == PacketInfo::kNotAProbe) 
         {
             return TPR_FALSE;
         }
     }

    //解析RTP
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
         //要发送的数据先加入sendedBuffer  加入到已经发送的数据缓存中 只放入原始的视频包
         if (!pPacedPacket->retransmission && (kHigTPRiority != pPacedPacket->priority))
         {
             nRet = m_sendedDataBuffer.InsertPacket(pPacedPacket->pBuffer, pPacedPacket->nDataLen);
             if (NPQ_OK != nRet)
             {
                 NPQ_ERROR("insert to SendedBuffer=%x", nRet);
                 return TPR_FALSE;
             }
         }
         
         //加入传输层扩展头再回调出去 Video 送入FEC模块
         if (NULL == m_paddExtension)
         {
             return TPR_FALSE;
         }

		 //音频包、私有数据(后续考虑)包需要加扩展头吗？ TBD
		 if(RtpPacket.m_enType == RTP_VIDEO)
		 {
			 nRet = m_paddExtension->AddTransExtension(RtpPacket, iProbeClusterId);
			 if(NPQ_OK != nRet)
			 {
				 NPQ_ERROR("AddTransExtension err iRet=%#x",nRet);
				 return TPR_FALSE;
			 }
		 }

         //数据回调到外层
         m_struOuterParam.DoCallback(RtpPacket);

         if (0 == m_nLastSendTime || TPR_TRUE == RtpPacket.m_struHeader.bMarkerBit)
         {
             if ((!RtpPacket.IsRetransPacket()) && (RTP_VIDEO == RtpPacket.m_enType))
             {
                 //NPQ_WARN("[key]outputDiffS=%d",RtpPacket.m_nArriveTimeUs - m_nLastSendTime);
                 m_nLastSendTime = RtpPacket.m_nArriveTimeUs;
             }
         }

         //视频送FEC打包
         if (RTP_VIDEO == RtpPacket.m_enType)
         {
             //扩展头参与FEC一起计算
             if(m_struOuterParam.m_enQosConfig & QOS_TYPE_FEC)
             {
                 if (NULL == m_pFec)
                 {
                      return TPR_FALSE;
                 }
                 nRet = m_pFec->InputData(RtpPacket);//重传包送FEC模块会直接回调出去
                 if(NPQ_OK != nRet)
                 {
                     NPQ_ERROR("m_fec InputData err");
                     RtpPacket.FreeBuffer();
                     return TPR_FALSE;
                 }
             }
         }
     }

     //如果需要，释放内存
     RtpPacket.FreeBuffer();
     //如果分配带宽给音频，应该考虑高优先级的包？？？
     if (kHigTPRiority != pPacedPacket->priority)
     {
         // 预算中减去相应的字节
         UpdateBudgetWithBytesSent(RtpPacket.m_nLen);
     }

     return TPR_TRUE;
}

/**	@fn  PacedSender::UpdateBudgetWithElapsedTime
*	@brief  增加预算
*	@param  delta_time_ms 预算时间  
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
*	@brief  减去预算
*	@param  bytes_sent 已经发送的字节数  
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

