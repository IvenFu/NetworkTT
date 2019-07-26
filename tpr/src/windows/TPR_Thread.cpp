

#include "TPR_Thread.h"

#if !defined _WIN32_WCE
#include <process.h>
#endif

/*************************************************
�������� : TPR_Thread_Create
�������� : �����߳�
�������˵�� : StartAddressΪ�߳�ִ�������̣�ParamsΪ
�����߳�ִ����Ĳ�����StackSizeΪ�̶߳�ջ�Ĵ�С��
IsSuspend��ʾ�������߳��Ƿ����PriorityΪ�߳����ȼ���
SchedPolicyΪ���Ȳ���
��������ֵ��˵�� : �ɹ������߳̾����ʧ�ܷ���TPR_INVALID_HANDLE
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

/*Start of zhouzhixiang on 2013-9-24 10:54 V3.0.0_������*/
/* ԭ�иýӿ�Windows�����±��������β�:
   TPR_BOOL IsSuspend : ��ʾ�̴߳������Ƿ��������Ϊ����Posix�ӿڱ���һ�£�
   ��ʵ�ʳ���Ӧ����û�д���ʱ�����ʹ�����
   TPR_INT32 SchedPolicy: �޸�ǰ��û��ʹ��
*/
#if 0
	if (IsSuspend)
	{ 
		initflag = CREATE_SUSPENDED;
	}
#endif
/*End of zhouzhixiang on 2013-9-24 10:54 V3.0.0_������*/

#if defined _WIN32_WCE
	return (TPR_HANDLE)CreateThread(NULL, StackSize, (LPTHREAD_START_ROUTINE)StartAddress, Params, initflag, NULL);
#else
	return (TPR_HANDLE)_beginthreadex(NULL, StackSize, (unsigned (__stdcall*)(void*))StartAddress, Params, initflag, NULL);
#endif
}

/*************************************************
�������� : TPR_Thread_Create
�������� : �����ɷ����߳�
�������˵�� : StartAddressΪ�߳�ִ�������̣�ParamsΪ
�����߳�ִ����Ĳ�����StackSizeΪ�̶߳�ջ�Ĵ�С��
Priority���ã� ��������TPR��Posixͳһ�ӿ���
��������ֵ��˵�� : �ɹ�:TPR_TRUE��ʧ��TPR_FALSE
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

	Priority;	/* TPR_ThreadDetached_CreateΪ��ƽ̨���нӿڣ���Win32������û���߳����ȼ�����������
				   ���ֽӿ�ͳһ���ò�����ֹpc-lint��鱨δʹ�õ��β� */
		
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
�������� : TPR_ThreadDetached_CreateEx
�������� : �����ɷ����߳�
�������˵�� : StartAddressΪ�߳�ִ�������̣�ParamsΪ
�����߳�ִ����Ĳ�����StackSizeΪ�̶߳�ջ�Ĵ�С��
��������ֵ��˵�� : �ɹ������߳̾����ʧ�ܷ���TPR_INVALID_HANDLE
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
�������� : TPR_Thread_Exit��windows��Ч��
�������� : �˳��߳�
*/
TPR_DECLARE TPR_INT32 CALLBACK TPR_Thread_Exit()
{
#if !defined _WIN32_WCE
	_endthread();
#endif
	return 0;
}
/*************************************************
�������� : TPR_Thread_Wait
�������� : �ȴ��߳��̵߳��˳����ر���Ӧ�ľ��
�������˵�� : ThreadHandleΪ�߳̾��
��������ֵ��˵�� : �ɹ�����TPR_OK��ʧ�ܷ���TPR_ERROR
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
�������� : TPR_Thread_Suspend
�������� : �����߳�
�������˵�� : ThreadHandleΪ�߳̾��
��������ֵ��˵�� : �ɹ�����TPR_OK��ʧ�ܷ���TPR_ERROR
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
�������� : TPR_Thread_Resume
�������� : �ָ�������߳�
�������˵�� : ThreadHandleΪ�߳̾��
��������ֵ��˵�� : �ɹ�����TPR_OK��ʧ�ܷ���TPR_ERROR
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
�������� : TPR_Thread_SetPriority
�������� : �����̵߳����ȼ�
�������˵�� : ThreadHandleΪ�߳̾����PriorityΪ�߳����ȼ�
��������ֵ��˵�� : �ɹ�����TPR_OK��ʧ�ܷ���TPR_ERROR
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
�������� : TPR_Thread_SetSchedPolicy
�������� : ���õ��Ȳ���
�������˵�� : ThreadHandleΪ�߳̾����SchedPolicyΪ���Ȳ���
��������ֵ��˵�� : �ɹ�����TPR_OK��ʧ�ܷ���TPR_ERROR
*************************************************/
TPR_DECLARE TPR_INT32 CALLBACK TPR_Thread_SetSchedPolicy(TPR_HANDLE ThreadHandle, TPR_INT32 SchedPolicy)
{
	if (ThreadHandle == (TPR_HANDLE)TPR_INVALID_THREAD)
	{
		return TPR_ERROR;
	}
#if !defined _WIN32_WCE
	//WinCEû��SetPriorityClass�������
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
 * @return �߳�ID
 * @sa
 */
TPR_DECLARE TPR_HANDLE CALLBACK TPR_Thread_GetSelfId()
{
	return (TPR_HANDLE)(LPARAM)::GetCurrentThreadId();
}

