/**
 * CopyRight:   HangZhou Hikvision System Technology Co., Ltd. All  Right Reserved.
 * FileName:    HRP_Thread.cpp
 * Desc:        OS Layer Component.
 * Author:      guanguochen
 * Date:        2009-03-30
 * Contact:     guanguochen@hikvision.com.cn
 * History:     Created By guanguochen 2009-03-30
 * */

#include "HPR_Thread.h"
#include <pthread.h>


#ifdef CPU_BOND_SUP
#define MARVELL_CPUS       (2)
#define MARVELL_CPU_OFFSET    (2)
static HPR_INT32 s_cpuCnt = 0;
static HPR_VOID hpr_thread_set_2_cpu(HPR_HANDLE thHandle, int cpu)
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
�������� : HPR_Thread_Create
�������� : �����߳�
�������˵�� : StartAddressΪ�߳�ִ�������̣�ParamsΪ
�����߳�ִ����Ĳ�����StackSizeΪ�̶߳�ջ�Ĵ�С��
IsSuspend��ʾ�������߳��Ƿ����PriorityΪ�߳����ȼ���
SchedPolicyΪ���Ȳ���
��������ֵ��˵�� : �ɹ������߳̾����ʧ�ܷ���HPR_INVALID_THREAD
*************************************************/
HPR_DECLARE HPR_HANDLE HPR_Thread_Create
(
    HPR_VOIDPTR(*StartAddress)(HPR_VOIDPTR), 
    HPR_VOID* Params, 
    HPR_UINT32 StackSize,
    HPR_INT32 Priority
)
{
    int min = 0;
    int max = 0;
	
    #ifdef CPU_BOND_SUP
	static HPR_INT32 s_iStreamRecvCount = 0;	
	HPR_INT32 iCpuCount = 0;
	HPR_BOOL bStreamRecvTask = ((Priority==61)?HPR_TRUE:HPR_FALSE);//���ȼ�Ϊ61���߳�Ϊ���������߳�
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
    /* ��ʼ���ɹ���������pthread_attr_destroy(); */
    int ret = pthread_attr_init(&threadattr);
    if (ret)
    {
        return (HPR_HANDLE)HPR_INVALID_THREAD;
    }

#if 0 /* �ݲ�����֧��APPLE �ӿ� */
#if (!defined (OS_APPLE)) //APPLE��֧��
    //struct sched_param schedparam;
    //schedparam.sched_priority = SchedPolicy;
    ret = pthread_attr_setschedpolicy(&threadattr, SchedPolicy);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        return (HPR_HANDLE)HPR_INVALID_THREAD;
    }
#endif
#endif

    /* use the round robin scheduling algorithm */
    ret = pthread_attr_setschedpolicy(&threadattr, SCHED_RR);
    if (ret != 0)
    {
        pthread_attr_destroy(&threadattr);
        return (HPR_HANDLE)HPR_INVALID_THREAD;
    }


#if defined (__linux__)
    struct sched_param param;
    min = sched_get_priority_min(SCHED_RR);
    if(-1 == min)
    {
        pthread_attr_destroy(&threadattr);
        return (HPR_HANDLE)HPR_INVALID_THREAD;
    }
    
    max = sched_get_priority_max(SCHED_RR);
    if(-1 == max)
    {
        pthread_attr_destroy(&threadattr);
        return (HPR_HANDLE)HPR_INVALID_THREAD;
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
        return (HPR_HANDLE)HPR_INVALID_THREAD;
    }
#endif

    if (StackSize != 0)
    {
        ret = pthread_attr_setstacksize(&threadattr, StackSize);
        if (ret != 0)
        {
            //printf("pthread_attr_setstacksize error\n");
            pthread_attr_destroy(&threadattr);
            return (HPR_HANDLE)(HPR_INVALID_THREAD);

        }
    }

    pthread_t threadid;
    ret = pthread_create(&threadid, &threadattr, StartAddress, Params);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        return (HPR_HANDLE)HPR_INVALID_THREAD;
    }
    else
    {
        pthread_attr_destroy(&threadattr);
        return (HPR_HANDLE)threadid;
    }

    
}

