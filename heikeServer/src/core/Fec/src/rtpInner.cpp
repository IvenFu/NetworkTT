/**	@rtp.cpp
*	@note 111.
*	@brief rtp相关代码
*
*	@author		222
*	@date		2017/1/7
*
*	@note 
*
*	@warning 
*/

#include "rtpInner.h"
#include "HikFecInterface.h"

/**	@fn RTPHeaderIn::RTPHeaderIn
*	@brief 构造函数
*	@return 
*/
RTPHeaderIn::RTPHeaderIn()
: bMarkerBit(HPR_FALSE),
iPayloadType(0),
sSeqNo(0),
nTimestamp(0),
nSsrc(0),
bPadding(HPR_FALSE),
bExtension(HPR_FALSE)
{
}

/**	@fn RTPHeaderIn::~RTPHeaderIn
*	@brief 析构函数
*	@return 
*/
RTPHeaderIn::~RTPHeaderIn()
{
}

/**	@fn CRtpPacketIn::~CRtpPacketIn
*	@brief 构造函数
*	@return 
*/
CRtpPacketIn::CRtpPacketIn()
:m_pData(NULL)
,m_nLen(0)
,m_pRawData(NULL)
,m_nRawLen(0)
{

}

/**	@fn CRtpPacketIn::~CRtpPacketIn
*	@brief 析构函数
*	@return 
*/
CRtpPacketIn::~CRtpPacketIn()
{

}

/**	@fn CRtpPacketIn::Parse
*	@brief 解析RTP头部数据
*	@param unsigned char* pData RTP包地址
*	@param unsigned int nLen RTP包长度
*	@return 见头文件错误码定义
*/
int CRtpPacketIn::Parse( unsigned char* pData,unsigned int nLen )
{
	int iRet;
	if (!pData || nLen<MIN_RTP_LEN) 
	{
		NPQ_ERROR("nLen err %d",nLen);
		return -1;
	}

	// 版本
	const HPR_UINT8 v  = pData[0] >> 6;
	// Padding
	const HPR_BOOL p  = ((pData[0] & 0x20) == 0) ? HPR_FALSE : HPR_TRUE;
	// 扩展字段
	const HPR_BOOL x = ((pData[0] & 0x10) == 0) ? HPR_FALSE : HPR_TRUE;
	const HPR_UINT8 c = pData[0] & 0x0f;
	//markbit
	const HPR_BOOL m  = ((pData[1] & 0x80) == 0) ? HPR_FALSE : HPR_TRUE;
	const HPR_UINT8 pt = pData[1] & 0x7f;
	const HPR_UINT16 sequenceNumber = (pData[2] << 8) + pData[3];

	HPR_UINT32 timeStamp = READ_BIG_ENDIAN_DWORD(pData+4);
	HPR_UINT32 ssrc = READ_BIG_ENDIAN_DWORD(pData+8);

	if (v != 2) 
	{
		NPQ_ERROR("v err %d pData=%p,nLen=%d",v,pData,nLen);
		return -1;
	}

	m_struHeader.bMarkerBit = m;
	m_struHeader.iPayloadType = pt;
	m_struHeader.sSeqNo = sequenceNumber;
	m_struHeader.nSsrc = ssrc;
	m_struHeader.nTimestamp = timeStamp;
	m_struHeader.bPadding = p;
	m_struHeader.bExtension = x;

	m_pRawData  = pData+RTP_HEAD_LEN;
	m_nRawLen = nLen-RTP_HEAD_LEN;

	if(x)
	{
		iRet = ParseExtension(pData+RTP_HEAD_LEN,nLen-RTP_HEAD_LEN);
		m_pRawData += iRet;
		m_nRawLen -= iRet;
	}

	if(p)
	{
		iRet = ParsePadding(pData,nLen);
		m_nRawLen -= iRet;
	}
	
	m_pData = pData;
	m_nLen = nLen;

	return 0;
}

/**	@fn CRtpPacketIn::ParseExtension
*	@brief 解析扩展头
*	@param  unsigned char* pData   扩展头开始位置地址
*	@param  unsigned int nLen   数据剩余长度
*	@return 返回扩展头长度
*/
int CRtpPacketIn::ParseExtension( unsigned char* pData,unsigned int nLen )
{
	unsigned char* ptr = pData;

	if (!pData || nLen < 4) 
	{
		NPQ_ERROR("Extension para err 1,nLen=%d",nLen);
		return 0;
	}
	
	/* RTP header extension, RFC 3550.
     0                   1                   2                   3
     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |      defined by profile       |           length              |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                        header extension                       |
    |                             ....                              |
    */
	HPR_UINT16 definedByProfile = READ_BIG_ENDIAN_WORD(ptr);
	ptr += 2;
	m_struHeader.struExtension.nProfile = definedByProfile;

	int iXLen = READ_BIG_ENDIAN_WORD(ptr);
	ptr += 2;
	iXLen *= 4; 
	m_struHeader.struExtension.nLength = iXLen;

	if (nLen < (4 + iXLen)) 
	{
		NPQ_ERROR("Extension para err 2,nLen=%d",nLen);
		return nLen;
	}

	if (definedByProfile == RTP_EXTENSION_ONE_BIT_ID) 
	{
		ParseOneByteExtensionHeader(ptr, iXLen);
	}
	else
	{
		//NPQ_WARN("unknow externion id definedByProfile = %x",definedByProfile);
	}
	return 4 + iXLen;
}

