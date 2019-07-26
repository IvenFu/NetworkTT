/*
 * CopyRight:	HangZhou Hikvision System Technology Co., Ltd. All	Right Reserved.
 * FileName:	HPR_Time.h
 * Desc:		OS Layer Component.
 * Author:		schina
 * Date:		2009-03-02
 * Contact:		zhaoyiji@hikvision.com.cn
 * History:		Created By Zhaoyiji 2009-03-02
 * */

/*! 
	\file HPR_Time.h
	\brief HPR Time Manager.

	Details.
*/

#ifndef __HPR_TIME_H__
#define __HPR_TIME_H__

#include "HPR_Config.h"
#include "HPR_Types.h"

/*! \def OS_WINDOWS
 *
 */

#if defined(OS_WINDOWS)
#if !defined OS_WINCE
#include <sys/timeb.h>
#endif
typedef SYSTEMTIME HPR_OS_EXP_TIME_T;
typedef FILETIME HPR_OS_TIME_T;
#elif defined(OS_POSIX)
#else
	#error OS Not Implement Yet.
#endif

typedef HPR_INT64 HPR_TIME_T; //(usec)

/**
 * struct HPR_TIME_EXP_T
 */
typedef struct HPR_TIME_EXP_S
{
	HPR_INT32 tm_usec;	/**<	micorseconds past tm_sec	*/
	HPR_INT32 tm_sec;	/**<	(0-59)secs past tm_min		*/
	HPR_INT32 tm_min;	/**<	(0-59)minutes past tm_hour	*/
	HPR_INT32 tm_hour;	/**<	(0-23)hours past midnight	*/
	HPR_INT32 tm_mday;	/**<	(0-31)day of month			*/
	HPR_INT32 tm_mon;	/**<	(0-11)month of year			*/
	HPR_INT32 tm_year;	/**<	year since 1900				*/
	HPR_INT32 tm_wday;	/**<	(0-6)days since sunday		*/
	HPR_INT32 tm_yday;	/**<	(0-365)days since Jan 1		*/
	HPR_INT32 tm_isdst; /**<	daylight saving time		*/
	HPR_INT32 tm_gmtoff;/**<	seconds east of UTC			*/
}HPR_TIME_EXP_T;

/**
 * HPR_TimeNow get now time
 * @return number of microsecond since January 1, 1970.
 * @sa HPR_GetTimeTick
 */
HPR_DECLARE HPR_TIME_T CALLBACK HPR_TimeNow();

/**
 * HPR_GetTimeTick get now time
 * @return number of milliseconds since system start. cpu time.
 * @sa HPR_TimeNow
 */
HPR_DECLARE HPR_UINT32 CALLBACK HPR_GetTimeTick();
HPR_DECLARE HPR_UINT64 CALLBACK HPR_GetTimeTick64();

/**
 * HPR_TimeFromAnsiTime convert time_t time to HPR_TIME_T time
 * @param tAnsiTime (IN) ansi time (may get by time())
 * @return HPR_TIME_T time
 * @sa HPR_TimeNow(),HPR_AnsiTimeFromTime().
 */
HPR_DECLARE HPR_TIME_T CALLBACK HPR_TimeFromAnsiTime(HPR_TIME_T tAnsiTime);

/**
 * HPR_AnsiTimeFromTime convert HPR_TIME_T time to time_t time
 * @param tAnsiTime (IN) ansi time (may get by time())
 * @return time_t time (from 1970.1.1)
 * @sa HPR_TimeFromAnsiTime().
 */
HPR_DECLARE HPR_TIME_T CALLBACK HPR_AnsiTimeFromTime(HPR_TIME_T iTime);

/**
 * HPR_TimeFromExpTime convert HPR_TIME_EXP_T time to HPR_TIME_T time
 * @param pExpTime (IN) HPR_TIME_EXP_T format time
 * @param pTime (OUT) interger time.
 * @return 0 success, -1 fail.
 * @sa HPR_TimeFromExpTimeGMT
 */
HPR_DECLARE HPR_INT32 CALLBACK HPR_TimeFromExpTime(HPR_TIME_EXP_T* pExpTime, HPR_TIME_T* pTime);

/**
 * HPR_TimeFromExpTimeGMT convert HPR_TIME_EXP_T time to HPR_TIME_T time
 * @param pExpTime (IN) HPR_TIME_EXP_T format time
 * @param pTime (OUT) interger time.
 * @return 0 success, -1 fail.
 * @sa HPR_TimeFromExpTime
 */
HPR_DECLARE HPR_INT32 CALLBACK HPR_TimeFromExpTimeGMT(HPR_TIME_EXP_T* pExpTime, HPR_TIME_T* pTime);

/**
 * HPR_ExpTimeFromTime convert HPR_TIME_T time to HPR_TIME_EXP_T
 * @param nTime (IN) interger time
 * @param iOff timezone (IN) from gmt time, in sec
 * @param pExpTime (OUT) struct HPR_TIME_EXP_T time
 * @return 0 success, -1 fail.
 * @sa HPR_TimeFromExpTimeGMT(),HPR_ExpTimeFromTimeLocal() 
 */
HPR_DECLARE HPR_INT32 CALLBACK HPR_ExpTimeFromTime(HPR_TIME_T nTime, HPR_INT32 iOff, HPR_TIME_EXP_T* pExpTime);

/**
 * HPR_ExpTimeFromTimeGMT convert HPR_TIME_T time to HPR_TIME_EXP_T time
 * @param iTime (IN) interger time
 * @param pExpTime (OUT) struct HPR_TIME_EXP_T time
 * @return 0 success, -1 fail.
 * @see HPR_TIME_EXP_T
 * @sa HPR_ExpTimeFromTimeLocal
 */
