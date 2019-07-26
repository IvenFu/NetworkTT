/**
 * CopyRight:   HangZhou Hikvision System Technology Co., Ltd. All  Right Reserved.
 * FileName:    HRP_Thread.cpp
 * Desc:        OS Layer Component.
 * Author:      guanguochen
 * Date:        2009-03-30
 * Contact:     guanguochen@hikvision.com.cn
 * History:     Created By guanguochen 2009-03-30
 * */

#include "TPR_Thread.h"
#include <pthread.h>


#ifdef CPU_BOND_SUP
#define MARVELL_CPUS       (2)
#define MARVELL_CPU_OFFSET    (2)
static TPR_INT32 s_cpuCnt = 0;
static TPR_VOID TPR_thread_set_2_cpu(TPR_HANDLE thHandle, int cpu)
{
    cpu_set_t cpu_mask;
    CPU_ZERO(&cpu_mask);
    CPU_SET(cpu, &cpu_mask);

    if( pthread_setaffinity_np(
        (pthread_t)thHandle, sizeof(cpu_mask), &cpu_mask) < 0 )
    {
        perror("pthread_setaffinity_np");
    }
    return;
}
#endif

/*************************************************
函数名称 : TPR_Thread_Create
函数功能 : 创建线程
输入参数说明 : StartAddress为线程执行体例程；Params为
传入线程执行体的参数；StackSize为线程堆栈的大小；
IsSuspend表示创建的线程是否挂起；Priority为线程优先级；
SchedPolicy为调度策略
函数返回值的说明 : 成功返回线程句柄，失败返回TPR_INVALID_THREAD
*************************************************/
TPR_DECLARE TPR_HANDLE TPR_Thread_Create
(
    TPR_VOIDPTR(*StartAddress)(TPR_VOIDPTR), 
    TPR_VOID* Params, 
    TPR_UINT32 StackSize,
    TPR_INT32 Priority
)
{
    int min = 0;
    int max = 0;
	
    #ifdef CPU_BOND_SUP
	static TPR_INT32 s_iStreamRecvCount = 0;	
	TPR_INT32 iCpuCount = 0;
	TPR_BOOL bStreamRecvTask = ((Priority==61)?TPR_TRUE:TPR_FALSE);//优先级为61的线程为码流接收线程
	if(bStreamRecvTask)
	{		
	    iCpuCount = s_iStreamRecvCount%MARVELL_CPUS+MARVELL_CPU_OFFSET;	
		s_iStreamRecvCount++;
	}
	else
	{
	    iCpuCount = s_cpuCnt%MARVELL_CPUS+MARVELL_CPU_OFFSET;			    
		s_cpuCnt++;
	}
	#endif

    pthread_attr_t threadattr;
    /* 初始化成功后必须调用pthread_attr_destroy(); */
    int ret = pthread_attr_init(&threadattr);
    if (ret)
    {
        return (TPR_HANDLE)TPR_INVALID_THREAD;
    }

#if 0 /* 暂不考虑支持APPLE 接口 */
#if (!defined (OS_APPLE)) //APPLE不支持
    //struct sched_param schedparam;
    //schedparam.sched_priority = SchedPolicy;
    ret = pthread_attr_setschedpolicy(&threadattr, SchedPolicy);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        return (TPR_HANDLE)TPR_INVALID_THREAD;
    }
#endif
#endif

    /* use the round robin scheduling algorithm */
    ret = pthread_attr_setschedpolicy(&threadattr, SCHED_RR);
    if (ret != 0)
    {
        pthread_attr_destroy(&threadattr);
        return (TPR_HANDLE)TPR_INVALID_THREAD;
    }


#if 0//defined (__linux__)
    struct sched_param param;
    min = sched_get_priority_min(SCHED_RR);
    if(-1 == min)
    {
        pthread_attr_destroy(&threadattr);
        return (TPR_HANDLE)TPR_INVALID_THREAD;
    }
    
    max = sched_get_priority_max(SCHED_RR);
    if(-1 == max)
    {
        pthread_attr_destroy(&threadattr);
        return (TPR_HANDLE)TPR_INVALID_THREAD;
    }
    
    if (Priority < min)
    {
        Priority = min;
    }
    else if (Priority > max)
    {
        Priority = max;
    }
    
    param.__sched_priority = Priority;
    ret = pthread_attr_setschedparam(&threadattr, &param);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        return (TPR_HANDLE)TPR_INVALID_THREAD;
    }
