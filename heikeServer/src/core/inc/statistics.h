/**	@statistics.h
*	@note 111.
*	@brief 统计实现
*
*	@author		222
*	@date		2016/11/14
*
*	@note 
*
*	@warning 
*/
#ifndef _STATISTICS_H_
#define _STATISTICS_H_

#include "TPR_Time.h"
#include "TPR_Mutex.h"
#include "rtp.h"
#include "rateStatistics.h"
#include "frameStatistics.h"

/**	@class	 ReceiverStatistics 
 *	@brief   接收统计
 *	@note	 
 */
class ReceiverStatistics
{
public:
	/*构造函数*/
	ReceiverStatistics();

	/*析构函数*/
	~ReceiverStatistics();
	
	/*收到RTP包处理*/
	void NotePacketStat( CRtpPacket& rtpPacket,TPR_TIME_T nArriveTimeUs,TPR_BOOL bCheckType);

	/*重置参数*/
	void Reset();
	
	/*获取reset后收到的包数量*/
	unsigned int PacketCountReset(){return m_iPacketCountReset;}
	
	/*获取reset后的最大序号*/
	unsigned int MaxSeqNumberReset(){return m_nMaxSeqNumberReset;}
	
	/*获取最大序号*/
	unsigned int MaxSeqNumber(){return m_nMaxSeqNumber;}
	
	/*获取初始序号*/
	unsigned int BaseSeqNumber(){return m_nBaseSeqNumber;}
	
	/*获取收到的包数量*/
	unsigned int PacketCount(){return m_iPacketCount;}
	
	/*获取抖动值*/
	unsigned int Jitter(){return (unsigned int)m_fJitter;}

private:
	void InitSeq(unsigned short sSeq);

	TPR_BOOL m_bFirst;					//是否是第一次标志
	int m_iPacketCountReset;			//数据包个数，reset后清0
	int m_iPacketCount;					//数据包个数
	unsigned int m_nMaxSeqNumber;		//已收到的数据包最大序号，高16位表示序号轮询次数，低16位表示序号值
	unsigned int m_nMaxSeqNumberReset;	//已收到的数据包最大序号，reset时赋值
	unsigned int m_nBaseSeqNumber;		//记录第一个数据包的序号
	int m_iLastTransit;					//上一次RTP传输时间
	unsigned int m_nLastTimestamp;		//上一次时间戳
	double m_fJitter;					//统计的抖动参数
};

/**	@class	 ReceiverStatisticsWrap 
 *	@brief   接收统计封装类
 *	@note	 
 */
class ReceiverStatisticsWrap
{
public:
	/*构造函数*/
	ReceiverStatisticsWrap();

	/*析构函数*/
	~ReceiverStatisticsWrap();

	/*收到RTP包处理*/
	void NotePacketStat(CRtpPacket& rtpPacket);
	
	/*获取丢包率*/
	unsigned char GetLossRate();

	/*获取码率*/
	TPR_UINT32 GetBitrate();

	/*获取帧率*/
	unsigned int GetFrameRate();
	
	/*获取抖动值*/
	unsigned int GetJitter();

private:
	/*收丢包率计算*/
	unsigned char GetLossRate1();

	ReceiverStatistics m_statistics;		//数据统计功能(丢包率、抖动)
	RateStatistics m_rate;					//码率统计
	TPR_TIME_T m_llRestTime;				//重置时间
	TPR_Mutex m_lock;						//互斥锁
	TPR_UINT8 m_cbLossRate;					//丢包率
	int m_iFecRecoveryCount;				//FEC恢复的包数量
	int m_iRtxCount;						//重传包数量
	FrameStatistics m_frameRate;			//帧率统计
};
#endif

