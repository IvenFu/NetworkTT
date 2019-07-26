/**	@rtcpReceiver.cpp
*	@note 111.
*	@brief rtcp 接收端派生类
*
*	@author		222
*	@date		2017/11/02
*
*	@note 
*
*	@warning 
*/
#include "rtcpReceiver.h"

/**	@fn RtcpReceiver::RtcpReceiver
*	@brief 构造函数
*	@param  OuterParam& struOuterParam 外部参数
*	@param   InnerParam& innerParam 内部参数
*	@param   RtcpCallback& callback rtcp回调
*	@return  
*/
RtcpReceiver::RtcpReceiver(OuterParam& struOuterParam,InnerParam& innerParam,RtcpCallback& callback)
:RTCP(struOuterParam,innerParam,callback)
,m_uMediaSsrc(innerParam.m_enMainType == NPQ_MAIN_AUDIO ? RTCP_AUDIO_SSRC:RTCP_VIDEO_SSRC)
,m_bySeqFir(0)
{

}

/**	@fn RtcpReceiver::~RtcpReceiver
*	@brief 析构函数
*	@return  
*/
RtcpReceiver::~RtcpReceiver()
{

}




int RtcpReceiver::ReportPLI()
{
	OutputRtcp(OUTPUT_PLI,NULL);
	return NPQ_OK;
}

int RtcpReceiver::ReportFIR()
{
	OutputRtcp(OUTPUT_FIR,NULL);
	return NPQ_OK;
}

int RtcpReceiver::ReportRemb(TPR_INT64 llBitratebps)
{
	RTCP_SPEC_FB_REMB_INFO info;

	info.llBitratebps = llBitratebps;
	info.aSsrcs = m_aSsrcs;

	OutputRtcp(OUTPUT_REMB,(void*)&info);
	return NPQ_OK;
}

/**	@fn RtcpReceiver::AddSsrc
*	@brief 增加ssrc
*	@param  unsigned int uSsrc 媒体ssrc
*	@return  
*/
void RtcpReceiver::AddSsrc(unsigned int uSsrc, TPR_BOOL bRealMedia)
{
	TPR_BOOL bNewSsrc = TPR_TRUE;
	vector<TPR_UINT32>::iterator it = m_aSsrcs.begin();

	for(;it!=m_aSsrcs.end();++it)
	{
		if(*it == uSsrc)
		{
			bNewSsrc = TPR_FALSE;
			break;
		}
	}
	
	if(bNewSsrc)
	{
		m_aSsrcs.push_back(uSsrc);
	}
	
	if(bRealMedia)
	{
		m_uMediaSsrc = uSsrc;
	}
}

/**	@fn RtcpReceiver::BuildRtcpBasic
*	@brief 构建RTCP报文
*	@param  RTCP_OUTPUT_TYPE enOutputType  需要构建的报文类型
*	@param void* pBuildPara 构建参数
*	@return  NPQ_OK
*/
int RtcpReceiver::BuildRtcpBasic( RTCP_OUTPUT_TYPE enOutputType, void* pBuildParam)
{
	if(enOutputType == OUTPUT_BASIC)
	{		
		AddRR();
		AddXRReciveReferenceTimeReport();

		ReceiverStatistics* pReStat = m_rtcpCallback.GetReceiverStatisticsInterface();
		if(!pReStat)
		{
			NPQ_ERROR("pReStat null");
			return NPQERR_GENRAL;
		}
		pReStat->Reset();
	}
	else if(enOutputType == OUTPUT_NACK)
	{

	}
	else if(enOutputType == OUTPUT_TRANSPORT)
	{

	}
	else if(enOutputType == OUTPUT_PLI)
	{
		AddPli();
	}
	else if(enOutputType == OUTPUT_FIR)
	{
		AddFir();
	}
	else if(enOutputType == OUTPUT_REMB)
	{
		if(!pBuildParam)
		{
			return NPQERR_PARA;
		}
		AddRemb((RTCP_SPEC_FB_REMB_INFO*)pBuildParam);
	}

	return NPQ_OK;
}

