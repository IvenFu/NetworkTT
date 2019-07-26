/**	@jitter.h
*	@note 111.
*	@brief jitterbuffer��Ķ���
*
*	@author		222
*	@date		2017/2/21
*	@note ����

*	@author		333
*	@date		2017/9/27
*	@note �Ż�jitter�����㷨
*
*	@warning 
*/

#ifndef _JITTER_H_
#define _JITTER_H_

#include "TPR_Thread.h"
#include "TPR_Semaphore.h"
#include "buffer.h"
#include "jitterEstimate.h"
#include "Vcmjitter.h"
#include "rtcpReceiver.h"
#include "pli.h"

class Pli;

/**	@class	 CJitterBuffer 
 *	@brief   ���������ඨ��
 *	@note	 
 */
class CJitterBuffer
{
public:
	/*���캯��*/
    CJitterBuffer(OuterParam& struOuterParam,InnerParam& innerParam);

    /*��������*/
	~CJitterBuffer();

    /*�������������ݰ�*/
	int InsertPacket(CRtpPacket& RtpPacket);
	
	/*��ʼ����*/
	int Init();

	/*����ʼ����*/
	int Fini();

	/*��������߳�*/
    int Start();

	/*ֹͣ����߳�*/
    int Stop();

	/*��ȡ������ʱ*/
    int GetjitterDelay();//���������ӿ�����ͬ��

	/*���ö�����������Сֵ*/
    int SetMinDelay(unsigned int nDelay);

	/*���ò����ٶȣ����ڱ��ٻط�*/
	int SetScale(float fScale);
private:

    /*����֡����߳�*/
	static TPR_VOIDPTR __stdcall OutThread( TPR_VOIDPTR pUserData);

    /*����֡����̺߳���*/
	int OutThreadRel();

	//int ExtractFrame();

	/*���������*/
    void CleanBuffer();

	FrameBuffer*    m_pCurFrame;			  //��ǰ����д��֡
	FrameBuffer*    m_pRecycle;				  //���յ�һ֡�ڴ�
	FrameList       m_decodableList;		  //����ɵ�֡����
	OuterParam&     m_struOuterParam;         //�ⲿ����
    InnerParam&     m_innerParam;		      //�ڲ�����

	TPR_HANDLE      m_hThread;                //����֡����߳̾��
	TPR_BOOL        m_bStart;                 //�Ƿ������

	TPR_SEM_T       m_hSem;                   //����֡�ź�
	unsigned int    m_nMinDelay;              //��С������ʱ

    VCMJitterSample m_stWaitingForCompletion; //������֡����
    VCMJitterBuffer m_jitterBuffer;           //����������
    VCMTiming       m_VcmTime;                //ʱ�ӹ�����
    TPR_INT64       m_nLastFrameArrive;       //debug ��һ֡����ʱ��
    TPR_INT64       m_nLastFrameOut;          //debug��һ֡���ʱ��
    TPR_BOOL        m_bFirstPacket;           //�Ƿ��ǵ�һ�����ݰ�
	float           m_fScale;				  //���ű��٣����ڱ��ٻط�
};


#endif

