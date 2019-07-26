
#include "config.h"


int Config::DoConfig()
{
	int iTemp=0;
	int iRet;
	char buf[128]={0};
	printf("is This Server? Y/N:\n");

	iRet = scanf("%s",buf);

	if (buf[0] == 'Y' || buf[0] == 'y')
	{
		bClient = TPR_FALSE;

		//m_szServerzIp.assign(buf);
	}
	else
	{
		bClient = TPR_TRUE;
	}

	

	return 0;
}


FILE* g_DhLogFile = fopen("demo.log", "wb+");


int DhlogformatWarp(char* szLevel, const char* format, ...)
{
	int iPos = 0;
	char szTemp[256] = { 0 };
	TPR_TIME_EXP_T struTime;
	TPR_TIME_T nTime = TPR_TimeNow();


	if (!szLevel)
	{
		return 0;
	}

	if (1)
	{
		if (1)
		{
			//时间
			memset(&struTime, 0, sizeof(TPR_TIME_EXP_T));

			//UTC/GMT时间
			TPR_ExpTimeFromTime(nTime, 0, &struTime);

			iPos += TPR_Snprintf(szTemp + iPos, sizeof(szTemp) - iPos, "[%04d-%02d-%02d %04d.%03d]",
				struTime.tm_year + 1900, struTime.tm_mon + 1, struTime.tm_mday,
				60 * struTime.tm_min + struTime.tm_sec,
				struTime.tm_usec / 1000);

			//线程ID
			iPos += TPR_Snprintf(szTemp + iPos, sizeof(szTemp) - iPos, "[0x%08x]", TPR_Thread_GetSelfId());

			//日志类型
			iPos += TPR_Snprintf(szTemp + iPos, sizeof(szTemp) - iPos, "[%05s]", szLevel);
		}

		int iRet;

		va_list al;
		va_start(al, format);
		iRet = TPR_Vsnprintf(szTemp + iPos, sizeof(szTemp) - iPos, format, al);
		va_end(al);

		//被截取
		if (iRet != -1)
		{
			iPos += iRet;
			iPos += TPR_Snprintf(szTemp + iPos, sizeof(szTemp) - iPos, "\n");
		}
		else
		{
			szTemp[255] = '\n';
		}

		if (g_DhLogFile)
		{
			fprintf(g_DhLogFile, "%s", szTemp);
			fflush(g_DhLogFile);
			return 0;
		}

#if defined(N_OS_ANDROID)
		//__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s", szTemp);
		LOGD(szTemp);
#elif defined(N_OS_IOS)
		printf(szTemp);
#endif
	}
	return 0;
}