/**	@fn RtcpReceiver::AddRR
*	@brief 构建RR报文
*	@return  NPQ_OK
*/
int RtcpReceiver::AddRR()
{
	AddCommonHead(RTCP_PT_RR,7,1); // (6*numReportingSources + 2) -1 = 7
	AddRRBlock();

	return NPQ_OK;
}

/**	@fn RtcpReceiver::AddRRBlock
*	@brief 构建RR块信息
*	@return  NPQ_OK
*/
int RtcpReceiver::AddRRBlock()
{
	unsigned int SSRC = m_uMediaSsrc;
	unsigned char lossFraction = 0;

	m_buffer.EnqueueDWord(SSRC);

	ReceiverStatistics* pReStat = m_rtcpCallback.GetReceiverStatisticsInterface();
	if(!pReStat)
	{
		NPQ_ERROR("pReStat null");
		return NPQERR_GENRAL;
	}

	unsigned int nMaxSeqNumber = pReStat->MaxSeqNumber();//收到的最大包序号
	unsigned int nExpectNum = nMaxSeqNumber - pReStat->MaxSeqNumberReset();//期望收到的包个数
	int nLostNum = nExpectNum - pReStat->PacketCountReset();//丢失的包个数

	//NPQ_DEBUG("nExpectNum = %d,realConut =%d, nLostNum=%d",nExpectNum, pReStat->PacketCountReset(), nLostNum);
	if (nExpectNum == 0 || nLostNum < 0) 
	{
		lossFraction = 0;
	} 
	else 
	{
		lossFraction = (unsigned char)((nLostNum << 8) / nExpectNum); //丢包率
	}

	unsigned int totNumExpected = nMaxSeqNumber - pReStat->BaseSeqNumber();//累计期望收到的包个数
	int totNumLost = totNumExpected - pReStat->PacketCount();//累计丢失的包
	if (totNumLost > 0x007FFFFF) 
	{
		totNumLost = 0x007FFFFF;
	}
	else if(totNumLost<0)
	{
		totNumLost = 0;
	}

	//NPQ_DEBUG("!!!!m_enMainType =%d,lossFraction = %f",m_innerParam.m_enMainType, (float)lossFraction/256);
	//if(m_innerParam.m_enMainType == NPQ_MAIN_AUDIO)
	{
		NPQ_DEBUG("[key]LossRate = %.02f",(float)lossFraction/256.0);
	}

	m_buffer.EnqueueDWord((lossFraction<<24) | totNumLost);//丢包率 以及 累计丢包个数
	m_buffer.EnqueueDWord(nMaxSeqNumber);
	m_buffer.EnqueueDWord(pReStat->Jitter());//jitter

	RTCP_SR_INFO info = {0};
	m_rtcpCallback.GetSrInfo(&info);

	unsigned int NTPmsw = info.nLastSRNTPmsw;  //单位秒
	unsigned int NTPlsw = info.nLastSRNTPlsw;  //单位232ps    1000 000 000 000 /2^32  = 232

	unsigned int LSR = 0;
	NtpTime::NtpShortTime(info.nLastSRNTPmsw,info.nLastSRNTPlsw,LSR);//截取NTP时间，防止NTP到2036年无法使用

	m_buffer.EnqueueDWord(LSR);

	TPR_TIME_T now = TPR_TimeNow();
	TPR_TIME_T delay = 0;
	unsigned int delayS = 0;
	unsigned int delayUs = 0;

	if(info.nlastReceiveSRTime>0 && now>info.nlastReceiveSRTime)
	{
		delay = now-info.nlastReceiveSRTime;// 算delay 是相对时间，不用NTP时间
		delayS = delay/1000000;
		delayUs = delay%1000000;
	}

	unsigned int DLSR=0;
	if (LSR == 0) 
	{
		DLSR = 0;
	} 
	else 
	{
		NtpTime::NtpRealToNtpTime(delayS,delayUs,DLSR);
	}
	m_buffer.EnqueueDWord(DLSR);

	m_innerParam.m_cLossFraction = lossFraction;//保存丢包率参数

	if(pReStat->Jitter()!=0)
	{
		NPQ_INFO("[iinfo]jitter =%d",pReStat->Jitter());
	}
	return NPQ_OK;
}

