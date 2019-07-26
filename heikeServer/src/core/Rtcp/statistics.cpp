/**	@statistics.cpp
*	@note 111.
*	@brief ͳ��ʵ��
*
*	@author		222
*	@date		2016/11/14
*
*	@note 
*
*	@warning 
*/
#include "TPR_Guard.h"
#include "Util.h"
#include "timeUtil.h"
#include "statistics.h"

/**	@fn ReceiverStatistics::ReceiverStatistics
*	@brief ���캯��
*	@return 
*/
ReceiverStatistics::ReceiverStatistics()
:m_bFirst(TPR_TRUE)
,m_iPacketCountReset(0)
,m_iPacketCount(0)
,m_nMaxSeqNumber(0)
,m_nMaxSeqNumberReset(0)
,m_nBaseSeqNumber(0)
,m_iLastTransit(0)
,m_nLastTimestamp(0)
,m_fJitter(0)
{
}

/**	@fn ReceiverStatistics::~ReceiverStatistics
*	@brief ��������
*	@return 
*/
ReceiverStatistics::~ReceiverStatistics()
{
}

/**	@fn ReceiverStatistics::NotePacketStat
*	@brief �յ�RTP������
*	@param CRtpPacket& rtpPacket  rtp����Ϣ
*	@param TPR_INT64 nArriveTimeUs  ����ʱ��
*	@param TPR_BOOL bCheckType  �Ƿ�������ͱ�־
*	@return void
*/
void ReceiverStatistics::NotePacketStat( CRtpPacket& rtpPacket,TPR_INT64 nArriveTimeUs,TPR_BOOL bCheckType)
{
	unsigned short sSeq = rtpPacket.m_struHeader.sSeqNo;
	
	//FEC�����ش�����ͳ��
	//�����  TBD
	if(bCheckType)
	{
		if(rtpPacket.IsFecPacket() || rtpPacket.IsRetransPacket())
		{
			return;
		}
	}

	if(m_bFirst)
	{
		InitSeq(sSeq);
		m_nLastTimestamp = rtpPacket.m_struHeader.nTimestamp;
	}

	unsigned short sOldSeqNum = (unsigned short)(m_nMaxSeqNumber&0xFFFF);
	unsigned short sSeqNumCycle = (unsigned short)((m_nMaxSeqNumber&0xFFFF0000) >>16);

	if(IsNewerSequenceNumber(sSeq, sOldSeqNum))
	{
		m_iPacketCountReset++;
		m_iPacketCount++;

		if(sSeq < sOldSeqNum)
		{
			sSeqNumCycle++;
		}
		m_nMaxSeqNumber = (sSeqNumCycle<<16) | sSeq;
	}
	else
	{
		//�ش������������
		if(sSeq != sOldSeqNum)
		{
			NPQ_INFO("attention! sSeq = %d,sOldSeqNum=%d,m_nMaxSeqNumber=%d",sSeq,sOldSeqNum,m_nMaxSeqNumber);
		}
	}

	//jitter ÿһ֡����һ��
	if(rtpPacket.m_struHeader.iPayLoadFrequence > 0 && rtpPacket.m_struHeader.bMarkerBit)
	{
		unsigned int  freq = rtpPacket.m_struHeader.iPayLoadFrequence;
		TPR_INT64 arrivalOneUs = (TPR_INT64)nArriveTimeUs;
		TPR_UINT32 arrivalRtp = 
			(TPR_UINT32)((2.0*freq*arrivalOneUs + 1000000.0)/2000000);

		int transit = arrivalRtp - rtpPacket.m_struHeader.nTimestamp;
		if (m_iLastTransit == 0) 
		{
			m_iLastTransit = transit; 
		}

		int d = transit - m_iLastTransit;
		m_iLastTransit = transit;

		//NPQ_DEBUG("!!!!!!!!!!!!!!!!!!!!!! d=%d",d);

		if (d < 0) 
		{
			d = -d;
		}

		m_fJitter += (1.0/16.0) * ((double)d - m_fJitter);

		//NPQ_DEBUG("!!!!!!!!!!!!!!!!!!!!!! m_fJitter=%f",m_fJitter);
		//m_nLastTimestamp = rtpPacket.m_struHeader.nTimestamp;
		
		//debug
		if(rtpPacket.m_enType == RTP_VIDEO)
		{
			static TPR_TIME_T t1 = 0;
			static TPR_TIME_T t2 = 0;
			if(bCheckType)
			{
				NPQ_DEBUG("[key]InputDiffR=%lld",TPR_TimeNow()-t1);
				t1 = TPR_TimeNow();
			}
			else
			{
				NPQ_DEBUG("[key]outputDiffR=%lld",TPR_TimeNow()-t2);
				t2 = TPR_TimeNow();
			}
		}
	}
}

