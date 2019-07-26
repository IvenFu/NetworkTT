

#ifndef __TPR_MUTEX_H__
#define __TPR_MUTEX_H__

#include "TPR_Config.h"
#include "TPR_Types.h"

#if defined(OS_WINDOWS)
	typedef CRITICAL_SECTION TPR_MUTEX_T;
	#define TPR_MUTEX_FAST			0
	#define TPR_MUTEX_RECURSIVE		0
	#define TPR_MUTEX_ERRORCHECK	0
#elif defined(OS_POSIX)
#include <pthread.h>
	typedef pthread_mutex_t TPR_MUTEX_T;
	///ª•≥‚À¯¿‡–Õª•≥‚
#if (defined(OS_APPLE) || defined(OS_S60) || defined(OS_ANDROID))
	#define TPR_MUTEX_FAST			PTHREAD_MUTEX_NORMAL
	#define TPR_MUTEX_RECURSIVE		PTHREAD_MUTEX_RECURSIVE
	#define TPR_MUTEX_ERRORCHECK	PTHREAD_MUTEX_ERRORCHECK
#elif defined (__linux__)
	#define TPR_MUTEX_FAST			PTHREAD_MUTEX_FAST_NP
	#define TPR_MUTEX_RECURSIVE		PTHREAD_MUTEX_RECURSIVE_NP
	#define TPR_MUTEX_ERRORCHECK	PTHREAD_MUTEX_ERRORCHECK_NP
#endif
#else
	#error OS Not Implement yet.
#endif

TPR_DECLARE TPR_INT32 CALLBACK TPR_MutexCreate(TPR_MUTEX_T* pMutex, TPR_INT32 iFlag = -1); 


TPR_DECLARE TPR_INT32 CALLBACK TPR_MutexDestroy(TPR_MUTEX_T* pMutex); 


TPR_DECLARE TPR_INT32 CALLBACK TPR_MutexLock(TPR_MUTEX_T* pMutex); 


TPR_DECLARE TPR_INT32 CALLBACK TPR_MutexTryLock(TPR_MUTEX_T* pMutex);

TPR_DECLARE TPR_INT32 CALLBACK TPR_MutexUnlock(TPR_MUTEX_T* pMutex); 

class TPR_DECLARE_CLASS TPR_Mutex
{
public:
	TPR_Mutex()
	{
		TPR_MutexCreate(&m_mutex);
	}

	TPR_Mutex(TPR_UINT32 nFlag)
	{
		TPR_MutexCreate(&m_mutex, nFlag);
	}

	~TPR_Mutex()
	{
		TPR_MutexDestroy(&m_mutex);
	}

	TPR_INT32 Lock()
	{
		return TPR_MutexLock(&m_mutex);
	}

	TPR_INT32 Unlock()
	{
		return TPR_MutexUnlock(&m_mutex);
	}

	TPR_INT32 TryLock()
	{
		return TPR_MutexTryLock(&m_mutex);
		 
	}

private:
	TPR_MUTEX_T m_mutex;
};

//add by zhaoyiji 2011-10-20
//////////////////////////////////////////////////////////////////////////
typedef TPR_MUTEX_T tpr_mutex_t;


	
#endif