HPR_DECLARE HPR_INT32 CALLBACK HPR_ExpTimeFromTimeGMT(HPR_TIME_T iTime, HPR_TIME_EXP_T* pExpTime);

/**
 * HPR_ExpTimeFromTimeLocal convert HPR_TIME_T time to HPR_TIME_EXP_T time
 * @param iTime (IN) interger time
 * @param pExpTime (OUT) struct HPR_TIME_EXP_T time
 * @return 0 success, -1 fail.
 * @see HPR_TIME_EXP_T
 * @sa HPR_ExpTimeFromTimeGMT
 */
HPR_DECLARE HPR_INT32 CALLBACK HPR_ExpTimeFromTimeLocal(HPR_TIME_T iTime, HPR_TIME_EXP_T* pExpTime);

/**
 * HPR_CTime convert HPR_TIME_T to formated string time.
 * @param pDateStr (OUT) data buffer to store formated time.
 * @param nTime (IN) interger time format.
 * @return 0 success, -1 fail.
 * @see HPR_TIME_T
 * @sa HPR_StrFmtTime
 */
HPR_DECLARE HPR_INT32 CALLBACK HPR_CTime(char* pDateStr, HPR_TIME_T nTime);

/**
 * HPR_StrFmtTime
 * @param pDst (OUT dest data buffer to store formated time.
 * @param iDstLen (OUT) formated time length
 * @param iMaxDstSize (IN) dest data buffer length.
 * @param pFmt (IN) time format
 * @param pTimeExp (IN) struct HPR_TIME_EXP_T formated time.
 * @return 0 success, -1 fail.
 * @sa HPR_CTime()
 */
HPR_DECLARE HPR_INT32 CALLBACK HPR_StrFmtTime(char* pDst, HPR_INT32* iDstLen, HPR_INT32 iMaxDstSize, const char* pFmt, HPR_TIME_EXP_T* pTimeExp);

//////////////////////////////////////////////////////////////////////////
//add by zhaoyiji 2011-10-20
#include <time.h>

#define HPR_US_PER_SEC (1000000)
#define HPR_MS_PER_SEC (1000)
#define HPR_US_PER_MS (1000)
#define HPR_NS_PER_US (1000)
#define HPR_NS_PER_MS (1000000)
#define HPR_NS_PER_SEC (1000000000)

namespace hpr
{
	class hpr_time;

	class HPR_DECLARE_CLASS hpr_time_tick
	{
		friend HPR_DECLARE_CLASS bool operator==(const hpr_time_tick&, const hpr_time_tick&);
		friend HPR_DECLARE_CLASS bool operator>(const hpr_time_tick&, const hpr_time_tick&);
		friend HPR_DECLARE_CLASS bool operator<(const hpr_time_tick&, const hpr_time_tick&);

	public:
		hpr_time_tick();
		hpr_time_tick(hpr_int64 t);
		hpr_time_tick(const hpr_time_tick&);

		virtual ~hpr_time_tick();

		static hpr_int64 timeval2ms(struct timeval* tv);
		static struct timeval* ms2timeval(hpr_int64 t, struct timeval* tv);
		static hpr_int64 timeval2us(struct timeval* tv);
		static struct timeval* us2timeval(hpr_int64 t, struct timeval* tv);
		static hpr_int64 us2ms(hpr_int64 t);
		static hpr_int64 us2s(hpr_int64 t);
		static hpr_int64 ms2s(hpr_int64 t);

	public:
		hpr_time_tick& operator=(const hpr_time_tick&);
		hpr_time_tick& operator+=(const hpr_time_tick&);
		hpr_time_tick& operator-=(const hpr_time_tick&);

		hpr_int64 now();//microsecond.
		hpr_int64 get_time();//get time initialized by now().
		hpr_int64 update_time();//update now time in microsecond.

	private:
		hpr_int64 m_nTime;//microsecond.
	};

	class HPR_DECLARE_CLASS hpr_time
	{
		friend HPR_DECLARE_CLASS bool operator==(const hpr_time&, const hpr_time&);
		friend HPR_DECLARE_CLASS bool operator!=(const hpr_time&, const hpr_time&);
		friend HPR_DECLARE_CLASS bool operator>(const hpr_time&, const hpr_time&);
		friend HPR_DECLARE_CLASS bool operator<(const hpr_time&, const hpr_time&);

	public:
		hpr_time();
		hpr_time(hpr_int64);
		hpr_time(const hpr_time&);
		virtual ~hpr_time();

		//static hpr_int32 get_timezone();

	public:
		hpr_time& operator=(const hpr_time&);
		hpr_time& operator+=(const hpr_time&);
		hpr_time& operator-=(const hpr_time&);

	public:
		hpr_int64 now();//second;
		hpr_int64 get_time();
		hpr_int64 update_time();

		hpr_int32 get_year();
		hpr_int32 get_mon();
		hpr_int32 get_day();
		hpr_int32 get_hour();
		hpr_int32 get_min();
		hpr_int32 get_sec();
		hpr_int32 get_week_day();
		hpr_int32 get_mon_day();
		hpr_int32 get_year_day();

	public:
		static struct tm* time2tm(hpr_int64 t, struct tm*);

	private:
		time_t m_tTime;//second;
		struct tm m_tmTime;//tm structure
	};

	HPR_DECLARE_CLASS bool operator==(const hpr_time&, const hpr_time&);
	HPR_DECLARE_CLASS bool operator!=(const hpr_time&, const hpr_time&);
	HPR_DECLARE_CLASS bool operator>(const hpr_time&, const hpr_time&);
	HPR_DECLARE_CLASS bool operator<(const hpr_time&, const hpr_time&);
		
};


#endif



