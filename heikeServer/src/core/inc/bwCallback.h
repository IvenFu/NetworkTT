#ifndef _BW_CALLBACK_H_
#define _BW_CALLBACK_H_

#include "callback.h"
#include "fec.h"
#include "PaceSender.h"
#include "ProbeControler.h"

enum FecMaskType 
{
	kFecMaskRandom,
	kFecMaskBursty,
};

struct FecProtectionParams 
{
	unsigned char fec_rate;
	int max_fec_frames;
	FecMaskType fec_mask_type;
};

class Fec;
class PacedSender;
class ProbeController;

class CBwCallback
{
public:
	CBwCallback(OuterParam& struOuterParam);
	~CBwCallback();
	void SetFecInterface(Fec* pFec);
	void SetPacedSenderInterface(PacedSender* pPacedSender);
    void SetProbeControlInterface(ProbeController* pProbeControl);

	int TriggerBwCallback(FecProtectionParams* pKey,FecProtectionParams* pDelta, TPR_UINT32* pVideobps,TPR_UINT32* pNackbps, TPR_UINT32* pFecbps);
	int TriggerBwCallback(int iBitrate);
private:
	
	OuterParam& m_struOuterParam;	/*外部参数结构体*/
	Fec* m_pFec;
	PacedSender* m_pPacedSender;
    ProbeController* m_pProbeControl;
};












#endif


