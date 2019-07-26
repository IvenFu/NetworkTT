/**	@transportFeedbackAdapter.h
*	@note 111.
*	@brief 传输层反馈相关类的接口定义
*
*	@author		333
*	@date		2017/1/7
*
*	@note 
*
*	@warning 
*/
#ifndef _TRANSPORTFEEDBACKADAPTER_H_
#define _TRANSPORTFEEDBACKADAPTER_H_

#include <map>
#include <vector>
#include <deque>
#include "TPR_Utils.h"
#include "delayBasedBwe.h"
#include "Util.h"
#include "transportFeedback.h"
#include "TransportPacketInfo.h"

using std::list;
using std::vector;

class TransportFeedback;


//typedef map<TPR_INT64, PacketInfo> HistoryMap;          //数据包信息映射
/**	@struct	 SendTimeHistory 
 *	@brief   发送端数据包信息
 *	@note	 
 */
class SendTimeHistory 
{
public:
    
    SendTimeHistory(TPR_INT64 nPacketAgeLimitMs);

    ~SendTimeHistory();

    /*清除数据包信息*/
    void Clear();

    /*清除老的数据包信息，增加新的数据包信息 */
    void AddAndRemoveOld(TPR_UINT16 nSequence_number, int nPayloadSize, int nProbeClusterId);

    /* 通过反馈序号和发送时间更新数据包信息*/
    TPR_BOOL OnSentPacket(TPR_UINT16 nSequence_number, TPR_INT64 nSendTimeMs);

    /*查找数据包信息*/
    TPR_BOOL GetInfo(PacketInfo* pPacketInfo, TPR_BOOL bRemove);

private:
    const TPR_INT64 nPacketAgeLimitMs;
    SequenceNumberUnwrapper nSeqNumUnwrapper;  //反馈序号
    HistoryMap m_history;
	TPR_Mutex m_lock;
};


/**	@struct	 TransportFeedbackAdapter 
 *	@brief   传输层反馈适配类
 *	@note	 
 */                                                                                                               
class TransportFeedbackAdapter
{
public:
    TransportFeedbackAdapter();
    virtual ~TransportFeedbackAdapter();

    /*初始化带宽*/
    void InitBwe();

    /*增加数据包*/
    void AddPacket(TPR_UINT16 nSequenceNumber, int nLength, int nProbeClusterId);

    /*发送数据包*/
    void OnSentPacket(TPR_UINT16 nSequenceNumber, TPR_INT64 nSendTimeMs);

    /*收到传输层反馈*/
    void OnTransportFeedback(TransportFeedback* pFeedback, Result* pResult);

    /*获取数据包信息*/
    vector<PacketInfo> GetTransportFeedbackVector() ;

    /*RTT更新*/
    void OnRttUpdate(TPR_INT64 nAvgRttMs, TPR_INT64 nMaxRttMs) ;

    /*设置最小比特率*/
    void SetMinBitrate(int nMinBitrateBps);

private:
	/*获取数据包信息*/
    vector<PacketInfo> GetPacketFeedbackVector(TransportFeedback* pFeedback); 

    TPR_Mutex m_bwelock;                //保护锁

    SendTimeHistory m_cSendTimeHistory; //发送时间历时信息类
    DelayBasedBwe m_cDelayBasedBwe;     //基于延时的拥塞控制类

    TPR_INT64 m_nCurrentOffsetMs;       //当前偏移时间
    TPR_INT64 m_nLastTimestampUs;       //最近的时间戳
    //BitrateController*  bitrate_controller_;
    vector<PacketInfo> m_aLastPacketFeedbackVector; //保存传输层反馈对应的数据包信息集
};




#endif