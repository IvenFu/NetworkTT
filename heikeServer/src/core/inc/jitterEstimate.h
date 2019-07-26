
#ifndef _JITTER_ESTIMATE_H_
#define _JITTER_ESTIMATE_H_

#include "TPR_tpr.h"

#define JITTER_MAX_CHANGE_TIME    (100)   //jitterbuffer time每次改变不超过100ms

class JitterEstimateSimple
{
public:
	JitterEstimateSimple();
	~JitterEstimateSimple();

	void InputSample(TPR_UINT32 nTimestamp, TPR_UINT32 iPayLoadFrequence, TPR_INT64 arrvalTime );
	
	TPR_UINT32 JitterBufferTime();
private:
	
	TPR_UINT32 m_iPayLoadFrequence;
	TPR_UINT32 m_nLastTimestamp;
	TPR_INT32 m_iLastTransit;
	double m_fJitter;					//~!!统计的抖动参数
	TPR_UINT32 m_iBufferTime;           //返回的jitter，单位ms
};


class TimestampExtrapolator
{
public:
	TimestampExtrapolator(TPR_INT64 start_ms);
	~TimestampExtrapolator();
	void Update(TPR_INT64 tMs, TPR_UINT32 ts90khz, float fScale);
	TPR_INT64 ExtrapolateLocalTime(TPR_UINT32 timestamp90khz, float fScale);
	void Reset(TPR_INT64 start_ms);

private:
	void CheckForWrapArounds(TPR_UINT32 ts90khz);
	bool DelayChangeDetection(double error);
	double                _w[2];
	double                _pP[2][2];
	TPR_INT64         _startMs;
	TPR_INT64         _prevMs;
	TPR_UINT32        _firstTimestamp;
	TPR_INT32         _wrapArounds;
	TPR_INT64         _prevUnwrappedTimestamp;
	TPR_INT64         _prevWrapTimestamp;
	const double          _lambda;
	bool                  _firstAfterReset;
	TPR_UINT32        _packetCount;
	const TPR_UINT32  _startUpFilterDelayInPackets;

	double              _detectorAccumulatorPos;
	double              _detectorAccumulatorNeg;
	const double        _alarmThreshold;
	const double        _accDrift;
	const double        _accMaxError;
	const double        _pP11;
};



#endif

