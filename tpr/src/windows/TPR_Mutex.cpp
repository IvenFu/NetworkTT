
#include "TPR_Mutex.h"


TPR_DECLARE TPR_INT32 CALLBACK TPR_MutexCreate(TPR_MUTEX_T* pMutex, TPR_INT32 nFlag)
{
	__try
	{
		InitializeCriticalSection(pMutex);
	}
	__except (GetExceptionCode() == STATUS_NO_MEMORY)
	{
		return TPR_ERROR;
	}
	return TPR_OK;
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_MutexDestroy(TPR_MUTEX_T* pMutex)
{
	DeleteCriticalSection(pMutex);
	return TPR_OK;
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_MutexLock(TPR_MUTEX_T* pMutex)
{
	if (!pMutex)
	{
		return TPR_ERROR;
	}

	EnterCriticalSection(pMutex);
	return TPR_OK;
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_MutexTryLock(TPR_MUTEX_T* pMutex)
{
	if (!pMutex)
	{
		return TPR_ERROR;
	}

	return (TryEnterCriticalSection(pMutex) ? TPR_OK : TPR_ERROR);
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_MutexUnlock(TPR_MUTEX_T* pMutex)
{
	if (!pMutex)
	{
		return TPR_ERROR;
	}

	LeaveCriticalSection(pMutex);
	return TPR_OK;
}





