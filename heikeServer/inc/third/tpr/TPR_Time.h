#ifndef _TPR_TIME_H_
#define _TPR_TIME_H_

#include "TPR_Config.h"
#include "TPR_Types.h"

#if defined(OS_WINDOWS)
#if !defined(OS_WINCE)
#include <sys/timeb.h>
#endif

typedef SYSTEMTIME TPR_OS_EXP_TIME_T;
typedef FILETIME TPR_OS_TIME_T;

#elif defined(OS_POSIX)
#else
#error OS Not Implement Yet.
#endif

typedef TPR_INT64 TPR_TIME_T; //(usec)

typedef struct TPR_TIME_EXP_S
{
	TPR_INT32 tm_usec; //
	TPR_INT32 tm_sec;  //(0-59)
	TPR_INT32 tm_min;  //(0-59)
	TPR_INT32 tm_hour; //(0-23)
	TPR_INT32 tm_mday; //(0-31)day of month
	TPR_INT32 tm_mon;  //(0-11)month of year
	TPR_INT32 tm_year; //year since 1900
	TPR_INT32 tm_wday; //(0-6) days since Sunday
	TPR_INT32 tm_yday; //(0-365)days since Jan 1
	TPR_INT32 tm_isdst; //daylight saving time
	TPR_INT32 tm_gmtoff; //seconds east of UTC
}TPR_TIME_EXP_T;




//返回从1970年1月1日 到现在的时间， 单位us
TPR_DECLARE TPR_TIME_T CALLBACK TPR_TimeNow();

//返回从操作系统启动所经过的毫秒
TPR_DECLARE TPR_UINT32 CALLBACK TPR_GetTimeTick();

//返回从操作系统启动所经过的毫秒
TPR_DECLARE TPR_UINT64 CALLBACK TPR_GetTimeTick64();

//与标准1970年时间（单位s）相互转化
TPR_DECLARE TPR_TIME_T CALLBACK TPR_TimeFromAnsiTime(time_t tAnsiTime);

//与标准1970年时间（单位s）相互转化
TPR_DECLARE time_t CALLBACK TPR_AnsiTimeFromTime(TPR_TIME_T iTime);

//将TPR_TIME_T 时间转化为 TPR_TIME_EXP_T时间， iOff是时区偏移（单位s）
TPR_DECLARE TPR_INT32 CALLBACK TPR_ExpTimeFromTime(TPR_TIME_T iTime, TPR_INT32 iOff, TPR_TIME_EXP_T* pExpTime);

//将TPR_TIME_T 时间转化为 TPR_TIME_EXP_T时间， iOff是0 ，GMT时间（伦敦时间）
TPR_DECLARE TPR_INT32 CALLBACK TPR_ExpTimeFromTimeGMT(TPR_TIME_T iTime, TPR_TIME_EXP_T* pExpTime);

//将TPR_TIME_T 时间转化为 TPR_TIME_EXP_T时间（本地时间），并保存时区信息
TPR_DECLARE TPR_INT32 CALLBACK TPR_ExpTimeFromTimeLocal(TPR_TIME_T iTime, TPR_TIME_EXP_T* pExpTime);

//将TPR_TIME_EXP_T  时间转化为 TPR_TIME_T 时间，不考虑时区
TPR_DECLARE TPR_INT32 CALLBACK TPR_TimeFromExpTime(TPR_TIME_EXP_T* pExpTime, TPR_TIME_T* pTime);

//将TPR_TIME_EXP_T  时间转化为GMT（伦敦时间） 0时区的 TPR_TIME_T 时间
TPR_DECLARE TPR_INT32 CALLBACK TPR_TimeFromExpTimeGMT(TPR_TIME_EXP_T* pExpTime, TPR_TIME_T* pTime);

//将TPR_TIME_T 格式化字符串输出
TPR_DECLARE TPR_INT32 CALLBACK TPR_CTime(char* pDateStr, TPR_TIME_T nTime);

//将TPR_TIME_EXP_T时间格式化输出
TPR_DECLARE TPR_INT32 CALLBACK TPR_StrFmtTime(char* pDst, TPR_INT32* iDstLen,TPR_INT32 iMaxDstSize, const char* pFmt, TPR_TIME_EXP_T* pTimeExp);


#endif
