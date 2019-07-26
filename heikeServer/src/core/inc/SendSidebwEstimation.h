
#ifndef _SENDSIDEBWESTIMATION_H_
#define _SENDSIDEBWESTIMATION_H_

#include "TPR_Mutex.h"
#include <deque>

using namespace std;

class SendSidebwEstimation 
{
public:
	/*构造函数*/
	SendSidebwEstimation();

	/*析构函数*/
	virtual ~SendSidebwEstimation();
	
	/*直接设置目标码率*/
	void SetSendBitrate(int bitrate);
	
	/*获取当前估计结果*/
	void CurrentEstimate(int* bitrate, TPR_UINT8* loss, TPR_INT64* rtt);

	/*由RTCP传输层反馈触发调用*/
	void UpdateDelayBasedEstimate(TPR_INT64 now_ms, TPR_UINT32 bitrate_bps);

	/*由RTCP RR报告触发调用*/
	void UpdateReceiverBlock(TPR_UINT8 fraction_loss, TPR_INT64 rtt,int number_of_packets,TPR_INT64 now_ms);
	
	/*设置拥塞调节的码率范围*/
	void SetMinMaxBitrate(int min_bitrate, int max_bitrate);

	/*由线程调用的更新*/
	void UpdateProcessEstimate(TPR_INT64 now_ms);
private:
	/*更新估计*/
	void UpdateEstimate(TPR_INT64 now_ms);

	TPR_BOOL IsInStartPhase(TPR_INT64 now_ms) const;

	/*根据设定的码率上下限过滤码率值*/
	TPR_UINT32 CapBitrateToThresholds(TPR_INT64 now_ms, TPR_UINT32 bitrate);

	//// Updates history of min bitrates.
	//// After this method returns min_bitrate_history_.front().second contains the
	//// min bitrate used during last kBweIncreaseIntervalMs.
	void UpdateMinHistory(TPR_INT64 now_ms);

	deque<pair<TPR_INT64, TPR_UINT32> > min_bitrate_history_;

	int lost_packets_since_last_loss_update_Q8_;		//两次运算之间 丢失的包个数
	int expected_packets_since_last_loss_update_;		//两次运算之间 期望得到的包总数

	TPR_UINT32 bitrate_;
	TPR_UINT32 min_bitrate_configured_;
	TPR_UINT32 max_bitrate_configured_;

	TPR_BOOL has_decreased_since_last_fraction_loss_;	//基于丢包率，已经下降了码率标志
	TPR_INT64 last_feedback_ms_;						//记录基于丢包率 收到RR报告的时间
	TPR_INT64 last_packet_report_ms_;					//记录基于丢包率 RR报告调用UpdateEstimate的时间
	TPR_INT64 last_timeout_ms_;							//记录每次超时处理时间
	TPR_UINT8 last_fraction_loss_;						//丢包率，单位1/256 
	TPR_INT64 last_round_trip_time_ms_;					//RTT

	TPR_UINT32 delay_based_bitrate_bps_;
	TPR_INT64 time_last_decrease_ms_;					//记录下调码率的时间
	TPR_INT64 first_report_time_ms_;					//基于丢包率 第一次RR报告的时间
	TPR_Mutex m_lock;
};







#endif

