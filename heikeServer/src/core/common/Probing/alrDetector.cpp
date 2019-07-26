
#include "TPR_Time.h"
#include "alrDetector.h"

static const int kDefaultAlrBandwidthUsagePercent = 65;
static const int kDefaultAlrStartBudgetLevelPercent = 80;
static const int kDefaultAlrStopBudgetLevelPercent = 50;


AlrDetector::AlrDetector()
    : bandwidth_usage_percent_(kDefaultAlrBandwidthUsagePercent),
      alr_start_budget_level_percent_(kDefaultAlrStartBudgetLevelPercent),
      alr_stop_budget_level_percent_(kDefaultAlrStopBudgetLevelPercent),
      alr_budget_(0, true),
	  alr_started_time_ms_(0)
{
}

AlrDetector::~AlrDetector() {}

void AlrDetector::OnBytesSent(size_t bytes_sent, TPR_INT64 delta_time_ms) 
{
  alr_budget_.UseBudget(bytes_sent);
  alr_budget_.IncreaseBudget(delta_time_ms);
  
  bool state_changed = false;
  if (alr_budget_.budgetLevelPercent() > alr_start_budget_level_percent_ &&  //大于80%触发
      !alr_started_time_ms_) 
 {
	 alr_started_time_ms_= OneTime::OneNowMs();
    state_changed = true;
  }
  else if (alr_budget_.budgetLevelPercent() <  //小于50% 停止
                 alr_stop_budget_level_percent_ &&
             alr_started_time_ms_) 
  {
    state_changed = true;
    alr_started_time_ms_ = 0;
  }
  //NPQ_DEBUG("alr_started_time_ms_ = %lld",alr_started_time_ms_);
}

void AlrDetector::SetEstimatedBitrate(int bitrate_bps)
{
  const int target_rate_kbps = bitrate_bps * bandwidth_usage_percent_ / (1000 * 100);  //65% 的码率
  alr_budget_.setTargetRateKbps(target_rate_kbps);
}

TPR_INT64 AlrDetector::GetApplicationLimitedRegionStartTime()const 
{
  return alr_started_time_ms_;
}


