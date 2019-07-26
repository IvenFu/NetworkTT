#ifndef _TPR_SOCKET_H_
#define _TPR_SOCKET_H_

#include "TPR_Config.h"
#include "TPR_Types.h"
#include "TPR_Addr.h"

#if defined(OS_WINDOWS)
	#define TPR_SOCK_ERROR SOCKET_ERROR
	#define TPR_INVALID_SOCKET INVALID_SOCKET
	#define TPR_SHUT_RD    SD_RECEIVE
	#define TPR_SHUT_WR    SD_SEND
	#define TPR_SHUT_RDWR  SD_BOTH
#elif defined(OS_POSIX)
	#define TPR_SOCK_ERROR (-1)
	#define TPR_INVALID_SOCKET (-1)
	#define TPR_SHUT_RD    SHUT_RD
	#define TPR_SHUT_WR    SHUT_WR
	#define TPR_SHUT_RDWR  SHUT_RDWR
#else
	#error OS Not Implement Yet.
#endif


TPR_DECLARE TPR_SOCK_T CALLBACK TPR_CreateSocket(TPR_INT32 iAf, TPR_INT32 iType, TPR_INT32 iProto);
TPR_DECLARE TPR_INT32 CALLBACK TPR_CloseSocket(TPR_SOCK_T iSockFd, TPR_BOOL bForce = TPR_FALSE);
TPR_DECLARE TPR_INT32 CALLBACK TPR_ShutDown(TPR_SOCK_T iSockFd, TPR_INT32 iHow);
TPR_DECLARE TPR_INT32 CALLBACK TRR_Bind(TPR_SOCK_T iSockFd, TPR_ADDR_T* pTprAddr);
TPR_DECLARE TPR_INT32 CALLBACK TRR_Listen(TPR_SOCK_T iSockFd, TPR_INT32 iBackLog);
TPR_DECLARE TPR_SOCK_T CALLBACK TRR_Accept(TPR_SOCK_T iSockFd, TPR_ADDR_T* pTprAddr, TPR_UINT32 nTimeOut);
TPR_DECLARE TPR_INT32 CALLBACK TPR_ConnectWithTimeOut(TPR_SOCK_T iSockFd, TPR_ADDR_T* pTprAddr, TPR_UINT32 nTimeOut);
TPR_DECLARE TPR_INT32 CALLBACK TPR_JoinMultiCastGroup(TPR_SOCK_T iSockFd, TPR_ADDR_T* pLocal,TPR_ADDR_T* pMcastGroupAddr);
TPR_DECLARE TPR_INT32 CALLBACK TPR_LeaveMultiCastGroup(TPR_SOCK_T iSockFd, TPR_ADDR_T* pLocal,TPR_ADDR_T* pMcastGroupAddr);
TPR_DECLARE TPR_INT32 CALLBACK TPR_SetNonBlock(TPR_SOCK_T iSockFd, TPR_BOOL bYesNo);
TPR_DECLARE TPR_INT32 CALLBACK TPR_SetTTL(TPR_SOCK_T iSockFd, TPR_INT32 iTTL);
TPR_DECLARE TPR_INT32 CALLBACK TPR_SetTTL_V6(TPR_SOCK_T iSockFd, TPR_INT32 iTTL);

TPR_DECLARE TPR_INT32 CALLBACK TPR_SetMultiCastTTL(TPR_SOCK_T iSockFd, TPR_INT32 iTTL);

TPR_DECLARE TPR_INT32 CALLBACK TPR_SetMultiCastTTL_V6(TPR_SOCK_T iSockFd, TPR_INT32 iTTL);


TPR_DECLARE TPR_INT32 CALLBACK TPR_GetTTL(TPR_SOCK_T iSockFd, TPR_INT32* iTTL);


TPR_DECLARE TPR_INT32 CALLBACK TPR_GetTTL_V6(TPR_SOCK_T iSockFd, TPR_INT32* iTTL);