/**	@fn RtcpReceiver::AddXRReciveReferenceTimeReport
*	@brief 构建XR ReciveReferenceTimeReport
*	@return  NPQ_OK
*/
int RtcpReceiver::AddXRReciveReferenceTimeReport()
{
	AddCommonHead(RTCP_PT_XR, 4, 0); //(12+8)/4 -1 = 4
	AddXRBlock(RTCP_XR_BT_REFERENCETIME);

	return NPQ_OK;
}

/**	@fn RtcpReceiver::AddXRBlock
*	@brief 构建XR 块信息
*	@return  NPQ_OK
*/
int RtcpReceiver::AddXRBlock(RTCP_XR_BT_TYPE enXRBT)
{
	unsigned int nFir = 0;

	// rfc3611
	//    0                   1                   2                   3
	//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//   |     BT=4      |   reserved    |       block length = 2        |
	//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//   |              NTP timestamp, most significant word             |
	//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//   |             NTP timestamp, least significant word             |
	//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	// Add Receiver Reference Time Report block.

	nFir |= enXRBT<<24;  //BT
	nFir |= (2);  //block length

	m_buffer.EnqueueDWord(nFir);

	unsigned int nSec;
	unsigned int nFrac;
	NtpTime::NtpNow(nSec,nFrac);

	m_buffer.EnqueueDWord(nSec);
	m_buffer.EnqueueDWord(nFrac);
	return NPQ_OK;
}

/**	@fn RtcpReceiver::AddPli
*	@brief 构建Pli报文
*	@return  NPQ_OK
*/
int RtcpReceiver::AddPli()
{
	// rfc4585
	//0                   1                   2                   3
	//0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//|V=2|P| FMT(1) |   PT(206)      |          length               |
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//|                  SSRC of packet sender                        |
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//|                  SSRC of media source                         |
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//:            Feedback Control Information (FCI)                 :

	unsigned int iRtcpHdr = 0x80000000; // version 2, no padding
	unsigned int iOurSSRC = m_cOurSsrc; 
	unsigned int iRemoteSSRC = m_uMediaSsrc; 

	iRtcpHdr |= (RTCP_SPEC_FB_FMI_PLI<<24);   //FMT
	iRtcpHdr |= (RTCP_PT_SPEC_FEEDBACK<<16);   //PT

	int iLen = 2;  // (4+8)/4 -1 = 2 
	iRtcpHdr |= iLen;

	m_buffer.EnqueueDWord(iRtcpHdr);
	m_buffer.EnqueueDWord(iOurSSRC);
	m_buffer.EnqueueDWord(iRemoteSSRC);

	//FCI为空

	return NPQ_OK;
}

/**	@fn RtcpReceiver::AddFir
*	@brief 构建FIR报文
*	@return  NPQ_OK
*/
int RtcpReceiver::AddFir()
{
	// RFC 4585: Feedback format.
	// Common packet format:
	//
	//   0                   1                   2                   3
	//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//  |V=2|P|   FMT   |       PT      |          length               |
	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//  |                  SSRC of packet sender                        |
	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//  |             SSRC of media source (unused) = 0                 |
	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//  :            Feedback Control Information (FCI)                 :
	//  :                                                               :
	// Full intra request (FIR) (RFC 5104).
	// The Feedback Control Information (FCI) for the Full Intra Request
	// consists of one or more FCI entries.
	// FCI:
	//   0                   1                   2                   3
	//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//  |                              SSRC                             |
	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//  | Seq nr.       |    Reserved = 0                               |
	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	unsigned int iRtcpHdr = 0x80000000; // version 2, no padding
	unsigned int iOurSSRC = m_cOurSsrc; 
	unsigned int iRemoteSSRC = m_uMediaSsrc; 

	iRtcpHdr |= (RTCP_SPEC_FB_FMI_FIR<<24);   //FMT
	iRtcpHdr |= (RTCP_PT_SPEC_FEEDBACK<<16);   //PT

	int iLen = 4;//(4+ 8 + 8*N)/4 -1 = 4 ;  N=1 
	iRtcpHdr |= iLen;

	m_buffer.EnqueueDWord(iRtcpHdr);
	m_buffer.EnqueueDWord(iOurSSRC);

	m_buffer.EnqueueDWord(0); //SSRC of media source (unused) = 0 

	m_buffer.EnqueueDWord(iRemoteSSRC); //FCI   SSRC 
	m_buffer.Enqueue(&m_bySeqFir,1); //FCI   seq 

	m_bySeqFir++;
	unsigned char byTemp = 0;
	m_buffer.Enqueue(&byTemp,3); //FCI   reserved 

	return NPQ_OK;
}

