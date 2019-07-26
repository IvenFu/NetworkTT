#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
#include "TPR_TPR.h"
#include "TPR_Thread.h"
#include "TPR_Utils.h"
#include "TPR_Socket.h"
#include "TPR_Select.h"
#include "TPR_Time.h"


#if !defined(WIN32)
#ifndef __stdcall
#define __stdcall
#endif
#endif



#define PACKET_RTP    0
#define PACKET_RTCPVIDEO   1
#define PACKET_RTCPAUDIO   2
#define PACKET_ERROR  3

typedef void (* CallbackFunc)(unsigned char* pData, unsigned int nDataLen, void* pUser);
typedef void (* CallbackFunc2)(TPR_BOOL bReceiver, TPR_BOOL bData, unsigned char* pData, unsigned int nDataLen, void* pUser);
typedef void (*CallbackFuncCmd)(void* pParam, void* pUser);


using std::string;

class Config
{
public:
	Config():
		m_sServerPort(5678)
	{
	
	}

	TPR_UINT16 m_sServerPort;
};


int DhlogformatWarp(char* szLevel, const char* format, ...);

#define DEMO_DEBUG(fmt, ...)            DhlogformatWarp("ERROR","<[%d] - %s> " fmt,  __LINE__, __FUNCTION__, ##__VA_ARGS__)

#endif
