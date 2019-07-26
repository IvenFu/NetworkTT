

#ifndef __TPR_THREAD_H__    
#define __TPR_THREAD_H__

#include "TPR_Config.h"
#include "TPR_Types.h"

#if defined (OS_WINDOWS)
#define TPR_INVALID_THREAD (HANDLE)(NULL)
#elif defined (OS_POSIX)
#define TPR_INVALID_THREAD (TPR_HANDLE)(-1)
#endif

/**
 * TPR_Thread_Create create thread.
 * must call TPR_Thread_Wait() release thread resource;
 * @param StartAddress (IN) thread body
 * @param Params (IN) parameter input into thread body.
 * @param StackSize (IN) thread body stack size.
 * @param IsSuspend (IN) is thread suspend after create, only valid in windows.
 * @param Priority (IN) thread priority
 * @param SchedPolicy (IN) thread sched policy
 * @return NULL fail, else success.
 * @sa TPR_ThreadDetached_Create(),TPR_Thread_Wait()
 */
TPR_DECLARE TPR_HANDLE CALLBACK TPR_Thread_Create
(
	TPR_VOIDPTR(CALLBACK *StartAddress)(TPR_VOIDPTR), 
	TPR_VOID* Params, 
	TPR_UINT32 StackSize,
	TPR_INT32 Priority
);


/**
 * TPR_ThreadDetached_Create create thread with detached attr.
 *  thread resource will be release by OS kernal automatically
 * @param StartAddress (IN) thread body
 * @param Params (IN) parameter input into thread body.
 * @param StackSize (IN) thread body stack size.
 * @param Priority (IN) no use for win32
 * @return 0 success, -1 fail
 * @sa TPR_Thread_Create()
 */
TPR_DECLARE TPR_BOOL CALLBACK TPR_ThreadDetached_Create
(
	 TPR_VOIDPTR(CALLBACK *StartAddress)(TPR_VOIDPTR),
	 TPR_VOID* Params, 
	 TPR_UINT32 StackSize,
	 TPR_INT32 Priority
 );

// added by fenggx begin!
/**
 * TPR_ThreadDetached_CreateEx create thread with detached attr.
 *  thread resource will be release by OS kernal automatically
 * @param StartAddress (IN) thread body
 * @param Params (IN) parameter input into thread body.
 * @param StackSize (IN) thread body stack size.
 * @return TPR_HANDLE if success, TPR_INVALID_HANDLE if fail!
 * @sa TPR_ThreadDetached_CreateEx()
 */
TPR_DECLARE TPR_HANDLE CALLBACK TPR_ThreadDetached_CreateEx
(
    TPR_VOIDPTR(CALLBACK *StartAddress)(TPR_VOIDPTR),
    TPR_VOID* Params, 
    TPR_UINT32 StackSize, 
    TPR_INT32 Priority
);
// added by fenggx end

/**
 * TPR_Thread_Wait wait thread and release resource.
 * @param ThreadHandle (IN) thread handle, created by TPR_Thread_Create
 * @return 0 success, -1 fail
 * @sa TPR_Thread_Create()
 */
TPR_DECLARE TPR_INT32 CALLBACK TPR_Thread_Wait(TPR_HANDLE ThreadHandle);

/**
 * TPR_Thread_Suspend suspend thread
 * @param ThreadHandle (IN) thread handle, created by TPR_Thread_Create
 * @return 0 success, -1 fail
 * @sa TPR_Thread_Create()
 */
TPR_DECLARE TPR_INT32 CALLBACK TPR_Thread_Suspend(TPR_HANDLE ThreadHandle);

/**
 * TPR_Thread_Resume resume thread, only valid in windows
 * @param ThreadHandle (IN) thread handle, created by TPR_Thread_Create
 * @return 0 success, -1 fail
 * @sa TPR_Thread_Create()
 */
TPR_DECLARE TPR_INT32 CALLBACK TPR_Thread_Resume(TPR_HANDLE ThreadHandle);

/**
 * TPR_Thread_SetPriority set thread priority
 * @param ThreadHandle (IN) thread handle, created by TPR_Thread_Create
 * @param Priority (IN) sched priority
 * @return 0 success, -1 fail
 * @sa TPR_Thread_Create()
 */
TPR_DECLARE TPR_INT32 CALLBACK TPR_Thread_SetPriority(TPR_HANDLE ThreadHandle, TPR_INT32 Priority);

/**
 * TPR_Thread_SetSchedPolicy set thread sched policy
 * @param ThreadHandle (IN) thread handle, created by TPR_Thread_Create
 * @param SchedPolicy (IN) sched policy
 * @return 0 success, -1 fail
 * @sa TPR_Thread_Create()
 */
TPR_DECLARE TPR_INT32 CALLBACK TPR_Thread_SetSchedPolicy(TPR_HANDLE ThreadHandle, TPR_INT32 SchedPolicy);

/**
 * TPR_Thread_Exit exit thread.
 * @return 0 success, -1 fail.
 * @sa TPR_ThreadDetached_Create()
 */
TPR_DECLARE TPR_INT32 CALLBACK TPR_Thread_Exit();

/**
 * TPR_Thread_GetId
 * @param void
 * @return Ïß³ÌID
 * @sa
 */
TPR_DECLARE TPR_HANDLE CALLBACK TPR_Thread_GetSelfId();

#endif
