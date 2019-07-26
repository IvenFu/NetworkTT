#ifndef _RTT_H_
#define _RTT_H_

#include "TPR_tpr.h"


class VCMRttFilter
{
public:
	VCMRttFilter();

	VCMRttFilter& operator=(const VCMRttFilter& rhs);

	void Reset();

	void Update(TPR_INT64 rttMs);

	TPR_INT64 RttMs() const;

private:

	enum { kMaxDriftJumpCount = 5 };

	bool JumpDetection(TPR_INT64 rttMs);

	bool DriftDetection(TPR_INT64 rttMs);

	void ShortRttFilter(TPR_INT64* buf, TPR_UINT32 length);

	bool                  _gotNonZeroUpdate;
	double                _avgRtt;
	double                _varRtt;
	TPR_INT64         _maxRtt;
	TPR_UINT32        _filtFactCount;
	const TPR_UINT32  _filtFactMax;
	const double          _jumpStdDevs;
	const double          _driftStdDevs;
	TPR_INT32         _jumpCount;
	TPR_INT32         _driftCount;
	const TPR_INT32   _detectThreshold;
	TPR_INT64         _jumpBuf[kMaxDriftJumpCount];
	TPR_INT64         _driftBuf[kMaxDriftJumpCount];
};

class RttStat
{
public:
	RttStat();
	~RttStat();

	unsigned int RttUs();

	void UpdateAvgRttMs(unsigned int nRttMs);

private:
	VCMRttFilter m_rttFilter;
};
#endif
