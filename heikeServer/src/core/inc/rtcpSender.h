/**	@rtcpSender.h
*	@note 111.
*	@brief rtcp 发送端派生类
*
*	@author		222
*	@date		2017/11/02
*
*	@note 
*
*	@warning 
*/

#ifndef _RTCP_SENDER_H_
#define _RTCP_SENDER_H_

#include "timeUtil.h"
#include "rtcpDefine.h"
#include "rtcp.h"
#include "rtcpCallback.h"

/**	@class	 RtcpSender 
 *	@brief   rtcp 发送端派生类
 *	@note	 
 */
class RtcpSender: public RTCP
{
public:
	/*构造函数*/
	RtcpSender(OuterParam& struOuterParam,InnerParam& innerParam,RtcpCallback& callback);

	/*析构函数*/
	~RtcpSender();
	
	/*更新数据包相关参数*/
	void UpdateMediaStamp(TPR_UINT32 nTimeStamp, TPR_TIME_T nArriveTimeUs,TPR_INT32 iPayLoadFrequence);
	
	/*获取是否产生第一个SR报告标志*/
	TPR_BOOL GetRttFlag();
private:
	/*构造RTCP报文*/
	int BuildRtcpBasic(RTCP_OUTPUT_TYPE enOutputType,void* pBuildParam);
	
	/*构造SR*/
	int AddSR();

	/*构造SR body*/
	int AddSendInfo();
	
	/*构造SDES*/
	int AddSDES();

	/*构造SDES具体信息*/
	int AddSDESItem();
	
	/*构造XR DLRR*/
	int AddXRDLRR();

	/*构造XR 块信息*/
	int AddXRBlock(RTCP_XR_BT_TYPE enXRBT);
	
	TPR_BOOL m_bRttFlag;				//是否发送第一个SR标志
	TPR_UINT32 m_nMediaTimeStamp;		//媒体数据时间戳  
	TPR_TIME_T m_nMediaCaptureTimeUs;	//媒体数据采集时间  
	TPR_INT32 m_iPayLoadFrequence;		//时钟频率
};

#endif
