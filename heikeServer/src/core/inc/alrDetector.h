

#ifndef _ALRDETECTOR_H_
#define _ALRDETECTOR_H_

#include "Util.h"
#include "PacedPacket.h"



class AlrDetector 
{

 public:
  AlrDetector();
  ~AlrDetector();

  void OnBytesSent(size_t bytes_sent, HPR_INT64 delta_time_ms);

  void SetEstimatedBitrate(int bitrate_bps);

  HPR_INT64 GetApplicationLimitedRegionStartTime() const;

  void UpdateBudgetWithElapsedTime(HPR_INT64 delta_time_ms);
  void UpdateBudgetWithBytesSent(size_t bytes_sent);

 private:
  int bandwidth_usage_percent_;
  int alr_start_budget_level_percent_;
  int alr_stop_budget_level_percent_;

  IntervalBudget alr_budget_;
  HPR_INT64 alr_started_time_ms_;
};


#endif 
