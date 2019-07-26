

#include "TPR_Thread.h"

#if !defined _WIN32_WCE
#include <process.h>
#endif

/*************************************************
函数名称 : TPR_Thread_Create
函数功能 : 创建线程
输入参数说明 : StartAddress为线程执行体例程；Params为
传入线程执行体的参数；StackSize为线程堆栈的大小；
IsSuspend表示创建的线程是否挂起；Priority为线程优先级；
SchedPolicy为调度策略
函数返回值的说明 : 成功返回线程句柄，失败返回TPR_INVALID_HANDLE
*************************************************/
TPR_DECLARE TPR_HANDLE CALLBACK TPR_Thread_Create
(
	TPR_VOIDPTR (CALLBACK *StartAddress)(TPR_VOIDPTR), 
	TPR_VOID* Params, 
	TPR_UINT32 StackSize,
	TPR_INT32 Priority
)
{
	unsigned int initflag = 0;

/*Start of zhouzhixiang on 2013-9-24 10:54 V3.0.0_高性能*/
/* 原有该接口Windows环境下保留两个形参:
   TPR_BOOL IsSuspend : 表示线程创建后是否挂起，这里为了与Posix接口保持一致，
   且实际程序应用中没有创建时挂起的使用情况
   TPR_INT32 SchedPolicy: 修改前就没有使用
*/
#if 0
	if (IsSuspend)
	{ 
		initflag = CREATE_SUSPENDED;
	}
#endif
/*End of zhouzhixiang on 2013-9-24 10:54 V3.0.0_高性能*/

#if defined _WIN32_WCE
	return (TPR_HANDLE)CreateThread(NULL, StackSize, (LPTHREAD_START_ROUTINE)StartAddress, Params, initflag, NULL);
#else
	return (TPR_HANDLE)_beginthreadex(NULL, StackSize, (unsigned (__stdcall*)(void*))StartAddress, Params, initflag, NULL);
#endif
}

/*************************************************
函数名称 : TPR_Thread_Create
函数功能 : 创建可分离线程
输入参数说明 : StartAddress为线程执行体例程；Params为
传入线程执行体的参数；StackSize为线程堆栈的大小；
Priority无用， 仅作保持TPR库Posix统一接口用
函数返回值的说明 : 成功:TPR_TRUE，失败TPR_FALSE
*************************************************/
TPR_DECLARE TPR_BOOL CALLBACK TPR_ThreadDetached_Create
(
	 TPR_VOIDPTR(CALLBACK *StartAddress)(TPR_VOIDPTR),
	 TPR_VOID* Params, 
	 TPR_UINT32 StackSize,
	 TPR_INT32 Priority
 )
{
	HANDLE hThreadId;

	Priority;	/* TPR_ThreadDetached_Create为跨平台公有接口，而Win32环境下没有线程优先级参数，这里
				   保持接口统一，该操作防止pc-lint检查报未使用的形参 */
		
#if defined _WIN32_WCE
	hThreadId =  (TPR_HANDLE)CreateThread(NULL, StackSize, (LPTHREAD_START_ROUTINE)StartAddress, Params, 0, NULL);
#else
	hThreadId =  (TPR_HANDLE)_beginthreadex(NULL, StackSize, (unsigned (__stdcall*)(void*))StartAddress, Params, 0, NULL);
#endif

	if (hThreadId != NULL)
	{
		CloseHandle(hThreadId);
		hThreadId = NULL;
		return TPR_TRUE;
	}

	return TPR_FALSE;
}

/*************************************************
函数名称 : TPR_ThreadDetached_CreateEx
函数功能 : 创建可分离线程
输入参数说明 : StartAddress为线程执行体例程；Params为
传入线程执行体的参数；StackSize为线程堆栈的大小；
函数返回值的说明 : 成功返回线程句柄，失败返回TPR_INVALID_HANDLE
*************************************************/
TPR_DECLARE TPR_HANDLE CALLBACK TPR_ThreadDetached_CreateEx
(
	 TPR_VOIDPTR(CALLBACK *StartAddress)(TPR_VOIDPTR),
	 TPR_VOID* Params, 
	 TPR_UINT32 StackSize,
	 TPR_INT32 Priority
 )
{
	HANDLE hThreadId;
#if defined _WIN32_WCE
	hThreadId =  (TPR_HANDLE)CreateThread(NULL, StackSize, (LPTHREAD_START_ROUTINE)StartAddress, Params, 0, NULL);
#else
	hThreadId =  (TPR_HANDLE)_beginthreadex(NULL, StackSize, (unsigned (__stdcall*)(void*))StartAddress, Params, 0, NULL);
#endif
	if (hThreadId != NULL)
	{
		CloseHandle(hThreadId);
		return (TPR_HANDLE)hThreadId;
	}

	return (TPR_HANDLE)TPR_INVALID_THREAD;
}


