#ifndef _WIN_TIME_H_
#define _WIN_TIME_H_

#include "TPR_Time.h"


void TPR_FileTimeToTime_Inter(PFILETIME pFileTime, TPR_TIME_T* pTprTime);
void TPR_TimeToFileTime(TPR_TIME_T iTprTime, PFILETIME pFileTime);
void TPR_SystemTimeToExpTime(LPSYSTEMTIME pSysTime, TPR_TIME_EXP_T* pExpTime);

#endif
