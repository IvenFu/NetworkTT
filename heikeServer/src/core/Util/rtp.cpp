/**	@rtp.cpp
*	@note 111.
*	@brief rtp��ش���
*
*	@author		222
*	@date		2017/1/7
*
*	@note 
*
*	@warning 
*/

#include "rtp.h"
#include "NPQosImpl.h"


/**	@fn RTPHeader::RTPHeader
*	@brief ���캯��
*	@return 
*/
RTPHeader::RTPHeader()
: bMarkerBit(TPR_FALSE),
iPayloadType(0),
sSeqNo(0),
nTimestamp(0),
nSsrc(0),
bPadding(TPR_FALSE),
iPayLoadFrequence(0),
bExtension(TPR_FALSE)
{
}

/**	@fn RTPHeader::~RTPHeader
*	@brief ��������
*	@return 
*/
RTPHeader::~RTPHeader()
{
}

/**	@fn CRtpPacket::~CRtpPacket
*	@brief ���캯��
*	@return 
*/
CRtpPacket::CRtpPacket()
:m_pData(NULL)
,m_nLen(0)
,m_pRawData(NULL)
,m_nRawLen(0)
,m_enType(RTP_VIDEO)
,m_nArriveTimeUs(0)
,m_bNeedFree(TPR_FALSE)
,m_bHaveCallback(TPR_FALSE)
,m_bPaddingPacket(TPR_FALSE)
,m_bFecRecoveryPacket(TPR_FALSE)
{
	memset(&m_unioCodecInfo,0,sizeof(CodecInfo));
}

/**	@fn CRtpPacket::~CRtpPacket
*	@brief ��������
*	@return 
*/
CRtpPacket::~CRtpPacket()
{

}

/**	@fn CRtpPacket::Parse
*	@brief ����RTPͷ������
*	@param unsigned char* pData RTP����ַ
*	@param unsigned int nLen RTP������
*	@return ��ͷ�ļ������붨��
*/
int CRtpPacket::Parse( unsigned char* pData,unsigned int nLen ,SdpInfo* pSdp)
{
	//  0                   1                   2                   3
	//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// |V=2|P|X|  CC   |M|     PT      |       sequence number         |
	// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// |                           timestamp                           |
	// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// |           synchronization source (SSRC) identifier            |
	// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	// |            Contributing source (CSRC) identifiers             |
	// |                             ....                              |
	// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	// |One-byte eXtensions id = 0xbede|       length in 32bits        |
	// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// |                          Extensions                           |
	// |                             ....                              |
	// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	// |                           Payload                             |
	// |             ....              :  padding...                   |
	// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// |               padding         | Padding size  |

	int iRet;
	SDP_INFO_CODEC struSdpCodecInfo;

	if (!pData || nLen<MIN_RTP_LEN) 
	{
		NPQ_ERROR("nLen err %d",nLen);
		return NPQERR_DATAFORMAT;
	}

	if(pSdp && !pSdp->m_bValid)
	{
		pSdp = NULL;
	}

	// �汾
	const TPR_UINT8 v  = pData[0] >> 6;
	// Padding
	const TPR_BOOL p  = ((pData[0] & 0x20) == 0) ? TPR_FALSE : TPR_TRUE;
	// ��չ�ֶ�
	const TPR_BOOL x = ((pData[0] & 0x10) == 0) ? TPR_FALSE : TPR_TRUE;
	const TPR_UINT8 c = pData[0] & 0x0f;
	//markbit
	const TPR_BOOL m  = ((pData[1] & 0x80) == 0) ? TPR_FALSE : TPR_TRUE;
	const TPR_UINT8 pt = pData[1] & 0x7f;
	const TPR_UINT16 sequenceNumber = (pData[2] << 8) + pData[3];

	TPR_UINT32 timeStamp = READ_BIG_ENDIAN_DWORD(pData+4);
	TPR_UINT32 ssrc = READ_BIG_ENDIAN_DWORD(pData+8);

	if (v != 2) 
	{
		NPQ_ERROR("v err %d pData=%#x,%#x,nLen=%d",v,pData[0], pData[1],nLen);
		return NPQERR_DATAFORMAT;
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
	
	//ȷ��m_enType
	m_enType = ConfirmType(pt);
	
	//ȷ��CodecInfo����
	if(m_enType == RTP_VIDEO)
	{
		
		{
			//���û��sdp��Ĭ��Ϊ264 TBD
			m_unioCodecInfo.struVideo.enVideoType = NPQ_H264;
		}

		if(m_struHeader.iPayLoadFrequence == 0)
		{
			m_struHeader.iPayLoadFrequence = 90000;
		}
	}
	else if(m_enType == RTP_UNKNOW)
	{
		NPQ_ERROR("rtp entype unknow");
		return NPQERR_DATAFORMAT;
	}
	return NPQ_OK;
}

/**	@fn CRtpPacket::ParseExtension
*	@brief ������չͷ
*	@param  unsigned char* pData   ��չͷ��ʼλ�õ�ַ
*	@param  unsigned int nLen   ����ʣ�೤��
*	@return ������չͷ����
*/
int CRtpPacket::ParseExtension( unsigned char* pData,unsigned int nLen )
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
	TPR_UINT16 definedByProfile = READ_BIG_ENDIAN_WORD(ptr);
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

	switch(definedByProfile)
	{
		case RTP_EXTENSION_ONE_BIT_ID:
			{
				ParseOneByteExtensionHeader(ptr, iXLen);
				break;
			}
		case RTP_EXTENSION_TWO_BIT_ID:
			{
				ParseTwoByteExtensionHeader(ptr, iXLen);
				break;
			}
		case RTP_EXTENSION_HIK_BASICSTREAMINFO:
			{
				ParseHikBasicstreaminfoExtensionHeader(ptr, iXLen);
				break;
			}
		case RTP_EXTENSION_HIK_ENCRYPT:
			{
				ParseHikEncryptExtensionHeader(ptr, iXLen);
				break;
			}
		default:
			break;
	}

	return 4 + iXLen;
}

