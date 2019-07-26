#ifndef _DEMO_AUIDO_H_
#define _DEMO_AUIDO_H_

#include "TPR_tpr.h"
#include "qos.h"
#include "file.h"
#include "netReceiver.h"
//#include "netSender.h"
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
	static void QosLostHandle(TPR_BOOL bReceiver, TPR_BOOL bData, unsigned char* pData, unsigned int uDataLen, void* pUser);
	int QosLostHandleRel(TPR_BOOL bReceiver, TPR_BOOL bData, unsigned char* pData, unsigned int uDataLen);


	static void NetUpBwHandle(unsigned char* pData, unsigned int nDataLen, void* pUser);
	int NetUpBwHandleRel(unsigned char* pData, unsigned int nDataLen);

	static void NetDownBwHandle(unsigned char* pData, unsigned int nDataLen, void* pUser);
	int NetDownBwHandleRel(unsigned char* pData, unsigned int nDataLen);
	static void CollectHandle(unsigned char* pData, unsigned int nDataLen, void* pUser);
	int CollectHandleRel(unsigned char* pData, unsigned int nDataLen);

	Net* m_netLostRTT;
	Qos* m_qosLostRTT;

	Net* m_netUpBw;
	RateStatistics m_rateUp;


	Net* m_netDownBw;
	Qos* m_qosDownBw;
	File* m_file;
	TPR_BOOL m_bStartDownBw;

	Config m_config;
	//RateStatistics m_rate;
};


#endif

