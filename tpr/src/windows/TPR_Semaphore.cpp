

#include "TPR_Semaphore.h"


TPR_DECLARE TPR_INT32 CALLBACK TPR_SemCreate(TPR_SEM_T* pSem, TPR_UINT32 nInitCount)
{
	HANDLE hHandle = CreateSemaphore(NULL, nInitCount, 65535, NULL);
	if(hHandle == NULL)
	{
		return TPR_ERROR;
	}

	*pSem = hHandle;
	return TPR_OK;
}


TPR_DECLARE TPR_INT32 CALLBACK TPR_SemDestroy(TPR_SEM_T* pSem)
{
	if (!pSem)
	{
		return TPR_ERROR;
	}

	return (CloseHandle(*pSem)) ? TPR_OK : TPR_ERROR;
}


TPR_DECLARE TPR_INT32 CALLBACK TPR_SemWait(TPR_SEM_T* pSem)
{
	if (!pSem)
	{
		return TPR_ERROR;
	}

	if (WAIT_OBJECT_0 == WaitForSingleObject(*pSem, INFINITE))
	{
		return TPR_OK;
	}

	return TPR_ERROR;
}


TPR_DECLARE TPR_INT32 CALLBACK TPR_SemTimedWait(TPR_SEM_T* pSem, TPR_UINT32 nTimeOut)
{
	if(!pSem)
	{
		return TPR_ERROR;
	}

	if (WAIT_OBJECT_0 == WaitForSingleObject(*pSem, nTimeOut))
	{
		return TPR_OK;
	}

	return TPR_ERROR;
}


TPR_DECLARE TPR_INT32 CALLBACK TPR_SemPost(TPR_SEM_T* pSem)
{
	if (!pSem)
	{
		return TPR_ERROR;
	}

	return ReleaseSemaphore(*pSem, 1, NULL) ? TPR_OK : TPR_ERROR;
}



