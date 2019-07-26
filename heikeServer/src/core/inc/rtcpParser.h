#ifndef _RTCP_PARSER_H_
#define _RTCP_PARSER_H_

#include "rtcpDefine.h"

class RtcpCallback;
class RtcpParser
{
public:
	RtcpParser(RtcpCallback& rtcpCallback);
	~RtcpParser();

	int InputRTCP(unsigned char* pData, unsigned int uDataLen,unsigned char** ppOutData,unsigned int* pOutLen);
private:

	int ParseRR( unsigned char* pData,RTCP_RR_INFO* pInfo);
	int ParseNack(unsigned char* pData,unsigned int uLen,NPQ_NACK_INFO* pInfo);

	RtcpCallback& m_rtcpCallback;
};

#endif