HPR_DECLARE HPR_BOOL HPR_ThreadDetached_Create
(
    HPR_VOIDPTR(*StartAddress)(HPR_VOIDPTR), 
    HPR_VOID* Params, 
    HPR_UINT32 StackSize,
    HPR_INT32 Priority
)
{
    int min = 0;
    int max = 0;
    
    pthread_attr_t threadattr;
    int ret = pthread_attr_init(&threadattr);
    if (ret)
    {
        return HPR_FALSE;
    }
    ret = pthread_attr_setdetachstate(&threadattr, PTHREAD_CREATE_DETACHED);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        return HPR_FALSE;
    }

    if (StackSize != 0)
    {
        ret = pthread_attr_setstacksize(&threadattr, StackSize);
        if (ret != 0)
        {
            pthread_attr_destroy(&threadattr);
            return HPR_FALSE;

        }
    }
    
    /* use the round robin scheduling algorithm */
    ret = pthread_attr_setschedpolicy(&threadattr, SCHED_RR);
    if (ret != 0)
    {
        pthread_attr_destroy(&threadattr);
        return HPR_FALSE;
    }

    struct sched_param param;
    min = sched_get_priority_min(SCHED_RR);
    if(-1 == min)
    {
        pthread_attr_destroy(&threadattr);
        return HPR_FALSE;
    }
    
    max = sched_get_priority_max(SCHED_RR);
    if(-1 == max)
    {
        pthread_attr_destroy(&threadattr);
        return HPR_FALSE;
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
        return HPR_FALSE;
    }


    pthread_t threadid;
    ret = pthread_create(&threadid, &threadattr, StartAddress, Params);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        return HPR_FALSE;
    }
    else
    {
        //return (HPR_HANDLE)threadid;
        pthread_attr_destroy(&threadattr);
        return HPR_TRUE;
    }
    
}

HPR_DECLARE HPR_HANDLE HPR_ThreadDetached_CreateEx
(HPR_VOIDPTR(*StartAddress)(HPR_VOIDPTR), HPR_VOID* Params, HPR_UINT32 StackSize, HPR_INT32 Priority)
{
    int min = 0;
    int max = 0;
    pthread_attr_t threadattr;
    int ret = pthread_attr_init(&threadattr);
    if (ret)
    {
        return (HPR_HANDLE)HPR_INVALID_THREAD;
    }
    ret = pthread_attr_setdetachstate(&threadattr, PTHREAD_CREATE_DETACHED);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        return (HPR_HANDLE)HPR_INVALID_THREAD;
    }

    if (StackSize != 0)
    {
        ret = pthread_attr_setstacksize(&threadattr, StackSize);
        if (ret != 0)
        {
            pthread_attr_destroy(&threadattr);
            return (HPR_HANDLE)HPR_INVALID_THREAD;

        }
    }

    /* use the round robin scheduling algorithm */
    ret = pthread_attr_setschedpolicy(&threadattr, SCHED_RR);
    if (ret != 0)
    {
        pthread_attr_destroy(&threadattr);
        return (HPR_HANDLE)HPR_INVALID_THREAD;
    }

    struct sched_param param;
    min = sched_get_priority_min(SCHED_RR);
    if(-1 == min)
    {
        pthread_attr_destroy(&threadattr);
        return (HPR_HANDLE)HPR_INVALID_THREAD;
    }
    
    max = sched_get_priority_max(SCHED_RR);
    if(-1 == max)
    {
        pthread_attr_destroy(&threadattr);
        return (HPR_HANDLE)HPR_INVALID_THREAD;
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
        return (HPR_HANDLE)HPR_INVALID_THREAD;
    }

    pthread_t threadid;
    ret = pthread_create(&threadid, &threadattr, StartAddress, Params);
    if (ret)
    {
        pthread_attr_destroy(&threadattr);
        return (HPR_HANDLE)HPR_INVALID_THREAD;
    }
    else
    {
        pthread_attr_destroy(&threadattr);
        return (HPR_HANDLE)threadid;
    }
    
}


