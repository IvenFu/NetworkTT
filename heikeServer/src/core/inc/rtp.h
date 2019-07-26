/**	@rtp.h
*	@note 111.
*	@brief rtp相关代码
*
*	@author		222
*	@date		2016/11/14
*
*	@note 
*
*	@warning 
*/
#ifndef _RTP_H_
#define _RTP_H_

#include <list>
#include "TPR_Time.h"
#include "NPQos.h"
#include "rtpDefine.h"
#include "sdpParse.h"

using std::list;


#define RTP_HEAD_LEN    (12)  //RTP标准头长度
#define MIN_RTP_LEN    RTP_HEAD_LEN  //最小RTP长度
#define MAX_RTP_LEN    (1500) 
#define RTP_MAX_PADDING_LEN    (8)
#define RTP_RETRRANS_PADDING_LEN    (4)


#define RTP_EXTENSION_ONE_BIT_ID    (0xBEDE)
#define RTP_EXTENSION_TWO_BIT_ID    (0x1000)
#define RTP_EXTENSION_HIK_BASICSTREAMINFO       (0x0001)	//私有描述信息类型
#define RTP_EXTENSION_HIK_ENCRYPT       (0x4000)			//加密描述信息类型

enum RTPExtensionTypeId 
{
	kRtpExtensionTransportSequenceNumberId = 8,
	kRtpExtensionAbsTimeId = 7,
	kRtpExtensionHikPrivateId = 0xff,
};


/**	@struct	 PaddingInfo 
 *	@brief   rtp padding信息
 *	@note	 
 */
struct PaddingInfo
{
	PaddingInfo():
		iVer(0)
		,bRetrans(0)
		,iPaddingSize(0)
		{
			memset(aPadding,0,sizeof(aPadding));
		}
	~PaddingInfo(){}

	TPR_INT32 iVer;								//padding方案版本号
	TPR_BOOL bRetrans;							//是否是重传包标志
	TPR_INT32 iPaddingSize;						//最后一个字节，表示padding长度，包含长度字节
	TPR_UINT8 aPadding[RTP_MAX_PADDING_LEN];	//padding内容，包含最后一个字节长度
};

/**	@struct	 HIK_AUDIO_INFO 
 *	@brief   定义的BasicStream扩展头中的音频描述字信息
 *	@note	 
 */
struct HIK_AUDIO_INFO
{
	TPR_BOOL     bAudio;
	TPR_UINT32   uFrameLen;			//音频帧长度
	TPR_UINT32   uAudioNum;			//音频声道数 0 单声道，1 双声道
	TPR_UINT32   uSampleRate;		//音频采样率
	TPR_UINT32   uBitRate;			//音频码率  bps
	TPR_UINT32   uBitsPerSample;    //样位
};

/**	@struct	 ExtensionBasicStreamInfo 
 *	@brief   定义的BasicStream扩展头信息
 *	@note	 
 */
struct ExtensionBasicStreamInfo
{
	HIK_AUDIO_INFO audioInfo;
};

/**	@struct	 RTPHeaderExtension 
 *	@brief   rtp扩展头信息
 *	@note	 
 */
struct RTPHeaderExtension 
{
	RTPHeaderExtension()
		:nProfile(0)
		,nLength(0)
		,hasTransportSequenceNumber(TPR_FALSE)
		,transportSequenceNumber(0)
		,hasHikBasicStream(TPR_FALSE)
		,hasHikEncypt(TPR_FALSE)
		{
			memset(&HikBasicStreaminfo,0,sizeof(ExtensionBasicStreamInfo));
		}
	~RTPHeaderExtension(){}

	TPR_UINT16 nProfile;					//扩展头标识
	TPR_UINT16 nLength;						//扩展头长度，表示字段乘以4之后的值, 不包括前面4字节
	TPR_BOOL hasTransportSequenceNumber;	//是否有传输层序号
	TPR_UINT16 transportSequenceNumber;		//传输层序号
	TPR_UINT8  byRes[2];
	TPR_BOOL hasHikBasicStream;				//是否有stream信息
	ExtensionBasicStreamInfo HikBasicStreaminfo; //定义的BasicStream扩展头信息
	TPR_BOOL hasHikEncypt;					//是否包含加密描述子
};

struct  VideoCodecInfo
{
	VIDEO_ENCODE_TYPE enVideoType;		//视频类型
	TPR_BOOL bKeyFrame;					//视频I帧标志，音频忽略
	unsigned int uWidth;				//宽(只有sps有该信息)
	unsigned int uHeight;				//高(只有sps有该信息)
};

struct  AudioCodecInfo
{
	AUDIO_ENCODE_TYPE enAudioType;		//音频类型，视频忽略
	TPR_UINT32 nAuidoBitRate;			//音频编码码率，视频忽略
};

union CodecInfo
{
	VideoCodecInfo struVideo;
	AudioCodecInfo struAudio;
};

/**	@struct	 RTPHeader 
 *	@brief   rtp头信息
 *	@note	 
 */
struct RTPHeader 
{
	RTPHeader();
	~RTPHeader();

