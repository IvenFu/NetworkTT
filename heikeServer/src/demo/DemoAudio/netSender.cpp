
#include "netSender.h"

NetSender::NetSender(Config& config)
:m_iSocket(TPR_INVALID_SOCKET)
,m_config(config)
,m_bStart(TPR_FALSE)
,m_hThread(TPR_INVALID_THREAD)
,m_pCallback(NULL)
,m_pUser(NULL)
{
}

NetSender::~NetSender()
{

}

int NetSender::Open(CallbackFunc pCallback, void* pUser)
{
	TPR_UINT16 sPort = m_config.m_sServerPort;
	string szClientIp(m_config.m_szServerzIp);

	if(!pCallback)
	{
		return -1;
	}

	//Qos
	m_iSocket = TPR_CreateSocket(AF_INET, SOCK_DGRAM, 0);
	if (TPR_INVALID_SOCKET == m_iSocket)
	{
		return -1;
	}

	TPR_SetBuffSize(m_iSocket,2*1024*1024,2*1024*1024);

	memset(&m_clientAddr,0,sizeof(m_clientAddr));
	m_clientAddr.SA.sin4.sin_family = AF_INET;
	m_clientAddr.SA.sin4.sin_port = htons(sPort);
	TPR_InetPton(szClientIp.c_str(), &m_clientAddr.SA.sin4.sin_addr);

	m_hThread = TPR_Thread_Create(NetThread, this, 0,0);
	if (TPR_INVALID_THREAD == m_hThread)
	{
		return -1;
	}

	m_pCallback = pCallback;
	m_pUser = pUser;
	return 0;
}

int NetSender::Close()
{
	m_bStart = TPR_FALSE;
	if(m_hThread!= TPR_INVALID_THREAD)
	{
		TPR_Thread_Wait(m_hThread);
		m_hThread = TPR_INVALID_THREAD;
	}

	TPR_CloseSocket(m_iSocket);
	return 0;
}

int NetSender::InputData( unsigned char* pData, int iDataLen )
{
	int iRet;
	
	iRet = TPR_SendTo(m_iSocket, pData,  iDataLen, &m_clientAddr);
	if(iRet == -1)
	{
		printf("TPR_SendTo device err\n");
		return -1;
	}
	return 0;
}

TPR_VOIDPTR __stdcall NetSender::NetThread( TPR_VOIDPTR pUserData )
{
	NetSender* p = (NetSender*)pUserData;

	if (NULL == p)
	{
		return NULL;
	}

	p->NetThreadRel();
	return NULL;
}

int NetSender::NetThreadRel()
{
	int iRet;
	TPR_ADDR_T clientAddr;

	fd_set fdsRead;
	timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	m_bStart = TPR_TRUE;
	while(m_bStart)
	{
		FD_ZERO(&fdsRead);
		FD_SET(m_iSocket,&fdsRead);
		int iMaxFd = m_iSocket + 1;

		iRet = TPR_Select(iMaxFd,&fdsRead,NULL,NULL,&tv);
		if(iRet > 0)
		{
			if(TPR_FdIsSet(m_iSocket,&fdsRead))
			{
				iRet = TPR_RecvFrom(m_iSocket, m_buf, BUF_LEN, &clientAddr);
				if(iRet > 0)
				{
					//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!NetSender recv len=%d\n",iRet);
					if(m_pCallback)
					{
						m_pCallback(m_buf,iRet,m_pUser);
					}
				}
				else
				{
					printf("read udp err, maybe port unreadable\n");
					continue;
				}
			}
		}
		else if(iRet < 0)
		{
			printf("client close connect 2\n");
			break;
		}	
	}
	return 0;
}

