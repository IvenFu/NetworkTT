/**	@ProbeControler.h
*	@note 111.
*	@brief ����̽�����
*
*	@author		222
*	@date		2018/4/18
*
*	@note 
*
*	@warning 
*/

#ifndef _PROBER_CONTROLER_H_
#define _PROBER_CONTROLER_H_
#include <list>
#include "TPR_Mutex.h"
#include "PaceSender.h"

using std::list;

/**	@enum EN_PROBE_STATE
 *	@brief  ̽��״̬
 *	@note	 
 */
enum EN_PROBE_STATE 
{
	kInit,						//��ʼ��
	kWaitingForProbingResult,	//�ȴ�̽����
	kProbingComplete,			//̽�����
};

class PacedSender;

/**	@class ProbeController
 *	@brief  ����̽�����
 *	@note	 
 */
class ProbeController
{
public:
	/*���캯��*/
	ProbeController(PacedSender& pacer);
	
	/*���������������Լ���ʼ����*/
	void SetBitrates(TPR_INT64 llMinBitratebps,TPR_INT64 llStartBitratebps,TPR_INT64 llMaxBitratebps);

	/*����ӵ��ģ����������*/
	void SetEstimatedBitrate(TPR_INT64 llBitratebps);
	
	/*���ò���*/
	void Reset();

private:
	/*��ʼ��̽��*/
	int InitiateExponentialProbing();
	
	/*��ʼ��̽��*/
	int InitiateProbing(TPR_INT64 llNowMs,list<TPR_INT64>* plBitratesToprobes, TPR_BOOL bProbeFurther);

	PacedSender&  m_pacer;						//pacing����
	EN_PROBE_STATE m_state;						//̽�����״̬
	TPR_INT64 m_llMinBitrateToProbeFurtherbps;	//����̽��Ԥ����С����
	TPR_INT64 m_llTimeLastProbingInitiatedMs;	//��¼��һ�γ�ʼ��̽���ʱ��
	TPR_INT64 m_llEstimatedBitratebps;			//���õĹ�������ֵ
	TPR_INT64 m_llStartBitratebps;				//��ʼ����
	TPR_INT64 m_llMaxBitratebps;				//�������
	TPR_INT64 llTimeOfLastLargeDropMs;			//���ʴ���»�ʱ��ʱ��
	TPR_INT64 llBitrateBeforeLastLargeDropbps;	//���ʴ���»�ǰ�ĵ�����
	TPR_Mutex m_lock;							//��
};

#endif  
