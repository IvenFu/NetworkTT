/**	@rtcpSender.h
*	@note 111.
*	@brief rtcp ���Ͷ�������
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
 *	@brief   rtcp ���Ͷ�������
 *	@note	 
 */
class RtcpSender: public RTCP
{
public:
	/*���캯��*/
	RtcpSender(OuterParam& struOuterParam,InnerParam& innerParam,RtcpCallback& callback);

	/*��������*/
	~RtcpSender();
	
	/*�������ݰ���ز���*/
	void UpdateMediaStamp(TPR_UINT32 nTimeStamp, TPR_TIME_T nArriveTimeUs,TPR_INT32 iPayLoadFrequence);
	
	/*��ȡ�Ƿ������һ��SR�����־*/
	TPR_BOOL GetRttFlag();
private:
	/*����RTCP����*/
	int BuildRtcpBasic(RTCP_OUTPUT_TYPE enOutputType,void* pBuildParam);
	
	/*����SR*/
	int AddSR();

	/*����SR body*/
	int AddSendInfo();
	
	/*����SDES*/
	int AddSDES();

	/*����SDES������Ϣ*/
	int AddSDESItem();
	
	/*����XR DLRR*/
	int AddXRDLRR();

	/*����XR ����Ϣ*/
	int AddXRBlock(RTCP_XR_BT_TYPE enXRBT);
	
	TPR_BOOL m_bRttFlag;				//�Ƿ��͵�һ��SR��־
	TPR_UINT32 m_nMediaTimeStamp;		//ý������ʱ���  
	TPR_TIME_T m_nMediaCaptureTimeUs;	//ý�����ݲɼ�ʱ��  
	TPR_INT32 m_iPayLoadFrequence;		//ʱ��Ƶ��
};

#endif
