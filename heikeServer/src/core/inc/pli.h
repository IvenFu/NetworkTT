/**	@pli.h
*	@note 
*	@brief pli功能实现
*
*	@author		222
*	@date		2017/11/12
*
*	@note 
*
*	@warning 
*/
#ifndef _PLI_H_
#define _PLI_H_

#include "threadUtil.h"
#include "frame.h"
#include "rtcpReceiver.h"

class RtcpReceiver;

/**	@class	 Pli 
 *	@brief   pli功能封装
 *	@note	 
 */
class Pli: public ThreadBase
{
public:
	/*构造函数*/
	Pli();

	/*析构函数*/
	~Pli();

	/*更新帧数据*/
	int UpdatePacket(CRtpPacket& RtpPacket);
	
	/*设置rtcp输出接口*/
	void SetRtcpInterface(RtcpReceiver* pRtcp);

private:
	/*线程处理函数实现*/
	int Process();

	/*关键帧请求*/
	void KeyFrameRequest();

	/*更新帧数据*/
	int UpdateFrame(CFrame* pFrame);

	CFrame m_frame;
	TPR_UINT16 m_sSeqRecord;
	TPR_TIME_T m_llPliTime;		//记录PLI报文触发时间  
	const int m_iTimeOut;		//PLI超时时间
	RtcpReceiver* m_pRtcp;		//rtcp输出接口
	TPR_BOOL m_bFirst;
};

#endif