#endif

    if (StackSize != 0)
    {
        ret = pthread_attr_setstacksize(&threadattr, StackSize);
        if (ret != 0)
        {
            //printf("pthread_attr_setstacksize error\n");
            pthread_attr_destroy(&threadattr);
            return (TPR_HANDLE)(TPR_INVALID_THREAD);

        }
    }

    pthread_t threadid;
    ret = pthread_create(&threadid, &threadattr, StartAddress, Params);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        return (TPR_HANDLE)TPR_INVALID_THREAD;
    }
    else
    {
        pthread_attr_destroy(&threadattr);
        return (TPR_HANDLE)threadid;
    }

    
}

TPR_DECLARE TPR_BOOL TPR_ThreadDetached_Create
(
    TPR_VOIDPTR(*StartAddress)(TPR_VOIDPTR), 
    TPR_VOID* Params, 
    TPR_UINT32 StackSize,
    TPR_INT32 Priority
)
{
	#if 0
    int min = 0;
    int max = 0;
    
    pthread_attr_t threadattr;
    int ret = pthread_attr_init(&threadattr);
    if (ret)
    {
        return TPR_FALSE;
    }
    ret = pthread_attr_setdetachstate(&threadattr, PTHREAD_CREATE_DETACHED);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        return TPR_FALSE;
    }

    if (StackSize != 0)
    {
        ret = pthread_attr_setstacksize(&threadattr, StackSize);
        if (ret != 0)
        {
            pthread_attr_destroy(&threadattr);
            return TPR_FALSE;

        }
    }
    
    /* use the round robin scheduling algorithm */
    ret = pthread_attr_setschedpolicy(&threadattr, SCHED_RR);
    if (ret != 0)
    {
        pthread_attr_destroy(&threadattr);
        return TPR_FALSE;
    }

    struct sched_param param;
    min = sched_get_priority_min(SCHED_RR);
    if(-1 == min)
    {
        pthread_attr_destroy(&threadattr);
        return TPR_FALSE;
    }
    
    max = sched_get_priority_max(SCHED_RR);
    if(-1 == max)
    {
        pthread_attr_destroy(&threadattr);
        return TPR_FALSE;
    }
    
    if (Priority < min)
    {
        Priority = min;
    }
    else if (Priority > max)
    {
        Priority = max;
    }
    
    param.__sched_priority = Priority;
    ret = pthread_attr_setschedparam(&threadattr, &param);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        return TPR_FALSE;
    }


    pthread_t threadid;
    ret = pthread_create(&threadid, &threadattr, StartAddress, Params);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        return TPR_FALSE;
    }
    else
    {
        //return (TPR_HANDLE)threadid;
        pthread_attr_destroy(&threadattr);
        return TPR_TRUE;
    }
    #endif
	
	return TPR_FALSE;
	
}

TPR_DECLARE TPR_HANDLE TPR_ThreadDetached_CreateEx
(TPR_VOIDPTR(*StartAddress)(TPR_VOIDPTR), TPR_VOID* Params, TPR_UINT32 StackSize, TPR_INT32 Priority)
{
	#if 0
    int min = 0;
    int max = 0;
    pthread_attr_t threadattr;
    int ret = pthread_attr_init(&threadattr);
    if (ret)
    {
        return (TPR_HANDLE)TPR_INVALID_THREAD;
    }
    ret = pthread_attr_setdetachstate(&threadattr, PTHREAD_CREATE_DETACHED);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        return (TPR_HANDLE)TPR_INVALID_THREAD;
    }

    if (StackSize != 0)
    {
        ret = pthread_attr_setstacksize(&threadattr, StackSize);
        if (ret != 0)
        {
            pthread_attr_destroy(&threadattr);
            return (TPR_HANDLE)TPR_INVALID_THREAD;

        }
    }

    /* use the round robin scheduling algorithm */
    ret = pthread_attr_setschedpolicy(&threadattr, SCHED_RR);
    if (ret != 0)
    {
        pthread_attr_destroy(&threadattr);
        return (TPR_HANDLE)TPR_INVALID_THREAD;
    }

    struct sched_param param;
    min = sched_get_priority_min(SCHED_RR);
    if(-1 == min)
    {
        pthread_attr_destroy(&threadattr);
        return (TPR_HANDLE)TPR_INVALID_THREAD;
    }
    
    max = sched_get_priority_max(SCHED_RR);
    if(-1 == max)
    {
        pthread_attr_destroy(&threadattr);
        return (TPR_HANDLE)TPR_INVALID_THREAD;
    }
    
    if (Priority < min)
    {
        Priority = min;
    }
    else if (Priority > max)
    {
        Priority = max;
    }
    param.__sched_priority = Priority;
    ret = pthread_attr_setschedparam(&threadattr, &param);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        return (TPR_HANDLE)TPR_INVALID_THREAD;
    }

    pthread_t threadid;
    ret = pthread_create(&threadid, &threadattr, StartAddress, Params);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        return (TPR_HANDLE)TPR_INVALID_THREAD;
    }
    else
    {
        pthread_attr_destroy(&threadattr);
        return (TPR_HANDLE)threadid;
    }
    #endif
	return NULL;
}