/**	@fn CRtpPacket::ParseHikBasicstreaminfoExtensionHeader
*	@brief ����basic˽����Ϣ
*	@param  unsigned char* pData   ��չͷ���ݿ�ʼ��ַ��������profile �� len��
*	@param  unsigned int nLen   ��չͷ���ȣ�������profile �� len��
*	@return  ��ͷ�ļ������붨��
*/
int CRtpPacket::ParseHikBasicstreaminfoExtensionHeader(unsigned char* pData,unsigned int nLen)
{
	int iRet = 0;
	unsigned char* ptr = pData;

	if(!pData || nLen == 0)
	{
		return NPQERR_PARA;
	}

	m_struHeader.struExtension.hasHikBasicStream = TPR_TRUE;
	memset(&m_struHeader.struExtension.HikBasicStreaminfo,0,sizeof(ExtensionBasicStreamInfo));

	while(nLen>0)
	{
		switch (pData[0])
		{
		case 0x43:
			{
				iRet = ParseHikAudioDescriptor(pData, nLen);
			}
			break;
		default:
			{
				//��������������
				iRet = 2 + pData[1];
			}
			break;
		}

		if (iRet < 0)
		{
			return NPQERR_GENRAL;
		}

		pData+= iRet;
		nLen-= iRet;
	}
	return NPQ_OK;
}

int CRtpPacket::ParseHikAudioDescriptor(unsigned char* pData, unsigned int nLen)
{
	if (nLen < 2)
	{
		return -1;
	}

	TPR_INT32 dwDescriptorLen = 2 + pData[1];
	if (nLen < dwDescriptorLen)
	{
		return -1;
	}

	TPR_UINT32 nframe_len   = (pData[2] << 8) + pData[3];
	TPR_UINT32 naudio_num   = pData[4] & 0x01;
	TPR_UINT32 nsample_rate = (pData[5] << 14) + (pData[6] << 6) + (pData[7] >> 2);
	TPR_UINT32 nbit_rate    = (pData[8] << 14) + (pData[9] << 6) + (pData[10] >> 2);

	HIK_AUDIO_INFO* pAudioIfo =  &m_struHeader.struExtension.HikBasicStreaminfo.audioInfo;

	/* ý��ͷ��1��ʾ����������˽����������0��ʾ������������1���ڴ��ģ���ж��Ƿ���Чֵ*/
	pAudioIfo->bAudio = TPR_TRUE;
	pAudioIfo->uAudioNum = naudio_num + 1;
	pAudioIfo->uSampleRate = nsample_rate;
	pAudioIfo->uBitRate = nbit_rate;
	pAudioIfo->uFrameLen = nframe_len;
	pAudioIfo->uBitsPerSample = 16;
	return dwDescriptorLen;
}