	TPR_BOOL bMarkerBit;			//帧结束标记
	TPR_UINT8 iPayloadType;			//payload类型
	TPR_UINT16 sSeqNo;				//序号
	TPR_UINT32 nTimestamp;			//时间戳
	TPR_UINT32 nSsrc;				//ssrc
	TPR_BOOL bPadding;				//padding标记
	TPR_INT32 iPayLoadFrequence;	//时钟频率
	TPR_BOOL bExtension;			//扩展头标记
	PaddingInfo paddingInfo;		//padding信息
	RTPHeaderExtension struExtension;	//扩展头信息
};

/**	@class	 CRtpPacketizer 
 *	@brief   裸数据打包RTP包
 *	@note	 
 */
class CRtpPacketizer
{
public:
	/*构造函数*/
	CRtpPacketizer();
	/*析构函数*/
	virtual ~CRtpPacketizer();
	/*打包RTP*/
	virtual unsigned char* Pack( unsigned char* pData,unsigned int nLen,RTPHeader& struRTPHeader);
	
private:
	unsigned char m_pData[MAX_RTP_LEN];		 //数据缓存地址
	unsigned int m_nLen;					 //数据长度
};


/**	@class	 CRtpPacket 
 *	@brief   RTP包信息
 *	@note	 
 */
class CRtpPacket
{
public:
	/*构造函数*/
	CRtpPacket();

	/*析构函数*/
	~CRtpPacket();

	/*分析RTP数据*/
	int Parse(unsigned char* pData,unsigned int nLen,SdpInfo* pSdp);
	
	/*根据playload确认数据类型*/
	RTP_DATA_TYPE ConfirmType(TPR_UINT8 byPayloadType);

	/*根据playload确认数据类型*/
	void ConfirmAudioType(TPR_UINT8 byPayloadType);

	/*根据内部数据类型确认NPQ数据类型*/
	NPQ_DATA_TYPE CovertType(RTP_DATA_TYPE enRtpType);
	
	/*是否是FEC包*/
	TPR_BOOL IsFecPacket();

	/*是否是重传包*/
	TPR_BOOL IsRetransPacket();

	/*是否是Fec恢复的包*/
	TPR_BOOL IsFecRecoveryPacket();

	/*是否是音频Red包*/
	TPR_BOOL IsAudioRedPacket();

	/*是否是Padding包，备注:Padding包必然是重传包*/
	TPR_BOOL IsPaddingPacket();
	
	/*处理缓存资源*/
	void FreeBuffer();

	RTPHeader m_struHeader;		//rtp头信息
	CodecInfo m_unioCodecInfo;	//编码层信息
	unsigned char* m_pData;		//数据首地址
	unsigned int m_nLen;		//RTP包长度
	unsigned char* m_pRawData;	//裸数据首地址(仅表示只去掉RTP头后的数据，H264未恢复起始码,aac也未处理)
	unsigned int m_nRawLen;		//裸数据长度

	RTP_DATA_TYPE m_enType;		//数据类型，音频/视频/私有数据
	TPR_TIME_T m_nArriveTimeUs;	//数据包达到时间 单位us
	TPR_BOOL m_bNeedFree;		//需要主动释放m_pData内存标记
	TPR_BOOL m_bHaveCallback;	//是否已回调标志
	TPR_BOOL m_bPaddingPacket;	//是否是padding填充包（只用于发送端，接收端该值为TPR_FLASE，padding包必然也是重传包）
	TPR_BOOL m_bFecRecoveryPacket; //FEC恢复包标记（接收端）

private:
	/*解析padding*/
	int ParsePadding(unsigned char* pData,unsigned int nLen);

	/*解析扩展头*/
	int ParseExtension(unsigned char* pData,unsigned int nLen);

	int ParseAvcCodecInfo(unsigned char* pData,unsigned int nLen,VIDEO_ENCODE_TYPE enVideoType);
	
	/*解析1字节类型的扩展头*/
	int ParseOneByteExtensionHeader(unsigned char* pData,unsigned int nLen);
	
	/*解析2字节类型的扩展头*/
	int ParseTwoByteExtensionHeader(unsigned char* pData,unsigned int nLen);

	int ParseHikBasicstreaminfoExtensionHeader(unsigned char* pData,unsigned int nLen);

	int ParseHikAudioDescriptor(unsigned char* pData, unsigned int nLen);

	int ParseHikEncryptExtensionHeader(unsigned char* pData,unsigned int nLen);
};

typedef list<CRtpPacket> CRtpPackets;

/**	@class	 CRtpAddPadding 
 *	@brief   原有数据增加padding
 *	@note	 
 */
class CRtpPadding
{
public:
	/*构造函数*/
	CRtpPadding(){}

	/*析构函数*/
	virtual ~CRtpPadding(){}
	
	/*增加padding*/
	virtual int Padding(CRtpPacket& rtpPacket, PaddingInfo& paddingInfo, unsigned char* pBuf,unsigned int nBufLen);
};


/**	@class	 CRetransPadding 
 *	@brief   重传包增加padding方案
 *	@note	 继承CRtpPadding
 */
class CRetransPadding :private CRtpPadding
{
public:
	/*构造函数*/
	CRetransPadding();

	/*析构函数*/
	~CRetransPadding(){}

	/*按重传标记增加padding*/
	virtual int RetransPadding(CRtpPacket& rtpPacket);

private:
	unsigned char m_pData[MAX_RTP_LEN+12];	 //数据缓存地址
};


#endif

