
#include "TPR_Addr.h"


#if defined(OS_POSIX)
#include <arpa/inet.h>
#define SOCKADDR sockaddr
#define PSOCKADDR sockaddr*
#endif


#define TPR_IN6ADDRSZ 16
#define TPR_INADDRSZ 4
#define TPR_INT16SZ 2

//将字符串IPV4 xxx.xxx.xxx.xxx 形式转化为十六进制内存保存 buf[4]
static int TPR_InetPton4_Inter(const char* pSrc, TPR_VOIDPTR pDst)
{
	const char szDigits[] = "0123456789";
	int iSawDigit;//每组数字开始标志
	int iOctets;//字节数计数
	int iCh;
	unsigned char chTmp[TPR_INADDRSZ]; //目标内存
	unsigned char* pTp;

	if(!pSrc || !pDst)
	{
		return -1;
	}

	iSawDigit = 0;
	iOctets = 0; 
	*(pTp = chTmp) = 0;

	while((iCh = *pSrc++) != '\0')
	{
		const char *pCh;
		if((pCh = (char*) strchr(szDigits,iCh))!= NULL)
		{
			unsigned int nNew = *pTp * 10 + (int)(pCh - szDigits);
			if(nNew > 255)
			{
				return 0;
			}
			
			*pTp = (unsigned char)(nNew);
			if(!iSawDigit)
			{
				if(++iOctets > 4)
				{
					return 0;
				}
				iSawDigit = 1;
			}
		}
		else if(iCh == '.'&& iSawDigit)
		{
			if(iOctets == 4)
			{
				return 0;
			}
			*++pTp = 0;
			iSawDigit = 0;
		}
		else
		{
			return 0;
		}
	}

	if(iOctets < 4)
	{
		return  0;
	}

	memcpy(pDst , chTmp ,TPR_INADDRSZ);
	return 1;
}

