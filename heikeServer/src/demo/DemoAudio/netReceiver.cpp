#include "netReceiver.h"


Net::Net(Config config)
:m_bStart(TPR_FALSE)
,m_hThread(TPR_INVALID_HANDLE)
,m_iDataSockFd(-1)
,m_pCallback(NULL)
,m_pUser(NULL)
{
	m_config = config;
}

Net::~Net()
{

}

int Net::Open(CallbackFunc pCallback, void* pUser)
{
	if(!pCallback)
	{
		return -1;
	}

	m_hThread = TPR_Thread_Create(NetThread, this, 0,0);
	if (TPR_INVALID_THREAD == m_hThread)
	{
		return -1;
	}

	m_pCallback = pCallback;
	m_pUser = pUser;
	return 0;
}

int Net::Close()
{
	m_bStart = TPR_FALSE;
	if(m_hThread!= TPR_INVALID_THREAD)
	{
		TPR_Thread_Wait(m_hThread);
		m_hThread = TPR_INVALID_THREAD;
	}
	return 0;
}

TPR_VOIDPTR __stdcall Net::NetThread( TPR_VOIDPTR pUserData )
{
	Net* pcClient = (Net*)pUserData;

	if (NULL == pcClient)
	{
		return NULL;
	}

	pcClient->NetThreadRel();
	return NULL;
}


int Net::NetThreadRel()
{
	TPR_ADDR_T clientAddr,serverAddr;
	int iRet = 0;

	TPR_UINT16 sPort = m_config.m_sServerPort;

	int iSockFdQos = TPR_CreateSocket(AF_INET, SOCK_DGRAM, 0);
	if (TPR_INVALID_SOCKET == iSockFdQos)
	{
		return 0;
	}

	memset(&serverAddr,0,sizeof(serverAddr));

	memset(&clientAddr,0,sizeof(clientAddr));
	clientAddr.SA.sin4.sin_family =AF_INET;
	clientAddr.SA.sin4.sin_addr.s_addr = htonl(INADDR_ANY);
	clientAddr.SA.sin4.sin_port =htons(sPort);
	iRet = TRR_Bind(iSockFdQos,&clientAddr);
	TPR_SetBuffSize(iSockFdQos,2*1024*1024,2*1024*1024);

	m_iDataSockFd = iSockFdQos;

	fd_set fdsRead;
	timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	m_bStart = TPR_TRUE;
	while(m_bStart)
	{
		FD_ZERO(&fdsRead);
		FD_SET(iSockFdQos,&fdsRead);
		int iMaxFd = iSockFdQos + 1;

		iRet = TPR_Select(iMaxFd,&fdsRead,NULL,NULL,&tv);
		if(iRet > 0)
		{
			if(TPR_FdIsSet(iSockFdQos,&fdsRead))
			{
				iRet = TPR_RecvFrom(iSockFdQos, m_buf, BUF_LEN, &serverAddr);
				if(iRet > 0)
				{
					if(m_pCallback)
					{
						m_pCallback(m_buf,iRet,m_pUser);
					}

					m_serverAddr = serverAddr; 
				}
			}
		}
		else if(iRet < 0)
		{
			TPR_Sleep(1000);
		}	
	}

	TPR_CloseSocket(iSockFdQos);
	return 0;
}

int Net::InputData( unsigned char* pData, int iDataLen )
{
	if(m_iDataSockFd==-1)
	{
		return -1;
	}

	if(iDataLen != TPR_SendTo(m_iDataSockFd,(TPR_VOIDPTR)pData,iDataLen, &m_serverAddr))
	{
		printf("TPR_SendTo device rtcp err\n");
		return -1;
	}
	return 0;
}
