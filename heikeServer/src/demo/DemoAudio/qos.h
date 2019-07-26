
#ifndef _QOS_H_
#define _QOS_H_

#include "NPQos.h"
#include "config.h"

class Qos
{
public:
	Qos(Config& config);
	~Qos();

	int Open(CallbackFunc2 pCallback, CallbackFuncCmd pCallbackCmd, void* pUser,NPQ_QOS_ROLE role);

	int Close();

	int InputData(TPR_BOOL bData,unsigned char* pData, int iDataLen);
	
	int OutPutData(unsigned char* pData, unsigned int* pDataLen);

	int GetStat(unsigned int* pRtt, unsigned char* pLossRate, unsigned int* pBitrate);
private:
	static void __stdcall QosCallback( int id , int iDataType, unsigned char* pData, unsigned int nDataLen, void* pUser);
	int QosCallbackRel(int iDataType, unsigned char* pData, unsigned int nDataLen);

	int m_nNPQId;
	CallbackFunc2 m_pCallback;
	CallbackFuncCmd m_pCmdCallback;
	void* m_pUser;
	NPQ_QOS_ROLE m_role;
	Config& m_config;
};
#endif
