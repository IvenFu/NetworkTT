/**	@sdpParse.h
*	@note 111.
*	@brief sdp解析定义
*
*	@author		222
*	@date		2017/5/18
*
*	@note 
*
*	@warning 
*/
#ifndef _SDP_PARSE_H_
#define _SDP_PARSE_H_

#include "TPR_TPR.h"
#include "TPR_Types.h"
#include <vector>
#include <list>
#include <string>
#include "rtpDefine.h"

using std::string;
using std::vector;
using std::list;


/**	@struct	 SDP_INFO_CODEC
 *	@brief  元素信息
 *	@note	 
 */
struct SDP_INFO_CODEC
{
	SDP_INFO_CODEC():nPayLoadType(-1),sFormatToConfirm(-1),sFormat(0),nClock(0),nBitRate(0){}

	int nPayLoadType;			     //payloadType值
    unsigned short sFormatToConfirm; //待确定的编码格式，需由多个参数决定
	unsigned short sFormat;		     //编码格式
	unsigned int   nClock;		     //时钟频率
	unsigned int   nBitRate;	     //比特率   单位:bps
};

typedef vector<TPR_UINT32> VecSsrcGroup;
typedef vector<SDP_INFO_CODEC> VecCodecs;

struct INFO_ELE
{
	INFO_ELE():bSupportPli(TPR_FALSE),bSupportNack(TPR_FALSE){}
	VecSsrcGroup aSsrcGroup;
	VecCodecs codecs;
	TPR_BOOL bSupportPli;
	TPR_BOOL bSupportNack;
};

#define MEDIA_VIDEO_INDEX 0
#define MEDIA_AUDIO_INDEX 1
#define MEDIA_INVALD_INDEX 2

/**	@class	 SdpInfo
 *	@brief  sdp信息
 *	@note	 
 */
class SdpInfo
{
public:
	SdpInfo();
	~SdpInfo();
	
	/*确定数据类型*/
	NPQ_DATA_TYPE ExplicitData(unsigned char* pData, unsigned int iDataLen);
	
	/*根据PT值确认音频编码类型*/
	AUDIO_ENCODE_TYPE ExplicitAudioDataType(int iPayloadType);
	
	/*根据PT值确认RTP数据类型*/
	RTP_DATA_TYPE ExplicitRtpDataTypeByPT(unsigned int iPayloadType);

	/*根据PT值确认音频编码参数*/
	int ExplicitCodecParam(int iPayloadType,SDP_INFO_CODEC* pCodec);

	TPR_BOOL m_bValid;		//如果有配置sdp且有效
	TPR_BOOL m_bBundle;		//是否是Budle模式
	TPR_BOOL m_bRtcpMux;	//是否是rtcp/rtp混合模式
	TPR_BOOL m_bHikRtx;		//是否是重传方案
	INFO_ELE m_media[2];
private:
	/*根据ssrc确认rtcp数据类型，需要rtcp-mux模式*/
	RTCP_DATA_TYPE ExplicitRtcpDataTypeBySSRC(TPR_UINT32 ssrc);
	
	/*根据ssrc确认rtp数据类型*/
	RTP_DATA_TYPE ExplicitRtpDataTypeBySSRC(TPR_UINT32 ssrc);
};

/**	@class	 SdpParse [SdpParse.cpp]    
 *	@brief   封装了sdp解析的操作
 *	@note	 
 */
class SdpParse
{
public:
	/*构造函数*/
	SdpParse();

	/*析构函数*/
	~SdpParse();
	
	/*解析sdp入口函数*/
	int Parse(const char* szSdp, SdpInfo* pInfo );
	
	/*解析sdp入口函数*/
	int Parse(const string& szSdp, SdpInfo* pInfo);
	
private:
	enum MEDIA_STATUS
	{
		STATUS_READY,
		STATUS_VIDEO,
		STATUS_AUDIO,
		STATUS_OTHER,
	};

	/*解析一行数据*/
	int ParseLine(string& szLine, SdpInfo* pInfo,list<int>& videoPts,list<int>& audioPts);
	
	/*解析rtpmap字段*/
	int ParseRtpmap(string& szSrc, SDP_INFO_CODEC* pEle);

    /*解析fmtp字段*/
    int ParseFmtp(string& szSrc,SdpInfo* pInfo,list<int>& videoPts,list<int>& audioPts);
	
	/*解析rtcp-fb*/
	int ParseRtcpFb(string& szSrc,SdpInfo* pInfo);

	/*读取一个单词*/
	int ReadWord(const string& szSrc, string& szWord, int nBegin);
	
	/*读取rtpmap字段*/
	int ReadRtpmap(const string& szSrc,string& szLeft,string& szRight);
	
	/*读取一行数据*/
	int ReadLine(const string& szSrc,string& szLine,int nBegin);
	
	/*读取以分隔符间隔的字符串*/
	int ReadSeq(const string& szSrc,const string& szSeq, string& szOut,int nBegin);
	
	TPR_BOOL IsExpectedMedia();

	MEDIA_STATUS m_enMediaStatus;
};

#endif