/**	@fn ReceiverStatistics::InitSeq
*	@brief ��ʼ�����
*	@param unsigned short sSeq  ���
*	@return void
*/
void ReceiverStatistics::InitSeq( unsigned short sSeq )
{
	m_nBaseSeqNumber = m_nMaxSeqNumberReset = m_nMaxSeqNumber = (0x10000 | (unsigned int)sSeq);
	m_bFirst = TPR_FALSE;
}

/**	@fn ReceiverStatistics::Reset
*	@brief ���ò���
*	@return void
*/
void ReceiverStatistics::Reset()
{
	m_iPacketCountReset = 0;
	m_nMaxSeqNumberReset = m_nMaxSeqNumber;
}

/**	@fn ReceiverStatisticsWrap::ReceiverStatisticsWrap
*	@brief ���캯��
*	@return 
*/
ReceiverStatisticsWrap::ReceiverStatisticsWrap()
:m_llRestTime(0)
,m_cbLossRate(0)
,m_iFecRecoveryCount(0)
,m_iRtxCount(0)
{

}

/**	@fn ReceiverStatisticsWrap::~ReceiverStatisticsWrap
*	@brief ��������
*	@return 
*/
ReceiverStatisticsWrap::~ReceiverStatisticsWrap()
{
	NPQ_DEBUG("[key]m_iRtxCount = %d,m_iFecRecoveryCount = %d",m_iRtxCount,m_iFecRecoveryCount);
}

/**	@fn ReceiverStatisticsWrap::NotePacketStat
*	@brief �յ�RTP������
*	@param CRtpPacket& rtpPacket  rtp����Ϣ
*	@return void
*/
void ReceiverStatisticsWrap::NotePacketStat( CRtpPacket& rtpPacket )
{
	TPR_Guard gurad(&m_lock);

	TPR_TIME_T llNow = TPR_TimeNow();

	m_statistics.NotePacketStat(rtpPacket,OneTime::OneNowUs(),TPR_FALSE);

	m_rate.Update(rtpPacket.m_nLen,OneTime::OneNowMs());
	
	if(llNow-m_llRestTime > 1*1000*1000)
	{
		m_cbLossRate = GetLossRate1();

		//if(m_cbLossRate!=0)
		{
			NPQ_INFO("[key]realLoss = %d",m_cbLossRate);
		}
		NPQ_INFO("real jitter = %d",m_statistics.Jitter());

		m_statistics.Reset();
		m_llRestTime = TPR_TimeNow();
	}

	if(rtpPacket.IsRetransPacket())
	{
		m_iRtxCount++;
	}

	if(rtpPacket.IsFecRecoveryPacket())
	{
		m_iFecRecoveryCount++;
	}

	if(rtpPacket.m_struHeader.bMarkerBit)
	{
		m_frameRate.UpdateFrame(OneTime::OneNowMs());
	}
}

/**	@fn ReceiverStatisticsWrap::GetLossRate
*	@brief ��ȡ���ն˶�����
*	@return ������
*/
unsigned char ReceiverStatisticsWrap::GetLossRate()
{
	return m_cbLossRate;
}

/**	@fn ReceiverStatisticsWrap::GetLossRate1
*	@brief ���㶪����
*	@return ������
*/
unsigned char ReceiverStatisticsWrap::GetLossRate1()
{
	int iRet;
	unsigned char byLossFractionQ8=0;
	
	//����trylock��������ܿ�ס
	iRet = m_lock.TryLock();
	if(iRet<0)
	{
		NPQ_ERROR("GetLossRate lock err");
		return 0;
	}

	unsigned int nMaxSeqNumber = m_statistics.MaxSeqNumber();//�յ����������
	unsigned int nExpectNum = nMaxSeqNumber - m_statistics.MaxSeqNumberReset();//�����յ��İ�����
	int nLostNum = nExpectNum - m_statistics.PacketCountReset();//��ʧ�İ�����

	if (nExpectNum == 0 || nLostNum < 0) 
	{
		byLossFractionQ8 = 0;
	} 
	else 
	{
		byLossFractionQ8 = (unsigned char)((nLostNum << 8) / nExpectNum); //������
	}

	m_lock.Unlock();

	return byLossFractionQ8;
}

/**	@fn ReceiverStatisticsWrap::GetJitter
*	@brief ��ȡ���ն˶���ֵ
*	@return ����ֵ
*/
unsigned int ReceiverStatisticsWrap::GetJitter()
{
	return m_statistics.Jitter();
}

TPR_UINT32 ReceiverStatisticsWrap::GetBitrate()
{
	return m_rate.Rate(OneTime::OneNowMs());
}

unsigned int ReceiverStatisticsWrap::GetFrameRate()
{
	return m_frameRate.FrameRate();
}