int CRtpPacketIn::ParseOneByteExtensionHeader(unsigned char* pData,unsigned int nLen)
{
	unsigned char* ptr = pData;

	if(!pData || nLen == 0)
	{
		return -1;
	}

	//  0
	//  0 1 2 3 4 5 6 7
	// +-+-+-+-+-+-+-+-+
	// |  ID   |  len  |
	// +-+-+-+-+-+-+-+-+
	
	while(ptr < pData+nLen)
	{
		const HPR_UINT8 id =  (*ptr & 0xf0) >> 4;
		const HPR_UINT8 len = (*ptr & 0x0f);
		ptr++;

		switch (id)
		{
			case kRtpExtensionTransportSequenceNumberIdIn:
				{
					//   0                   1                   2
					//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
					//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
					//  |  ID   | L=1   |transport wide sequence number |
					//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

					HPR_UINT16 sequence_number = READ_BIG_ENDIAN_WORD(ptr);

					m_struHeader.struExtension.transportSequenceNumber = sequence_number;
					m_struHeader.struExtension.hasTransportSequenceNumber = 1;
				}
				break;
			default:
				break;
		}

		ptr+=(len+1);
		
		//padding
		while(*ptr == 0)
		{
			ptr++;
		}
	}

	return 0;
}

/**	@fn CRtpPacketIn::ParsePadding
*	@brief 解析padding字段
*	@param  unsigned char* pData   RTP开始位置
*	@param  unsigned int nLen   RTP长度
*	@return 返回padding长度
*/
int CRtpPacketIn::ParsePadding( unsigned char* pData,unsigned int nLen )
{
	/*
	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|ver  |R|  res  |         other                 |       length
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


	ver：padding方案版本
	R： 重传标志，1表示重传包，0表示非重传包
	res：空闲
	other： 合成流方案使用字段
	length： padding长度
	*/
	HPR_UINT8* pPaddingRetran=NULL;

	if (!pData || nLen<MIN_RTP_LEN) 
	{
		NPQ_ERROR("nLen err %d",nLen);
		return 0;
	}

	if(pData[nLen-1]> RTP_MAX_PADDING_LEN)
	{
		NPQ_ERROR("m_struHeader.paddingInfo.iPaddingSize err %d",pData[nLen-1]);
		return 0;
	}
	m_struHeader.paddingInfo.iPaddingSize = pData[nLen-1];
	
	if(m_struHeader.paddingInfo.iPaddingSize < RTP_RETRRANS_PADDING_LEN)
	{
		m_struHeader.paddingInfo.iVer = 0;
		m_struHeader.paddingInfo.bRetrans = HPR_FALSE;

		int iPos = nLen - m_struHeader.paddingInfo.iPaddingSize;
		memcpy(m_struHeader.paddingInfo.aPadding, pData+iPos ,m_struHeader.paddingInfo.iPaddingSize);
	}
	else
	{
		int iPos = nLen - m_struHeader.paddingInfo.iPaddingSize;
		memcpy(m_struHeader.paddingInfo.aPadding, pData+iPos ,m_struHeader.paddingInfo.iPaddingSize);
		
		iPos = m_struHeader.paddingInfo.iPaddingSize - RTP_RETRRANS_PADDING_LEN;
		pPaddingRetran = m_struHeader.paddingInfo.aPadding + iPos;

		m_struHeader.paddingInfo.iVer = pPaddingRetran[0]>>5;
		m_struHeader.paddingInfo.bRetrans = ((pPaddingRetran[0] & 0x10) == 0 ? HPR_FALSE : HPR_TRUE);
		if(m_struHeader.paddingInfo.bRetrans)
		{
			//NPQ_DEBUG("it is a retrans packet!!!!!!!!!!!! %d",m_struHeader.sSeqNo);
		}
	}
	
	return m_struHeader.paddingInfo.iPaddingSize;
}


/**	@fn CRtpPacketIn::IsFecPacket
*	@brief 是否是FEC包
*	@return HPR_TRUE/HPR_FALSE
*/
HPR_BOOL CRtpPacketIn::IsFecPacket()
{
	//通过PT判断是否是FEC
	return (m_struHeader.iPayloadType == FEC_PACKET_VIDEO_PAYLOAD_TYPE 
		|| m_struHeader.iPayloadType == FEC_PACKET_AUDIO_PAYLOAD_TYPE);
}

/**	@fn CRtpPacket::IsRetransPacket
*	@brief 是否是重传包标记
*	@return HPR_TRUE/HPR_FALSE
*/
HPR_BOOL CRtpPacketIn::IsRetransPacket()
{
	if(!m_struHeader.bPadding)
	{
		return HPR_FALSE;
	}

	return m_struHeader.paddingInfo.bRetrans;
}


