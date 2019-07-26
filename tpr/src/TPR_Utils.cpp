
#include "TPR_Utils.h"
#include "TPR_Time.h"
#include <stdarg.h>



#if defined (OS_WINDOWS)
//#include <Rpcdce.h>
#elif defined(__APPLE__)
	#include <sys/sysctl.h>
#endif


TPR_DECLARE TPR_VOID CALLBACK TPR_Debug(const char* format, ...)
{
#if defined(_DEBUG)
	char szDebug[5*1024] = {0};

	va_list al;
	va_start(al, format);

	int retval = TPR_Vsnprintf(szDebug, sizeof(szDebug), format, al);

#if defined (OS_WINDOWS)
	#if defined _WIN32_WCE
		wchar_t wDebug[5 * 1024] = {0};
		MultiByteToWideChar(CP_ACP, 0, szDebug, strlen(szDebug), wDebug, sizeof(wDebug));
		OutputDebugString(wDebug);
	#else
		OutputDebugString((LPCSTR)szDebug);
	#endif
#else
	szDebug[retval] = '\n';
	szDebug[retval + 1] = '\0';
	OutputDebugString(szDebug);
#endif

	va_end(al);
#endif
}

TPR_DECLARE TPR_VOID CALLBACK TPR_OutputDebug(const char* format, ...)
{
#if defined OS_WINCE
	char szDebug[1024] = {0};
#else
	char szDebug[5*1024] = {0};
#endif
	va_list al;

	va_start(al, format);
	
	int retval = TPR_Vsnprintf(szDebug, sizeof(szDebug), format, al);
#if defined (OS_WINDOWS)
	#if defined _WIN32_WCE
		wchar_t wDebug[1024] = {0};
		MultiByteToWideChar(CP_ACP, 0, szDebug, strlen(szDebug), wDebug, sizeof(wDebug));
		OutputDebugString(wDebug);
	#else
		OutputDebugString((LPCSTR)szDebug);
	#endif
#elif defined (OS_POSIX)
	szDebug[retval] = '\0';
    //OutputDebugString("%s\n", szDebug);
#endif
	
	va_end(al);
}

TPR_DECLARE TPR_VOID CALLBACK TPR_OutputDebugString(const char* lpOutputString)
{
#if defined OS_WINDOWS
	#if defined _WIN32_WCE
		int len = strlen(lpOutputString);
		//需要多分配一个wchar_t用于放置结束符
		wchar_t *tmp = (wchar_t *)calloc(len + sizeof(wchar_t), sizeof(wchar_t));
		if (tmp == NULL)
		{
			return;
		}
		MultiByteToWideChar(CP_ACP, 0, lpOutputString, len, tmp, len * sizeof(wchar_t));
		tmp[len] = 0;
		OutputDebugString(tmp);
		free(tmp);
	#else
		OutputDebugString(lpOutputString);
	#endif
#elif defined OS_POSIX
	//OutputDebugString("%s", lpOutputString);
#endif
}

/**
Function:	TPR_USleep
Desc:		sleep,让出cpu
Input:		nUSec~微秒为单位的时间;
Output:
Return:
*/
TPR_DECLARE TPR_VOID CALLBACK TPR_USleep(TPR_INT64 nUSec)
{
#if defined(OS_WINDOWS)
	Sleep((DWORD)nUSec/1000);
#else
    usleep(nUSec);
#endif
}

/**
Function:	TPR_Sleep
Desc:		sleep,让出cpu
Input:		millisecond~毫秒为单位的时间;
Output:
Return:
*/
TPR_DECLARE TPR_VOID CALLBACK TPR_Sleep(TPR_INT32 millisecond)
{
#if defined(OS_WINDOWS)
	Sleep(millisecond);
#else
    usleep(millisecond*1000);
#endif
}

/**
 * Function: TPR_Rand
 * Desc:	获取随机数,随机数种子由内部TPR_GetTimeTick()获取
 * Input:	
 * Output:
 * Return:	随机数
 */
