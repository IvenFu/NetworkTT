#include "timeUtil.h"
#include "TPR_tpr.h"


namespace NtpTime
{
	const unsigned long kNtpJan1970 = 2208988800UL;   //NTP��linuxԪ��ʱ���
	const double kMagicNtpFractionalUnit = 4.294967296E+9;   //����ntp frac��ϵ��  2^32 
	const double kMagicNtpFractionalMs = 4.294967296E+6; 

	void NtpNow( unsigned int& nSec, unsigned int& nFrac )
	{
		TPR_TIME_T now = TPR_TimeNow();

		nSec = now/1000000;
		nSec += kNtpJan1970;//��linuxԪ��ת��ΪNTP

		unsigned int uSec = now%1000000;//΢��

		double dsec;//��
		dsec = uSec / 1e6;

		nFrac = (unsigned int)(dsec * kMagicNtpFractionalUnit + 0.5);	
	}

	void NtpShortTime( unsigned int nSec, unsigned int nFrac,unsigned int& nShort )
	{
		nShort = ((nSec&0xFFFF)<<16)|(nFrac>>16);
	}

	void NtpNtpTimeToReal(unsigned int nShort,unsigned int& nMmSec)
	{
		//short ��16λ��λ�룬��16λ����:

		//��16λ��λΪ  10^6/2^16
		//us * 2^16/10^6 = X
		//us = X *10^6 /2^16
		//ms = X *10^3 /2^16
		//X *1000 >>16

		nMmSec =(((nShort & 0xffff0000) >> 16) * 1000) + (((nShort & 0x0000ffff) * 1000) >> 16);
	}

	void NtpNtpTimeToRealPre(unsigned int nSec, unsigned int nFrac, TPR_INT64& llMmSec)
	{
		const double dNtpFracMs = (double)(nFrac) / kMagicNtpFractionalMs;
		llMmSec = 1000 * (TPR_INT64)(nSec) + (TPR_INT64)(dNtpFracMs + 0.5);
	}

	void NtpRealToNtpTime( unsigned int nSec, unsigned int uSec,unsigned int& nShort )
	{
		//��16λ ��λ1/65536��    ��16λ   ��λ 1000000/65536 ->  1024/15625 
		nShort = (nSec<<16) | ( ((uSec<<10)/15625) & 0xFFFF);
		//nShort = (nSec<<16) | ( (((uSec<<11)+15625)/31250) & 0xFFFF);
	}
}

namespace OneTime
{
	TPR_INT64 OneNowMs()
	{
		TPR_TIME_T now = TPR_TimeNow();

		return (TPR_INT64)(now/1000);
	}

	TPR_INT64 OneNowUs()
	{
		return (TPR_INT64)TPR_TimeNow();
	}
}
