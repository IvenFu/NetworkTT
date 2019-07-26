/**	@transportFeedbackAdapter.h
*	@note 111.
*	@brief ����㷴�������Ľӿڶ���
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


//typedef map<TPR_INT64, PacketInfo> HistoryMap;          //���ݰ���Ϣӳ��
/**	@struct	 SendTimeHistory 
 *	@brief   ���Ͷ����ݰ���Ϣ
 *	@note	 
 */
class SendTimeHistory 
{
public:
    
    SendTimeHistory(TPR_INT64 nPacketAgeLimitMs);

    ~SendTimeHistory();

    /*������ݰ���Ϣ*/
    void Clear();

    /*����ϵ����ݰ���Ϣ�������µ����ݰ���Ϣ */
    void AddAndRemoveOld(TPR_UINT16 nSequence_number, int nPayloadSize, int nProbeClusterId);

    /* ͨ��������źͷ���ʱ��������ݰ���Ϣ*/
    TPR_BOOL OnSentPacket(TPR_UINT16 nSequence_number, TPR_INT64 nSendTimeMs);

    /*�������ݰ���Ϣ*/
    TPR_BOOL GetInfo(PacketInfo* pPacketInfo, TPR_BOOL bRemove);

private:
    const TPR_INT64 nPacketAgeLimitMs;
    SequenceNumberUnwrapper nSeqNumUnwrapper;  //�������
    HistoryMap m_history;
	TPR_Mutex m_lock;
};


/**	@struct	 TransportFeedbackAdapter 
 *	@brief   ����㷴��������
 *	@note	 
 */                                                                                                               
class TransportFeedbackAdapter
{
public:
    TransportFeedbackAdapter();
    virtual ~TransportFeedbackAdapter();

    /*��ʼ������*/
    void InitBwe();

    /*�������ݰ�*/
    void AddPacket(TPR_UINT16 nSequenceNumber, int nLength, int nProbeClusterId);

    /*�������ݰ�*/
    void OnSentPacket(TPR_UINT16 nSequenceNumber, TPR_INT64 nSendTimeMs);

    /*�յ�����㷴��*/
    void OnTransportFeedback(TransportFeedback* pFeedback, Result* pResult);

    /*��ȡ���ݰ���Ϣ*/
    vector<PacketInfo> GetTransportFeedbackVector() ;

    /*RTT����*/
    void OnRttUpdate(TPR_INT64 nAvgRttMs, TPR_INT64 nMaxRttMs) ;

    /*������С������*/
    void SetMinBitrate(int nMinBitrateBps);

private:
	/*��ȡ���ݰ���Ϣ*/
    vector<PacketInfo> GetPacketFeedbackVector(TransportFeedback* pFeedback); 

    TPR_Mutex m_bwelock;                //������

    SendTimeHistory m_cSendTimeHistory; //����ʱ����ʱ��Ϣ��
    DelayBasedBwe m_cDelayBasedBwe;     //������ʱ��ӵ��������

    TPR_INT64 m_nCurrentOffsetMs;       //��ǰƫ��ʱ��
    TPR_INT64 m_nLastTimestampUs;       //�����ʱ���
    //BitrateController*  bitrate_controller_;
    vector<PacketInfo> m_aLastPacketFeedbackVector; //���洫��㷴����Ӧ�����ݰ���Ϣ��
};




#endif