static int TPR_InetPton_Inter(TPR_INT32 iAf,const char* pSrc,TPR_VOIDPTR pDst)
{
	switch(iAf)
	{
	case AF_INET:
		{
			return TPR_InetPton4_Inter(pSrc,pDst);
		}
		break;
	case AF_INET6:
		{
			//TBD;
		}
		break;
	default:
		break;
	}
	return TPR_OK;
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_InetPton(const char* pSrc, TPR_VOIDPTR pDst)
{
	if(!pSrc || !pDst)
	{
		return TPR_ERROR;
	}

	if(strchr(pSrc, ':') == NULL)
	{
		return TPR_InetPton4_Inter(pSrc,pDst) ? TPR_OK: TPR_ERROR;
	}
	else
	{
		//TBD
		//return TPR_InetPton6_Inter(pSrc,pDst) ? TPR_OK: TPR_ERROR;
	}
}

//为十六进制内存保存pSrc  转化为字符串IPV4 xxx.xxx.xxx.xxx 形式
static const char* TPR_InetNtop4(const unsigned char* pSrc,char* pDst, int iSize)
{
	static const char szFmt[] = "%u.%u.%u.%u";
	char szTmp[sizeof "255.255.255.255"] = {0};

	if(!pSrc || !pDst || !iSize)
	{
		return NULL;
	}

#if(_MSC_VER >= 1500 && !defined _WIN32_WCE)
	sprintf_s(szTmp,sizeof(szTmp),szFmt, pSrc[0],pSrc[1],pSrc[2],pSrc[3]);
#else
	sprintf(szTmp,szFmt, pSrc[0],pSrc[1],pSrc[2],pSrc[3]);
#endif

	if((int)strlen(szTmp) > iSize)
	{
		return NULL;
	}

#if(_MSC_VER >= 1500 && !defined _WIN32_WCE)
	strcpy_s(pDst,iSize,szTmp);
#else
	strncpy(pDst,szTmp, iSize);
#endif

	return pDst;
}

TPR_DECLARE const char* CALLBACK TPR_InetNtop(TPR_INT32 iAf, const unsigned char* pSrc, char* pDst, int iCnt)
{
	switch(iAf)
	{
	case AF_INET:
		{
			return TPR_InetNtop4(pSrc,pDst,iCnt);
		}
		break;
	case AF_INET6:
		{
			//TBD;
		}
		break;
	default:
		break;
	}
	return TPR_OK;
}

//将字符串IPV4 xx1.xx2.xx3.xx4 形式转化为十六进制内存保存在int中（高位 xx1 xx2 xx3 xx4 低位 （网络字节序）)
int Ipv4ToI(const char* ipv4, unsigned int* intipv4)
{
	if(ipv4!=NULL && intipv4!=NULL)
	{
		TPR_UINT32 addr[4]= {0};
		if(sscanf(ipv4, "%d.%d.%d.%d",&addr[0],&addr[1],&addr[2],&addr[3]) == 4)
		{
			TPR_BOOL bFalse = TPR_FALSE;
			for(int i =0;i<4;i++)
			{
				if(addr[i]>=256)
				{
					bFalse = TPR_TRUE;
					break;
				}
			}

			if(!bFalse)
			{
				*intipv4 = (((addr[0] & 0xff) <<24) | ((addr[1]&0x0ff)<<16) | ((addr[2]&0x0ff)<<8) | (addr[3]&0x0ff));
				return 1;
			}
		}
	}

	return 0;
}

//过滤字符串中的空格
char* StringWhiteSpaceTrim(char* str)
{
	int iIndex;
	int iNewIndex;
	int iStrLength;

	iStrLength = strlen(str);

	for(iIndex = 0, iNewIndex = 0; iIndex  < iStrLength;  iIndex++)
	{
		if(!isspace((unsigned char)str[iIndex]))
		{
			str[iNewIndex] = str[iIndex];
			iNewIndex++;
		}
	}

	str[iNewIndex] = '\0';
	return str;
}

int StringCharCount(const char* string, char character)
{
	int i;
	int iStrLen;
	int iCount = 0;

	iStrLen = strlen(string);
	for(i = 0; i< iStrLen; i++)
	{
		if(string[i] == character)
		{
			iCount++;
		}
	}

	return iCount;
}

//判断 ipv6地址域类型
int Ipv6AddressFieldTypeGet(const char* fieldStr)
{
	int i = 0;
	int length;
	int type;
	unsigned int ipv4Addr;

	length = strlen(fieldStr);
	
	//通过长度判断 
	//16进制数字域  :1-4
	// "::"域  0
	// ipv4地址域: 7-15

	if(0 == length)
	{
		type = 1;
	}
	else if(length <= 4)
	{
		for(i =0 ; i<length; i++)
		{
			if(!isxdigit((unsigned char)fieldStr[i]))
			{
				return -1;
			}
		}
		type = 0;
	}
	else if((length >=7) && (length <=15))
	{
		if(Ipv4ToI(fieldStr, &ipv4Addr))
		{
			type = 2;
		}
		else
		{
			type = -1;
		}
	}
	else
	{
		type = -1;
	}

	return type;
}



//通过地址族，端口，字符串地址组建TPR_ADDR_T 地址结构
TPR_DECLARE TPR_INT32 CALLBACK TPR_MakeAddrByString(TPR_INT32 iAf, const char* pAddr, TPR_UINT16 nPort,TPR_ADDR_T* pTprAddr)
{
	TPR_INT32 iRet = TPR_ERROR;

	if(!pTprAddr)
	{
		return TPR_ERROR;
	}

	memset(pTprAddr,0,sizeof(*pTprAddr));

	if(NULL == pAddr || strlen(pAddr) == 0)
	{
		if(iAf == AF_INET)
		{
			pTprAddr->SA.sin4.sin_addr.s_addr = INADDR_ANY;
			pTprAddr->SA.sin4.sin_family = AF_INET;
			pTprAddr->SA.sin4.sin_port = htons(nPort);
		}
		else if(iAf == AF_INET6)
		{
			memset(&pTprAddr->SA.sin6.sin6_addr, 0 ,16);
			pTprAddr->SA.sin6.sin6_family = AF_INET6;
			pTprAddr->SA.sin6.sin6_port = htons(nPort);
		}
		else
		{
			return TPR_ERROR;
		}
		return TPR_OK;
	}

	switch(iAf)
	{
	case AF_INET:
		{
			pTprAddr->SA.sin4.sin_addr.s_addr = inet_addr(pAddr);
			if(pTprAddr->SA.sin4.sin_addr.s_addr == INADDR_NONE)
			{
				break;
			}
			pTprAddr->SA.sin4.sin_family = AF_INET;
			pTprAddr->SA.sin4.sin_port = htons(nPort);
			iRet = TPR_OK;
		}
		break;
	case  AF_INET6:
		{
			if(TPR_OK == TPR_InetPton_Inter(AF_INET6,pAddr,&pTprAddr->SA.sin6.sin6_addr))
			{
				pTprAddr->SA.sin6.sin6_family = AF_INET6;
				pTprAddr->SA.sin6.sin6_port = htons(nPort);
				iRet = TPR_OK;
			}
		}
		break;
	default:
		break;
	}
	
	return iRet;
}

//通过整形地址及端口组建TPR_ADDR_T 地址结构 V4
TPR_DECLARE TPR_INT32 CALLBACK TPR_MakeAdd4rByInt(TPR_UINT32 nAddr, TPR_UINT16 nPort,TPR_ADDR_T* pTprAddr)
{
	if(!pTprAddr)
	{
		return TPR_ERROR;
	}

	memset(pTprAddr,0,sizeof(*pTprAddr));

	pTprAddr->SA.sin4.sin_addr.s_addr = nAddr;
	pTprAddr->SA.sin4.sin_family = AF_INET;
	pTprAddr->SA.sin4.sin_port = htons(nPort);
	return TPR_OK;
}

//通过整形地址及端口组建TPR_ADDR_T 地址结构 V6
TPR_DECLARE TPR_INT32 CALLBACK TPR_MakeAdd6rByInt(TPR_UINT8 nAddr[16], TPR_UINT16 nPort,TPR_ADDR_T* pTprAddr)
{
	if(!pTprAddr)
	{
		return TPR_ERROR;
	}

	memset(pTprAddr,0,sizeof(*pTprAddr));

	pTprAddr->SA.sin6.sin6_family = AF_INET6;
	pTprAddr->SA.sin6.sin6_port = htons(nPort);
	memcpy(pTprAddr->SA.sin6.sin6_addr.s6_addr,nAddr,16);
	return TPR_OK;
}

//通过SOCKADDR结构 组建TPR_ADDR_T 地址结构
TPR_DECLARE TPR_INT32 CALLBACK TPR_MakeAddr(TPR_VOIDPTR pSockAddr,TPR_INT32 iScokAddrLen,TPR_ADDR_T* pTprAddr)
{
	if(!pSockAddr || ! iScokAddrLen || !pTprAddr)
	{
		return TPR_ERROR;
	}

	memcpy(&pTprAddr->SA.sin4, pSockAddr,iScokAddrLen);
	return TPR_OK;
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_SetAddrPort(TPR_ADDR_T* pTprAddr,TPR_UINT16 nPort)
{
	if(!pTprAddr)
	{
		return TPR_ERROR;
	}
	
	pTprAddr->SA.sin4.sin_port = htons(nPort);
	return TPR_OK;
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_GetAddrType(TPR_ADDR_T* pTprAddr)
{
	if(!pTprAddr)
	{
		return TPR_ERROR;
	}
	
	return pTprAddr->SA.sin4.sin_family;
}

//通过TPR_ADDR_T 地址结构获取字符串地址
TPR_DECLARE const char* CALLBACK TPR_GetAddrString(TPR_ADDR_T* pTprAddr)
{
	static char szDst[128]={0};
	return TPR_GetAddrStringEx(pTprAddr,szDst,sizeof(szDst));
}

TPR_DECLARE const char* CALLBACK TPR_GetAddrStringEx(TPR_ADDR_T* pTprAddr, char* pAddrBuf, size_t nBuflen)
{
	if(!pTprAddr || !pAddrBuf || nBuflen == 0)
	{
		return NULL;
	}

	switch(((PSOCKADDR)&(pTprAddr->SA))->sa_family)
	{
	case AF_INET:
		{
			return TPR_InetNtop(AF_INET,(unsigned char*)&(pTprAddr->SA.sin4.sin_addr),pAddrBuf,nBuflen);
		}
	case AF_INET6:
		{
			return TPR_InetNtop(AF_INET6,(unsigned char*)&(pTprAddr->SA.sin6.sin6_addr),pAddrBuf,nBuflen);
		}
	default:
		return NULL;
	}
}

TPR_DECLARE TPR_UINT16 CALLBACK TPR_GetAddrPort(TPR_ADDR_T* pTprAddr)
{
	if(!pTprAddr)
	{
		return 0;
	}

	if(pTprAddr->SA.sin4.sin_family == AF_INET)
	{
		return ntohs(pTprAddr->SA.sin4.sin_port);
	}
	else
	{
		return ntohs(pTprAddr->SA.sin6.sin6_port);
	}
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_GetAddr4Int(TPR_ADDR_T* pTprAddr, TPR_UINT32* nAddr)
{
	if(!pTprAddr || !nAddr|| pTprAddr->SA.sin4.sin_family != AF_INET)
	{
		return TPR_ERROR;
	}

	*nAddr = pTprAddr->SA.sin4.sin_addr.s_addr;
	return TPR_OK;
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_GetAddr6Int(TPR_ADDR_T* pTprAddr,TPR_UINT8 nAddr[16])
{
	if(!pTprAddr || !nAddr|| pTprAddr->SA.sin4.sin_family == AF_INET)
	{
		return TPR_ERROR;
	}
	
	memcpy(nAddr,&(pTprAddr->SA.sin6.sin6_addr),16);
	return TPR_OK;
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_GetAddrInfo(TPR_ADDR_T* pTprAddr, TPR_ADDR_EXP_T* pTprExp)
{
	if(!pTprAddr || !pTprExp)
	{
		return TPR_ERROR;
	}


	pTprExp->iAf = pTprAddr->SA.sin4.sin_family;
	pTprExp->nPort = ntohs(pTprAddr->SA.sin4.sin_port);
	if(pTprExp->iAf == AF_INET)
	{
		pTprExp->ADDR.nAddr4 = pTprAddr->SA.sin4.sin_addr.s_addr;
	}
	else
	{
		memcpy(&(pTprExp->ADDR.nAddr6),&(pTprAddr->SA.sin6.sin6_addr),16);
	}
	return TPR_OK;
}

//根据sockfd 获取地址信息
TPR_DECLARE TPR_INT32 CALLBACK TPR_GetAddrBySockFd(TPR_SOCK_T iSockFd,TPR_ADDR_T* pTprLocal, TPR_ADDR_T* pRemote)
{
	TPR_INT32 iRet = TPR_OK;
	int iAddrSize = sizeof(TPR_ADDR_T);

	if(pRemote)
	{
		iRet = getpeername(iSockFd,(PSOCKADDR)&(pRemote->SA.sin4),(socklen_t*)&iAddrSize);
		if(iRet !=0)
		{
			return TPR_ERROR;
		}
	}

	if(pTprLocal)
	{
		iRet = getsockname(iSockFd, (PSOCKADDR)&(pTprLocal->SA.sin4),(socklen_t*)&iAddrSize);
		if(iRet !=0)
		{
			return TPR_ERROR;
		}
	}
	return TPR_OK;
}



//TPR_DECLARE int test()
//{
//	TPR_ADDR_T addr;
//
//	TPR_MakeAddrByString(AF_INET,"10.12.1.15",80,&addr);
//	getchar();
//	return 1;
//}