/**	@callback.h
*	@note 111.
*	@brief 数据回调
*
*	@author		222
*	@date		2017/1/24
*
*	@note 
*
*	@warning 
*/
#ifndef _CALLBACK_H_
#define _CALLBACK_H_

#include "TPR_Mutex.h"
#include "NPQos.h"
#include "rtp.h"
#include "outerCtrl.h"
#include "sdpParse.h"

/**	@enum	 QOS_TYPE 
 *	@brief   QOS类型
 *	@note	 
 */
enum QOS_TYPE
{
	QOS_NACK = QOS_TYPE_NACK,				//Nack		
	QOS_FEC = QOS_TYPE_FEC,					//FEC
	QOS_DEJITTER = QOS_TYPE_DEJITTER,		//De-Jitter Buffer  只适用于接收端配置
	QOS_BW =  QOS_TYPE_BW,					//拥塞控制
	QOS_PLI = QOS_TYPE_PLI,					//PLI
	QOS_SYNCHRONOUS = QOS_TYPE_SYNCHRONOUS,	//音视频同步
};

/**	@class	 OuterParam 
 *	@brief   外部参数
 *	@note	 
 */
class OuterParam
{
public:
	OuterParam():
	  pCallback(NULL),
		  pUser(NULL),
		  id(0),
		  enType(NPQ_QOS_RECEIVER),
		  m_enQosConfig(QOS_NACK | QOS_FEC | QOS_DEJITTER | QOS_TYPE_BW),
		  m_bVideo(TPR_TRUE),
		  m_bAudio(TPR_TRUE),
		  m_pReceiverCtrl(NULL),
		  m_pSenderCtrl(NULL),
		  m_bInit(TPR_FALSE)
	  {}

	~OuterParam();

	NPQ_QOS_ROLE enType;		//Qos角色
	int m_enQosConfig;			//Qos功能配置
	int m_bVideo;
	int m_bAudio;
	NPQCb pCallback;			//回调函数
	void* pUser;				//回调参数
	int id;						//句柄id	

	/*调用回调函数*/
	int DoCallback(CRtpPacket& rtpPacket);

	/*调用回调函数*/
	int DoCallback(int iDataType, unsigned char* pData, unsigned int nDataLen);

	/*调用回调函数*/
	int DoCallback(NPQ_CMD& cmd);

	/*获取统计码率*/
	void GetRate(TPR_UINT32* pVideo, TPR_UINT32* pTetrans,TPR_UINT32* pFec,TPR_UINT32* pPadding,TPR_INT64 nowMs);
	
	/*获取视频丢包率*/
	unsigned char GetVideoLossRate();

	TPR_UINT32 GetVideoBitrate();

private:
	int InitCtrl();
	TPR_Mutex m_lock;			//数据回调互斥锁
	OuterCtrlReceiver* m_pReceiverCtrl;
	OuterCtrlSender* m_pSenderCtrl;
	TPR_BOOL m_bInit;
};

#endif

