#ifndef _INTER_INCLUDE_H_
#define _INTER_INCLUDE_H_

#include "TPR_Config.h"
#include "TPR_Types.h"

#define TPR_USEC_PER_SEC 1000000
#define TPR_MSEC_PER_SEC 1000
#define TPR_USEC_PER_MSEC 1000

#define TPR_DELTA_EPOCH_IN_USEC  TPR_TIME_T(11644473600000000)

//TBD
#define TPR_TIME_SEC(time) ((time)/TPR_USEC_PER_SEC)

//≈–∂œ»ÚƒÍ
#define IsLeapYear(y) ((!((y)%4)) ? (((!((y)%400)) || ((y)%100)) ? 1: 0 ):0)

#if defined(OS_WINDOWS)
#define SetLastError(TPR_ERRNO) SetLastError(TPR_ERRNO)
#else
#define SetLastError(TPR_ERRNO) (errno = (TPR_ERRNO))
#endif
#endif

