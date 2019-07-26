#include "TPR_Socket.h"
#include "TPR_Select.h"

#if defined(OS_WINDOWS)
	#define TPR_SOCKET_ERROR() WSAGetLastError()
	#define TPR_SET_SOCKET_ERROR(errcode) \
		do{WSASetLastError(errcode);}while(0)

	static TPR_BOOL g_bWSAStartup = TPR_FALSE;
#elif defined(OS_POSIX)
	#include <netinet/tcp.h>
	#include <signal.h>
	#include <fcntl.h>
	#define closesocket close
	#ifndef SOCKADDR
		typedef sockaddr SOCKADDR;
		typedef sockaddr* PSOCKADDR;
	#endif
	#define TPR_SOCKET_ERROR() (errno)
	#define TPR_SET_SOCKET_ERROR(errcode) \
		do{errno = (errcode);}while(0)
#endif

TPR_INT32 TPR_InitNetwork_Inter()
{
#if defined (OS_WINDOWS)
	if(!g_bWSAStartup)
	{
		WSADATA wsaData;
		g_bWSAStartup = TPR_TRUE;
		return ::WSAStartup(MAKEWORD(2,2), &wsaData);
	}
#else
	struct sigaction sa;
	sa.sa_handler  = SIG_IGN;
	sigaction(SIGPIPE, &sa,0);
#endif
	return TPR_OK;
}

TPR_INT32 TPR_FiniNetwork_Inter()
{
#if defined (OS_WINDOWS)
	if(g_bWSAStartup)
	{
		::WSACleanup();
		g_bWSAStartup = TPR_FALSE;
	}
#endif
	return TPR_OK;
}

