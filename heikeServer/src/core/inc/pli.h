/**	@pli.h
*	@note 
*	@brief pli����ʵ��
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
 *	@brief   pli���ܷ�װ
 *	@note	 
 */
class Pli: public ThreadBase
{
public:
	/*���캯��*/
	Pli();

	/*��������*/
	~Pli();

	/*����֡����*/
	int UpdatePacket(CRtpPacket& RtpPacket);
	
	/*����rtcp����ӿ�*/
	void SetRtcpInterface(RtcpReceiver* pRtcp);

private:
	/*�̴߳�����ʵ��*/
	int Process();

	/*�ؼ�֡����*/
	void KeyFrameRequest();

	/*����֡����*/
	int UpdateFrame(CFrame* pFrame);

	CFrame m_frame;
	TPR_UINT16 m_sSeqRecord;
	TPR_TIME_T m_llPliTime;		//��¼PLI���Ĵ���ʱ��  
	const int m_iTimeOut;		//PLI��ʱʱ��
	RtcpReceiver* m_pRtcp;		//rtcp����ӿ�
	TPR_BOOL m_bFirst;
};

#endif
