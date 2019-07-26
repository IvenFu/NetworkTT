

#ifndef _OUTERCTRL_H_
#define _OUTERCTRL_H_

#include "rateStatistics.h"
#include "statistics.h"

class OuterParam;
class OuterCtrlReceiver
{
public:
	OuterCtrlReceiver(OuterParam& struOuterParam);
	~OuterCtrlReceiver();

	/*��ȡ��Ƶ������*/
	unsigned char GetVideoLossRate();

	/*��ȡ��Ƶ��������*/
	TPR_UINT32 GetVideoBitrate();

	/*���²���ͳ��*/
	void UpdateVideoStatistics(CRtpPacket& rtpPacket);
private:
	ReceiverStatisticsWrap* m_pStatistics;//״̬ͳ��
	OuterParam& m_struOuterParam;
};

class OuterCtrlSender
{
public:
	OuterCtrlSender(OuterParam& struOuterParam);
	~OuterCtrlSender();

	/*��ȡͳ������*/
	void GetRate(TPR_UINT32* pVideo, TPR_UINT32* pTetrans,TPR_UINT32* pFec,TPR_UINT32* pPadding,TPR_INT64 nowMs);

	/*��������ͳ������*/
	void UpdateRateStatistics(CRtpPacket& rtpPacket,TPR_INT64 nowMs);

	int Init();
private:
	RateStatistics* m_pRetrans;	//�ش�����ͳ��
	RateStatistics* m_pFec;		//FEC����ͳ��
	RateStatistics* m_pVideo;	//��Ƶ��������ͳ��
	RateStatistics* m_pPadding;	//padding����ͳ��
	TPR_BOOL m_bInit;			//��ʼ�����
	OuterParam& m_struOuterParam;
};










#endif

















