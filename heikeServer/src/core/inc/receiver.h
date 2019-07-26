
#ifndef _RECEIVER_H_
#define _RECEIVER_H_


#include "Util.h"
#include "base.h"
#include "receiverImpl.h"
#include "rtcpDefine.h"

class RtpStreamsSynchronizer;
class Receiver: public Base
{
public:
	static Base* Create(OuterParam& struOuterParam);

	~Receiver();

	int Start();

	int Stop();

	int InputData(int iDataType, unsigned char* pData, unsigned int nDataLen);
	
	int GetStat (NPQ_MAIN_TYPE enMainType,NPQ_STAT* pStat);

	int SetMinDelay(unsigned int nDelay, TPR_BOOL bVideo);

	int GetSrInfo( RTCP_SR_INFO* pInfo, TPR_BOOL bVideo );

	int Get10msPcmRtp(int iDataType,unsigned char* pOutData, unsigned int* pLen);

	int SetNotifyParam(NPQ_SET_NOTIFY_PARAM* pNotifyParam);
protected:
	Receiver(OuterParam& struOuterParam);
	
private:
	ReceiverImpl* m_pRecvVideo;
};

#endif


