#ifndef __TPR_UTILS_H__
#define __TPR_UTILS_H__

#include "TPR_Config.h"
#include "TPR_Types.h"

#if defined LLONG_MAX
    #define TPR_INT64_MAX (LLONG_MAX) //(TPR_INT64)(0x7fffffffffffffff)
    #define TPR_INT64_MIN (LLONG_MIN) //((TPR_INT64)0x7fffffffffffffff) - (TPR_INT64)1)
#else
    #define TPR_INT64_MAX (TPR_INT64)(0x7fffffffffffffff)
    #define TPR_INT64_MIN ((TPR_INT64)(0x7fffffffffffffff) - (TPR_INT64)1)
#endif
#define TPR_INT32_MAX (INT_MAX)
#define TPR_INT32_MIN (INT_MIN)

#if defined(OS_WINDOWS)
    #define TPR_Vsnprintf _vsnprintf
    //#define TPR_Snprintf _snprintf
#elif defined(OS_POSIX)
    #define TPR_Vsnprintf vsnprintf
    //#define TPR_Snprintf snprintf
#else
    #error OS Not Implement Yet.
#endif

#if defined (TPR_SUPPORT_NOTHROW)
#define TPR_NEW_RETURN(POINTER,CONSTRUCTURE) \
    do { POINTER = new (::std::nothrow) CONSTRUCTURE;\
    if (POINTER == 0) {return NULL;}\
    return POINTER;\
    }while(0)
#else
#define TPR_NEW_RETURN(POINTER,CONSTRUCTURE) \
    do {    try {POINTER = new CONSTRUCTURE;\
            return POINTER;}\
    catch (::std::bad_alloc & a){POINTER=0,return NULL}\
        }while(0)
#endif


#define TPR_SAFE_FREE(p) do{if(p){free(p);(p=NULL);}}while(0);
#define TPR_SAFE_DELETE(p) do{if(p){(delete (p));p=NULL;}}while(0);
#define TPR_SAFE_DELETE_ARR(p) do{if(p){(delete []p);(p=NULL);}}while(0);
#define TPR_POINTER_REF(p,b) do{if(p){*(p) = (b);}}while(0);

#define TPR_MAX_VALUE(a,b) (((a)>(b))?(a):(b))
#define TPR_MIN_VALUE(a,b) (((a)>(b))?(b):(a))


/**
 * TPR_Debug format and print data in debug mode.
 * @param format (IN) string format.
 * @return void
 * @sa TPR_OutputDebug
 */
TPR_DECLARE TPR_VOID  CALLBACK TPR_Debug(const char* format, ...);

/**
 * TPR_OutputDebug format and print data in debug/release mode.
 * @param format (IN) string format.
 * @return void
 * @sa TPR_Debug
 */
TPR_DECLARE TPR_VOID CALLBACK TPR_OutputDebug(const char* format, ...);

/**
 * TPR_OutputDebugString output string directly in debug/release mode.
 * @param format (IN) string pointer.
 * @return void
 * @sa TPR_OutputDebug
 */
TPR_DECLARE TPR_VOID CALLBACK TPR_OutputDebugString(const char* lpOutputString);

/**
 * TPR_USleep suspend execute for millisecond intervals
 * @param millisecond (IN) count of millisecond interval
 * @return void
 * @sa TPR_USleep
 */
TPR_DECLARE TPR_VOID CALLBACK TPR_Sleep(TPR_INT32 millisecond);

/**
 * TPR_USleep suspend execute for microsecond intervals
 * @param nUSec (IN) count of microsecond interval
 * @return void
 * @sa TPR_Sleep
 */
TPR_DECLARE TPR_VOID CALLBACK TPR_USleep(TPR_INT64 nUSec);

/**
 * Function: TPR_Rand
 * Desc:    获取随机数,随机数种子由内部TPR_GetTimeTick()获取
 * Input:   
 * Output:
 * Return:  随机数
 */
TPR_DECLARE TPR_UINT32 CALLBACK TPR_Rand();

/**
 * Function: TPR_ZeroMemory
 * Desc:    把内存清零调用 memset(mem, 0, len);
 * Input:   @param mem : 内存首指针
            @param len : 内存长度
 * Output:
 * Return:  
 */
TPR_DECLARE TPR_VOID CALLBACK TPR_ZeroMemory(void* mem, int len);

/**
 * Function: TPR_GetCPUNumber
 * Desc:    获取CPU个数
 * Input:   
 * Output:
 * Return:  
 */
TPR_DECLARE TPR_INT32 CALLBACK TPR_GetCPUNumber();

typedef unsigned char TPR_UUID[16];
TPR_DECLARE TPR_INT32 CALLBACK TPR_GetUUID(TPR_UUID uuid);
TPR_DECLARE TPR_INT32 CALLBACK TPR_StringFromUUID(TPR_UUID uuid, char* uuidString, TPR_INT32 len);
TPR_DECLARE TPR_INT32 CALLBACK TPR_Snprintf
(char *str, size_t size, const char *format, ...);

#endif
