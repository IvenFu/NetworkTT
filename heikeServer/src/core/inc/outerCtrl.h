

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

	/*获取视频丢包率*/
	unsigned char GetVideoLossRate();

	/*获取视频最终码率*/
	TPR_UINT32 GetVideoBitrate();

	/*更新参数统计*/
	void UpdateVideoStatistics(CRtpPacket& rtpPacket);
private:
	ReceiverStatisticsWrap* m_pStatistics;//状态统计
	OuterParam& m_struOuterParam;
};

class OuterCtrlSender
{
public:
	OuterCtrlSender(OuterParam& struOuterParam);
	~OuterCtrlSender();

	/*获取统计码率*/
	void GetRate(TPR_UINT32* pVideo, TPR_UINT32* pTetrans,TPR_UINT32* pFec,TPR_UINT32* pPadding,TPR_INT64 nowMs);

	/*更新码率统计样本*/
	void UpdateRateStatistics(CRtpPacket& rtpPacket,TPR_INT64 nowMs);

	int Init();
private:
	RateStatistics* m_pRetrans;	//重传码率统计
	RateStatistics* m_pFec;		//FEC码率统计
	RateStatistics* m_pVideo;	//视频数据码率统计
	RateStatistics* m_pPadding;	//padding码率统计
	TPR_BOOL m_bInit;			//初始化标记
	OuterParam& m_struOuterParam;
};










#endif

















