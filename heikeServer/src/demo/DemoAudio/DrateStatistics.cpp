

#include <iostream>
#include <assert.h>
#include "DrateStatistics.h"


static float kbpsScale = 8000.0f;				//将字节转化为bps  size*kbpsScale/windows_size
static int kBitrateStatisticsWindowMs = 1000;	//统计码率的时间窗口

RateStatistics::RateStatistics()
:buckets_(new (std::nothrow)Bucket[kBitrateStatisticsWindowMs]()),
accumulated_count_(0),
num_samples_(0),
oldest_time_(-kBitrateStatisticsWindowMs),
oldest_index_(0),
scale_(kbpsScale),
max_window_size_ms_(kBitrateStatisticsWindowMs),
current_window_size_ms_(max_window_size_ms_) {}

RateStatistics::~RateStatistics() 
{
	delete []buckets_;
}

void RateStatistics::Reset() 
{
	accumulated_count_ = 0;
	num_samples_ = 0;
	oldest_time_ = -max_window_size_ms_;
	oldest_index_ = 0;
	current_window_size_ms_ = max_window_size_ms_;
	for (int i = 0; i < max_window_size_ms_; i++)
	{
		buckets_[i] = Bucket();
	}
}

void RateStatistics::Update(int count, TPR_INT64 nowMs) 
{
	TPR_Guard guard(&m_lock);

	if (nowMs < oldest_time_) 
	{
		// Too old data is ignored.
		return;
	}

	EraseOld(nowMs);

	// First ever sample, reset window to start now.
	if (!IsInitialized())
	{
		oldest_time_ = nowMs;
	}
	
	TPR_UINT32 now_offset = static_cast<TPR_UINT32>(nowMs - oldest_time_);
	assert(now_offset < max_window_size_ms_);
	TPR_UINT32 index = oldest_index_ + now_offset;
	if (index >= max_window_size_ms_)
	index -= max_window_size_ms_;
	buckets_[index].sum += count;
	++buckets_[index].samples;
	accumulated_count_ += count;
	++num_samples_;
}

TPR_UINT32 RateStatistics::Rate(TPR_INT64 nowMs) 
{
	TPR_Guard guard(&m_lock);

	EraseOld(nowMs);

	// If window is a single bucket or there is only one sample in a data set that
	// has not grown to the full window size, treat this as rate unavailable.
	TPR_INT64 active_window_size = nowMs - oldest_time_ + 1;
	if (num_samples_ == 0 || active_window_size <= 1 ||
	  (num_samples_ <= 1 && active_window_size < current_window_size_ms_)) 
	{
		return 0;
	}

	float scale = scale_ / active_window_size;
	return static_cast<TPR_UINT32>(accumulated_count_ * scale + 0.5f);
}

void RateStatistics::EraseOld(TPR_INT64 nowMs) 
{
	if (!IsInitialized())
	{
		return;
	}
	// New oldest time that is included in data set.
	TPR_INT64 new_oldest_time = nowMs - current_window_size_ms_ + 1;

	// New oldest time is older than the current one, no need to cull data.
	if (new_oldest_time <= oldest_time_)
	{
		return;
	}

	// Loop over buckets and remove too old data points.
	while (num_samples_ > 0 && oldest_time_ < new_oldest_time) 
	{
		const Bucket& oldest_bucket = buckets_[oldest_index_];
		assert(accumulated_count_ >= oldest_bucket.sum);
		assert(num_samples_ >= oldest_bucket.samples);
		accumulated_count_ -= oldest_bucket.sum;
		num_samples_ -= oldest_bucket.samples;
		buckets_[oldest_index_] = Bucket();
		if (++oldest_index_ >= max_window_size_ms_)
		{
			oldest_index_ = 0;
		}
		++oldest_time_;
	}
	oldest_time_ = new_oldest_time;
}

TPR_BOOL RateStatistics::SetWindowSize(TPR_INT64 windowSizeMs, TPR_INT64 nowMs) 
{
	if (windowSizeMs <= 0 || windowSizeMs > max_window_size_ms_)
	{
		return TPR_FALSE;
	}

	current_window_size_ms_ = windowSizeMs;
	EraseOld(nowMs);
	return TPR_TRUE;
}

TPR_BOOL RateStatistics::IsInitialized() const 
{
	return oldest_time_ != -max_window_size_ms_;
}