/**
函数名称 : TPR_Thread_Exit（windows无效）
函数功能 : 退出线程
*/
TPR_DECLARE TPR_INT32 CALLBACK TPR_Thread_Exit()
{
#if !defined _WIN32_WCE
	_endthread();
#endif
	return 0;
}
/*************************************************
函数名称 : TPR_Thread_Wait
函数功能 : 等待线程线程的退出并关闭相应的句柄
输入参数说明 : ThreadHandle为线程句柄
函数返回值的说明 : 成功返回TPR_OK，失败返回TPR_ERROR
*************************************************/
TPR_DECLARE TPR_INT32 CALLBACK TPR_Thread_Wait(TPR_HANDLE ThreadHandle)
{
	if (ThreadHandle == (TPR_HANDLE)TPR_INVALID_THREAD)
	{
		return TPR_ERROR;
	}

	if (WAIT_OBJECT_0 == WaitForSingleObject(ThreadHandle, INFINITE))
	{
		CloseHandle(ThreadHandle);
		return TPR_OK;
	}

	return TPR_ERROR;
}

/*************************************************
函数名称 : TPR_Thread_Suspend
函数功能 : 挂起线程
输入参数说明 : ThreadHandle为线程句柄
函数返回值的说明 : 成功返回TPR_OK，失败返回TPR_ERROR
*************************************************/
TPR_DECLARE TPR_INT32 CALLBACK TPR_Thread_Suspend(TPR_HANDLE ThreadHandle)
{
	if (ThreadHandle == (TPR_HANDLE)TPR_INVALID_THREAD)
	{
		return TPR_ERROR;
	}

	DWORD ret = SuspendThread(ThreadHandle);
	if (ret == 0xFFFFFFFF)
	{
		return TPR_ERROR;
	}
	else
	{
		return TPR_OK;
	}
}

/*************************************************
函数名称 : TPR_Thread_Resume
函数功能 : 恢复挂起的线程
输入参数说明 : ThreadHandle为线程句柄
函数返回值的说明 : 成功返回TPR_OK，失败返回TPR_ERROR
*************************************************/
TPR_DECLARE TPR_INT32 CALLBACK TPR_Thread_Resume(TPR_HANDLE ThreadHandle)
{
	if (ThreadHandle == (TPR_HANDLE)TPR_INVALID_THREAD)
	{
		return TPR_ERROR;
	}

	DWORD ret = ResumeThread(ThreadHandle);
	if (ret == 0xFFFFFFFF)
	{
		return TPR_ERROR;
	}
	else
	{
		return TPR_OK;
	}
}

/*************************************************
函数名称 : TPR_Thread_SetPriority
函数功能 : 设置线程的优先级
输入参数说明 : ThreadHandle为线程句柄；Priority为线程优先级
函数返回值的说明 : 成功返回TPR_OK，失败返回TPR_ERROR
*************************************************/
TPR_DECLARE TPR_INT32 CALLBACK TPR_Thread_SetPriority(TPR_HANDLE ThreadHandle, TPR_INT32 Priority)
{
	if (ThreadHandle == (TPR_HANDLE)TPR_INVALID_THREAD)
	{
		return TPR_ERROR;
	}

	BOOL bret = SetThreadPriority(ThreadHandle, Priority);
	if (bret)
	{
		return TPR_OK;
	}
	else
	{
		return TPR_ERROR;
	}
}

/*************************************************
函数名称 : TPR_Thread_SetSchedPolicy
函数功能 : 设置调度策略
输入参数说明 : ThreadHandle为线程句柄；SchedPolicy为调度策略
函数返回值的说明 : 成功返回TPR_OK，失败返回TPR_ERROR
*************************************************/
TPR_DECLARE TPR_INT32 CALLBACK TPR_Thread_SetSchedPolicy(TPR_HANDLE ThreadHandle, TPR_INT32 SchedPolicy)
{
	if (ThreadHandle == (TPR_HANDLE)TPR_INVALID_THREAD)
	{
		return TPR_ERROR;
	}
#if !defined _WIN32_WCE
	//WinCE没有SetPriorityClass这个函数
	BOOL bret = SetPriorityClass(GetCurrentProcess(), SchedPolicy);
	if (bret)
	{
		return TPR_OK;
	}
	else
	{
		return TPR_ERROR;
	}
#else
	return TPR_NOSUPPORT;
#endif
}

/**
 * TPR_Thread_GetId
 * @param void
 * @return 线程ID
 * @sa
 */
TPR_DECLARE TPR_HANDLE CALLBACK TPR_Thread_GetSelfId()
{
	return (TPR_HANDLE)(LPARAM)::GetCurrentThreadId();
}

