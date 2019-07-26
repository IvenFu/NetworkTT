/**	@RTCP.h
*	@note 111.
*	@brief rtcp ����
*
*	@author		222
*	@date		2017/11/02
*
*	@note 
*
*	@warning 
*/

#ifndef _RTCP_H_
#define _RTCP_H_

#include "TPR_Time.h"
#include "TPR_Thread.h"
#include "TPR_Semaphore.h"
#include "baseImpl.h"
#include "callback.h"
#include "rtcpDefine.h"
#include "rtcpParser.h"

class RtcpParser;
class RtcpCallback;

/**	@class	 CRtcpPacket 
 *	@brief   RTCP������
 *	@note	 
 */
class CRtcpPacket
{
public:
	/*���캯��*/
	CRtcpPacket();

	/*��������*/
	~CRtcpPacket();

	/*����RTCP����*/
	int Parse(unsigned char* pData,unsigned int nLen);

	unsigned char* m_pData;		//�����׵�ַ
	unsigned int m_nLen;		//RTCP������
	TPR_TIME_T m_nArriveTimeUs;	//���ݰ��ﵽʱ�� ��λus
};

#define BUF_LEN 2048

/**	@class	 RtcpBuffer 
 *	@brief   RTCP������
 *	@note	 
 */
class RtcpBuffer
{
public:
	/*���캯��*/
	RtcpBuffer();

	/*��������*/
	~RtcpBuffer();
	
	/*�������*/
	void Clear();
	
	/*���ö�Ӧλ���ֽ�����*/
	int SetPosDWord( unsigned int dWord, unsigned int Pos);
	
	/*����ָ����������*/
	int Enqueue(unsigned char* pData, int iLen);

	/*����2�ֽ�����*/
	int EnqueueWord(unsigned short sWord);

	/*����4�ֽ�����*/
	int EnqueueDWord(unsigned int nWord);
	
	/*��ȡ���ݵ�ַ�ͳ���*/
	int GetValidData(unsigned char** ppData, int* pLen);

private:
	unsigned char m_buf[BUF_LEN];		//����������
	unsigned int m_nPos;				//��Ч����λ��
};

/**	@class	 RTCP 
 *	@brief   RTCP���ඨ��
 *	@note	 
 */
class RTCP
{
public:
	/*���캯��*/
	RTCP(OuterParam& struOuterParam,InnerParam& innerParam,RtcpCallback& callback);

	/*��������*/
	~RTCP();
	
	/*��������*/
	int InputData(unsigned char* pData, unsigned int uDataLen);
	
	/*�����ڲ��߳�����*/
	int Start();
	
	/*ֹͣ�ڲ��߳�����*/
	int Stop();
	
protected:
	/*����RTCP��*/
	int OutputRtcp(RTCP_OUTPUT_TYPE enOutputType,void* pBuildParam);

	/*����RTCP��ͨ�ñ�ͷ*/
	int AddCommonHead(RTCP_PT_TYPE enType, int nPacketLength, int numReportingSources);

	RtcpBuffer m_buffer;			//rtcp������
	const unsigned int m_cOurSsrc;	//rtcp ssrc
	RtcpCallback& m_rtcpCallback;	//rtcp �����ص�
	InnerParam& m_innerParam;		//�ڲ�����

private:
	/*RTCP�ڲ��߳�*/
	static TPR_VOIDPTR __stdcall RtcpThread( TPR_VOIDPTR pUserData);

	/*RTCP�ڲ��̺߳���*/
	int RtcpThreadRel();
	
	/*����RTCP����*/
	virtual int BuildRtcpBasic(RTCP_OUTPUT_TYPE enOutputType,void* pBuildParam);
	
	/*���RTCP����*/
	void Flush();

	TPR_BOOL m_bStart;				//�Ƿ����̱߳�־
	TPR_HANDLE m_hThread;			//�߳̾��
	OuterParam& m_struOuterParam;	//�ⲿ����
	TPR_SEM_T m_hSem;				//�ź���
	TPR_Mutex m_lock;				//��������
	RtcpParser* m_pRtcpParser;		//rtcp���Ľ�����
};
#endif

