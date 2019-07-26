#ifndef _RTPTONTP_UTIL_H_
#define _RTPTONTP_UTIL_H_
#include "Util.h"

int CheckForWrapArounds(TPR_UINT32 nNewTimestamp, TPR_UINT32 nOldTimestamp);
bool CompensateForWrapAround(TPR_UINT32 new_timestamp, TPR_UINT32 old_timestamp, TPR_INT64* compensated_timestamp);
//��RTP/NTPʱ������м���RTPʱ���Ƶ��
bool CalculateFrequency(TPR_INT64 rtcp_ntp_ms1, TPR_UINT32 rtp_timestamp1, TPR_INT64 rtcp_ntp_ms2, TPR_UINT32 rtp_timestamp2, double* frequency_khz); 

#endif