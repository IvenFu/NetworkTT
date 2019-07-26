/**	@rtpInner.h
*	@note 111.
*	@brief rtp相关代码
*
*	@author		222
*	@date		2016/11/14
*
*	@note   注意外部rtp.h 文件的更新
*
*	@warning 
*/
#ifndef _RTPINNER_H_
#define _RTPINNER_H_

#include "utilInner.h"

#ifndef RTP_HEAD_LEN
#define RTP_HEAD_LEN    (12)  //RTP标准头长度
#define MIN_RTP_LEN    RTP_HEAD_LEN  //最小RTP长度
#define MAX_RTP_LEN    (1500) 
#define RTP_MAX_PADDING_LEN    (8)
#define RTP_RETRRANS_PADDING_LEN    (4)
#define RTP_EXTENSION_ONE_BIT_ID    (0xBEDE)
#endif

enum RTPExtensionTypeIdIn 
{
	kRtpExtensionTransportSequenceNumberIdIn = 8
};

/**	@struct	 PaddingInfo 
 *	@brief   rtp padding信息
 *	@note	 
 */
struct PaddingInfoIn
{
	PaddingInfoIn():
		iVer(0)
		,bRetrans(0)
		,iPaddingSize(0)
		{
			memset(aPadding,0,sizeof(aPadding));
		}
	~PaddingInfoIn(){}

	HPR_INT32 iVer;								//padding方案版本号
	HPR_BOOL bRetrans;							//是否是重传包标志
	HPR_INT32 iPaddingSize;						//最后一个字节，表示padding长度，包含长度字节
	HPR_UINT8 aPadding[RTP_MAX_PADDING_LEN];	//padding内容，包含最后一个字节长度
};

/**	@struct	 RTPHeaderExtensionIn 
 *	@brief   rtp扩展头信息
 *	@note	 
 */
struct RTPHeaderExtensionIn 
{
	RTPHeaderExtensionIn()
		:nProfile(0)
		,nLength(0)
		,hasTransportSequenceNumber(HPR_FALSE)
		,transportSequenceNumber(0)
		{
		}
	~RTPHeaderExtensionIn(){}

	HPR_UINT16 nProfile;					//扩展头标识
	HPR_UINT16 nLength;						//扩展头长度，表示字段乘以4之后的值, 不包括前面4字节
	HPR_BOOL hasTransportSequenceNumber;	//是否有传输层序号
	HPR_UINT16 transportSequenceNumber;		//传输层序号
};


/**	@struct	 RTPHeader 
 *	@brief   rtp头信息
 *	@note	 
 */
struct RTPHeaderIn 
{
	RTPHeaderIn();
	~RTPHeaderIn();

	HPR_BOOL bMarkerBit;			//帧结束标记
	HPR_UINT8 iPayloadType;			//payload类型
	HPR_UINT16 sSeqNo;				//序号
	HPR_UINT32 nTimestamp;			//时间戳
	HPR_UINT32 nSsrc;				//ssrc
	HPR_BOOL bPadding;				//padding标记
	HPR_BOOL bExtension;			//扩展头标记
	PaddingInfoIn paddingInfo;		//padding信息
	RTPHeaderExtensionIn struExtension;	//扩展头信息
};


/**	@class	 CRtpPacketIn 
 *	@brief   RTP包信息
 *	@note	 
 */
class CRtpPacketIn 
{
public:
	/*构造函数*/
	CRtpPacketIn();

	/*析构函数*/
	~CRtpPacketIn();

	/*分析RTP数据*/
	int Parse(unsigned char* pData,unsigned int nLen);
	
	/*是否是FEC包*/
	HPR_BOOL IsFecPacket();

	/*是否是重传包*/
	HPR_BOOL IsRetransPacket();

	RTPHeaderIn m_struHeader;	//rtp头信息
	unsigned char* m_pData;		//数据首地址
	unsigned int m_nLen;		//RTP包长度
	unsigned char* m_pRawData;	//裸数据首地址(仅表示只去掉RTP头后的数据，H264未恢复起始码)
	unsigned int m_nRawLen;		//裸数据长度

private:
	/*解析padding*/
	int ParsePadding(unsigned char* pData,unsigned int nLen);

	/*解析扩展头*/
	int ParseExtension(unsigned char* pData,unsigned int nLen);
	
	/*解析1字节类型的扩展头*/
	int ParseOneByteExtensionHeader(unsigned char* pData,unsigned int nLen);
};

#endif

