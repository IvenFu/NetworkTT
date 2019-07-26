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




//���ش�1970��1��1�� �����ڵ�ʱ�䣬 ��λus
TPR_DECLARE TPR_TIME_T CALLBACK TPR_TimeNow();

//���شӲ���ϵͳ�����������ĺ���
TPR_DECLARE TPR_UINT32 CALLBACK TPR_GetTimeTick();

//���شӲ���ϵͳ�����������ĺ���
TPR_DECLARE TPR_UINT64 CALLBACK TPR_GetTimeTick64();

//���׼1970��ʱ�䣨��λs���໥ת��
TPR_DECLARE TPR_TIME_T CALLBACK TPR_TimeFromAnsiTime(time_t tAnsiTime);

//���׼1970��ʱ�䣨��λs���໥ת��
TPR_DECLARE time_t CALLBACK TPR_AnsiTimeFromTime(TPR_TIME_T iTime);

//��TPR_TIME_T ʱ��ת��Ϊ TPR_TIME_EXP_Tʱ�䣬 iOff��ʱ��ƫ�ƣ���λs��
TPR_DECLARE TPR_INT32 CALLBACK TPR_ExpTimeFromTime(TPR_TIME_T iTime, TPR_INT32 iOff, TPR_TIME_EXP_T* pExpTime);

//��TPR_TIME_T ʱ��ת��Ϊ TPR_TIME_EXP_Tʱ�䣬 iOff��0 ��GMTʱ�䣨�׶�ʱ�䣩
TPR_DECLARE TPR_INT32 CALLBACK TPR_ExpTimeFromTimeGMT(TPR_TIME_T iTime, TPR_TIME_EXP_T* pExpTime);

//��TPR_TIME_T ʱ��ת��Ϊ TPR_TIME_EXP_Tʱ�䣨����ʱ�䣩��������ʱ����Ϣ
TPR_DECLARE TPR_INT32 CALLBACK TPR_ExpTimeFromTimeLocal(TPR_TIME_T iTime, TPR_TIME_EXP_T* pExpTime);

//��TPR_TIME_EXP_T  ʱ��ת��Ϊ TPR_TIME_T ʱ�䣬������ʱ��
TPR_DECLARE TPR_INT32 CALLBACK TPR_TimeFromExpTime(TPR_TIME_EXP_T* pExpTime, TPR_TIME_T* pTime);

//��TPR_TIME_EXP_T  ʱ��ת��ΪGMT���׶�ʱ�䣩 0ʱ���� TPR_TIME_T ʱ��
TPR_DECLARE TPR_INT32 CALLBACK TPR_TimeFromExpTimeGMT(TPR_TIME_EXP_T* pExpTime, TPR_TIME_T* pTime);

//��TPR_TIME_T ��ʽ���ַ������
TPR_DECLARE TPR_INT32 CALLBACK TPR_CTime(char* pDateStr, TPR_TIME_T nTime);

//��TPR_TIME_EXP_Tʱ���ʽ�����
TPR_DECLARE TPR_INT32 CALLBACK TPR_StrFmtTime(char* pDst, TPR_INT32* iDstLen,TPR_INT32 iMaxDstSize, const char* pFmt, TPR_TIME_EXP_T* pTimeExp);


#endif
