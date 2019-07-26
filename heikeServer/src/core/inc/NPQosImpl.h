#ifndef _NPQOSIMPH_H_
#define _NPQOSIMPH_H_

#include "versionCtrl.h"
#include "Util.h"
#include "base.h"
#include "callback.h"
#include "rtp.h"


#define NPQ_DEFAULT_RTT       (50*1000)		//默认RTT  50ms
#define NPQ_MAX_RTT           (500*1000)    //最大RTT  500ms

#define NACK_REORDER_TIME     (10*1000)     //乱序等待时间
#define NACK_FEC_TIME         (40*1000)     //FEC等待时间
#define NACK_MUTIL_INTERVAL   (10*1000)     //10ms误差时间
#define NACK_THREAD_TIME      (10*1000)     //NACK线程池启动时间


typedef NPQ_MAIN_TYPE NPQ_QOS_MAIN_TYPE;


class Base;
class NPQosImpl
{
public:
	NPQosImpl();
	~NPQosImpl();

	int Create(NPQ_QOS_ROLE enType);
	int Destroy(int id);
	int Start();
	int Stop();
	int RegisterDataCallBack(NPQCb callback, void* pUser);
	int InputData(int iDataType, unsigned char* pData, unsigned int iDataLen);
	int SetParam(NPQ_PARAM* pParam);
	int SetNotifyParam(NPQ_SET_NOTIFY_PARAM* pNotifyParam);
	int GetStat(NPQ_MAIN_TYPE enMainType,NPQ_STAT* pStat);
private:

	Base* CreateQos(NPQ_QOS_ROLE enType);
	void DestroyQos(Base* pBase);

	OuterParam m_struOuterPram;
	Base* m_pQosBase;
};

#endif



