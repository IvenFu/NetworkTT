
#include "TPR_Select.h"
#include "TPR_Time.h"


TPR_DECLARE TPR_INT32 CALLBACK TPR_FdIsSet(TPR_INT32 fd, fd_set* set)
{
	return FD_ISSET(fd,set);
}

//与windows 的select功能一支的select
TPR_DECLARE TPR_INT32 CALLBACK TPR_Select(TPR_INT32 iMaxFd, fd_set* fdset_r, fd_set* fdset_w, fd_set* fdset_e,timeval* tv)
{
#if defined(OS_WINDOWS)
	return ::select(iMaxFd,fdset_r,fdset_w,fdset_e,tv);
#elif defined(OS_POSIX)
	timeval t;
	if(!tv)
	{
		return ::select(iMaxFd,fdset_r,fdset_w,fdset_e,NULL);
	}
	
	t = *tv;
	return ::select(iMaxFd,fdset_r,fdset_w,fdset_e,&t);
#endif
}

//与linux 的select功能一支的select
TPR_DECLARE TPR_INT32 CALLBACK TPR_SelectEx(TPR_INT32 iMaxFd, fd_set* fdset_r, fd_set* fdset_w, fd_set* fdset_e,timeval* tv)
{
#if defined(OS_WINDOWS)
	TPR_INT32 iRet;
	TPR_UINT64 iStartTime;
	TPR_UINT64 iTimeDiff;
	
	if(!tv)
	{
		return ::select(iMaxFd,fdset_r,fdset_w,fdset_e,NULL);
	}

	iStartTime = (TPR_UINT64)TPR_GetTimeTick64();
	iRet = ::select(iMaxFd,fdset_r,fdset_w,fdset_e,tv);
	iTimeDiff = (TPR_UINT64)TPR_GetTimeTick64() - iStartTime;

	if(iTimeDiff>0)
	{
		//TBD 是否有问题？
		if(tv->tv_sec > (long)(iTimeDiff/1000))
		{
			tv->tv_sec -= (long)(iTimeDiff/1000);
		}
		else
		{
			tv->tv_sec = 0;
		}

		if(tv->tv_usec > (long)(iTimeDiff%1000)*1000)
		{
			tv->tv_usec -= (iTimeDiff%1000)*1000;
		}
		else
		{
			tv->tv_usec = 0;
		}
	}
	else if(iTimeDiff == 0)
	{
	
	}

	return iRet;
	
#elif defined(OS_POSIX)
	return ::select(iMaxFd,fdset_r,fdset_w,fdset_e,tv);
#endif
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_Poll(struct pollfd* fds, TPR_INT32 iFds, TPR_INT32 iTimeOut)
{
#if defined(OS_POSIX)
	return poll(fds,iFds, iTimeOut);
#endif
	return TPR_NPTSUPPORT;
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_PollEx(struct pollfd* fds, TPR_INT32 iFds, TPR_INT32* iTimeO)
{
#if defined(OS_POSIX)
	TPR_INT32 iRet = TPR_ERROR;
	TPR_UINT64 iStartTime;
	TPR_UINT64 iTimeOff;

	if((iTimeO == NULL) || (*iTimeO == -1))
	{
RETRY:
		iRet = poll(fds, iFds, -1);
		if(iRet == -1 && errno == EINTR)
		{
			goto RETRY;
		}

		return iRet;
	}

REPOLL:
	iStartTime = (TPR_UINT64)TPR_GetTimeTick64();
	iRet = poll(fds,iFds, *iTimeO);
	iTimeOff = (TPR_UINT64)TPR_GetTimeTick64()-iStartTime;

	if(iRet == -1&& errno == EINTR)
	{
		*iTimeO = iTimeOff;
		if(*iTimeO > 0)
		{
			goto REPOLL;
		}
	}

	if(*iTimeO > iTimeOff)
	{
		*iTimeO -= iTimeOff;
	}
	else
	{
		*iTimeO = 0;
	}

	return iRet;
#endif

	return TPR_NPTSUPPORT;
}
