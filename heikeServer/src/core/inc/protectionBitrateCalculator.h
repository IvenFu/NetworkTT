
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
	
	/*宽高或者层数有变化的时候调用*/
	void SetEncodingData(int iWidth,int height,int num_temporal_layers,int max_payload_size);
	
	/*每帧更新一次参数调用此函数*/
	void UpdateWithEncodedData(CFrame& frame);
	
	/*初始化功能*/
	void SetProtectionMethod(TPR_BOOL enable_fec,TPR_BOOL enable_nack);
	
	/*拥塞控制计算得到码率时调用*/
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

	/*输入rtp数据*/
	int InputData(CRtpPacket& RtpPacket);
	
	/*设置分辨率等编码参数*/
	void SetEncodingData(int iWidth,int iHeight);
	
	/*设置码率并触发计算，返回设置编码器码率*/
	int OnBitrateChanged(BwEstimateResult& result);

private:
	ProtectionBitrateCalculator* m_pBitrateCalulator;		//结合FEC计算时间需要设给编码器的码率
	CFrame m_frame;

	TPR_BOOL m_bInit;
};


#endif

