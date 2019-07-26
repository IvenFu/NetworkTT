
#ifndef __TPR_SEMAPHORE_H__
#define __TPR_SEMAPHORE_H__

#include "TPR_Config.h"
#include "TPR_Types.h"

#if defined (OS_WINDOWS)
	typedef TPR_HANDLE TPR_SEM_T;
#elif defined(OS_POSIX)
	#include <semaphore.h>
#if defined(__APPLE__)
	typedef sem_t* TPR_SEM_T;
#else
	typedef sem_t TPR_SEM_T;
#endif
#else
	#error OS Not Implement Yet.
#endif

/**
 * TPR_SemCreate
 * @param pSem (OUT) sem want to create.
 * @param nInitCount (IN) init resources after create.
 * @return 0 success, -1 fail.
 * @sa TPR_SemDestroy(),TPR_SemWait(),TPR_SemTimedWait(),TPR_SemPost()
 */
TPR_DECLARE TPR_INT32 CALLBACK TPR_SemCreate(TPR_SEM_T* pSem, TPR_UINT32 nInitCount);

/**
 * TPR_SemDestroy
 * @param pSem (IN) created by TPR_SemCreate
 * @return 0 success, -1 fail.
 * @sa TPR_SemCreate(),TPR_SemWait(),TPR_SemTimedWait(),TPR_SemPost()
 */
TPR_DECLARE TPR_INT32 CALLBACK TPR_SemDestroy(TPR_SEM_T* pSem);

/**
 * TPR_SemWait
 * @param pSem (IN) created by TPR_SemCreate
 * @return 0 success, -1 fail.
 * @sa TPR_SemCreate(),TPR_SemDestroy(),TPR_SemTimedWait(),TPR_SemPost()
 */
TPR_DECLARE TPR_INT32 CALLBACK TPR_SemWait(TPR_SEM_T* pSem);

/**
 * TPR_SemTimedWait
 * @param pSem (IN) created by TPR_SemCreate
 * @param nTimeOut (IN) wait timeout.
 * @return 0 success, -1 fail.
 * @sa TPR_SemCreate(),TPR_SemDestroy(),TPR_SemWait(),TPR_SemPost()
 */
TPR_DECLARE TPR_INT32 CALLBACK TPR_SemTimedWait(TPR_SEM_T* pSem, TPR_UINT32 nTimeOut);

/**
 * TPR_SemPost
 * @param pSem (IN) created by TPR_SemCreate
 * @return 0 success, -1 fail.
 * @sa TPR_SemCreate(),TPR_SemDestroy(),TPR_SemWait(),TPR_SemTimedWait()
 */
TPR_DECLARE TPR_INT32 CALLBACK TPR_SemPost(TPR_SEM_T* pSem);


#endif
