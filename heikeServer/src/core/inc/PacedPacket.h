/**	@PacePacket.h
*	@note 111.
*	@brief pacing packet����
*
*	@author		333
*	@date		2017/2/13
*
*	@note 
*
*	@warning 
*/
#ifndef _PACEDPACKET_H_
#define _PACEDPACKET_H_
#include <list>
#include <queue>
#include <map>
#include <set>
#include "timeUtil.h"

using std::list;
#define  RTP_PACKET_LEN 1500
enum Priority 
{
	kHigTPRiority   = 0,    // Pass through; will be sent immediately.
	kNormalPriority = 2,  // Put in back of the line.
	kLowPriority    = 3,     // Put in back of the low priority line.
};

struct PacedPacket 
{
    Priority priority;
    TPR_INT64 capture_time_us;
    TPR_INT64 enqueue_time_ms;
    int bytes;
    bool retransmission;
    TPR_UINT64 enqueue_order;
    unsigned char pBuffer[RTP_PACKET_LEN];
    unsigned int nDataLen;
};

//typedef list<PacedPacket*> PacedPackets;

//���ȼ����бȽ���
struct Comparator 
{
    bool operator()(const PacedPacket* first, const PacedPacket* second) 
    {
        if (first->priority != second->priority)
        {
            return first->priority > second->priority;
        }

        // �ش�������
        if (second->retransmission != first->retransmission)
        {
            return second->retransmission > first->retransmission;
        }

        // �ɼ�ʱ��ȷ�����ȼ�
        if (first->capture_time_us != second->capture_time_us)
        {
            return first->capture_time_us > second->capture_time_us;
        }

        return first->enqueue_order > second->enqueue_order;
    }
};


// Class encapsulating a priority queue with some extensions.
class PacketQueue 
{
public:
    explicit PacketQueue()
        : bytes_(0),
        queue_time_sum_(0),
        time_last_updated_(OneTime::OneNowMs()) {}
    virtual ~PacketQueue() {}

    void Push(PacedPacket* packet) 
    {
        UpdateQueueTime(packet->enqueue_time_ms);

        //�Ѱ������б��� ���ȶ����е�ָ�������ƶ����б���packet��һ��ָ���Լ���ָ��
        prio_queue_.push(packet);  // ָ��list��ָ��
        bytes_ += packet->bytes;
    }

    PacedPacket* BeginPop() 
    {
        PacedPacket* packet = prio_queue_.top();
        //prio_queue_.pop();
        //NPQ_DEBUG("prio_queue_.size=%d",prio_queue_.size());
        return packet;
    }

    void CancelPop(PacedPacket* packet) 
    { 
        prio_queue_.push(packet); 
    }

    void FinalizePop(PacedPacket* packet) 
    {
		if(!packet)
		{
			return;
		}

        bytes_ -= packet->bytes;
        if (time_last_updated_ >= packet->enqueue_time_ms)
        {
            queue_time_sum_ -= (time_last_updated_ - packet->enqueue_time_ms);
        }
        else
        {
            NPQ_ERROR("time error!");
        }
        prio_queue_.pop();
        SAFE_DEL(packet);
    }

    bool Empty() const 
    {
        return prio_queue_.empty(); 
    }

    TPR_UINT64 SizeInBytes() const 
    { 
        return bytes_; 
    }

    void UpdateQueueTime(TPR_INT64 timestamp_ms) 
    {
        if (timestamp_ms < time_last_updated_)
        {
            NPQ_ERROR("timestamp_ms < time_last_updated_!!!!");
        }
        TPR_INT64 delta = timestamp_ms - time_last_updated_;
        //����ʹ��packet_list�Ĵ�С���������ȶ��еĴ�С����Ϊ���ȶ����п���������ȡһ��Ԫ��
        queue_time_sum_ += delta * prio_queue_.size();
        time_last_updated_ = timestamp_ms;
    }

    TPR_INT64 AverageQueueTimeMs() const 
    {
        if (prio_queue_.empty())
        {
            return 0;
        }
        return queue_time_sum_ / prio_queue_.size();
    }

private:

    //���ȶ���ͨ���Ƚ����Ƚ��Ⱥ�˳��
    std::priority_queue<PacedPacket*, std::vector<PacedPacket*>, Comparator> prio_queue_;
    //�������ֽ���
    TPR_UINT64    bytes_;
    TPR_INT64     queue_time_sum_;
    TPR_INT64     time_last_updated_;
};


//���Ԥ����
class IntervalBudget 
{
public:
    explicit IntervalBudget(int initial_target_rate_kbps,bool can_build_up_underuse)
        : target_rate_kbps_(initial_target_rate_kbps),
        bytes_remaining_(0),
		can_build_up_underuse_(can_build_up_underuse) 
	{}

	explicit IntervalBudget(int initial_target_rate_kbps)
	{
		IntervalBudget(initial_target_rate_kbps, false);
	}

    //����Ŀ������
    void setTargetRateKbps(int target_rate_kbps) 
    {
        target_rate_kbps_ = target_rate_kbps;
		max_bytes_in_budget_ = (kWindowMs * target_rate_kbps_) / 8;  //@500ms ���ֽ���
		bytes_remaining_ = npq_min(npq_max(-max_bytes_in_budget_, bytes_remaining_),max_bytes_in_budget_);
    }

    //����Ԥ��
    void IncreaseBudget(TPR_INT64 delta_time_ms) 
    {
        TPR_INT64 bytes = target_rate_kbps_ * delta_time_ms / 8;
        if (bytes_remaining_ < 0 || can_build_up_underuse_) 
        {
            //overuse�ϴμ���������˼��
            bytes_remaining_ = npq_min(bytes_remaining_ + bytes, max_bytes_in_budget_);
        } 
        else 
        {
            bytes_remaining_ = npq_min(bytes, max_bytes_in_budget_);
        }
    }

    //����Ԥ��
    void UseBudget(int bytes) 
    {
        bytes_remaining_ = npq_max(bytes_remaining_ - static_cast<int>(bytes),  -max_bytes_in_budget_);
    }

    //ʣ���ֽ���
    int bytesRemaining() const 
    {
        return static_cast<int>(npq_max(0, bytes_remaining_));
    }

	int budgetLevelPercent() const 
	{
		//NPQ_DEBUG("max_bytes_in_budget_ = %d,bytes_remaining_=%d,percent = %d",max_bytes_in_budget_,bytes_remaining_,bytes_remaining_ * 100 / max_bytes_in_budget_);
		return bytes_remaining_ * 100 / max_bytes_in_budget_;  //���ؿ��б���
	}

    //Ŀ������
    int targetRateKbps() const 
    { 
        return target_rate_kbps_;
    }

private:
    static const int kWindowMs = 500;

    int target_rate_kbps_;
    int bytes_remaining_;
	int max_bytes_in_budget_;
	bool can_build_up_underuse_;
};

#endif