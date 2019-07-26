/**	@PaceSender.h
*	@note 111.
*	@brief pacing类的接口定义
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
 *	@brief   分步发送实现类
 *	@note	 
 */
class PacedSender : public NextBase
{
public:
    /*创建Pacing类*/
	static PacedSender* Create(OuterParam& struOuterParam, CTrasnportAddExtension* TransportAddTExtension);

    /*销毁Pacing类*/
    static int Destroy(PacedSender* pRtpStreamSync);

    /*设置估计的网络容量 在每次包发送前调用 用|bitrate_bps| * kDefaultPaceMultiplier作为burst分组包的比特率*/
    virtual void SetEstimatedBitrate(TPR_UINT32 nBitratebps);

    /*塞入数据包到优先队列*/
    int InputData(CRtpPacket& rtpPacket);

    /*开启Pacing*/
    int Start();

    /*停止Pacing*/
    int Stop();

    /*设置FEC接口 用于FEC打包*/
    void SetFecInterface(Fec* pFec);

    void CreateProbeCluster(TPR_INT64 llBitrate);
private:
	/*构造函数*/
	PacedSender(OuterParam& struOuterParam, CTrasnportAddExtension* TransportAddTExtension);

	/*析构函数*/
	virtual ~PacedSender();

    /*初始化操作*/
    int Init();

    /*反初始化操作*/
    int Fini();

    /*pacing处理函数 数据包在此函数发送 并加上传输层扩展头和重传标记*/
    void Process() ;
    
    /*Pacing处理线程*/
    static TPR_VOIDPTR __stdcall PacingThread( TPR_VOIDPTR pUserData);

    /*Pacing处理线程调用函数*/
    void PacingThreadRel();

	void PacingCallback( TPR_VOIDPTR pUserData);

	TPR_INT64 TimeUntilNextProcess();
    
    /*更新下一间隔要发送的字节数 增加预算*/
    void UpdateBudgetWithElapsedTime(TPR_INT64 delta_time_in_ms);

    /*减去预算*/
    void UpdateBudgetWithBytesSent(int bytes);

    /*发送数据包 发送成功返回true 发送失败 返回false*/
    TPR_BOOL SendPacket(PacedPacket* packet, int iProbeClusterId);

    /*发送padding*/
    int SendPadding(int padding_needed, int iProbeClusterId);

    /*参数重置*/
    void Reset();

    /*加扩展头输出数据*/
    int  OutputData(CRtpPacket& rtpPacket);
private:

    TPR_Mutex              m_lock;                 //队列锁
    IntervalBudget*        m_pcMediaBudget;        //媒体数据预算器
    IntervalBudget*        m_pcPaddingBudget;      //padding预算器
    TPR_UINT32             m_nEstimatedBitratebps ;//记录外部配置码率
    TPR_UINT32             m_nPacingBitratekbps ;  //Pacing预算的码率 m_nEstimatedBitratebps*系数
    TPR_INT64              m_llTimeLastUpdateUs;   //上一次process的时间记录
    PacketQueue*           m_pcPackets;            //存储数据包的队列
    TPR_UINT64             m_nPacketCounter;       //数据包计数器
    TPR_BOOL               m_bStart;               //模块开始运行标记
    EventTimer*            m_pTimer;               //定时器
    PacingBuffer           m_sendedDataBuffer;     //重复数据包缓存，用于padding
    Fec*                   m_pFec;                 //用于视频数据送FEC打包
    OuterParam&            m_struOuterParam;       //输出数据接口
    CTrasnportAddExtension* m_paddExtension;       //增加扩展头处理
    TPR_BOOL               m_bPacketArrive;        //第一个数据包是否到来标记
    TPR_BOOL               m_bInit;                //初始化标记
    TPR_INT64              m_nLastSendTime;		   //debug
    BitrateProber*         m_pProber;			   //带宽探测子类
	TPR_HANDLE			   m_hThread;			   //Pacing功能运行线程句柄
	AlrDetector*		   m_pAlr;				   //alr检测
};

#endif