/**	@threadUtil.cpp
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

#include "threadUtil.h"
#include "Util.h"

/**	@fn ThreadBase::ThreadBase
*	@brief 构造函数
*	@param  int iTimeOutMs 线程处理超时时间 ，-1表示永久等待
*	@return  
*/
ThreadBase::ThreadBase(int iTimeOutMs)
:	
m_bStart(TPR_FALSE)
,m_hThread(TPR_INVALID_HANDLE)
#if defined(N_OS_WINDOWS) || defined(N_OS_WINDOWS64) || defined(N_OS_IOS)
,m_hSem(TPR_INVALID_HANDLE)
#endif
,m_iTimeOutMs(iTimeOutMs)
{

}

/**	@fn ThreadBase::~ThreadBase
*	@brief 析构函数
*	@return  
*/
ThreadBase::~ThreadBase()
{

}

/**	@fn ThreadBase::Start
*	@brief 启动线程
*	@return  见错误码定义
*/
int ThreadBase::Start()
{
	int iRet;
	iRet = TPR_SemCreate(&m_hSem,0);
	if(iRet!= TPR_OK)
	{
		return NPQERR_GENRAL;
	}

	m_hThread = TPR_Thread_Create(BaseThread, this, 0, 0);
	if (TPR_INVALID_THREAD == m_hThread)
	{
		NPQ_ERROR("BaseThread error");
		TPR_SemDestroy(&m_hSem);
		return NPQERR_SYSTEM;
	}

	return NPQ_OK;
}

/**	@fn ThreadBase::Stop
*	@brief 停止线程
*	@return  见错误码定义
*/
int ThreadBase::Stop()
{
	if(m_bStart == TPR_FALSE)
	{
		return NPQ_OK;
	}

	m_bStart = TPR_FALSE;
	TPR_SemPost(&m_hSem);
	if(m_hThread!= TPR_INVALID_THREAD)
	{
		TPR_Thread_Wait(m_hThread);
		m_hThread = TPR_INVALID_THREAD;
	}

	TPR_SemDestroy(&m_hSem);
#if defined(N_OS_WINDOWS) || defined(N_OS_WINDOWS64) || defined(N_OS_IOS)
	m_hSem = TPR_INVALID_HANDLE;
#endif
	return NPQ_OK;
}

/**	@fn ThreadBase::BaseThread
*	@brief 线程函数
*	@param  TPR_VOIDPTR pUserData 线程参数
*	@return  NULL
*/
TPR_VOIDPTR __stdcall ThreadBase::BaseThread( TPR_VOIDPTR pUserData )
{
	ThreadBase* p = (ThreadBase*)pUserData;

	if (NULL == p)
	{
		return NULL;
	}

	p->BaseThreadRel();
	return NULL;
}

/**	@fn ThreadBase::BaseThreadRel
*	@brief 线程函数
*	@return  NPQ_OK
*/
int ThreadBase::BaseThreadRel()
{
	m_bStart = TPR_TRUE;
	while(m_bStart)
	{
		if(m_iTimeOutMs == -1)
		{
			TPR_SemWait(&m_hSem);
		}
		else
		{
			TPR_SemTimedWait(&m_hSem,m_iTimeOutMs);
		}
		
		if(!m_bStart)
		{
			break;
		}

		Process();
	}
	return NPQ_OK;
}

/**	@fn ThreadBase::DoPost
*	@brief 触发线程处理
*	@return  void
*/
void ThreadBase::DoPost()
{
	if(!m_bStart)
	{
		return;
	}

	TPR_SemPost(&m_hSem);
}

