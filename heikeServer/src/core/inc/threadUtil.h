/**	@threadUtil.h
*	@note 111.
*	@brief �߳�ʹ�÷�����װ
*
*	@author		222
*	@date		2017/11/12
*
*	@note 
*
*	@warning 
*/
#ifndef _THREAD_UTIL_H_
#define _THREAD_UTIL_H_

#include "NPQos.h"
#include "TPR_Thread.h"
#include "TPR_Semaphore.h"

/**	@class	 ThreadBase 
 *	@brief   �߳�ʹ�÷�װ
 *	@note	 
 */
class ThreadBase
{
public:
	/*���캯��*/
	ThreadBase(int iTimeOutMs);

	/*��������*/
	~ThreadBase();
	
	/*�����߳�*/
	int Start();
	
	/*ֹͣ�߳�*/
	int Stop();

protected:
	/*�����̴߳���*/
	void DoPost();
	
	/*�̴߳����麯��*/
	virtual int Process() = 0;

private:
	/*�̺߳���*/
	static TPR_VOIDPTR __stdcall BaseThread( TPR_VOIDPTR pUserData);
	
	/*�̺߳���*/
	int BaseThreadRel();

	TPR_BOOL m_bStart;				//�Ƿ����̱߳�־
	TPR_HANDLE m_hThread;			//�߳̾��
	TPR_SEM_T m_hSem;				//�ź���
	int m_iTimeOutMs;				//�̵߳ȴ�ִ�г�ʱʱ�䣬��λms
};

#endif

