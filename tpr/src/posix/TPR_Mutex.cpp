
#include "TPR_Mutex.h"


TPR_DECLARE TPR_INT32 CALLBACK TPR_MutexCreate(TPR_MUTEX_T* pMutex, TPR_INT32 nFlag)
{
	if (!pMutex)
	{
		return TPR_ERROR;
	}
	
	pthread_mutexattr_t attr;
	
	pthread_mutexattr_init(&attr);
	
	if(nFlag!=-1)
	{
		pthread_mutexattr_settype(&attr, nFlag);
	}
	else
	{
		pthread_mutexattr_settype(&attr, TPR_MUTEX_RECURSIVE);
	}
	
	pthread_mutex_init(pMutex,  &attr);
	pthread_mutexattr_destroy(&attr);
	
	return TPR_OK;
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_MutexDestroy(TPR_MUTEX_T* pMutex)
{
	if (!pMutex)
	{
		return TPR_ERROR;
	}

	return pthread_mutex_destroy(pMutex) == 0? TPR_OK:TPR_ERROR;
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_MutexLock(TPR_MUTEX_T* pMutex)
{
	if (!pMutex)
	{
		return TPR_ERROR;
	}

	return pthread_mutex_lock(pMutex) == 0? TPR_OK:TPR_ERROR;
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_MutexTryLock(TPR_MUTEX_T* pMutex)
{
	if (!pMutex)
	{
		return TPR_ERROR;
	}

	return pthread_mutex_trylock(pMutex) == 0? TPR_OK:TPR_ERROR;
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_MutexUnlock(TPR_MUTEX_T* pMutex)
{
	if (!pMutex)
	{
		return TPR_ERROR;
	}

	return pthread_mutex_unlock(pMutex) == 0? TPR_OK:TPR_ERROR;
}

TPR_Mutex::TPR_Mutex()
{
	TPR_MutexCreate(&m_mutex);
}

TPR_Mutex::TPR_Mutex(TPR_UINT32 nFlag)
{
	TPR_MutexCreate(&m_mutex, nFlag);
}

TPR_Mutex::~TPR_Mutex()
{
	TPR_MutexDestroy(&m_mutex);
}

TPR_INT32 TPR_Mutex::Lock()
{
	return TPR_MutexLock(&m_mutex);
}

TPR_INT32 TPR_Mutex::Unlock()
{
	return TPR_MutexUnlock(&m_mutex);
}

TPR_INT32 TPR_Mutex::TryLock()
{
	return TPR_MutexTryLock(&m_mutex);
}