TPR_DECLARE TPR_SOCK_T CALLBACK TPR_CreateSocket(TPR_INT32 iAf, TPR_INT32 iType, TPR_INT32 iProto)
{
	return ::socket(iAf,iType,iProto);
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_CloseSocket(TPR_SOCK_T iSockFd, TPR_BOOL bForce)
{
	if(bForce)
	{
		TPR_ShutDown(iSockFd, TPR_SHUT_RDWR);
	}
	return ::closesocket(iSockFd);
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_ShutDown(TPR_SOCK_T iSockFd, TPR_INT32 iHow)
{
	return ::shutdown(iSockFd, iHow);
}

TPR_DECLARE TPR_INT32 CALLBACK TRR_Bind(TPR_SOCK_T iSockFd, TPR_ADDR_T* pTprAddr)
{
	if(!pTprAddr)
	{
		return TPR_ERROR;
	}

	sockaddr_in sin4 = pTprAddr->SA.sin4;
	if(sin4.sin_family == AF_INET)
	{
		return ::bind(iSockFd, (PSOCKADDR)&pTprAddr->SA, sizeof(sockaddr_in));
	}
	else 
	{
		return ::bind(iSockFd, (PSOCKADDR)&pTprAddr->SA, sizeof(TPR_ADDR_T));
	}
}

TPR_DECLARE TPR_INT32 CALLBACK TRR_Listen(TPR_SOCK_T iSockFd, TPR_INT32 iBackLog)
{
	return ::listen(iSockFd, iBackLog);	
}

TPR_DECLARE TPR_SOCK_T CALLBACK TPR_Accept(TPR_SOCK_T iSockFd, TPR_ADDR_T* pTprAddr, TPR_UINT32 nTimeOut)
{
	TPR_SOCK_T iTmpSockFd = TPR_INVALID_SOCKET;

	int iAddrSize = sizeof(pTprAddr->SA);

	if(nTimeOut == TPR_INFINITE)
	{
		if(pTprAddr != NULL)
		{
			iTmpSockFd = ::accept(iSockFd,(sockaddr*)&pTprAddr->SA,(socklen_t*)&iAddrSize);
		}
		else
		{
			iTmpSockFd = ::accept(iSockFd,NULL,NULL);
		}
	}
	else
	{
#if defined (OS_WINDOWS)
		fd_set fdset_r;
		timeval tv = {nTimeOut/1000, (nTimeOut%1000)*1000};
		int iSelectRet = -1;

		FD_ZERO(&fdset_r);
		FD_SET(iSockFd,&fdset_r);

		iSelectRet = TPR_SelectEx((int)iSockFd+1, &fdset_r, NULL,NULL,&tv);
		if(iSelectRet > 0 &&FD_ISSET(iSockFd,&fdset_r))
#elif defined(OS_POSIX)
		struct pollfd fds[1]={0};
		int iPollRet = -1;
		fds[0].fd = iSockFd;
		fds[0].events = POLLRDNORM;
		iPollRet = TPR_PollEx(fds,1,(TPR_INT32*)&nTimeOut);
		if((iPollRet>0) && (fds[0].revents & POLLRDNORM))
#else
		#error OS NOT implement.
#endif
		{
			if(pTprAddr !=NULL)
			{
				iTmpSockFd = ::accept(iSockFd,(sockaddr*)&pTprAddr->SA,(socklen_t*)&iAddrSize);
			}
			else
			{
				iTmpSockFd = ::accept(iSockFd,NULL,NULL);
			}

		}
	}

	return iTmpSockFd;
}


TPR_DECLARE TPR_INT32 CALLBACK TPR_ConnectWithTimeOut(TPR_SOCK_T iSockFd, TPR_ADDR_T* pTprAddr, TPR_UINT32 nTimeOut)
{
	if(!pTprAddr)
	{
		return TPR_ERROR;
	}

	if(nTimeOut == TPR_INFINITE)
	{
		return ::connect(iSockFd,(PSOCKADDR)&pTprAddr->SA, sizeof(sockaddr_in));
	}

	int iSystemLastError = 0;
	TPR_INT32 iSelectRet = -1;
	TPR_INT32 iRet =TPR_ERROR;

	::TPR_SetNonBlock(iSockFd, TPR_TRUE);
	sockaddr_in sin4 = pTprAddr->SA.sin4;

	if(sin4.sin_family == AF_INET)
	{
		iRet = ::connect(iSockFd,(PSOCKADDR)&pTprAddr->SA.sin4, sizeof(pTprAddr->SA.sin4));
	}
	else
	{
		iRet = ::connect(iSockFd,(PSOCKADDR)&pTprAddr->SA.sin6, sizeof(pTprAddr->SA.sin6));
	}

	if(iRet == 0)
	{
		return iRet;
	}

#if defined(OS_WINDOWS)
	if(TPR_SOCKET_ERROR() != WSAEWOULDBLOCK)
#else
	if(TPR_SOCKET_ERROR() != EINPROGRESS)
#endif
	{
		return iRet;
	}

#if defined(OS_WINDOWS)
	timeval tv;
	tv.tv_sec = nTimeOut/1000;
	tv.tv_usec = (nTimeOut%1000)*1000;
	fd_set fdset_w;
	FD_ZERO(&fdset_w);
	FD_SET(iSockFd,&fdset_w);

	iSelectRet = TPR_SelectEx((int)iSockFd+1, NULL, &fdset_w,NULL,&tv);
	if(iSelectRet > 0 &&FD_ISSET(iSockFd,&fdset_w))
#elif defined(OS_POSIX)
	struct pollfd fds[1]={0};
	fds[0].fd = iSockFd;
	fds[0].events = POLLWRNORM;
	iSelectRet = TPR_PollEx(fds,1,(TPR_INT32*)&nTimeOut);
	if((iSelectRet>0) && (fds[0].revents & POLLWRNORM))
#else
	#error OS NOT implement.
#endif
	{
		TPR_INT32 iError;
		TPR_INT32 iErrorLen;
		iErrorLen = sizeof(iError);

		TPR_INT32 iTmp = ::getsockopt(iSockFd, SOL_SOCKET, SO_ERROR,(char*)&iError,(socklen_t*)&iErrorLen);
		if((iTmp == 0) && (iError ==0))
		{
			iRet = 0;
		}
		else
		{
			if(iTmp == 0)
			{
				iSystemLastError = iError;
			}
			else
			{
				iSystemLastError = TPR_SOCKET_ERROR();
			}
		}
	}
	else if(iSelectRet<0)
	{
		iSystemLastError = TPR_SOCKET_ERROR();
#if defined(OS_POSIX)
		if(nTimeOut == 0)
		{
			iSystemLastError = ETIMEDOUT;
		}
#endif
	}
	else
	{
#if defined(OS_WINDOWS)
		iSystemLastError = WSAETIMEDOUT;
#else
		iSystemLastError = ETIMEDOUT;
#endif
	}
	
	//如果是非阻塞的套接字，改变了套接字的阻塞属性
	TPR_SetNonBlock(iSockFd, TPR_FALSE);

	if(iRet!=0)
	{
		TPR_SET_SOCKET_ERROR(iSystemLastError);
	}

	return iRet;
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_JoinMultiCastGroup(TPR_SOCK_T iSockFd, TPR_ADDR_T* pLocal,TPR_ADDR_T* pMcastGroupAddr)
{
	if(!pLocal || !pMcastGroupAddr)
	{
		return TPR_ERROR;
	}

	sockaddr_in McastGroupAddrSin4 = pMcastGroupAddr->SA.sin4;
	sockaddr_in6_tpr McastGroupAddrSin6 = pMcastGroupAddr->SA.sin6;
	sockaddr_in LocalSin4 = pLocal->SA.sin4;

	PSOCKADDR pSockAddr = (PSOCKADDR)&(pMcastGroupAddr->SA);

	if(pSockAddr->sa_family == AF_INET)
	{
		struct ip_mreq struMerq = {0};
		struMerq.imr_multiaddr.s_addr = McastGroupAddrSin4.sin_addr.s_addr;
		struMerq.imr_interface.s_addr = LocalSin4.sin_addr.s_addr;
		return ::setsockopt(iSockFd, IPPROTO_IP,IP_ADD_MEMBERSHIP, (char*)&struMerq,sizeof(struMerq));
	}
	else if(pSockAddr->sa_family == AF_INET6)
	{
		struct ipv6_mreq struMerq6 = {0};
		struMerq6.ipv6mr_interface = McastGroupAddrSin4.sin_addr.s_addr;
		memcpy(struMerq6.ipv6mr_multiaddr.s6_addr, McastGroupAddrSin6.sin6_addr.s6_addr,16);
#if defined(__APPLE__)
		return ::setsockopt(iSockFd, IPPROTO_IPV6,IPV6_JOIN_GROUP, (char*)&struMerq6,sizeof(struMerq6));
#else
		return ::setsockopt(iSockFd, IPPROTO_IPV6,IPV6_ADD_MEMBERSHIP, (char*)&struMerq6,sizeof(struMerq6));
#endif
	}
	else
	{
		return TPR_ERROR;
	}
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_LeaveMultiCastGroup(TPR_SOCK_T iSockFd, TPR_ADDR_T* pLocal,TPR_ADDR_T* pMcastGroupAddr)
{
	if(!pLocal || !pMcastGroupAddr)
	{
		return TPR_ERROR;
	}

	sockaddr_in McastGroupAddrSin4 = pMcastGroupAddr->SA.sin4;
	sockaddr_in6_tpr McastGroupAddrSin6 = pMcastGroupAddr->SA.sin6;
	sockaddr_in LocalSin4 = pLocal->SA.sin4;

	PSOCKADDR pSockAddr = (PSOCKADDR)&(pMcastGroupAddr->SA);

	if(pSockAddr->sa_family == AF_INET)
	{
		struct ip_mreq struMerq = {0};
		struMerq.imr_multiaddr.s_addr = McastGroupAddrSin4.sin_addr.s_addr;
		struMerq.imr_interface.s_addr = LocalSin4.sin_addr.s_addr;
		return ::setsockopt(iSockFd, IPPROTO_IP,IP_DROP_MEMBERSHIP, (char*)&struMerq,sizeof(struMerq));
	}
	else if(pSockAddr->sa_family == AF_INET6)
	{
		struct ipv6_mreq struMerq6 = {0};
		struMerq6.ipv6mr_interface = McastGroupAddrSin4.sin_addr.s_addr;
		memcpy(struMerq6.ipv6mr_multiaddr.s6_addr, McastGroupAddrSin6.sin6_addr.s6_addr,16);
#if defined(__APPLE__)
		return ::setsockopt(iSockFd, IPPROTO_IPV6,IPV6_LEAVE_GROUP, (char*)&struMerq6,sizeof(struMerq6));
#else
		return ::setsockopt(iSockFd, IPPROTO_IPV6,IPV6_DROP_MEMBERSHIP, (char*)&struMerq6,sizeof(struMerq6));
#endif
	}
	else
	{
		return TPR_ERROR;
	}
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_SetNonBlock(TPR_SOCK_T iSockFd, TPR_BOOL bYesNo)
{
#if defined(OS_WINDOWS)
	return ::ioctlsocket(iSockFd,(long)FIONBIO,(u_long*)&bYesNo);
#else
	int flags = fcntl(iSockFd, F_GETFL,0);
	if(bYesNo)
	{
		return (fcntl(iSockFd, F_SETFL,flags| O_NONBLOCK) == -1) ? TPR_ERROR:TPR_OK;
	}
	else
	{
		return (fcntl(iSockFd, F_SETFL,flags&~O_NONBLOCK) == -1) ? TPR_ERROR:TPR_OK;
	}
#endif
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_SetTTL(TPR_SOCK_T iSockFd, TPR_INT32 iTTL)
{
	return ::setsockopt(iSockFd, IPPROTO_IP,IP_TTL, (char*)&iTTL,sizeof(iTTL));
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_SetTTL_V6(TPR_SOCK_T iSockFd, TPR_INT32 iTTL)
{
	return ::setsockopt(iSockFd, IPPROTO_IPV6,IPV6_UNICAST_HOPS, (char*)&iTTL,sizeof(iTTL));
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_SetMultiCastTTL(TPR_SOCK_T iSockFd, TPR_INT32 iTTL)
{
	return ::setsockopt(iSockFd, IPPROTO_IP,IP_MULTICAST_TTL, (char*)&iTTL,sizeof(iTTL));
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_SetMultiCastTTL_V6(TPR_SOCK_T iSockFd, TPR_INT32 iTTL)
{
	return ::setsockopt(iSockFd, IPPROTO_IPV6,IPV6_MULTICAST_HOPS, (char*)&iTTL,sizeof(iTTL));
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_GetTTL(TPR_SOCK_T iSockFd, TPR_INT32* iTTL)
{
	int iSize = sizeof(*iTTL);
	return ::getsockopt(iSockFd, IPPROTO_IP,IP_TTL, (char*)iTTL, (socklen_t*)&iSize);
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_GetTTL_V6(TPR_SOCK_T iSockFd, TPR_INT32* iTTL)
{
	int iSize = sizeof(*iTTL);
	return ::getsockopt(iSockFd, IPPROTO_IPV6,IPV6_UNICAST_HOPS, (char*)iTTL, (socklen_t*)&iSize);
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_SetTOS(TPR_SOCK_T iSockFd, TPR_INT32 iTOS)
{
	return ::setsockopt(iSockFd, IPPROTO_IP,IP_TOS, (char*)&iTOS,sizeof(iTOS));
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_GetTOS(TPR_SOCK_T iSockFd, TPR_INT32* iTOS)
{
	int iSize = sizeof(*iTOS);
	return ::getsockopt(iSockFd, IPPROTO_IP,IP_TOS, (char*)iTOS,(socklen_t*)&iSize);
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_SetReuseAddr(TPR_SOCK_T iSockFd, TPR_BOOL bYesNo)
{
	return ::setsockopt(iSockFd, SOL_SOCKET,SO_REUSEADDR, (char*)&bYesNo,sizeof(bYesNo));
}

//时间单位ms
TPR_DECLARE TPR_INT32 CALLBACK TPR_SetTimeOut(TPR_SOCK_T iSockFd, TPR_INT32 iSndTime0, TPR_INT32 iRcvTime0)
{
	TPR_INT32 iRet1 = TPR_OK;
	TPR_INT32 iRet2 = TPR_OK;

#if defined(OS_WINDOWS)
	if(iRcvTime0)
	{
		iRet1 = ::setsockopt(iSockFd, SOL_SOCKET, SO_RCVTIMEO,(char*)&iRcvTime0, sizeof(iRcvTime0));
	}

	if(iSndTime0)
	{
		iRet2 = ::setsockopt(iSockFd, SOL_SOCKET, SO_SNDTIMEO,(char*)&iSndTime0, sizeof(iSndTime0));
	}
#elif defined(OS_POSIX)
	if(iRcvTime0)
	{
		timeval tv = {iRcvTime0/1000, (iRcvTime0%1000)*1000};
		iRet1 = ::setsockopt(iSockFd, SOL_SOCKET, SO_RCVTIMEO,(char*)&tv, sizeof(tv));
	}

	if(iSndTime0)
	{
		timeval tv = {iSndTime0/1000, (iSndTime0%1000)*1000};
		iRet2 = ::setsockopt(iSockFd, SOL_SOCKET, SO_SNDTIMEO,(char*)&tv, sizeof(tv));
	}
#endif

	return (iRet1 == TPR_OK && iRet2 == TPR_OK)? TPR_OK:TPR_ERROR;
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_LingerOff(TPR_SOCK_T iSockFd)
{
	linger lin = {0};
	lin.l_onoff = 0;

	return ::setsockopt(iSockFd, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(linger));
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_LingerOn(TPR_SOCK_T iSockFd,TPR_UINT16 nTimeOutSec)
{
	linger lin = {0};
	lin.l_onoff = 1;
	lin.l_linger = nTimeOutSec;

	return ::setsockopt(iSockFd, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(linger));
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_SetNoDelay(TPR_SOCK_T iSockFd,TPR_BOOL bYesNo)
{
	return ::setsockopt(iSockFd, IPPROTO_TCP,TCP_NODELAY, (char*)&bYesNo,sizeof(TPR_BOOL));
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_BroadCast(TPR_SOCK_T iSockFd)
{
	TPR_BOOL bYes = TPR_TRUE;
	return ::setsockopt(iSockFd, SOL_SOCKET,SO_BROADCAST, (char*)&bYes,sizeof(bYes));
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_SetBuffSize(TPR_SOCK_T iSockFd,TPR_INT32 iSndBuffSize, TPR_INT32 iRcvBuffSize)
{
	TPR_INT32 iRet1 = TPR_OK;
	TPR_INT32 iRet2 = TPR_OK;

	if(iSndBuffSize)
	{
		iRet1 = ::setsockopt(iSockFd, SOL_SOCKET, SO_SNDBUF,(char*)&iSndBuffSize, sizeof(iSndBuffSize));
	}

	if(iRcvBuffSize)
	{
		iRet2 = ::setsockopt(iSockFd, SOL_SOCKET, SO_RCVBUF,(char*)&iRcvBuffSize, sizeof(iRcvBuffSize));
	}

	return (iRet1 == TPR_OK && iRet2 == TPR_OK)? TPR_OK:TPR_ERROR;
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_GetBuffSize(TPR_SOCK_T iSockFd,TPR_INT32* iSndBuffSize, TPR_INT32* iRcvBuffSize)
{
	TPR_INT32 iRet1 = TPR_OK;
	TPR_INT32 iRet2 = TPR_OK;
	TPR_INT32 iSize1 = sizeof(TPR_INT32);
	TPR_INT32 iSize2 = sizeof(TPR_INT32);

	if(iSndBuffSize)
	{
		iRet1 = ::getsockopt(iSockFd, SOL_SOCKET, SO_SNDBUF,(char*)iSndBuffSize, (socklen_t*)&iSize1);
	}

	if(iRcvBuffSize)
	{
		iRet2 = ::getsockopt(iSockFd, SOL_SOCKET, SO_RCVBUF,(char*)iRcvBuffSize, (socklen_t*)&iSize2);
	}
	return (iRet1 == TPR_OK && iRet2 == TPR_OK)? TPR_OK:TPR_ERROR;
}

//写一段长度数据,如果缓冲区可写长度少于iBufLen，返回可写长度
TPR_DECLARE TPR_INT32 CALLBACK TPR_Send(TPR_SOCK_T iSockFd,TPR_VOIDPTR pBuf, TPR_INT32 iBufLen)
{
	return ::send(iSockFd, (char*)pBuf,iBufLen,0);
}

//在超时时间内，循环写固定长度数据
TPR_DECLARE TPR_INT32 CALLBACK TPR_Sendn(TPR_SOCK_T iSockFd,TPR_VOIDPTR pBuf, TPR_INT32 iBufLen,TPR_UINT32 nTimeOut)
{
	int iSendLen = 0;
	int iTmpLen = 0;
	int iRet = -1;

#if defined(OS_WINDOWS)
	timeval tv;
	fd_set fdset_w;

	if(nTimeOut != TPR_INFINITE)
	{
		tv.tv_sec = nTimeOut/1000;
		tv.tv_usec = (nTimeOut%1000)*1000;
	}
	else
	{
		tv.tv_sec = 0;
		tv.tv_usec = 0;
	}
#endif

	do 
	{
#if defined(OS_WINDOWS)
		FD_ZERO(&fdset_w);
		FD_SET(iSockFd, &fdset_w);
		
		//linux 的select会修改tv时间，返回剩余等待时间
		if(nTimeOut == TPR_INFINITE)
		{
			iRet = ::TPR_SelectEx((int)iSockFd+1, NULL,&fdset_w,NULL,NULL);
		}
		else
		{
			iRet = ::TPR_SelectEx((int)iSockFd+1, NULL,&fdset_w,NULL,&tv);
			nTimeOut = tv.tv_sec*1000 + tv.tv_usec/1000;
		}

		if(iRet > 0 && FD_ISSET(iSockFd, &fdset_w))
#elif defined(OS_POSIX)
		struct pollfd  fds[1] = {0};
		fds[0].fd = iSockFd;
		fds[0].events = POLLWRNORM;

		iRet = TPR_PollEx(fds,1,(TPR_INT32*)&nTimeOut);  //nTimeOut 是否修改？
		if((iRet>0) && (fds[0].revents & POLLWRNORM))
#else
	#errors OS Not Implement
#endif
		{
			iTmpLen = ::TPR_Send(iSockFd, (char*)pBuf+iSendLen, iBufLen-iSendLen);
			if(iTmpLen > 0)
			{
				iSendLen += iTmpLen;
				if(iSendLen == iBufLen)
				{
					//发送完毕
					break;
				}
			}
			else
			{
				//发送失败
				break;
			}
		}
		else if(iRet == 0)
		{	
			//这里不break, 通过判断nTimeOut退出
		}
		else
		{
			//select失败
			break;
		}

	} while (nTimeOut >0);

	return iSendLen;
}

//接收一次数据, 如果缓冲区中没数据立即返回，有多少数据返回多少
TPR_DECLARE TPR_INT32 CALLBACK TPR_Recv(TPR_SOCK_T iSockFd,TPR_VOIDPTR pBuf, TPR_INT32 iBufCount)
{
	return ::recv(iSockFd, (char*)pBuf,iBufCount,0);
}

//在超时时间内接收数据，如果超时时间内可读，直接读一次数据返回
TPR_DECLARE TPR_INT32 CALLBACK TPR_RecvWithTimeOut(TPR_SOCK_T iSockFd,TPR_VOIDPTR pBuf, TPR_INT32 iBufCount,TPR_UINT32 nTimeOut)
{
	int iRecvLen =0;
	int iTmpLen =0;

#if defined(OS_WINDOWS)
	int iSelectRet = -1;
	timeval tv;
	fd_set fdset_r;

	if(nTimeOut != TPR_INFINITE)
	{
		tv.tv_sec = nTimeOut/1000;
		tv.tv_usec = (nTimeOut%1000)*1000;
	}
#endif

#if defined(OS_WINDOWS)
	FD_ZERO(&fdset_r);
	FD_SET(iSockFd, &fdset_r);

	if(nTimeOut == TPR_INFINITE)
	{
		iSelectRet = ::TPR_SelectEx((int)iSockFd+1, &fdset_r ,NULL,NULL,NULL);
	}
	else
	{
		iSelectRet = ::TPR_SelectEx((int)iSockFd+1, &fdset_r,NULL,NULL,&tv);
	}

	if(iSelectRet > 0 && FD_ISSET(iSockFd, &fdset_r))
#elif defined(OS_POSIX)
	struct pollfd  fds[1] = {0};
	int iPollRet = -1;
	fds[0].fd = iSockFd;
	fds[0].events = POLLRDNORM;

	iPollRet = TPR_PollEx(fds,1,(TPR_INT32*)&nTimeOut);
	if((iPollRet>0) && (fds[0].revents & POLLRDNORM))
#else
#errors OS Not Implement
#endif
	{
		iTmpLen = ::TPR_Recv(iSockFd, (char*)pBuf+iRecvLen, iBufCount-iRecvLen);
		if(iTmpLen > 0)
		{
			return iTmpLen;
		}
		else
		{
			return -1;
		}
	}
#if defined(OS_WINDOWS)
	else if(iSelectRet == 0)
	{
		return -1;
	}
#elif defined(OS_POSIX)
	else if(iPollRet ==0)
	{
		return -1;
	}
#endif
	else
	{
		return -1;
	}
}

//在超时时间内循环接收固定长度数据
TPR_DECLARE TPR_INT32 CALLBACK TPR_Recvn(TPR_SOCK_T iSockFd,TPR_VOIDPTR pBuf, TPR_INT32 iBufCount,TPR_UINT32 nTimeOut)
{
	int iRecvLen =0;
	int iTmpLen =0;

#if defined(OS_WINDOWS)
	int iSelectRet = -1;
	timeval tv;
	fd_set fdset_r;

	if(nTimeOut != TPR_INFINITE)
	{
		tv.tv_sec = nTimeOut/1000;
		tv.tv_usec = (nTimeOut%1000)*1000;
	}
#endif
	
	do 
	{
#if defined(OS_WINDOWS)
		FD_ZERO(&fdset_r);
		FD_SET(iSockFd, &fdset_r);

		if(nTimeOut == TPR_INFINITE)
		{
			iSelectRet = ::TPR_SelectEx((int)iSockFd+1, &fdset_r ,NULL,NULL,NULL);
		}
		else
		{
			iSelectRet = ::TPR_SelectEx((int)iSockFd+1, &fdset_r,NULL,NULL,&tv);
			//nTimeOut ？？ 是否修改
			nTimeOut = tv.tv_sec*1000 + tv.tv_usec/1000;
		}

		if(iSelectRet > 0 && FD_ISSET(iSockFd, &fdset_r))
#elif defined(OS_POSIX)
		struct pollfd  fds[1] = {0};
		int iPollRet = -1;
		fds[0].fd = iSockFd;
		fds[0].events = POLLRDNORM;

		iPollRet = TPR_PollEx(fds,1,(TPR_INT32*)&nTimeOut);
		if((iPollRet>0) && (fds[0].revents & POLLRDNORM))
#else
		#errors OS Not Implement
#endif
		{
			iTmpLen = ::TPR_Recv(iSockFd, (char*)pBuf+iRecvLen, iBufCount-iRecvLen);
			if(iTmpLen > 0)
			{
				iRecvLen+=iTmpLen;
				if(iRecvLen == iBufCount)
				{
					//接受满了
					break;
				}
			}
			else
			{
				//接收出错
				return -1;
			}
		}
#if defined(OS_WINDOWS)
		else if(iSelectRet == 0)
		{
			//这里不break, 通过判断nTimeOut退出
		}
#elif defined(OS_POSIX)
		else if(iPollRet ==0)
		{
			//这里不break, 通过判断nTimeOut退出
		}
#endif
		else
		{
			//select出错
			return -1;
		}

	} while (nTimeOut>0);

	return iRecvLen;
}

//udp
TPR_DECLARE TPR_INT32 CALLBACK TPR_SendTo(TPR_SOCK_T iSockFd,TPR_VOIDPTR pBuf, TPR_INT32 iBufLen, TPR_ADDR_T* pTprAddr)
{
	sockaddr_in sin4 = pTprAddr->SA.sin4;
	if(sin4.sin_family == AF_INET)
	{
		return ::sendto(iSockFd, (char*)pBuf, iBufLen,0,(PSOCKADDR)&pTprAddr->SA, sizeof(pTprAddr->SA.sin4));
	}
	else
	{
		return ::sendto(iSockFd, (char*)pBuf, iBufLen,0,(PSOCKADDR)&pTprAddr->SA, sizeof(pTprAddr->SA.sin6));
	}
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_RecvFrom(TPR_SOCK_T iSockFd,TPR_VOIDPTR pBuf, TPR_INT32 iBufLen, TPR_ADDR_T* pTprAddr)
{
	int iAddSize = sizeof(TPR_ADDR_T);
	return ::recvfrom(iSockFd,(char*)pBuf,iBufLen,0,(PSOCKADDR)&pTprAddr->SA, (socklen_t*)&iAddSize);
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_ioctl(TPR_SOCK_T iSockFd, TPR_INT32 iCmd, TPR_INT32* pData)
{
#if defined(OS_WINDOWS)
	return ::ioctlsocket(iSockFd,iCmd,(unsigned long*)pData);
#elif defined(OS_POSIX)
	return ::ioctl(iSockFd,iCmd,(unsigned long*)pData);
#endif
}


TPR_DECLARE int test()
{
	TPR_ADDR_T addr;
	
	TPR_InitNetwork_Inter();

	getchar();
	return 1;
}