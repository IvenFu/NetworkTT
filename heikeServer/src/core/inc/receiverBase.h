/**	@receiverBase.h
*	@note 111.
*	@brief receiverBase实现
*
*	@author		222
*	@date		2017/8/28
*
*	@note 
*
*	@warning 
*/

#ifndef _RECEIVERBASE_H_
#define _RECEIVERBASE_H_

#include "NPQosImpl.h"
#include "callback.h"
#include "rtcpReceiver.h"
#include "rtcpCallback.h"
#include "statistics.h"
#include "rateStatistics.h"

class CRtcpPacket;
class RtcpReceiver;
class RtcpCallback;
class ReceiverStatistics;

/**	@class	 ReceiverBase 
 *	@brief   接收端处理基类
 *	@note	 
 */
class ReceiverBase
{
public:
	/*构造函数*/
	ReceiverBase(OuterParam& struOuterParam,NPQ_QOS_MAIN_TYPE enMainType);
	
	/*析构函数*/
	~ReceiverBase();
	
	/*初始化*/
	int Init();
	
	/*反初始化*/
	int Fini();
	
	/*开启功能*/
	int Start();
	
	/*停止功能*/
	int Stop();
	
	/*输入RTP数据*/
	int InputData(CRtpPacket& RtpPacket);
	
	/*输入RTCP数据*/
	int InputDataRtcp(CRtcpPacket& RtpPacket);
	
	/*获取状态*/
	int GetStat (NPQ_STAT* pStat);
	
	/*获取sr报告信息*/
	int GetSrInfo( RTCP_SR_INFO* pInfo);

protected:
	RtcpReceiver* m_pRtcp;					//RTCP
	RtcpCallback* m_pRtcpCallback;			//RTCP回调
	
	OuterParam& m_struOuterParam;			//外部参数引用
	InnerParam m_innerParam;				//内部参数
private:
	TPR_BOOL m_bStartBase;					//是否开启线程标志
	ReceiverStatistics* m_pStat;			//网络状态统计
	RateStatistics* m_pBitrateStatistics;	//数据码率统计
};
#endif

