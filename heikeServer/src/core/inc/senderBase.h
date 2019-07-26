
/**	@sendImpl.h
*	@note 111.
*	@brief 发送端实现类 相关定义
*
*	@author		333
*	@date		2016/10/08
*
*	@note 
*
*	@warning 
*/

#ifndef _SENDERBASE_H_
#define _SENDERBASE_H_

#include "NPQosImpl.h"
#include "callback.h"
#include "rtcpSender.h"
#include "rtcpCallback.h"

class CRtcpPacket;
class RtcpSender;
class RtcpCallback;
/**	@class	 SenderBase 
 *	@brief   发送实现类
 *	@note	 
 */
class SenderBase
{
public:
	/*构造函数*/
	SenderBase(OuterParam& struOuterParam,NPQ_QOS_MAIN_TYPE enMainType);

	/*析构函数*/
	~SenderBase();

	/*初始化函数*/
	int Init();

	/*反初始化函数*/
	int Fini();

    /*开始rtcp*/
    int Start();

    /*停止rtcp*/
    int Stop();
	
	int InputData(CRtpPacket& RtpPacket);

	/*输入rtcp数据*/
	int InputDataRtcp(CRtcpPacket& RtcpPacket);

	int GetStat(NPQ_STAT* pStat);
protected:
	RtcpSender* m_pRtcp;								/*RTCP处理类*/
	RtcpCallback* m_pRtcpCallback;						/*RTCP 回调*/
protected:
	OuterParam& m_struOuterParam;						/*外部参数结构体*/
	InnerParam m_innerParam;							/*内部参数结构体*/

};

#endif


