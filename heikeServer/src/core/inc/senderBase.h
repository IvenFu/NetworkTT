
/**	@sendImpl.h
*	@note 111.
*	@brief ���Ͷ�ʵ���� ��ض���
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
 *	@brief   ����ʵ����
 *	@note	 
 */
class SenderBase
{
public:
	/*���캯��*/
	SenderBase(OuterParam& struOuterParam,NPQ_QOS_MAIN_TYPE enMainType);

	/*��������*/
	~SenderBase();

	/*��ʼ������*/
	int Init();

	/*����ʼ������*/
	int Fini();

    /*��ʼrtcp*/
    int Start();

    /*ֹͣrtcp*/
    int Stop();
	
	int InputData(CRtpPacket& RtpPacket);

	/*����rtcp����*/
	int InputDataRtcp(CRtcpPacket& RtcpPacket);

	int GetStat(NPQ_STAT* pStat);
protected:
	RtcpSender* m_pRtcp;								/*RTCP������*/
	RtcpCallback* m_pRtcpCallback;						/*RTCP �ص�*/
protected:
	OuterParam& m_struOuterParam;						/*�ⲿ�����ṹ��*/
	InnerParam m_innerParam;							/*�ڲ������ṹ��*/

};

#endif