/**	@fn CRtpPacket::ParseHikEncryptExtensionHeader
*	@brief ��������������
*	@param  unsigned char* pData   ��չͷ���ݿ�ʼ��ַ��������profile �� len��
*	@param  unsigned int nLen   ��չͷ���ȣ�������profile �� len��
*	@return  ��ͷ�ļ������붨��
*/
int CRtpPacket::ParseHikEncryptExtensionHeader(unsigned char* pData,unsigned int nLen)
{
	m_struHeader.struExtension.hasHikEncypt = TPR_TRUE;
	//������Ϣ���� TBD
	return NPQ_OK;
}

/**	@fn CRtpPacket::ParseOneByteExtensionHeader
*	@brief ������׼һ�ֽ���չͷ���ݣ�RFC 8285 ��
*	@param  unsigned char* pData   ��չͷ���ݿ�ʼ��ַ��������profile �� len��
*	@param  unsigned int nLen   ��չͷ���ȣ�������profile �� len��
*	@return  ��ͷ�ļ������붨��
*/
int CRtpPacket::ParseOneByteExtensionHeader(unsigned char* pData,unsigned int nLen)
{
	unsigned char* ptr = pData;

	if(!pData || nLen == 0)
	{
		return NPQERR_PARA;
	}

	//  0
	//  0 1 2 3 4 5 6 7
	// +-+-+-+-+-+-+-+-+
	// |  ID   |  len  |
	// +-+-+-+-+-+-+-+-+
	
	while(ptr < pData+nLen)
	{
		const TPR_UINT8 id =  (*ptr & 0xf0) >> 4;
		const TPR_UINT8 len = (*ptr & 0x0f);
		ptr++;

		switch (id)
		{
			case kRtpExtensionTransportSequenceNumberId:
				{
					//   0                   1                   2
					//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
					//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
					//  |  ID   | L=1   |transport wide sequence number |
					//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

					TPR_UINT16 sSeqNum = READ_BIG_ENDIAN_WORD(ptr);

					m_struHeader.struExtension.transportSequenceNumber = sSeqNum;
					m_struHeader.struExtension.hasTransportSequenceNumber = 1;
				}
				break;
			default:
				break;
		}

		ptr+=(len+1); //len+1
		
		//padding
		while(*ptr == 0)
		{
			ptr++;
		}
	}

	return NPQ_OK;
}

/**	@fn CRtpPacket::ParseOneByteExtensionHeader
*	@brief ������׼���ֽ���չͷ���ݣ�RFC 8285 ��
*	@param  unsigned char* pData   ��չͷ���ݿ�ʼ��ַ��������profile �� len��
*	@param  unsigned int nLen   ��չͷ���ȣ�������profile �� len��
*	@return  ��ͷ�ļ������붨��
*/
int CRtpPacket::ParseTwoByteExtensionHeader( unsigned char* pData,unsigned int nLen )
{
	//�ο���RTPͷ��չ��ʽ.docx��
	unsigned char* ptr = pData;

	if(!pData || nLen == 0)
	{
		return NPQERR_PARA;
	}

	while(ptr < pData+nLen)
	{
		const TPR_UINT8 id =  *ptr++;
		const TPR_UINT8 len = *ptr++;

		switch(id)
		{
		case kRtpExtensionTransportSequenceNumberId:
			{
				TPR_UINT16 sSeqNum = READ_BIG_ENDIAN_WORD(ptr);
				m_struHeader.struExtension.transportSequenceNumber = sSeqNum;
				m_struHeader.struExtension.hasTransportSequenceNumber = 1;
			}
			break;
		case kRtpExtensionHikPrivateId:
			{
				//�ݲ�������������TBD
				break;
			}
			break;
		default:
			break;
		}

		ptr+=(len); //len

		//padding
		while(*ptr == 0)
		{
			ptr++;
		}
	}

	return NPQ_OK;
}

