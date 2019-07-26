/**	@statistics.h
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
#ifndef _STATISTICS_H_
#define _STATISTICS_H_

#include "TPR_Time.h"
#include "TPR_Mutex.h"
#include "rtp.h"
#include "rateStatistics.h"
#include "frameStatistics.h"

/**	@class	 ReceiverStatistics 
 *	@brief   ����ͳ��
 *	@note	 
 */
class ReceiverStatistics
{
public:
	/*���캯��*/
	ReceiverStatistics();

	/*��������*/
	~ReceiverStatistics();
	
	/*�յ�RTP������*/
	void NotePacketStat( CRtpPacket& rtpPacket,TPR_TIME_T nArriveTimeUs,TPR_BOOL bCheckType);

	/*���ò���*/
	void Reset();
	
	/*��ȡreset���յ��İ�����*/
	unsigned int PacketCountReset(){return m_iPacketCountReset;}
	
	/*��ȡreset���������*/
	unsigned int MaxSeqNumberReset(){return m_nMaxSeqNumberReset;}
	
	/*��ȡ������*/
	unsigned int MaxSeqNumber(){return m_nMaxSeqNumber;}
	
	/*��ȡ��ʼ���*/
	unsigned int BaseSeqNumber(){return m_nBaseSeqNumber;}
	
	/*��ȡ�յ��İ�����*/
	unsigned int PacketCount(){return m_iPacketCount;}
	
	/*��ȡ����ֵ*/
	unsigned int Jitter(){return (unsigned int)m_fJitter;}

private:
	void InitSeq(unsigned short sSeq);

	TPR_BOOL m_bFirst;					//�Ƿ��ǵ�һ�α�־
	int m_iPacketCountReset;			//���ݰ�������reset����0
	int m_iPacketCount;					//���ݰ�����
	unsigned int m_nMaxSeqNumber;		//���յ������ݰ������ţ���16λ��ʾ�����ѯ��������16λ��ʾ���ֵ
	unsigned int m_nMaxSeqNumberReset;	//���յ������ݰ������ţ�resetʱ��ֵ
	unsigned int m_nBaseSeqNumber;		//��¼��һ�����ݰ������
	int m_iLastTransit;					//��һ��RTP����ʱ��
	unsigned int m_nLastTimestamp;		//��һ��ʱ���
	double m_fJitter;					//ͳ�ƵĶ�������
};

/**	@class	 ReceiverStatisticsWrap 
 *	@brief   ����ͳ�Ʒ�װ��
 *	@note	 
 */
class ReceiverStatisticsWrap
{
public:
	/*���캯��*/
	ReceiverStatisticsWrap();

	/*��������*/
	~ReceiverStatisticsWrap();

	/*�յ�RTP������*/
	void NotePacketStat(CRtpPacket& rtpPacket);
	
	/*��ȡ������*/
	unsigned char GetLossRate();

	/*��ȡ����*/
	TPR_UINT32 GetBitrate();

	/*��ȡ֡��*/
	unsigned int GetFrameRate();
	
	/*��ȡ����ֵ*/
	unsigned int GetJitter();

private:
	/*�ն����ʼ���*/
	unsigned char GetLossRate1();

	ReceiverStatistics m_statistics;		//����ͳ�ƹ���(�����ʡ�����)
	RateStatistics m_rate;					//����ͳ��
	TPR_TIME_T m_llRestTime;				//����ʱ��
	TPR_Mutex m_lock;						//������
	TPR_UINT8 m_cbLossRate;					//������
	int m_iFecRecoveryCount;				//FEC�ָ��İ�����
	int m_iRtxCount;						//�ش�������
	FrameStatistics m_frameRate;			//֡��ͳ��
};
#endif

