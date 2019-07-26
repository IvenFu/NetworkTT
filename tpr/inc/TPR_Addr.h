#ifndef _TPR_ADDR_H_
#define _TPR_ADDR_H_

#include "TPR_Config.h"
#include "TPR_Types.h"


#if defined (OS_WINDOWS)
	typedef SOCKET TPR_SOCK_T;
	#if (_MSC_VER < 1310)
		#ifndef s6_addr
			struct in6_addr{
				union{
					u_char Byte[16];
					u_short Word[8];
				}u;
			};

			//RFC2553
			#define _S6_un    u
			#define _S6_u8    Byte
			#define s6_addr     _S6_un._S6_u8

			#define s6_bytes  u.Byte
			#define s6_words  u.Word

			#define in_addr6 in6_addr
		#endif

		typedef struct{
			short sin6_family;
			u_short sin6_port;
			u_long sin6_flowinfo;
			struct in6_addr sin6_addr;
			u_long sin6_scope_id;
		}sockaddr_in6_tpr;
	#else	
		typedef sockaddr_in6 sockaddr_in6_tpr;
	#endif

#elif defined(OS_POSIX)
	typedef int TPR_SOCK_T;
	typedef sockaddr_in6 sockaddr_in6_tpr;
#else
	#error OS Not Implenment Yet.
#endif


typedef struct TPR_ADDR_T
{
	union
	{
		sockaddr_in sin4;
#if defined(OS_SUPPORT_IPV6)
		sockaddr_in6_tpr sin6;
#endif
	}SA;
}TPR_ADDR_T;

typedef struct TPR_ADDR_EXP_T
{
	TPR_INT32 iAf;  //地址族 AF_INET或AF_INET6
	union
	{
		TPR_UINT32 nAddr4;		//ipv4地址 网络字节序
		TPR_UINT8 nAddr6[16];	//ipv6地址 网络字节序
	}ADDR;
	TPR_UINT16 nPort;   //端口号 主机字节序
	TPR_UINT16 nReserved;
}TPR_ADDR_EXP_T;

#define TPR_AF_IET    AF_INET
#if defined(OS_SUPPORT_IPV6)
	#define TPR_AF_INET6  AF_INET6
#endif

//通过地址族，端口，字符串地址组建TPR_ADDR_T 地址结构
TPR_DECLARE TPR_INT32 CALLBACK TPR_MakeAddrByString(TPR_INT32 iAf, const char* pAddr, TPR_UINT16 nPort,TPR_ADDR_T* pTprAddr);

//通过整形地址及端口组建TPR_ADDR_T 地址结构 V4
TPR_DECLARE TPR_INT32 CALLBACK TPR_MakeAdd4rByInt(TPR_UINT32 nAddr, TPR_UINT16 nPort,TPR_ADDR_T* pTprAddr);

#if defined(OS_SUPPORT_IPV6)
//通过整形地址及端口组建TPR_ADDR_T 地址结构 V6
TPR_DECLARE TPR_INT32 CALLBACK TPR_MakeAdd6rByInt(TPR_UINT8 nAddr[16], TPR_UINT16 nPort,TPR_ADDR_T* pTprAddr);
#endif

//通过SOCKADDR结构 组建TPR_ADDR_T 地址结构
TPR_DECLARE TPR_INT32 CALLBACK TPR_MakeAddr(TPR_VOIDPTR pSockAddr,TPR_INT32 iScokAddrLen,TPR_ADDR_T* pTprAddr);

TPR_DECLARE TPR_INT32 CALLBACK TPR_SetAddrPort(TPR_ADDR_T* pTprAddr,TPR_UINT16 nPort);

TPR_DECLARE TPR_INT32 CALLBACK TPR_GetAddrType(TPR_ADDR_T* pTprAddr);


//通过TPR_ADDR_T 地址结构获取字符串地址
TPR_DECLARE const char* CALLBACK TPR_GetAddrString(TPR_ADDR_T* pTprAddr);
TPR_DECLARE const char* CALLBACK TPR_GetAddrStringEx(TPR_ADDR_T* pTprAddr, char* pAddrBuf, size_t nBuflen);

TPR_DECLARE TPR_UINT16 CALLBACK TPR_GetAddrPort(TPR_ADDR_T* pTprAddr);

TPR_DECLARE TPR_INT32 CALLBACK TPR_GetAddr4Int(TPR_ADDR_T* pTprAddr, TPR_UINT32* nAddr);

#if defined(OS_SUPPORT_IPV6)
TPR_DECLARE TPR_INT32 CALLBACK TPR_GetAddr6Int(TPR_ADDR_T* pTprAddr,TPR_UINT8 nAddr[16]);
#endif

TPR_DECLARE TPR_INT32 CALLBACK TPR_GetAddrInfo(TPR_ADDR_T* pTprAddr, TPR_ADDR_EXP_T* pTprExp);

//根据sockfd 获取地址信息
TPR_DECLARE TPR_INT32 CALLBACK TPR_GetAddrBySockFd(TPR_SOCK_T iSockFd,TPR_ADDR_T* pTprLocal, TPR_ADDR_T* pRemote);


TPR_DECLARE TPR_INT32 CALLBACK TPR_InetPton(const char* pSrc, TPR_VOIDPTR pDst);

TPR_DECLARE const char* CALLBACK TPR_InetNtop(TPR_INT32 iAf, const unsigned char* pSrc, char* pDst, int iCnt);


TPR_DECLARE int test();

#endif