/**	@fn CRtpPacket::ParsePadding
*	@brief ����padding�ֶ�
*	@param  unsigned char* pData   RTP��ʼλ��
*	@param  unsigned int nLen   RTP����
*	@return ����padding����
*/
int CRtpPacket::ParsePadding( unsigned char* pData,unsigned int nLen )
{
	/*
	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|ver  |R|  res  |         other                 |       length
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


	ver��padding�����汾
	R�� �ش���־��1��ʾ�ش�����0��ʾ���ش���
	res������
	other�� �ϳ�������ʹ���ֶ�
	length�� padding����
	*/
	TPR_UINT8* pPaddingRetran=NULL;

	if (!pData || nLen<MIN_RTP_LEN) 
	{
		NPQ_ERROR("nLen err %d",nLen);
		return 0;
	}

	if(pData[nLen-1]> RTP_MAX_PADDING_LEN || pData[nLen-1] == 0)
	{
		NPQ_ERROR("m_struHeader.paddingInfo.iPaddingSize err %d",pData[nLen-1]);
		return 0;
	}
	m_struHeader.paddingInfo.iPaddingSize = pData[nLen-1];
	
	if(m_struHeader.paddingInfo.iPaddingSize < RTP_RETRRANS_PADDING_LEN)
	{
		m_struHeader.paddingInfo.iVer = 0;
		m_struHeader.paddingInfo.bRetrans = TPR_FALSE;

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
		m_struHeader.paddingInfo.bRetrans = ((pPaddingRetran[0] & 0x10) == 0 ? TPR_FALSE : TPR_TRUE);
		if(m_struHeader.paddingInfo.bRetrans)
		{
			//NPQ_DEBUG("it is a retrans packet!!!!!!!!!!!! %d",m_struHeader.sSeqNo);
		}
	}
	
	return m_struHeader.paddingInfo.iPaddingSize;
}

/**	@fn CRtpPacket::ConfirmType
*	@brief ����playloadȷ����������
*	@param  TPR_UINT8 byPayloadType  PTֵ
*	@return ������������
*/
RTP_DATA_TYPE CRtpPacket::ConfirmType( TPR_UINT8 byPayloadType )
{
	if(byPayloadType == 0x60
		|| byPayloadType == 0x1A)
	{
		return RTP_VIDEO;
	}
	else if(byPayloadType == 0x70)
	{
		return RTP_PRIVATE;
	}
	else
	{
		return RTP_AUDIO;
	}
}

void CRtpPacket::ConfirmAudioType( TPR_UINT8 byPayloadType )
{
	switch(byPayloadType)
	{
	case 0x00:
		{
			m_unioCodecInfo.struAudio.enAudioType = NPQ_G711U;
		}
		break;
	case 0x08:
		{
			m_unioCodecInfo.struAudio.enAudioType = NPQ_G711A;
		}
		break;
	case 0x0e:
		{
			m_unioCodecInfo.struAudio.enAudioType = NPQ_MPA;
		}
		break;
	case 0x62:
		{
			m_unioCodecInfo.struAudio.enAudioType = NPQ_G7221;
		}
		break;
	case 0x66:
		{
			m_unioCodecInfo.struAudio.enAudioType = NPQ_G726;
		}
		break;
	case 0x68:
		{
			m_unioCodecInfo.struAudio.enAudioType = NPQ_AAC;
		}
		break;
    case 0x64:
        {
            m_unioCodecInfo.struAudio.enAudioType = NPQ_AAC_LD;
        }
        break;
	case 0x73:
		{
			m_unioCodecInfo.struAudio.enAudioType = NPQ_OPUS;
		}
		break;
	case 0x0b:
		{
			m_unioCodecInfo.struAudio.enAudioType = NPQ_PCM;
		}	
		break;
	default:
		{
			m_unioCodecInfo.struAudio.enAudioType = NPQ_AUDIO_UNKNOW;
		}
		break;
	}
}

/**	@fn CRtpPacket::CovertType
*	@brief �����ڲ���������ȷ��QOS��������
*	@param  RTP_DATA_TYPE enRtpType  �ڲ���������
*	@return ����QOS��������
*/
NPQ_DATA_TYPE CRtpPacket::CovertType( RTP_DATA_TYPE enRtpType)
{
	switch (enRtpType)
	{
	case RTP_VIDEO:
		{
			return NPQ_DATA_RTP_VIDEO;
		}
	case RTP_AUDIO:
		{
			return NPQ_DATA_RTP_AUDIO;
		}
	case RTP_PRIVATE:
		{
			return NPQ_DATA_RTP_PRIVATE;
		}
	default:
		break;
	}
	
	NPQ_ERROR("CovertType err enRtpType=%d",enRtpType);
	return NPQ_DATA_MULITIPLEX;
}

