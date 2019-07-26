#ifndef _DEMO_AUIDO_H_
#define _DEMO_AUIDO_H_

#include "TPR_TPR.h"
#include "qos.h"
#include "file.h"
#include "netReceiver.h"
#include "netSender.h"
#include "player.h"
#include "DrateStatistics.h"

class InitTPR
{
public:
	InitTPR(){}
	~InitTPR(){}

	static int Init()
	{
		if(!s_bInit)
		{
			s_bInit = TPR_TRUE;
			return TPR_Init();
		}
		return TPR_OK;
	}

	static int Fini()
	{
		if(s_bInit)
		{
			s_bInit = TPR_FALSE;
		}
		return TPR_OK;
	}

private:
	static TPR_BOOL s_bInit; 
};

class Audio
{
public:
	Audio();
	~Audio();

	int Start();
	int Stop();
private:
	int SimpleParsePacket(unsigned char* pData, unsigned int nDataLen);

	static void NetLosteHandle(unsigned char* pData, unsigned int nDataLen, void* pUser);
	int NetLosteHandleRel(unsigned char* pData, unsigned int nDataLen);

	static void NetUpBwHandle(unsigned char* pData, unsigned int nDataLen, void* pUser);
	int NetUpBwHandleRel(unsigned char* pData, unsigned int nDataLen);

	static void QosLostHandle(TPR_BOOL bReceiver, TPR_BOOL bData, unsigned char* pData, unsigned int uDataLen, void* pUser);
	int QosLostHandleRel(TPR_BOOL bReceiver, TPR_BOOL bData, unsigned char* pData, unsigned int uDataLen);


	Net* m_netLostRTT;
	Net* m_netUpBw;
	RateStatistics m_rateUp;


	Net* m_netDownBw;
	Qos* m_qosDownBw;
	Qos* m_qosLostRTT;

	Config m_config;
	//RateStatistics m_rate;
};


#endif

