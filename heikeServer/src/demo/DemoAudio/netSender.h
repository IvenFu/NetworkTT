
#ifndef _NET_SENDER_H_
#define _NET_SENDER_H_

#include "config.h"
#include <string>
using std::string;

class NetSender
{
public:
	NetSender(Config& config);
	~NetSender();

	int Open(CallbackFunc pCallback, void* pUser);
	int Close();
	int InputData(unsigned char* pData, int iDataLen);
private:

	TPR_SOCK_T m_iSocket;
	TPR_ADDR_T m_clientAddr;
	Config& m_config;

	static TPR_VOIDPTR __stdcall NetThread(TPR_VOIDPTR pUserData);
	int NetThreadRel();
	TPR_BOOL m_bStart;
	TPR_HANDLE m_hThread;

#define BUF_LEN 2048
	unsigned char m_buf[BUF_LEN];
	CallbackFunc m_pCallback;
	void* m_pUser;
};

#endif
