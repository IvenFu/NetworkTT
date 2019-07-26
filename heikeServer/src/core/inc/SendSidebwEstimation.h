
#ifndef _SENDSIDEBWESTIMATION_H_
#define _SENDSIDEBWESTIMATION_H_

#include "TPR_Mutex.h"
#include <deque>

using namespace std;

class SendSidebwEstimation 
{
public:
	/*���캯��*/
	SendSidebwEstimation();

	/*��������*/
	virtual ~SendSidebwEstimation();
	
	/*ֱ������Ŀ������*/
	void SetSendBitrate(int bitrate);
	
	/*��ȡ��ǰ���ƽ��*/
	void CurrentEstimate(int* bitrate, TPR_UINT8* loss, TPR_INT64* rtt);

	/*��RTCP����㷴����������*/
	void UpdateDelayBasedEstimate(TPR_INT64 now_ms, TPR_UINT32 bitrate_bps);

	/*��RTCP RR���津������*/
	void UpdateReceiverBlock(TPR_UINT8 fraction_loss, TPR_INT64 rtt,int number_of_packets,TPR_INT64 now_ms);
	
	/*����ӵ�����ڵ����ʷ�Χ*/
	void SetMinMaxBitrate(int min_bitrate, int max_bitrate);

	/*���̵߳��õĸ���*/
	void UpdateProcessEstimate(TPR_INT64 now_ms);
private:
	/*���¹���*/
	void UpdateEstimate(TPR_INT64 now_ms);

	TPR_BOOL IsInStartPhase(TPR_INT64 now_ms) const;

	/*�����趨�����������޹�������ֵ*/
	TPR_UINT32 CapBitrateToThresholds(TPR_INT64 now_ms, TPR_UINT32 bitrate);

	//// Updates history of min bitrates.
	//// After this method returns min_bitrate_history_.front().second contains the
	//// min bitrate used during last kBweIncreaseIntervalMs.
	void UpdateMinHistory(TPR_INT64 now_ms);

	deque<pair<TPR_INT64, TPR_UINT32> > min_bitrate_history_;

	int lost_packets_since_last_loss_update_Q8_;		//��������֮�� ��ʧ�İ�����
	int expected_packets_since_last_loss_update_;		//��������֮�� �����õ��İ�����

	TPR_UINT32 bitrate_;
	TPR_UINT32 min_bitrate_configured_;
	TPR_UINT32 max_bitrate_configured_;

	TPR_BOOL has_decreased_since_last_fraction_loss_;	//���ڶ����ʣ��Ѿ��½������ʱ�־
	TPR_INT64 last_feedback_ms_;						//��¼���ڶ����� �յ�RR�����ʱ��
	TPR_INT64 last_packet_report_ms_;					//��¼���ڶ����� RR�������UpdateEstimate��ʱ��
	TPR_INT64 last_timeout_ms_;							//��¼ÿ�γ�ʱ����ʱ��
	TPR_UINT8 last_fraction_loss_;						//�����ʣ���λ1/256 
	TPR_INT64 last_round_trip_time_ms_;					//RTT

	TPR_UINT32 delay_based_bitrate_bps_;
	TPR_INT64 time_last_decrease_ms_;					//��¼�µ����ʵ�ʱ��
	TPR_INT64 first_report_time_ms_;					//���ڶ����� ��һ��RR�����ʱ��
	TPR_Mutex m_lock;
};







#endif

