/*
 * CopyRight:	HangZhou Hikvision System Technology Co., Ltd. All	Right Reserved.
 * FileName:	HRP_String.h
 * Desc:		OS Layer Component.
 * Author:		schina
 * Date:		2009-03-02
 * Contact:		zhaoyiji@hikvision.com.cn
 * History:		Created By Zhaoyiji 2009-03-02
 * */

/*! 
	\file HPR_String.h
	\brief HPR String Manager.

	Details.
*/

#ifndef __HPR_STRING_H__
#define __HPR_STRING_H__


#include "HPR_Config.h"
#include "HPR_Types.h"
#if defined (OS_POSIX)
#include <ctype.h>
#endif

/**
 * HPR_Itoa
 * @param str  (OUT)
 * @param val (IN)
 * @param radix (IN)
 * @return
 * @sa HPR_Ltoa().
 * */
HPR_DECLARE char* CALLBACK HPR_Itoa(char* str, HPR_INT32 val, HPR_INT32 radix);

/**
 * HPR_Ltoa
 * @param str (OUT)
 * @param val (IN)
 * @param radix (IN)
 * @return
 * @sa HPR_Itoa().
 * */
HPR_DECLARE char* CALLBACK HPR_Ltoa(char* str, HPR_LONG val, HPR_INT32 radix);

/**
 * HPR_Atoi32
 * @param pSrc (IN)
 * @return
 * @sa HPR_Atoi32(),HPR_Atoi64().
 * */
HPR_DECLARE HPR_INT64 CALLBACK HPR_Atoi64(const char* pSrc);
HPR_DECLARE HPR_INT32 CALLBACK HPR_Atoi32(const char* pSrc);

/**
 * HPR_StrToi32
 * @param pSrc (IN)
 * @param pEnd (OUT)
 * @param iBase (IN)
 * @return
 * @sa HPR_StrToi64(),HPR_StrToi32().
 * */
HPR_DECLARE HPR_INT64 CALLBACK HPR_StrToi64(const char* pSrc, char** pEnd, int iBase); 
HPR_DECLARE HPR_INT32 CALLBACK HPR_StrToi32(const char* pSrc, char** pEnd, int iBase); 

/*
 * snprintf
 * */
//HPR_DECLARE HPR_INT32 HPR_Snprintf(char* str, size_t n, const char* format, va_list ap);

/*
 * vsprintf
 * */
//HPR_DECLARE HPR_INT32 HPR_VSnprintf(char* str, size_t n, const char* format, va_list ap);

/**
 * HPR_Strcpy
 * @param to  (IN OUT)
 * @param from (IN)
 * @return
 * @sa HPR_Strncpy
 * */
HPR_DECLARE char* CALLBACK HPR_Strcpy(char* to, const char* from);

/**
 * HPR_Strncpy
 * @param s1  (OUT)
 * @param s2 (IN)
 * @param n (IN)
 * @return
 * @sa HPR_Strcpy
 * */
HPR_DECLARE char* CALLBACK HPR_Strncpy(char* to, const char* from, int n);

/**
 * HPR_Strcmp
 * @param s1  (IN)
 * @param s2 (IN)
 * @return
 * @sa HPR_Strncmp
 * */
HPR_DECLARE HPR_INT32 CALLBACK HPR_Strcmp(const char* s1, const char* s2);

/**
 * HPR_Strncmp
 * @param s1  (IN)
 * @param s2 (IN)
 * @param n (IN)
 * @return
 * @sa HPR_Strcmp
 * */
HPR_DECLARE HPR_INT32 CALLBACK HPR_Strncmp(const char* s1, const char* s2, int n);

/**
 * HPR_Strcasecmp
 * @param s1  (IN)
 * @param s2 (IN)
 * @return
 * @sa HPR_Strncasecmp().
 * */
HPR_DECLARE HPR_INT32 CALLBACK HPR_Strcasecmp(const char* s1, const char* s2);

/**
 * HPR_Strncasecmp
 * @param s1 (IN)
 * @param s2 (IN)
 * @param n (IN)
 * @return
 * @sa HPR_Strcasecmp().
 * */
HPR_DECLARE HPR_INT32 CALLBACK HPR_Strncasecmp(const char* s1, const char* s2, int n);


/**
 * HPR_Strchr
 * @param s1 (IN)
 * @param c (IN)
 * @return
 * @sa HPR_Strnchr().
 * */
HPR_DECLARE char* CALLBACK HPR_Strchr(const char* s, char c);

/**
 * HPR_Strnchr
 * @param s1 (IN)
 * @param c (IN)
 * @param n (IN)
 * @return
 * @sa HPR_Strchr().
 * */
HPR_DECLARE char* CALLBACK HPR_Strnchr(const char* s, char c, int n);

/**
 * HPR_Strstr
 * @param s1 (IN)
 * @param s2 (IN)
 * @return
 * @sa HPR_Strcasestr().
 * */
HPR_DECLARE char* CALLBACK HPR_Strstr(const char* s1, const char* s2);

/**
 * HPR_Strnstr
 * @param s1 (IN)
 * @param s2 (IN)
 * @param n (IN)
 * @return
 * @sa HPR_Strstr().
 * */
HPR_DECLARE char* CALLBACK HPR_Strnstr(const char* s1, const char* s2, int n);

/**
 * HPR_strcasestr
 * @param s1 (IN)
 * @param s2 (IN)
 * @return
 * @sa HPR_Strstr().
 * */
HPR_DECLARE char* CALLBACK HPR_Strcasestr(const char* s1, const char* s2);

///add by zhaoyiji 2011-10-20
//////////////////////////////////////////////////////////////////////////
#include <string>

namespace hpr
{

#if (defined _WIN32 || defined _WIN64)
#define hpr_printf printf
#define hpr_sprintf sprintf
#define hpr_snprintf _snprintf
#define hpr_vsprintf _vsnprintf
#else
#define hpr_printf printf
#define hpr_sprintf sprintf
#define hpr_snprintf snprintf
#define hpr_vsprintf vsnprintf
#endif

	class HPR_DECLARE_CLASS hpr_string
	{
	public:
		//hpr_string();
		//hpr_string(const hpr_string&);
		//hpr_string(const char* buf);

		//virtual ~hpr_string();

	public:
		static hpr_int32 atoi(const char* src);
		static char* strcpy(char* src, const char* dst);
		static char* strncpy(char* src, const char* dst, hpr_int32 len);
		static char* strcat(char* dst, char* src);
		static hpr_int32 strlen(const char* str);
		static hpr_int32 strcmp(const char* str1, const char* str2);
		static hpr_int32 strcasecmp(const char* str1, const char* str2);
		static hpr_int32 strncasecmp(const char* str1, const char* str2, hpr_int32 len);
		static const char* strchr(const char* src, char c);
		static const char* strrchr(const char* src, char c);
		static const char* strstr(const char* src, const char* substr);
		static char* itoa(char* dst, hpr_int32 val, hpr_int32 radix);
		//static hpr_int32 split(const char* str, char c, char** dst, hpr_int32* count);
	public:
		//hpr_string& operator=(const hpr_string&);

	private:
		//std::string m_str;
	};

};


#endif

