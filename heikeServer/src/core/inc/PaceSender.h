/**	@PaceSender.h
*	@note 111.
*	@brief pacing��Ľӿڶ���
*
*	@author		333
*	@date		2017/2/7
*
*	@note 
*
*	@warning 
*/
#ifndef _PACESENDER_H_
#define _PACESENDER_H_

#include "TPR_Utils.h"
#include "Util.h"
#include "timeUtil.h"
#include "tickTimer.h"
#include "nextBase.h"
#include "Pacingbuffer.h"
#include "PacedPacket.h"
#include "RtpAddExtension.h"
#include "fec.h"
#include "BitRateProber.h"
#include "alrDetector.h"

class PacingBuffer;
class CTrasnportAddExtension;

/**	@class	 PacedSender 
 *	@brief   �ֲ�����ʵ����
 *	@note	 
 */
class PacedSender : public NextBase
{
public:
    /*����Pacing��*/
	static PacedSender* Create(OuterParam& struOuterParam, CTrasnportAddExtension* TransportAddTExtension);

    /*����Pacing��*/
    static int Destroy(PacedSender* pRtpStreamSync);

    /*���ù��Ƶ��������� ��ÿ�ΰ�����ǰ���� ��|bitrate_bps| * kDefaultPaceMultiplier��Ϊburst������ı�����*/
    virtual void SetEstimatedBitrate(TPR_UINT32 nBitratebps);

    /*�������ݰ������ȶ���*/
    int InputData(CRtpPacket& rtpPacket);

    /*����Pacing*/
    int Start();

    /*ֹͣPacing*/
    int Stop();

    /*����FEC�ӿ� ����FEC���*/
    void SetFecInterface(Fec* pFec);

    void CreateProbeCluster(TPR_INT64 llBitrate);
private:
	/*���캯��*/
	PacedSender(OuterParam& struOuterParam, CTrasnportAddExtension* TransportAddTExtension);

	/*��������*/
	virtual ~PacedSender();

    /*��ʼ������*/
    int Init();

    /*����ʼ������*/
    int Fini();

    /*pacing������ ���ݰ��ڴ˺������� �����ϴ������չͷ���ش����*/
    void Process() ;
    
    /*Pacing�����߳�*/
    static TPR_VOIDPTR __stdcall PacingThread( TPR_VOIDPTR pUserData);

    /*Pacing�����̵߳��ú���*/
    void PacingThreadRel();

	void PacingCallback( TPR_VOIDPTR pUserData);

	TPR_INT64 TimeUntilNextProcess();
    
    /*������һ���Ҫ���͵��ֽ��� ����Ԥ��*/
    void UpdateBudgetWithElapsedTime(TPR_INT64 delta_time_in_ms);

    /*��ȥԤ��*/
    void UpdateBudgetWithBytesSent(int bytes);

    /*�������ݰ� ���ͳɹ�����true ����ʧ�� ����false*/
    TPR_BOOL SendPacket(PacedPacket* packet, int iProbeClusterId);

    /*����padding*/
    int SendPadding(int padding_needed, int iProbeClusterId);

    /*��������*/
    void Reset();

    /*����չͷ�������*/
    int  OutputData(CRtpPacket& rtpPacket);
private:

    TPR_Mutex              m_lock;                 //������
    IntervalBudget*        m_pcMediaBudget;        //ý������Ԥ����
    IntervalBudget*        m_pcPaddingBudget;      //paddingԤ����
    TPR_UINT32             m_nEstimatedBitratebps ;//��¼�ⲿ��������
    TPR_UINT32             m_nPacingBitratekbps ;  //PacingԤ������� m_nEstimatedBitratebps*ϵ��
    TPR_INT64              m_llTimeLastUpdateUs;   //��һ��process��ʱ���¼
    PacketQueue*           m_pcPackets;            //�洢���ݰ��Ķ���
    TPR_UINT64             m_nPacketCounter;       //���ݰ�������
    TPR_BOOL               m_bStart;               //ģ�鿪ʼ���б��
    EventTimer*            m_pTimer;               //��ʱ��
    PacingBuffer           m_sendedDataBuffer;     //�ظ����ݰ����棬����padding
    Fec*                   m_pFec;                 //������Ƶ������FEC���
    OuterParam&            m_struOuterParam;       //������ݽӿ�
    CTrasnportAddExtension* m_paddExtension;       //������չͷ����
    TPR_BOOL               m_bPacketArrive;        //��һ�����ݰ��Ƿ������
    TPR_BOOL               m_bInit;                //��ʼ�����
    TPR_INT64              m_nLastSendTime;		   //debug
    BitrateProber*         m_pProber;			   //����̽������
	TPR_HANDLE			   m_hThread;			   //Pacing���������߳̾��
	AlrDetector*		   m_pAlr;				   //alr���
};

#endif