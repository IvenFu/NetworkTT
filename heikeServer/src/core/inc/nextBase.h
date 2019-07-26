
#ifndef _NEXTBASE_H_
#define _NEXTBASE_H_

#include "rtp.h"

class NextBase
{
public:
	NextBase(){}
	~NextBase(){}

	virtual int InputData(CRtpPacket& RtpPacket) = 0;
};

#endif
