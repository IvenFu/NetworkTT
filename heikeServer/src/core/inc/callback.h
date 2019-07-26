/**	@callback.h
*	@note 111.
*	@brief ���ݻص�
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
 *	@brief   QOS����
 *	@note	 
 */
enum QOS_TYPE
{
	QOS_NACK = QOS_TYPE_NACK,				//Nack		
	QOS_FEC = QOS_TYPE_FEC,					//FEC
	QOS_DEJITTER = QOS_TYPE_DEJITTER,		//De-Jitter Buffer  ֻ�����ڽ��ն�����
	QOS_BW =  QOS_TYPE_BW,					//ӵ������
	QOS_PLI = QOS_TYPE_PLI,					//PLI
	QOS_SYNCHRONOUS = QOS_TYPE_SYNCHRONOUS,	//����Ƶͬ��
};

/**	@class	 OuterParam 
 *	@brief   �ⲿ����
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

	NPQ_QOS_ROLE enType;		//Qos��ɫ
	int m_enQosConfig;			//Qos��������
	int m_bVideo;
	int m_bAudio;
	NPQCb pCallback;			//�ص�����
	void* pUser;				//�ص�����
	int id;						//���id	

	/*���ûص�����*/
	int DoCallback(CRtpPacket& rtpPacket);

	/*���ûص�����*/
	int DoCallback(int iDataType, unsigned char* pData, unsigned int nDataLen);

	/*���ûص�����*/
	int DoCallback(NPQ_CMD& cmd);

	/*��ȡͳ������*/
	void GetRate(TPR_UINT32* pVideo, TPR_UINT32* pTetrans,TPR_UINT32* pFec,TPR_UINT32* pPadding,TPR_INT64 nowMs);
	
	/*��ȡ��Ƶ������*/
	unsigned char GetVideoLossRate();

	TPR_UINT32 GetVideoBitrate();

private:
	int InitCtrl();
	TPR_Mutex m_lock;			//���ݻص�������
	OuterCtrlReceiver* m_pReceiverCtrl;
	OuterCtrlSender* m_pSenderCtrl;
	TPR_BOOL m_bInit;
};

#endif

