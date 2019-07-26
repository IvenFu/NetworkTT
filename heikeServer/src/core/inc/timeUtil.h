#ifndef _TIMEUTIL_H_
#define _TIMEUTIL_H_

#include "TPR_Time.h"

namespace OneTime
{
	TPR_INT64 OneNowMs();
	TPR_INT64 OneNowUs();
};


namespace NtpTime
{
	void NtpNow( unsigned int& nSec, unsigned int& nFrac );
	void NtpShortTime( unsigned int nSec, unsigned int nFrac,unsigned int& nShort);
	void NtpNtpTimeToReal(unsigned int nShort,unsigned int& nMmSec);
	void NtpRealToNtpTime( unsigned int nSec, unsigned int uSec,unsigned int& nShort );
	void NtpNtpTimeToRealPre(unsigned int nSec, unsigned int nFrac, TPR_INT64& llMmSec);
};

#endif
