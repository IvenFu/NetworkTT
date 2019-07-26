
#ifndef _PROTECTION_BITRATE_CALCULATOR_H_
#define _PROTECTION_BITRATE_CALCULATOR_H_

#include "rtp.h"
#include "frame.h"
#include "fecFactor.h"
#include "bwManager.h"
#include "bwCallback.h"

class CBwCallback;
class ProtectionBitrateCalculator
{
public:
	ProtectionBitrateCalculator(CBwCallback& bwCallback);
	~ProtectionBitrateCalculator();
	
	/*��߻��߲����б仯��ʱ�����*/
	void SetEncodingData(int iWidth,int height,int num_temporal_layers,int max_payload_size);
	
	/*ÿ֡����һ�β������ô˺���*/
	void UpdateWithEncodedData(CFrame& frame);
	
	/*��ʼ������*/
	void SetProtectionMethod(TPR_BOOL enable_fec,TPR_BOOL enable_nack);
	
	/*ӵ�����Ƽ���õ�����ʱ����*/
	TPR_UINT32 SetTargetRates( TPR_UINT32 iEstimatedBitratebps, int iActualFramerateFps, TPR_UINT8 aFractionLost,TPR_INT64 rttMs); 

private:
	int m_MaxPayloadSize;

	VCMLossProtectionLogic* m_pProtectionLogic;
	CBwCallback& m_bwCallback;
};

struct BwEstimateResult;
class ProtectionBitrateCalculatorWrap
{
public:
	ProtectionBitrateCalculatorWrap(CBwCallback& bwCallback);
	~ProtectionBitrateCalculatorWrap();

	/*����rtp����*/
	int InputData(CRtpPacket& RtpPacket);
	
	/*���÷ֱ��ʵȱ������*/
	void SetEncodingData(int iWidth,int iHeight);
	
	/*�������ʲ��������㣬�������ñ���������*/
	int OnBitrateChanged(BwEstimateResult& result);

private:
	ProtectionBitrateCalculator* m_pBitrateCalulator;		//���FEC����ʱ����Ҫ���������������
	CFrame m_frame;

	TPR_BOOL m_bInit;
};


#endif

