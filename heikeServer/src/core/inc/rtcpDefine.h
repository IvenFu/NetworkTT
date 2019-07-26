#ifndef _RTCP_DEFINE_H_
#define _RTCP_DEFINE_H_

#include <vector>
#include "TPR_Time.h"
#define  SDES_TEXT       "unknown@unknown"
#define  SDES_ITEM_TYPE   0x06
#define  SDES_TEXT_1     "nnnnnnnnnnn"

using std::vector;

#define RTCP_VIDEO_SSRC  0x55667788
#define RTCP_AUDIO_SSRC  0x11223344

enum RTCP_PT_TYPE
{
	RTCP_PT_SR = 200,		//rfc3550
	RTCP_PT_RR = 201,		//rfc3550
	RTCP_PT_SDES = 202,
	RTCP_PT_BYE = 203,
	RTCP_PT_APP = 204,
	RTCP_PT_FEEDBACK = 205,      //rfc4585
	RTCP_PT_SPEC_FEEDBACK = 206, //rfc4585
	RTCP_PT_XR = 207,
};

//rfc3611
enum RTCP_XR_BT_TYPE
{
	RTCP_XR_BT_REFERENCETIME = 4,
	RTCP_XR_BT_DLRR = 5,
};

enum RTCP_FB_FMI_TYPE
{
	RTCP_FB_FMI_NACK = 1,			//NACK丢包反馈       rfc4588
	RTCP_FB_FMI_TMMBR = 3,			//带宽反馈   rfc5104
	RTCP_FB_FMI_TMMBN = 4,			//带宽反馈   rfc5104
	RTCP_FB_FMI_TRANSPORT = 15,		//传输层反馈 rfc5285 + draft-holmer-rmcat-transport-wide-cc-extensions-01 - RTP Extensions for Transport-wide Congestion Control
};

enum RTCP_SPEC_FB_FMI_TYPE
{
	RTCP_SPEC_FB_FMI_PLI = 1,			// PLI rfc4585
	RTCP_SPEC_FB_FMI_SLI = 2,			// SLI rfc4585
	RTCP_SPEC_FB_FMI_RPSI = 3,			// SLI rfc4585
	RTCP_SPEC_FB_FMI_FIR = 4,			// FIR rfc5104
	RTCP_SPEC_FB_FMI_REMB = 15,			// draft-alvestrand-rmcat-remb-03 - RTCP message for Receiver Estimated Maximum Bitrate
};

enum RTCP_INFO_TYPE
{
	INDEX_RR_INFO,
	INDEX_SR_INFO,
	INDEX_XR_REFERENCETIME_INFO,
	INDEX_XR_DLRR_INFO,
	INDEX_FB_NACK_INFO,
	INDEX_FB_TRANSPORT_INFO,
	INDEX_SPEC_FB_PLI_INFO,
	INDEX_SPEC_FB_FIR_INFO,
};

enum RTCP_OUTPUT_TYPE
{
	OUTPUT_BASIC,
	OUTPUT_NACK,
	OUTPUT_TRANSPORT,
	OUTPUT_PLI,
	OUTPUT_FIR,
	OUTPUT_REMB,
};

#define NPQ_MAX_NACK_NUMBER  256
struct NPQ_NACK_INFO
{
	unsigned int iNackNumber;
	unsigned int aNackList[NPQ_MAX_NACK_NUMBER];
};

struct RTCP_RR_INFO
{
	unsigned int nLsr;
	unsigned int nDlsr;
	unsigned int nMaxSeqNumber;
	unsigned char cLossFraction;	//丢包率  单位1/256
};

struct RTCP_SR_INFO
{
	unsigned int nLastSRNTPmsw;   //SR报告解析到的时间
	unsigned int nLastSRNTPlsw;	//SR报告解析到的时间
	TPR_TIME_T nlastReceiveSRTime; //us
	TPR_UINT32 nLastRtpTimeStamp;
};

struct RTCP_XR_REFERENCETIME_INFO
{
	unsigned int nLastXRNTPmsw;  
	unsigned int nLastXRNTPlsw;	
	TPR_TIME_T nlastReceiveXRTime; 
};

struct RTCP_XR_DLRR_INFO
{
	unsigned int nLrr;
	unsigned int nDlrr;
};

struct RTCP_FB_TRANSPORT_INFO
{
	int res;
};

struct RTCP_SPEC_FB_REMB_INFO
{
	TPR_INT64 llBitratebps;
	vector<TPR_UINT32> aSsrcs;
};
#endif