TPR_DECLARE TPR_UINT32 CALLBACK TPR_Rand()
{
#if defined (OS_WINDOWS)
	srand(TPR_GetTimeTick());
	return rand();
#else
	srandom(TPR_GetTimeTick());
	return random();
#endif
}

/**
 * Function: TPR_ZeroMemory
 * Desc:	把内存清零调用 memset(mem, 0, len);
 * Input:	mem : 内存首指针
			len : 内存长度
 * Output:
 * Return:	
 */
TPR_DECLARE TPR_VOID CALLBACK TPR_ZeroMemory(void* mem, int len)
{
	memset(mem, 0, len);
	return;
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_GetCPUNumber()
{
#if defined (OS_WINDOWS)
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	return systemInfo.dwNumberOfProcessors;
#elif defined(__linux__)
    return sysconf(_SC_NPROCESSORS_CONF);
#elif defined(__APPLE__)
	int name[2] = {CTL_HW, HW_NCPU};
	int number = 0;
	size_t length = sizeof(number);
	if (sysctl(name, 2, &number, &length, NULL, 0) < 0)
	{
		return 1;
	}
	return number;
#endif
}

TPR_DECLARE TPR_INT32 CALLBACK TPR_GetUUID(TPR_UUID uuid)
{
	return 0;
#if 0
#if defined (OS_WINDOWS)
	#if defined _WIN32_WCE
		return CoCreateGuid((GUID*)uuid) == S_OK ? TPR_OK : TPR_ERROR;
	#else
		return UuidCreate((UUID*)uuid) == RPC_S_OK ? TPR_OK : TPR_ERROR;
	#endif
#else
    /* commented by fenggx begin */
    /*
    uuid_generate(uuid);
    */
    /* commented by fenggx end */
    return TPR_OK;
#endif
#endif
}

typedef struct
{
    TPR_UINT32  Data1;
    TPR_UINT16  Data2;
    TPR_UINT16  Data3;
    TPR_UINT8   Data4[8];
}TPR_GUID;

#define GUID_STRING_LEN (sizeof(TPR_GUID) * 2 + 4)
TPR_DECLARE TPR_INT32 CALLBACK TPR_StringFromUUID(TPR_UUID uuid, char* uuidString, TPR_INT32 len)
{
    if(len < (TPR_INT32)GUID_STRING_LEN)

    {
        return TPR_ERROR;        
    }

    TPR_GUID* guid = (TPR_GUID*)uuid;
    TPR_Snprintf(uuidString, len, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", 
                 guid->Data1, guid->Data2, guid->Data3,
                 guid->Data4[0], guid->Data4[1], guid->Data4[2],
                 guid->Data4[3], guid->Data4[4], guid->Data4[5],
                 guid->Data4[6], guid->Data4[7]);

    return TPR_OK;                 
}

/*
 * @fn TPR_Snprintf
 * @brief 重新输出snprintf接口
 * @param[in]str : 目标内存
 * @param[in]size : 内存大小
 * @param[in]format : 格式化内容
 * @return 0,或者 实际写入的大小,最大不超过size
 * @note 
 *  
 */
TPR_DECLARE TPR_INT32 CALLBACK TPR_Snprintf
(char *str, size_t size, const char *format, ...)
{
    int pos = 0;
    va_list ap;
    va_start(ap, format);

    if(!str || !format || !size)
    {
        va_end(ap);
        return 0;
    }
    /* 不同的系统支持的snprintf的返回值是不同的 ,
    因此我们统一处理*/
#if defined (OS_WINDOWS)
    pos = _vsnprintf(str, size, format, ap);
    
    if(pos == size)
    {
        str[size] = 0x00;
    }
    else if(pos < 0)
    {
        str[size] = 0x00;
        pos = 0;
    }
#elif defined (OS_POSIX)
    pos = vsnprintf(str, size, format, ap);
    
    if(pos > (int)size)
    {
        pos = (int)size;
    }
    else if(pos < 0)
    {
        pos = 0;
    }
#else
    #error OS Not Implement Yet.
#endif
    va_end(ap);
    
    return pos;
}