TPR_DECLARE TPR_INT32 TPR_Thread_Exit()
{
    pthread_exit(0);
}

/*************************************************
函数名称 : TPR_Thread_Wait
函数功能 : 等待线程线程的退出并关闭相应的句柄
输入参数说明 : ThreadHandle为线程句柄
函数返回值的说明 : 成功返回TPR_OK，失败返回TPR_ERROR
*************************************************/
TPR_DECLARE TPR_INT32 TPR_Thread_Wait(TPR_HANDLE ThreadHandle)
{
    if (ThreadHandle == (TPR_HANDLE)TPR_INVALID_THREAD)
    {
        return TPR_ERROR;
    }

    int ret = pthread_join((pthread_t)ThreadHandle, 0);
    if (ret)
    {
        return TPR_ERROR;
    }
    else
    {
        //close((int)ThreadHandle);
        return TPR_OK;
    }
}

TPR_DECLARE TPR_HANDLE CALLBACK TPR_Thread_GetSelfId()
{
    return (TPR_HANDLE)pthread_self();
}


/*************************************************
函数名称 : TPR_Thread_Suspend
函数功能 : 挂起线程
输入参数说明 : ThreadHandle为线程句柄
函数返回值的说明 : 返回TPR_OK
*************************************************/
TPR_DECLARE TPR_INT32 TPR_Thread_Suspend(TPR_HANDLE ThreadHandle)
{
    return TPR_OK;
}

/*************************************************
函数名称 : TPR_Thread_Resume
函数功能 : 恢复挂起的线程
输入参数说明 : ThreadHandle为线程句柄
函数返回值的说明 : 返回TPR_OK
*************************************************/
TPR_DECLARE TPR_INT32 TPR_Thread_Resume(TPR_HANDLE ThreadHandle)
{
    return TPR_OK;
}

/*************************************************
函数名称 : TPR_Thread_SetPriority
函数功能 : 设置线程的优先级
输入参数说明 : ThreadHandle为线程句柄；Priority为线程优先级
函数返回值的说明 : 成功返回TPR_OK，失败返回TPR_ERROR
*************************************************/
TPR_DECLARE TPR_INT32 TPR_Thread_SetPriority(TPR_HANDLE ThreadHandle, TPR_INT32 Priority)
{
    if (ThreadHandle == (TPR_HANDLE)TPR_INVALID_THREAD)
    {
        return TPR_ERROR;
    }

    int policy;
    struct sched_param param;
    int ret = pthread_getschedparam((pthread_t)ThreadHandle, &policy, &param);
    if (ret)
    {
        return TPR_ERROR;
    }

#if 0//defined (__linux__)
    param.__sched_priority = Priority;
    ret = pthread_setschedparam((pthread_t)ThreadHandle, policy, &param);
    if (ret)
    {
        return TPR_ERROR;
    }
    else
    {
        return TPR_OK;
    }
#endif
    
    return TPR_OK;
}

/*************************************************
函数名称 : TPR_Thread_SetSchedPolicy
函数功能 : 设置调度策略
输入参数说明 : ThreadHandle为线程句柄；SchedPolicy为调度策略
函数返回值的说明 : 成功返回TPR_OK，失败返回TPR_ERROR
*************************************************/
TPR_DECLARE TPR_INT32 TPR_Thread_SetSchedPolicy(TPR_HANDLE ThreadHandle, TPR_INT32 SchedPolicy)
{
    if (ThreadHandle == (TPR_HANDLE)TPR_INVALID_THREAD)
    {
        return TPR_ERROR;
    }

    pthread_attr_t thread_attr;
    int ret = pthread_attr_init(&thread_attr);
    if (ret)
    {
        return TPR_ERROR;
    }

    ret = pthread_attr_setschedpolicy(&thread_attr, SchedPolicy);
    pthread_attr_destroy(&thread_attr);
    if (ret)
    {
        return TPR_ERROR;
    }
    else
    {
        return TPR_OK;
    }
}


