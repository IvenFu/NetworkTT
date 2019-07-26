

#ifndef _RATE_STATISTICS_H_
#define _RATE_STATISTICS_H_

#include "TPR_TPR.h"
#include "TPR_Mutex.h"
#include "TPR_Guard.h"

class RateStatistics 
{
public:
	RateStatistics();
	~RateStatistics();

	// Reset instance to original state.
	void Reset();

	/*更新数据*/
	void Update(int count, TPR_INT64 nowMs);

	// Note that despite this being a const method, it still updates the internal
	// state (moves averaging window), but it doesn't make any alterations that
	// are observable from the other methods, as long as supplied timestamps are
	// from a monotonic clock. Ie, it doesn't matter if this call moves the
	// window, since any subsequent call to Update or Rate would still have moved
	// the window as much or more.
	TPR_UINT32 Rate(TPR_INT64 nowMs);

	// Update the size of the averaging window. The maximum allowed value for
	// window_size_ms is max_window_size_ms as supplied in the constructor.
	TPR_BOOL SetWindowSize(TPR_INT64 windowSizeMs, TPR_INT64 nowMs);

private:
	void EraseOld(TPR_INT64 nowMs);
	TPR_BOOL IsInitialized() const;

	// Counters are kept in buckets (circular buffer), with one bucket
	// per millisecond.
	struct Bucket 
	{
		int sum;      // Sum of all samples in this bucket.
		int samples;  // Number of samples in this bucket.
	};
	Bucket* buckets_;

	// Total count recorded in buckets.
	int accumulated_count_;

	// The total number of samples in the buckets.
	int num_samples_;

	// Oldest time recorded in buckets.
	TPR_INT64 oldest_time_;

	// Bucket index of oldest counter recorded in buckets.
	TPR_UINT32 oldest_index_;

	// To convert counts/ms to desired units
	const float scale_;

	// The window sizes, in ms, over which the rate is calculated.
	const TPR_INT64 max_window_size_ms_;
	TPR_INT64 current_window_size_ms_;

	TPR_Mutex m_lock;
};

#endif