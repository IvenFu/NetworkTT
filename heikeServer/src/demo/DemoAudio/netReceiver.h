
#ifndef _NET_RECEIVER_H_
#define _NET_RECEIVER_H_

#include "config.h"

class Net
{
public:
	Net(Config config);
	~Net();

	int Open(CallbackFunc pCallback, void* pUser);

	int Close();

	int InputData(unsigned char* pData, int iDataLen);
private:
	static TPR_VOIDPTR __stdcall NetThread(TPR_VOIDPTR pUserData);
	int NetThreadRel();
	TPR_BOOL m_bStart;
	TPR_HANDLE m_hThread;
	TPR_ADDR_T m_serverAddr;
	int m_iDataSockFd;
	Config m_config;

#define BUF_LEN 2048
	unsigned char m_buf[BUF_LEN];
	CallbackFunc m_pCallback;
	void* m_pUser;
};

#endif