TPR_DECLARE TPR_INT32 CALLBACK TPR_SetTOS(TPR_SOCK_T iSockFd, TPR_INT32 iTOS);

TPR_DECLARE TPR_INT32 CALLBACK TPR_GetTOS(TPR_SOCK_T iSockFd, TPR_INT32* iTOS);

TPR_DECLARE TPR_INT32 CALLBACK TPR_SetReuseAddr(TPR_SOCK_T iSockFd, TPR_BOOL bYesNo);

//时间单位ms
TPR_DECLARE TPR_INT32 CALLBACK TPR_SetTimeOut(TPR_SOCK_T iSockFd, TPR_INT32 iSndTime0, TPR_INT32 iRcvTime0);

TPR_DECLARE TPR_INT32 CALLBACK TPR_LingerOff(TPR_SOCK_T iSockFd);


TPR_DECLARE TPR_INT32 CALLBACK TPR_LingerOn(TPR_SOCK_T iSockFd,TPR_UINT16 nTimeOutSec);


TPR_DECLARE TPR_INT32 CALLBACK TPR_SetNoDelay(TPR_SOCK_T iSockFd,TPR_BOOL bYesNo);

TPR_DECLARE TPR_INT32 CALLBACK TPR_BroadCast(TPR_SOCK_T iSockFd);


TPR_DECLARE TPR_INT32 CALLBACK TPR_SetBuffSize(TPR_SOCK_T iSockFd,TPR_INT32 iSndBuffSize, TPR_INT32 iRcvBuffSize);


TPR_DECLARE TPR_INT32 CALLBACK TPR_GetBuffSize(TPR_SOCK_T iSockFd,TPR_INT32* iSndBuffSize, TPR_INT32* iRcvBuffSize);

//写一段长度数据,如果缓冲区可写长度少于iBufLen，返回可写长度
TPR_DECLARE TPR_INT32 CALLBACK TPR_Send(TPR_SOCK_T iSockFd,TPR_VOIDPTR pBuf, TPR_INT32 iBufLen);

//在超时时间内，循环写固定长度数据
TPR_DECLARE TPR_INT32 CALLBACK TPR_Sendn(TPR_SOCK_T iSockFd,TPR_VOIDPTR pBuf, TPR_INT32 iBufLen,TPR_UINT32 nTimeOut);

//接收一次数据, 如果缓冲区中没数据立即返回，有多少数据返回多少
TPR_DECLARE TPR_INT32 CALLBACK TPR_Recv(TPR_SOCK_T iSockFd,TPR_VOIDPTR pBuf, TPR_INT32 iBufCount);


//在超时时间内接收数据，如果超时时间内可读，直接读一次数据返回
TPR_DECLARE TPR_INT32 CALLBACK TPR_RecvWithTimeOut(TPR_SOCK_T iSockFd,TPR_VOIDPTR pBuf, TPR_INT32 iBufCount,TPR_UINT32 nTimeOut);


//在超时时间内循环接收固定长度数据
TPR_DECLARE TPR_INT32 CALLBACK TPR_Recvn(TPR_SOCK_T iSockFd,TPR_VOIDPTR pBuf, TPR_INT32 iBufCount,TPR_UINT32 nTimeOut);
//udp
TPR_DECLARE TPR_INT32 CALLBACK TPR_SendTo(TPR_SOCK_T iSockFd,TPR_VOIDPTR pBuf, TPR_INT32 iBufLen, TPR_ADDR_T* pTprAddr);

TPR_DECLARE TPR_INT32 CALLBACK TPR_RecvFrom(TPR_SOCK_T iSockFd,TPR_VOIDPTR pBuf, TPR_INT32 iBufLen, TPR_ADDR_T* pTprAddr);

TPR_DECLARE TPR_INT32 CALLBACK TPR_ioctl(TPR_SOCK_T iSockFd, TPR_INT32 iCmd, TPR_INT32* pData);
#endif
