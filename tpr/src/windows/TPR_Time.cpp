
#include "interInclude.h"
#include "Win_Time.h"
#include <time.h>

static const char tpr_month_snames[12][4] =
{
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const char tpr_day_snames[7][4] =
{
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

//��FILETIME ת����TPR_TIME_T
void TPR_FileTimeToTime_Inter(FILETIME* pFileTime, TPR_TIME_T* pHprTime)
{
#if 0
	*pHprTime = ((Int64ShllMod32(pFileTime->dwHighDateTime,32) | pFileTime->dwLowDateTime))/10;
	*pHprTime -= HPR_DELTA_EPOCH_IN_USEC;
#else
	/* Convert FILETIME one 64 bit number so we can work with it. */
    *pHprTime = pFileTime->dwHighDateTime;
    *pHprTime = (*pHprTime) << 32;
    *pHprTime |= pFileTime->dwLowDateTime;
    *pHprTime /= 10;    /* Convert from 100 nano-sec periods to micro-seconds. */
    *pHprTime -= TPR_DELTA_EPOCH_IN_USEC;  /* Convert from Windows epoch to Unix epoch */
    return;
#endif
}

//��TPR_TIME_T ת����FILETIME
void TPR_TimeToFileTime_Inter(TPR_TIME_T iHprTime, FILETIME* pFileTime)
{
	TPR_INT64 ll;

    iHprTime += TPR_DELTA_EPOCH_IN_USEC;
    ll = (iHprTime) * 10;
    pFileTime->dwLowDateTime = (DWORD)ll;
    pFileTime->dwHighDateTime = (DWORD) (ll >> 32);
	
    return;
}

//��LPSYSTEMTIME ת����TPR_TIME_EXP_T
void TPR_SystemTimeToExpTime_Inter(LPSYSTEMTIME pSysTime, TPR_TIME_EXP_T* pExpTime)
{
	static const int dayoffset[12] =
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    /* Note; the caller is responsible for filling in detailed tm_usec,
     * tm_gmtoff and tm_isdst data when applicable.
     */
	pExpTime->tm_usec = pSysTime->wMilliseconds * 1000;
    pExpTime->tm_sec  = pSysTime->wSecond;
    pExpTime->tm_min  = pSysTime->wMinute;
    pExpTime->tm_hour = pSysTime->wHour;
    pExpTime->tm_mday = pSysTime->wDay;
    pExpTime->tm_mon  = pSysTime->wMonth - 1;
    pExpTime->tm_year = pSysTime->wYear - 1900;
    pExpTime->tm_wday = pSysTime->wDayOfWeek;
    pExpTime->tm_yday = dayoffset[pExpTime->tm_mon] + (pSysTime->wDay - 1);
    pExpTime->tm_isdst = 0;
    pExpTime->tm_gmtoff = 0;

    /* If this is a leap year, and we're past the 28th of Feb. (the
     * 58th day after Jan. 1), we'll increment our tm_yday by one.
     */
    if (IsLeapYear(pSysTime->wYear) && (pExpTime->tm_yday > 58))
	{
        pExpTime->tm_yday++;
	}
}

//���ش�1970��1��1�� �����ڵ�ʱ�䣬 ��λus
TPR_DECLARE TPR_TIME_T CALLBACK TPR_TimeNow()
{
	TPR_TIME_T nTime = 0;
	FILETIME fileTime;

#ifndef _WIN32_WCE
	GetSystemTimeAsFileTime(&fileTime);
#else
	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);
	//SystemHPR_TimeToFileTime_Inter(&sysTime,&fileTime);
	SystemTimeToFileTime(&sysTime, &fileTime);
#endif

	TPR_FileTimeToTime_Inter(&fileTime, &nTime);
	return nTime;
}

//���شӲ���ϵͳ�����������ĺ���
TPR_DECLARE TPR_UINT32 CALLBACK TPR_GetTimeTick()
{
	return GetTickCount();
}

//���شӲ���ϵͳ�����������ĺ���
TPR_DECLARE TPR_UINT64 CALLBACK TPR_GetTimeTick64()
{
	//TBD
	return GetTickCount();
}

//���׼1970��ʱ�䣨��λs���໥ת��
TPR_DECLARE TPR_TIME_T CALLBACK TPR_TimeFromAnsiTime(time_t tAnsiTime)
{
	TPR_TIME_T tTime = tAnsiTime;
	tTime *= TPR_USEC_PER_SEC;
	return tTime;
}

//���׼1970��ʱ�䣨��λs���໥ת��
TPR_DECLARE time_t CALLBACK TPR_AnsiTimeFromTime(TPR_TIME_T iTime)
{
	return (time_t)iTime/TPR_USEC_PER_SEC;
}

//��TPR_TIME_T ʱ��ת��Ϊ TPR_TIME_EXP_Tʱ�䣬 iOff��ʱ��ƫ�ƣ���λs��
TPR_DECLARE TPR_INT32 CALLBACK TPR_ExpTimeFromTime(TPR_TIME_T iTime, TPR_INT32 iOff, TPR_TIME_EXP_T* pExpTime)
{
	FILETIME fileTime;
    SYSTEMTIME sysTime;
    TPR_TimeToFileTime_Inter(iTime + (iOff *  TPR_USEC_PER_SEC), &fileTime);
    FileTimeToSystemTime(&fileTime, &sysTime);
    /* The Platform SDK documents that SYSTEMTIME/FILETIME are
     * generally UTC, so we will simply note the offs used.
     */
    TPR_SystemTimeToExpTime_Inter(&sysTime, pExpTime);
    pExpTime->tm_usec = TPR_INT32 (iTime % TPR_USEC_PER_SEC);
    pExpTime->tm_gmtoff = iOff;

	return TPR_OK;
}

//��TPR_TIME_T ʱ��ת��Ϊ TPR_TIME_EXP_Tʱ�䣬 iOff��0 ��GMTʱ�䣨�׶�ʱ�䣩
TPR_DECLARE TPR_INT32 CALLBACK TPR_ExpTimeFromTimeGMT(TPR_TIME_T iTime, TPR_TIME_EXP_T* pExpTime)
{
	return TPR_ExpTimeFromTime(iTime, 0, pExpTime);
}

//��TPR_TIME_T ʱ��ת��Ϊ TPR_TIME_EXP_Tʱ�䣨����ʱ�䣩��������ʱ����Ϣ
TPR_DECLARE TPR_INT32 CALLBACK TPR_ExpTimeFromTimeLocal(TPR_TIME_T iTime, TPR_TIME_EXP_T* pExpTime)
{
	SYSTEMTIME st;
    FILETIME ft, localft;

    TPR_TimeToFileTime_Inter(iTime, &ft);

	TIME_ZONE_INFORMATION tz;
	/* XXX: This code is simply *wrong*.  The time converted will always
         * map to the *now current* status of daylight savings time.
         */

    FileTimeToLocalFileTime(&ft, &localft);
    FileTimeToSystemTime(&localft, &st);
    TPR_SystemTimeToExpTime_Inter(&st, pExpTime);
    pExpTime->tm_usec = (TPR_INT32) (iTime % TPR_USEC_PER_SEC);

    switch (GetTimeZoneInformation(&tz)) 
	{
        case TIME_ZONE_ID_UNKNOWN:
            pExpTime->tm_isdst = 0;
            /* Bias = UTC - local time in minutes
                * tm_gmtoff is seconds east of UTC
                */
            pExpTime->tm_gmtoff = tz.Bias * -60;
            break;
        case TIME_ZONE_ID_STANDARD:
            pExpTime->tm_isdst = 0;
            pExpTime->tm_gmtoff = (tz.Bias + tz.StandardBias) * -60;
            break;
        case TIME_ZONE_ID_DAYLIGHT:
            pExpTime->tm_isdst = 1;
            pExpTime->tm_gmtoff = (tz.Bias + tz.DaylightBias) * -60;
            break;
        default:
            /* noop */;
    }

	return TPR_OK;
}

//��TPR_TIME_EXP_T  ʱ��ת��Ϊ TPR_TIME_T ʱ�䣬������ʱ��
TPR_DECLARE TPR_INT32 CALLBACK TPR_TimeFromExpTime(TPR_TIME_EXP_T* pExpTime, TPR_TIME_T* pTime)
{
	TPR_TIME_T nYear = pExpTime->tm_year;
    TPR_TIME_T nDays;
    static const int dayoffset[12] =
    {306, 337, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275};

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

//��TPR_TIME_EXP_T  ʱ��ת��ΪGMT���׶�ʱ�䣩 0ʱ���� TPR_TIME_T ʱ��
TPR_DECLARE TPR_INT32 CALLBACK TPR_TimeFromExpTimeGMT(TPR_TIME_EXP_T* pExpTime, TPR_TIME_T* pTime)
{
	TPR_INT32 iRet = TPR_TimeFromExpTime(pExpTime, pTime);
    if (iRet == TPR_OK)
	{
        *pTime -= (TPR_TIME_T) pExpTime->tm_gmtoff * TPR_USEC_PER_SEC;
	}

	return TPR_OK;
}

//��TPR_TIME_T ��ʽ���ַ������
TPR_DECLARE TPR_INT32 CALLBACK TPR_CTime(char* pDateStr, TPR_TIME_T nTime)
{
	TPR_TIME_EXP_T xt;
    const char *s;
    int real_year;

    /* example: "Wed Jun 30 21:49:08 1993" */
    /*           123456789012345678901234  */

    TPR_ExpTimeFromTimeLocal(nTime, &xt);

    s = &tpr_day_snames[xt.tm_wday][0];
    *pDateStr++ = *s++;
    *pDateStr++ = *s++;
    *pDateStr++ = *s++;
    *pDateStr++ = ' ';
    s = &tpr_month_snames[xt.tm_mon][0];
    *pDateStr++ = *s++;
    *pDateStr++ = *s++;
    *pDateStr++ = *s++;
    *pDateStr++ = ' ';
    *pDateStr++ = (char)(xt.tm_mday / 10 + '0');
    *pDateStr++ = (char)(xt.tm_mday % 10 + '0');
    *pDateStr++ = ' ';
    *pDateStr++ = (char)(xt.tm_hour / 10 + '0');
    *pDateStr++ = (char)(xt.tm_hour % 10 + '0');
    *pDateStr++ = ':';
    *pDateStr++ = (char)(xt.tm_min / 10 + '0');
    *pDateStr++ = (char)(xt.tm_min % 10 + '0');
    *pDateStr++ = ':';
    *pDateStr++ = (char)(xt.tm_sec / 10 + '0');
    *pDateStr++ = (char)(xt.tm_sec % 10 + '0');
    *pDateStr++ = ' ';
    real_year = 1900 + xt.tm_year;
    *pDateStr++ = (char)(real_year / 1000 + '0');
    *pDateStr++ = (char)(real_year % 1000 / 100 + '0');
    *pDateStr++ = (char)(real_year % 100 / 10 + '0');
    *pDateStr++ = real_year % 10 + '0';
    *pDateStr++ = 0;

	return TPR_OK;
}

int win32_strftime_extra(char *s, size_t max, const char *format,\
                         const struct tm *tm) 
{
   /* If the new format string is bigger than max, the result string won't fit
    * anyway. If format strings are added, made sure the padding below is
    * enough */
    char *new_format = (char *) malloc(max + 11);
    size_t i;
	size_t j; 
	size_t format_length = strlen(format);
    int return_value;
    int length_written;

    for (i=0, j=0; (i < format_length && j < max);) 
	{
        if (format[i] != '%') 
		{
            new_format[j++] = format[i++];
            continue;
        }
        switch (format[i+1]) 
		{
            case 'C':
                length_written = _snprintf(new_format + j, max - j, "%2d",
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
                length_written = _snprintf(new_format + j, max - j, "%2d",
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
#if defined _WIN32_WCE
		//WinCEû��strftime�������
		return 0;
#else
        return_value = (int)strftime(s, max, new_format, tm);
#endif
    }
    free(new_format);
    return return_value;
}

//��TPR_TIME_EXP_Tʱ���ʽ�����
TPR_DECLARE TPR_INT32 CALLBACK TPR_StrFmtTime(char* pDst, TPR_INT32* iDstLen,TPR_INT32 iMaxDstSize, const char* pFmt, TPR_TIME_EXP_T* pTimeExp)
{
#if defined _WIN32_WCE
	//WinCEû��strftime����������˺���������ʱ�޷�ʵ�֣��ʲ�֧��
	return HPR_NOSUPPORT;
#else
	struct tm ttm;
    memset(&ttm, 0, sizeof(ttm));

    ttm.tm_sec  = pTimeExp->tm_sec;
    ttm.tm_min  = pTimeExp->tm_min;
    ttm.tm_hour = pTimeExp->tm_hour;
    ttm.tm_mday = pTimeExp->tm_mday;
    ttm.tm_mon  = pTimeExp->tm_mon;
    ttm.tm_year = pTimeExp->tm_year;
    ttm.tm_wday = pTimeExp->tm_wday;
    ttm.tm_yday = pTimeExp->tm_yday;
    ttm.tm_isdst = pTimeExp->tm_isdst;

    (*iDstLen) = win32_strftime_extra(pDst, iMaxDstSize, pFmt, &ttm);

	return TPR_OK;
#endif
}

