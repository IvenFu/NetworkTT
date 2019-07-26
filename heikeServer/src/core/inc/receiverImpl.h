
#ifndef _RECEIVEIMPL_H_
#define _RECEIVEIMPL_H_

#include "TPR_Mutex.h"
#include "baseImpl.h"
#include "Util.h"
#include "receiverBase.h"
#include "buffer.h"
#include "transportFeedback.h"

class TransportFeedback;
class Nack;
class Fec;
class ReceiverImpl:public ReceiverBase
{
public:
	static ReceiverImpl* Create(OuterParam& struOuterParam, NPQ_QOS_MAIN_TYPE enMainType);
	static int Destroy(ReceiverImpl* pReceiver);

	int Start();

	int Stop();

	int InputData(CRtpPacket& RtpPacket);

	int SetMinDelay(unsigned int nDelay);

	int GetStat(NPQ_STAT* pStat);

protected:
	ReceiverImpl(OuterParam& struOuterParam,NPQ_QOS_MAIN_TYPE enMainType);
	~ReceiverImpl();

private:
	int Init();
	int Fini();

	TPR_BOOL m_bStart;						//是否启动功能标致
};
#endif