/**	@fn CRtpPacket::IsFecPacket
*	@brief �Ƿ���FEC��
*	@return TPR_TRUE/TPR_FALSE
*/
TPR_BOOL CRtpPacket::IsFecPacket()
{
	//ͨ��PT�ж��Ƿ���FEC
	return 0;
}

/**	@fn CRtpPacket::IsRetransPacket
*	@brief �Ƿ����ش������
*	@return TPR_TRUE/TPR_FALSE
*/
TPR_BOOL CRtpPacket::IsRetransPacket()
{
	if(!m_struHeader.bPadding)
	{
		return TPR_FALSE;
	}

	return m_struHeader.paddingInfo.bRetrans;
}

TPR_BOOL CRtpPacket::IsFecRecoveryPacket()
{
	return m_bFecRecoveryPacket;
}

TPR_BOOL CRtpPacket::IsAudioRedPacket()
{
	return 0;
}

TPR_BOOL CRtpPacket::IsPaddingPacket()
{
	return m_bPaddingPacket;
}

void CRtpPacket::FreeBuffer()
{
	if(m_bNeedFree)
	{
		SAFE_ARR_DEL(m_pData);
	}
}

int CRtpPacket::ParseAvcCodecInfo( unsigned char* pData,unsigned int nLen,VIDEO_ENCODE_TYPE enVideoType)
{
	int iRet=0;
	TPR_UINT32 dwType = 0;

	if(!pData || nLen == 0)
	{
		return NPQERR_PARA;
	}

	switch(enVideoType)
	{
	case NPQ_H264:
		{
			if((pData[0] & 0x1f) == 0x1c)
			{
				dwType = pData[1] & 0x1f;
			}
			else
			{
				dwType = pData[0] & 0x1f;
			}

			//TBD intra refresh������I֡�޷�ʶ��
			m_unioCodecInfo.struVideo.bKeyFrame = (dwType==5 || dwType==7 || dwType==8);

			//sps
			if(dwType == 7)
			{

			}
			break;
		}
	case NPQ_H265:
		{
            dwType = (pData[0] & 0x7e) >> 1;
            if (dwType == 49)//FU-A��ʽ ����nalu����
            {
                dwType = pData[2] & 0x3f;
            }

            if (dwType == 33)//�����ֱ���
            {
 
            }

           //֡�����ж�
            m_unioCodecInfo.struVideo.bKeyFrame = (dwType == 19 || dwType == 32 || dwType == 33 || dwType == 34 || dwType == 39);//IDR��VPS��SPS��PPS SEI���ж�ΪI֡ 
			break;
		}
	default:
		break;
	}

	return NPQ_OK;
}

/**	@fn CRtpPacketizer::CRtpPacketizer
*	@brief ���캯��
*	@return 
*/
CRtpPacketizer::CRtpPacketizer()
:m_nLen(0)
{
	memset(m_pData,0,sizeof(m_pData));
}

/**	@fn CRtpPacketizer::CRtpPacketizer
*	@brief ��������
*	@return 
*/
CRtpPacketizer::~CRtpPacketizer()
{
}

/**	@fn CRtpPacketizer::Pack
*	@brief ���RTP��׼ͷ
*	@param  unsigned char* pData   �����ݵ�ַ
*	@param  unsigned int nLen	�����ݳ���
*	@param  RTPHeader& struRTPHeader  ���ͷ��Ϣ
*	@return �����������׵�ַ
*/
unsigned char* CRtpPacketizer::Pack( unsigned char* pData,unsigned int nLen, RTPHeader& struRTPHeader)
{
	if (!pData || nLen > MAX_RTP_LEN - MIN_RTP_LEN) 
	{
		NPQ_ERROR("len err %d",nLen);
		return NULL;
	}

	memset(m_pData,0,MAX_RTP_LEN);

	TPR_UINT8 v = 2;

	m_pData[0] |= (v<<6); //version
	m_pData[0] &= ~0x20;  //padding
	m_pData[0] &= ~0x10;  //extern
	m_pData[0] |= (0 & 0x0f);  //csrc
	
	m_pData[1] |= (struRTPHeader.bMarkerBit? 0x80 : 0);//markbit
	m_pData[1] |= (struRTPHeader.iPayloadType & 0x7f); //PT

	//big endian
	m_pData[2] = struRTPHeader.sSeqNo >>8;
	m_pData[3] = struRTPHeader.sSeqNo;

	//big endian nTimeStamp
	m_pData[4] = struRTPHeader.nTimestamp >>24;
	m_pData[5] = struRTPHeader.nTimestamp >>16;
	m_pData[6] = struRTPHeader.nTimestamp >>8;
	m_pData[7] = struRTPHeader.nTimestamp;
	
	//big endian nSsrc
	m_pData[8] = struRTPHeader.nSsrc >>24;
	m_pData[9] = struRTPHeader.nSsrc >>16;
	m_pData[10] = struRTPHeader.nSsrc >>8;
	m_pData[11] = struRTPHeader.nSsrc;

	//��padding TBD
	//����չͷ TBD
	memcpy(m_pData+RTP_HEAD_LEN,pData, nLen);
	return m_pData;
}

