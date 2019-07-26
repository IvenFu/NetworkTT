/**	@threadUtil.h
*	@note 111.
*	@brief 线程使用方法封装
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
 *	@brief   线程使用封装
 *	@note	 
 */
class ThreadBase
{
public:
	/*构造函数*/
	ThreadBase(int iTimeOutMs);

	/*析构函数*/
	~ThreadBase();
	
	/*启动线程*/
	int Start();
	
	/*停止线程*/
	int Stop();

protected:
	/*触发线程处理*/
	void DoPost();
	
	/*线程处理虚函数*/
	virtual int Process() = 0;

private:
	/*线程函数*/
	static TPR_VOIDPTR __stdcall BaseThread( TPR_VOIDPTR pUserData);
	
	/*线程函数*/
	int BaseThreadRel();

	TPR_BOOL m_bStart;				//是否开启线程标志
	TPR_HANDLE m_hThread;			//线程句柄
	TPR_SEM_T m_hSem;				//信号量
	int m_iTimeOutMs;				//线程等待执行超时时间，单位ms
};

#endif

