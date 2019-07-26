#include "TPR_Time.h"
#include "interInclude.h"

#include <sys/time.h>
#include <time.h>

static const char hpr_month_snames[12][4] =
{
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const char hpr_day_snames[7][4] =
{
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

TPR_DECLARE TPR_TIME_T TPR_TimeNow()
{
	timeval tv;
	gettimeofday(&tv, NULL);
	
	//fprintf(stderr, "gettimeofday %d : %d\n", tv.tv_sec, tv.tv_usec);

	TPR_TIME_T tTmp = tv.tv_sec;
	tTmp *= TPR_USEC_PER_SEC;
	tTmp += tv.tv_usec;

	return tTmp;

	
	//return tv.tv_sec*HPR_USEC_PER_SEC + tv.tv_usec;
}

TPR_DECLARE TPR_UINT32 TPR_GetTimeTick()
{
#if defined(OS_APPLE)
	struct timeval tv;
	memset(&tv, 0, sizeof(tv));
	gettimeofday(&tv, NULL);
	return (HPR_UINT32)((tv.tv_sec * 1000) + (tv.tv_usec/1000));
#else
    struct timespec spec;
    memset(&spec, 0x0, sizeof(struct timespec));
    clock_gettime(CLOCK_MONOTONIC, &spec);
    return (TPR_UINT32)((spec.tv_sec * 1000) + (spec.tv_nsec + 1000000 / 2) / 1000000);
#endif
}

//返回从操作系统启动所经过的毫秒
TPR_DECLARE TPR_UINT64 CALLBACK TPR_GetTimeTick64()
{
	//TBD
	return (TPR_UINT64)TPR_GetTimeTick();
}

TPR_DECLARE TPR_UINT32 CALLBACK TPR_GetTimeTickSecs()
{
#if defined(OS_APPLE)
    struct timeval tv;
    memset(&tv, 0, sizeof(tv));
    gettimeofday(&tv, NULL);
    return (HPR_UINT32)(tv.tv_sec);
#else
    struct timespec spec;
    memset(&spec, 0x0, sizeof(struct timespec));
    clock_gettime(CLOCK_MONOTONIC, &spec);
    return spec.tv_sec;
#endif
}


TPR_DECLARE TPR_TIME_T TPR_TimeFromAnsiTime(time_t tAnsiTime)
{
	TPR_TIME_T tTime = tAnsiTime;
	tTime *= TPR_USEC_PER_SEC;
	return tTime;
}

TPR_DECLARE time_t CALLBACK TPR_AnsiTimeFromTime(TPR_TIME_T iTime)
{
	return (time_t)(iTime/TPR_USEC_PER_SEC);
}

TPR_DECLARE TPR_INT32 TPR_ExpTimeFromTime(TPR_TIME_T iTime, TPR_INT32 iOff, TPR_TIME_EXP_T* pExpTime)
{
	struct tm t;
	time_t tt = iTime/TPR_USEC_PER_SEC + iOff;

	if ( !pExpTime )
	{
		return TPR_ERROR;
	}

	t = *gmtime(&tt);
	pExpTime->tm_sec = t.tm_sec;
	pExpTime->tm_min = t.tm_min;
	pExpTime->tm_hour = t.tm_hour;
	pExpTime->tm_mday = t.tm_mday;
	pExpTime->tm_mon = t.tm_mon;
	pExpTime->tm_year = t.tm_year;
	pExpTime->tm_wday = t.tm_wday;
	pExpTime->tm_yday = t.tm_yday;
	pExpTime->tm_isdst = t.tm_isdst;
    pExpTime->tm_usec = TPR_INT32 (iTime % TPR_USEC_PER_SEC);
    pExpTime->tm_gmtoff = iOff;

	return TPR_OK;
}

TPR_DECLARE TPR_INT32 TPR_ExpTimeFromTimeGMT(TPR_TIME_T iTime, TPR_TIME_EXP_T* pExpTime)
{
	return TPR_ExpTimeFromTime(iTime, 0, pExpTime);
}

TPR_DECLARE TPR_INT32 TPR_ExpTimeFromTimeLocal(TPR_TIME_T iTime, TPR_TIME_EXP_T* pExpTime)
{
	struct tm t;
	time_t tt = iTime/TPR_USEC_PER_SEC;

	if (!pExpTime)
	{
		return TPR_ERROR;
	}

	t = *localtime (&tt);
	pExpTime->tm_sec = t.tm_sec;
	pExpTime->tm_min = t.tm_min;
	pExpTime->tm_hour = t.tm_hour;
	pExpTime->tm_mday = t.tm_mday;
	pExpTime->tm_mon = t.tm_mon;
	pExpTime->tm_year = t.tm_year;
	pExpTime->tm_wday = t.tm_wday;
	pExpTime->tm_yday = t.tm_yday;
	pExpTime->tm_isdst = t.tm_isdst;
    pExpTime->tm_usec = TPR_INT32 (iTime % TPR_USEC_PER_SEC);
    pExpTime->tm_gmtoff = 0;

	return TPR_OK;
}

TPR_DECLARE TPR_INT32 TPR_TimeFromExpTime(TPR_TIME_EXP_T* pExpTime, TPR_TIME_T* pTime)
{
	TPR_TIME_T nYear = pExpTime->tm_year;
    TPR_TIME_T nDays;
    static const int dayoffset[12] =
    {306, 337, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275};

	if (!pExpTime || !pTime)
	{
		return TPR_ERROR;
	}

    /* shift new year to 1st March in order to make leap year calc easy */

    if (pExpTime->tm_mon < 2)
	{
        nYear--;
	}

    /* Find number of days since 1st March 1900 (in the Gregorian calendar). */

    nDays = nYear * 365 + nYear / 4 - nYear / 100 + (nYear / 100 + 3) / 4;
    nDays += dayoffset[pExpTime->tm_mon] + pExpTime->tm_mday - 1;
    nDays -= 25508;              /* 1 jan 1970 is 25508 days since 1 mar 1900 */

    nDays = ((nDays * 24 + pExpTime->tm_hour) * 60 + pExpTime->tm_min) * 60 + pExpTime->tm_sec;

    if (nDays < 0) 
	{
        return TPR_ERROR;
    }

    *pTime = nDays * TPR_USEC_PER_SEC + pExpTime->tm_usec;

    return TPR_OK;
}

TPR_DECLARE TPR_INT32 TPR_TimeFromExpTimeGMT(TPR_TIME_EXP_T* pExpTime, TPR_TIME_T* pTime)
{
	if (!pExpTime || !pTime)
	{
		return TPR_ERROR;
	}

	TPR_INT32 iRet = TPR_TimeFromExpTime(pExpTime, pTime);
    if ( TPR_OK == iRet )
	{
        *pTime -= (TPR_TIME_T) pExpTime->tm_gmtoff * TPR_USEC_PER_SEC;
	}

	return TPR_OK;
}

TPR_DECLARE TPR_INT32 TPR_CTime(char* pDateStr, TPR_TIME_T nTime)
{
	TPR_TIME_EXP_T xt;
    const char *s;
    int real_year;

	if ( !pDateStr )
	{
		return TPR_ERROR;
	}

    /* example: "Wed Jun 30 21:49:08 1993" */
    /*           123456789012345678901234  */

    TPR_ExpTimeFromTimeLocal(nTime, &xt);

    s = &hpr_day_snames[xt.tm_wday][0];
    *pDateStr++ = *s++;
    *pDateStr++ = *s++;
    *pDateStr++ = *s++;
    *pDateStr++ = ' ';
    s = &hpr_month_snames[xt.tm_mon][0];
    *pDateStr++ = *s++;
    *pDateStr++ = *s++;
    *pDateStr++ = *s++;
    *pDateStr++ = ' ';
    *pDateStr++ = xt.tm_mday / 10 + '0';
    *pDateStr++ = xt.tm_mday % 10 + '0';
    *pDateStr++ = ' ';
    *pDateStr++ = xt.tm_hour / 10 + '0';
    *pDateStr++ = xt.tm_hour % 10 + '0';
    *pDateStr++ = ':';
    *pDateStr++ = xt.tm_min / 10 + '0';
    *pDateStr++ = xt.tm_min % 10 + '0';
    *pDateStr++ = ':';
    *pDateStr++ = xt.tm_sec / 10 + '0';
    *pDateStr++ = xt.tm_sec % 10 + '0';
    *pDateStr++ = ' ';
    real_year = 1900 + xt.tm_year;
    *pDateStr++ = real_year / 1000 + '0';
    *pDateStr++ = real_year % 1000 / 100 + '0';
    *pDateStr++ = real_year % 100 / 10 + '0';
    *pDateStr++ = real_year % 10 + '0';
    *pDateStr++ = 0;

	return TPR_OK;
}

int win32_strftime_extra(char *s, size_t max, const char *format,
                         const struct tm *tm) 
{
   /* If the new format string is bigger than max, the result string won't fit
    * anyway. If format strings are added, made sure the padding below is
    * enough */
    char *new_format = (char *) malloc(max + 11);
    size_t i, j, format_length = strlen(format);
    int return_value;
    int length_written;

    for (i = 0, j = 0; (i < format_length && j < max);) 
	{
        if (format[i] != '%') 
		{
            new_format[j++] = format[i++];
            continue;
        }
        switch (format[i+1]) 
		{
            case 'C':
                length_written = snprintf(new_format + j, max - j, "%2d",
                    (tm->tm_year + 1970)/100);
                j = (length_written == -1) ? max : (j + length_written);
                i += 2;
                break;
            case 'D':
                /* Is this locale dependent? Shouldn't be...
                   Also note the year 2000 exposure here */
                memcpy(new_format + j, "%m/%d/%y", 8);
                i += 2;
                j += 8;
                break;
            case 'r':
                memcpy(new_format + j, "%I:%M:%S %p", 11);
                i += 2;
                j += 11;
                break;
            case 'R':
                memcpy(new_format + j, "%H:%M", 5);
                i += 2;
                j += 5;
                break;
            case 'T':
                memcpy(new_format + j, "%H:%M:%S", 8);
                i += 2;
                j += 8;
                break;
            case 'e':
                length_written = snprintf(new_format + j, max - j, "%2d",
                    tm->tm_mday);
                j = (length_written == -1) ? max : (j + length_written);
                i += 2;
                break;
            default:
                /* We know we can advance two characters forward here. Also
                 * makes sure that %% is preserved. */
                new_format[j++] = format[i++];
                new_format[j++] = format[i++];
        }
    }
    if (j >= max) 
	{
        *s = '\0';  /* Defensive programming, okay since output is undefined*/
        return_value = 0;
    } 
	else 
	{
        new_format[j] = '\0';
        return_value = (int)strftime(s, max, new_format, tm);
    }
    free(new_format);
    return return_value;
}

TPR_DECLARE TPR_INT32 TPR_StrFmtTime(char* pDst, TPR_INT32* iDstLen, TPR_INT32 iMaxDstSize, const char* pFmt, TPR_TIME_EXP_T* pTimeExp)
{
	struct tm tm;

	if (!pDst || !iDstLen)
	{
		return TPR_ERROR;
	}

    memset(&tm, 0, sizeof tm);

    tm.tm_sec  = pTimeExp->tm_sec;
    tm.tm_min  = pTimeExp->tm_min;
    tm.tm_hour = pTimeExp->tm_hour;
    tm.tm_mday = pTimeExp->tm_mday;
    tm.tm_mon  = pTimeExp->tm_mon;
    tm.tm_year = pTimeExp->tm_year;
    tm.tm_wday = pTimeExp->tm_wday;
    tm.tm_yday = pTimeExp->tm_yday;
    tm.tm_isdst = pTimeExp->tm_isdst;

    (*iDstLen) = win32_strftime_extra(pDst, iMaxDstSize, pFmt, &tm);
   
	return TPR_OK;
}