int RtcpReceiver::AddRemb(RTCP_SPEC_FB_REMB_INFO* pInfo)
{
	// draft-alvestrand-rmcat-remb-03 - RTCP message for Receiver Estimated Maximum Bitrate 
	//  0                   1                   2                   3
	//	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//	|V=2|P| FMT=15  |   PT=206      |             length            |
	//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//	|                  SSRC of packet sender                        |
	//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//	|                  SSRC of media source                         |
	//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//	|  Unique identifier 'R' 'E' 'M' 'B'                            |
	//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//	|  Num SSRC     | BR Exp    |  BR Mantissa                      |
	//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//	|   SSRC feedback                                               |
	//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//	|  ...      

	if(!pInfo || pInfo->llBitratebps <= 0 || pInfo->aSsrcs.empty())
	{
		if(pInfo)
		{
			NPQ_ERROR("llBitratebps=%lld,aSsrcs size is %d", pInfo->llBitratebps,pInfo->aSsrcs.size());
		}
		return NPQERR_PARA;
	}

	unsigned int iRtcpHdr = 0x80000000; // version 2, no padding
	unsigned int iOurSSRC = m_cOurSsrc; 

	iRtcpHdr |= (RTCP_SPEC_FB_FMI_REMB<<24);   //FMT
	iRtcpHdr |= (RTCP_PT_SPEC_FEEDBACK<<16);   //PT

	int iLen = (4+8+(2+pInfo->aSsrcs.size())*4)/4 -1  ; //(4+ 8 + 8 + n*ssrc)/4 -1; 
	iRtcpHdr |= iLen;

	m_buffer.EnqueueDWord(iRtcpHdr);
	m_buffer.EnqueueDWord(iOurSSRC);
	m_buffer.EnqueueDWord(0); //SSRC of media source (unused) = 0 
	
	int iTmep = 0x52454D42;  // 'R' 'E' 'M' 'B'.
	m_buffer.EnqueueDWord(iTmep);
	
	TPR_UINT8 cTemp = pInfo->aSsrcs.size();
	m_buffer.Enqueue(&cTemp,1);

	const TPR_UINT32 iMaxMantissa = 0x3ffff;  // 18 bits.
	TPR_UINT64 llMantissa = pInfo->llBitratebps;
	TPR_UINT8 cExponenta = 0;

	// bitrate = Mantissa* 2^Exp
	while (llMantissa > iMaxMantissa) 
	{
		llMantissa >>= 1;
		++cExponenta;
	}
	
	cTemp = (cExponenta << 2) | (llMantissa >> 16);
	m_buffer.Enqueue(&cTemp,1);

	TPR_UINT16 sTemp = llMantissa&0xffff;
	m_buffer.EnqueueWord(sTemp);

	vector<TPR_UINT32>::iterator it = pInfo->aSsrcs.begin();
	for(;it!=pInfo->aSsrcs.end();++it)
	{
		m_buffer.EnqueueDWord(*it);
	}

	return NPQ_OK;
}



