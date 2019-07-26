/**	@sdpParse.h
*	@note 111.
*	@brief sdp��������
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
 *	@brief  Ԫ����Ϣ
 *	@note	 
 */
struct SDP_INFO_CODEC
{
	SDP_INFO_CODEC():nPayLoadType(-1),sFormatToConfirm(-1),sFormat(0),nClock(0),nBitRate(0){}

	int nPayLoadType;			     //payloadTypeֵ
    unsigned short sFormatToConfirm; //��ȷ���ı����ʽ�����ɶ����������
	unsigned short sFormat;		     //�����ʽ
	unsigned int   nClock;		     //ʱ��Ƶ��
	unsigned int   nBitRate;	     //������   ��λ:bps
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
 *	@brief  sdp��Ϣ
 *	@note	 
 */
class SdpInfo
{
public:
	SdpInfo();
	~SdpInfo();
	
	/*ȷ����������*/
	NPQ_DATA_TYPE ExplicitData(unsigned char* pData, unsigned int iDataLen);
	
	/*����PTֵȷ����Ƶ��������*/
	AUDIO_ENCODE_TYPE ExplicitAudioDataType(int iPayloadType);
	
	/*����PTֵȷ��RTP��������*/
	RTP_DATA_TYPE ExplicitRtpDataTypeByPT(unsigned int iPayloadType);

	/*����PTֵȷ����Ƶ�������*/
	int ExplicitCodecParam(int iPayloadType,SDP_INFO_CODEC* pCodec);

	TPR_BOOL m_bValid;		//���������sdp����Ч
	TPR_BOOL m_bBundle;		//�Ƿ���Budleģʽ
	TPR_BOOL m_bRtcpMux;	//�Ƿ���rtcp/rtp���ģʽ
	TPR_BOOL m_bHikRtx;		//�Ƿ����ش�����
	INFO_ELE m_media[2];
private:
	/*����ssrcȷ��rtcp�������ͣ���Ҫrtcp-muxģʽ*/
	RTCP_DATA_TYPE ExplicitRtcpDataTypeBySSRC(TPR_UINT32 ssrc);
	
	/*����ssrcȷ��rtp��������*/
	RTP_DATA_TYPE ExplicitRtpDataTypeBySSRC(TPR_UINT32 ssrc);
};

/**	@class	 SdpParse [SdpParse.cpp]    
 *	@brief   ��װ��sdp�����Ĳ���
 *	@note	 
 */
class SdpParse
{
public:
	/*���캯��*/
	SdpParse();

	/*��������*/
	~SdpParse();
	
	/*����sdp��ں���*/
	int Parse(const char* szSdp, SdpInfo* pInfo );
	
	/*����sdp��ں���*/
	int Parse(const string& szSdp, SdpInfo* pInfo);
	
private:
	enum MEDIA_STATUS
	{
		STATUS_READY,
		STATUS_VIDEO,
		STATUS_AUDIO,
		STATUS_OTHER,
	};

	/*����һ������*/
	int ParseLine(string& szLine, SdpInfo* pInfo,list<int>& videoPts,list<int>& audioPts);
	
	/*����rtpmap�ֶ�*/
	int ParseRtpmap(string& szSrc, SDP_INFO_CODEC* pEle);

    /*����fmtp�ֶ�*/
    int ParseFmtp(string& szSrc,SdpInfo* pInfo,list<int>& videoPts,list<int>& audioPts);
	
	/*����rtcp-fb*/
	int ParseRtcpFb(string& szSrc,SdpInfo* pInfo);

	/*��ȡһ������*/
	int ReadWord(const string& szSrc, string& szWord, int nBegin);
	
	/*��ȡrtpmap�ֶ�*/
	int ReadRtpmap(const string& szSrc,string& szLeft,string& szRight);
	
	/*��ȡһ������*/
	int ReadLine(const string& szSrc,string& szLine,int nBegin);
	
	/*��ȡ�Էָ���������ַ���*/
	int ReadSeq(const string& szSrc,const string& szSeq, string& szOut,int nBegin);
	
	TPR_BOOL IsExpectedMedia();

	MEDIA_STATUS m_enMediaStatus;
};

#endif
