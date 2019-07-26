#include "TPR_Utils.h"
#include "TPR_Time.h"
#include "TPR_Semaphore.h"


TPR_DECLARE TPR_INT32 CALLBACK TPR_SemCreate(TPR_SEM_T* pSem, TPR_UINT32 nInitCount)
{
	if (!pSem)
	{
		return TPR_ERROR;
	}
	
	return sem_init(pSem,0,nInitCount);
}


TPR_DECLARE TPR_INT32 CALLBACK TPR_SemDestroy(TPR_SEM_T* pSem)
{
	if (!pSem)
	{
		return TPR_ERROR;
	}

	return sem_destroy(pSem);
}


TPR_DECLARE TPR_INT32 CALLBACK TPR_SemWait(TPR_SEM_T* pSem)
{
	if (!pSem)
	{
		return TPR_ERROR;
	}
	
	int ret = TPR_OK;
	while((ret = sem_wait(pSem))!=TPR_OK && EINTR ==errno)
	{}

	return ret;
}


TPR_DECLARE TPR_INT32 CALLBACK TPR_SemTimedWait(TPR_SEM_T* pSem, TPR_UINT32 nTimeOut)
{
	if(!pSem)
	{
		return TPR_ERROR;
	}
	
	int ret = TPR_OK;
	TPR_UINT32 tStart = TPR_GetTimeTick();
	
	while((ret = sem_trywait(pSem))!=TPR_OK)
	{
		TPR_UINT32 tEnd = TPR_GetTimeTick();
		
		if(tEnd - tStart > nTimeOut)
		{
			ret = TPR_ERROR;
			break;
		}
		TPR_Sleep(10);
	}
	
	return ret;
}


TPR_DECLARE TPR_INT32 CALLBACK TPR_SemPost(TPR_SEM_T* pSem)
{
	if (!pSem)
	{
		return TPR_ERROR;
	}

	return sem_post(pSem);
}