/**	@fn CRtpPadding::Padding
*	@brief ����RTP padding
*	@param  CRtpPacket& rtpPacket  ԭ��RTP��Ϣ
*	@param  PaddingInfo& paddingInfo  ��Ҫ���ӵ�padding��Ϣ
*	@param  unsigned char* pBuf  ������ݵ�ַ
*	@param  unsigned int nBufLen ������ݳ���
*	@return �������붨��
*/
int CRtpPadding::Padding( CRtpPacket& rtpPacket, PaddingInfo& paddingInfo, unsigned char* pBuf,unsigned int nBufLen )
{
	int iRawOffset = 0;

	if(!pBuf || nBufLen< rtpPacket.m_nLen + paddingInfo.iPaddingSize )
	{
		NPQ_ERROR("padding para err");
		return NPQERR_PARA;
	}

	iRawOffset = rtpPacket.m_pRawData - rtpPacket.m_pData;
	memcpy(pBuf,rtpPacket.m_pData,rtpPacket.m_nLen);

	if(!rtpPacket.m_struHeader.bPadding)
	{
		//����ͷ����־
		rtpPacket.m_struHeader.bPadding = TPR_TRUE;
		pBuf[0] |= 0x20;
		
		memcpy(pBuf+rtpPacket.m_nLen, paddingInfo.aPadding, paddingInfo.iPaddingSize);//��������
		memcpy(&rtpPacket.m_struHeader.paddingInfo,&paddingInfo,sizeof(PaddingInfo));//���½ṹ
		
		//�޸�ָ��ͳ���
		rtpPacket.m_pData = pBuf;
		rtpPacket.m_nLen +=  paddingInfo.iPaddingSize;
	}
	else
	{
		//���½ṹ
		rtpPacket.m_struHeader.paddingInfo.iVer = paddingInfo.iVer;
		rtpPacket.m_struHeader.paddingInfo.bRetrans = paddingInfo.bRetrans;
		rtpPacket.m_struHeader.paddingInfo.iPaddingSize += paddingInfo.iPaddingSize;
		
		//ԭ�г��ȸ�Ϊ0
		pBuf[rtpPacket.m_nLen-1] = 0;
		memcpy(pBuf+rtpPacket.m_nLen, paddingInfo.aPadding, paddingInfo.iPaddingSize);
	
		//�޸�ָ��ͳ���
		rtpPacket.m_pData = pBuf;
		rtpPacket.m_nLen +=  paddingInfo.iPaddingSize;
		
		//��д�µ�padding����
		pBuf[rtpPacket.m_nLen-1] = rtpPacket.m_struHeader.paddingInfo.iPaddingSize;
	}

	if(iRawOffset > 0)
	{
		rtpPacket.m_pRawData = rtpPacket.m_pData+iRawOffset;
	}
	return NPQ_OK;
}

/**	@fn CRetransPadding::CRetransPadding
*	@brief ���캯��
*	@return void
*/
CRetransPadding::CRetransPadding()
{
	memset(m_pData,0,sizeof(m_pData));
}

/**	@fn CRetransPadding::RetransPadding
*	@brief ���Ӵ��ش�����ǵ�padding
*	@param  CRtpPacket& rtpPacket  ԭ��RTP��Ϣ
*	@return �������붨��
*/
int CRetransPadding::RetransPadding( CRtpPacket& rtpPacket )
{
	PaddingInfo info;

#define FIXLEN 4
	memset(&info,0,sizeof(PaddingInfo));

	info.iVer = 0;
	info.bRetrans = TPR_TRUE;
	info.aPadding[0] |= 0x10;

	info.iPaddingSize = FIXLEN;
	info.aPadding[3] = FIXLEN;
	return Padding(rtpPacket,info,m_pData,sizeof(m_pData));
}


