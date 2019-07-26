/**	@receiverBase.h
*	@note 111.
*	@brief receiverBaseʵ��
*
*	@author		222
*	@date		2017/8/28
*
*	@note 
*
*	@warning 
*/

#ifndef _RECEIVERBASE_H_
#define _RECEIVERBASE_H_

#include "NPQosImpl.h"
#include "callback.h"
#include "rtcpReceiver.h"
#include "rtcpCallback.h"
#include "statistics.h"
#include "rateStatistics.h"

class CRtcpPacket;
class RtcpReceiver;
class RtcpCallback;
class ReceiverStatistics;

/**	@class	 ReceiverBase 
 *	@brief   ���ն˴������
 *	@note	 
 */
class ReceiverBase
{
public:
	/*���캯��*/
	ReceiverBase(OuterParam& struOuterParam,NPQ_QOS_MAIN_TYPE enMainType);
	
	/*��������*/
	~ReceiverBase();
	
	/*��ʼ��*/
	int Init();
	
	/*����ʼ��*/
	int Fini();
	
	/*��������*/
	int Start();
	
	/*ֹͣ����*/
	int Stop();
	
	/*����RTP����*/
	int InputData(CRtpPacket& RtpPacket);
	
	/*����RTCP����*/
	int InputDataRtcp(CRtcpPacket& RtpPacket);
	
	/*��ȡ״̬*/
	int GetStat (NPQ_STAT* pStat);
	
	/*��ȡsr������Ϣ*/
	int GetSrInfo( RTCP_SR_INFO* pInfo);

protected:
	RtcpReceiver* m_pRtcp;					//RTCP
	RtcpCallback* m_pRtcpCallback;			//RTCP�ص�
	
	OuterParam& m_struOuterParam;			//�ⲿ��������
	InnerParam m_innerParam;				//�ڲ�����
private:
	TPR_BOOL m_bStartBase;					//�Ƿ����̱߳�־
	ReceiverStatistics* m_pStat;			//����״̬ͳ��
	RateStatistics* m_pBitrateStatistics;	//��������ͳ��
};
#endif

