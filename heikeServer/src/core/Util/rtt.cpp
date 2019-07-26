


#include <math.h>
#include "rtt.h"
#include "Util.h"


VCMRttFilter::VCMRttFilter()
: _filtFactMax(35),
_jumpStdDevs(2.5),
_driftStdDevs(3.5),
_detectThreshold(kMaxDriftJumpCount) {
	Reset();
}

VCMRttFilter& VCMRttFilter::operator=(const VCMRttFilter& rhs)
{
	if (this != &rhs)
	{
		_gotNonZeroUpdate = rhs._gotNonZeroUpdate;
		_avgRtt = rhs._avgRtt;
		_varRtt = rhs._varRtt;
		_maxRtt = rhs._maxRtt;
		_filtFactCount = rhs._filtFactCount;
		_jumpCount = rhs._jumpCount;
		_driftCount = rhs._driftCount;
		memcpy(_jumpBuf, rhs._jumpBuf, sizeof(_jumpBuf));
		memcpy(_driftBuf, rhs._driftBuf, sizeof(_driftBuf));
	}
	return *this;
}

void VCMRttFilter::Reset()
{
	_gotNonZeroUpdate = false;
	_avgRtt = 0;
	_varRtt = 0;
	_maxRtt = 0;
	_filtFactCount = 1;
	_jumpCount = 0;
	_driftCount = 0;
	memset(_jumpBuf, 0, kMaxDriftJumpCount);
	memset(_driftBuf, 0, kMaxDriftJumpCount);
}

void VCMRttFilter::Update(TPR_INT64 rttMs)
{
	if (!_gotNonZeroUpdate)
	{
		if (rttMs == 0)
		{
			return;
		}
		_gotNonZeroUpdate = true;
	}

	// 最大不超过3秒
	if (rttMs > 3000)
	{
		NPQ_ERROR("!!!!!!!!!!!!!!rtt filter max 3000,rttMs=%lld",rttMs);
		rttMs = 3000;
	}

	double filtFactor = 0;
	if (_filtFactCount > 1)
	{
		filtFactor = static_cast<double>(_filtFactCount - 1) / _filtFactCount;
	}
	_filtFactCount++;
	if (_filtFactCount > _filtFactMax)
	{
		// This prevents filtFactor from going above
		// (_filtFactMax - 1) / _filtFactMax,
		// e.g., _filtFactMax = 50 => filtFactor = 49/50 = 0.98
		_filtFactCount = _filtFactMax;
	}
	double oldAvg = _avgRtt;
	double oldVar = _varRtt;
	_avgRtt = filtFactor * _avgRtt + (1 - filtFactor) * rttMs;
	_varRtt = filtFactor * _varRtt + (1 - filtFactor) *
		(rttMs - _avgRtt) * (rttMs - _avgRtt);
	_maxRtt = npq_max(rttMs, _maxRtt);
	if (!JumpDetection(rttMs) || !DriftDetection(rttMs))
	{
		// In some cases we don't want to update the statistics
		_avgRtt = oldAvg;
		_varRtt = oldVar;
	}
}

bool VCMRttFilter::JumpDetection(TPR_INT64 rttMs)
{
	double diffFromAvg = _avgRtt - rttMs;
	if (fabs(diffFromAvg) > _jumpStdDevs * sqrt(_varRtt))
	{
		int diffSign = (diffFromAvg >= 0) ? 1 : -1;
		int jumpCountSign = (_jumpCount >= 0) ? 1 : -1;
		if (diffSign != jumpCountSign)
		{
			// Since the signs differ the samples currently
			// in the buffer is useless as they represent a
			// jump in a different direction.
			_jumpCount = 0;
		}
		if (abs(_jumpCount) < kMaxDriftJumpCount)
		{
			// Update the buffer used for the short time
			// statistics.
			// The sign of the diff is used for updating the counter since
			// we want to use the same buffer for keeping track of when
			// the RTT jumps down and up.
			_jumpBuf[abs(_jumpCount)] = rttMs;
			_jumpCount += diffSign;
		}
		if (abs(_jumpCount) >= _detectThreshold)
		{
			// Detected an RTT jump
			ShortRttFilter(_jumpBuf, abs(_jumpCount));
			_filtFactCount = _detectThreshold + 1;
			_jumpCount = 0;
		}
		else
		{
			return false;
		}
	}
	else
	{
		_jumpCount = 0;
	}
	return true;
}

bool VCMRttFilter::DriftDetection(TPR_INT64 rttMs)
{
	if (_maxRtt - _avgRtt > _driftStdDevs * sqrt(_varRtt))
	{
		if (_driftCount < kMaxDriftJumpCount)
		{
			// Update the buffer used for the short time
			// statistics.
			_driftBuf[_driftCount] = rttMs;
			_driftCount++;
		}
		if (_driftCount >= _detectThreshold)
		{
			// Detected an RTT drift
			ShortRttFilter(_driftBuf, _driftCount);
			_filtFactCount = _detectThreshold + 1;
			_driftCount = 0;
		}
	}
	else
	{
		_driftCount = 0;
	}
	return true;
}

void VCMRttFilter::ShortRttFilter(TPR_INT64* buf, TPR_UINT32 length)
{
	if (length == 0)
	{
		return;
	}
	_maxRtt = 0;
	_avgRtt = 0;
	for (TPR_UINT32 i=0; i < length; i++)
	{
		if (buf[i] > _maxRtt)
		{
			_maxRtt = buf[i];
		}
		_avgRtt += buf[i];
	}
	_avgRtt = _avgRtt / static_cast<double>(length);
}

TPR_INT64 VCMRttFilter::RttMs() const
{
	return static_cast<TPR_INT64>(_maxRtt + 0.5);
}


RttStat::RttStat()
{

}

RttStat::~RttStat()
{

}

void RttStat::UpdateAvgRttMs( unsigned int nRttMs )
{
	//NPQ_DEBUG("nRttMs = %u",nRttMs);
	m_rttFilter.Update((TPR_INT64)nRttMs);
}

unsigned int RttStat::RttUs()
{
	return (unsigned int)(m_rttFilter.RttMs()*1000);
}