HPR_DECLARE HPR_INT32 HPR_Thread_Exit()
{
    pthread_exit(0);
}

/*************************************************
�������� : HPR_Thread_Wait
�������� : �ȴ��߳��̵߳��˳����ر���Ӧ�ľ��
�������˵�� : ThreadHandleΪ�߳̾��
��������ֵ��˵�� : �ɹ�����HPR_OK��ʧ�ܷ���HPR_ERROR
*************************************************/
HPR_DECLARE HPR_INT32 HPR_Thread_Wait(HPR_HANDLE ThreadHandle)
{
    if (ThreadHandle == (HPR_HANDLE)HPR_INVALID_THREAD)
    {
        return HPR_ERROR;
    }

    int ret = pthread_join((pthread_t)ThreadHandle, 0);
    if (ret)
    {
        return HPR_ERROR;
    }
    else
    {
        //close((int)ThreadHandle);
        return HPR_OK;
    }
}

HPR_DECLARE HPR_HANDLE CALLBACK HPR_Thread_GetSelfId()
{
    return (HPR_HANDLE)pthread_self();
}


/*************************************************
�������� : HPR_Thread_Suspend
�������� : �����߳�
�������˵�� : ThreadHandleΪ�߳̾��
��������ֵ��˵�� : ����HPR_OK
*************************************************/
HPR_DECLARE HPR_INT32 HPR_Thread_Suspend(HPR_HANDLE ThreadHandle)
{
    return HPR_OK;
}

/*************************************************
�������� : HPR_Thread_Resume
�������� : �ָ�������߳�
�������˵�� : ThreadHandleΪ�߳̾��
��������ֵ��˵�� : ����HPR_OK
*************************************************/
HPR_DECLARE HPR_INT32 HPR_Thread_Resume(HPR_HANDLE ThreadHandle)
{
    return HPR_OK;
}

/*************************************************
�������� : HPR_Thread_SetPriority
�������� : �����̵߳����ȼ�
�������˵�� : ThreadHandleΪ�߳̾����PriorityΪ�߳����ȼ�
��������ֵ��˵�� : �ɹ�����HPR_OK��ʧ�ܷ���HPR_ERROR
*************************************************/
HPR_DECLARE HPR_INT32 HPR_Thread_SetPriority(HPR_HANDLE ThreadHandle, HPR_INT32 Priority)
{
    if (ThreadHandle == (HPR_HANDLE)HPR_INVALID_THREAD)
    {
        return HPR_ERROR;
    }

    int policy;
    struct sched_param param;
    int ret = pthread_getschedparam((pthread_t)ThreadHandle, &policy, &param);
    if (ret)
    {
        return HPR_ERROR;
    }

#if defined (__linux__)
    param.__sched_priority = Priority;
    ret = pthread_setschedparam((pthread_t)ThreadHandle, policy, &param);
    if (ret)
    {
        return HPR_ERROR;
    }
    else
    {
        return HPR_OK;
    }
#endif
    
    return HPR_OK;
}

/*************************************************
�������� : HPR_Thread_SetSchedPolicy
�������� : ���õ��Ȳ���
�������˵�� : ThreadHandleΪ�߳̾����SchedPolicyΪ���Ȳ���
��������ֵ��˵�� : �ɹ�����HPR_OK��ʧ�ܷ���HPR_ERROR
*************************************************/
HPR_DECLARE HPR_INT32 HPR_Thread_SetSchedPolicy(HPR_HANDLE ThreadHandle, HPR_INT32 SchedPolicy)
{
    if (ThreadHandle == (HPR_HANDLE)HPR_INVALID_THREAD)
    {
        return HPR_ERROR;
    }

    pthread_attr_t thread_attr;
    int ret = pthread_attr_init(&thread_attr);
    if (ret)
    {
        return HPR_ERROR;
    }

    ret = pthread_attr_setschedpolicy(&thread_attr, SchedPolicy);
    pthread_attr_destroy(&thread_attr);
    if (ret)
    {
        return HPR_ERROR;
    }
    else
    {
        return HPR_OK;
    }
}


