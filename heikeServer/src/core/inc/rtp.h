/**	@rtp.h
*	@note 111.
*	@brief rtp��ش���
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


#define RTP_HEAD_LEN    (12)  //RTP��׼ͷ����
#define MIN_RTP_LEN    RTP_HEAD_LEN  //��СRTP����
#define MAX_RTP_LEN    (1500) 
#define RTP_MAX_PADDING_LEN    (8)
#define RTP_RETRRANS_PADDING_LEN    (4)


#define RTP_EXTENSION_ONE_BIT_ID    (0xBEDE)
#define RTP_EXTENSION_TWO_BIT_ID    (0x1000)
#define RTP_EXTENSION_HIK_BASICSTREAMINFO       (0x0001)	//˽��������Ϣ����
#define RTP_EXTENSION_HIK_ENCRYPT       (0x4000)			//����������Ϣ����

enum RTPExtensionTypeId 
{
	kRtpExtensionTransportSequenceNumberId = 8,
	kRtpExtensionAbsTimeId = 7,
	kRtpExtensionHikPrivateId = 0xff,
};


/**	@struct	 PaddingInfo 
 *	@brief   rtp padding��Ϣ
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

	TPR_INT32 iVer;								//padding�����汾��
	TPR_BOOL bRetrans;							//�Ƿ����ش�����־
	TPR_INT32 iPaddingSize;						//���һ���ֽڣ���ʾpadding���ȣ����������ֽ�
	TPR_UINT8 aPadding[RTP_MAX_PADDING_LEN];	//padding���ݣ��������һ���ֽڳ���
};

/**	@struct	 HIK_AUDIO_INFO 
 *	@brief   �����BasicStream��չͷ�е���Ƶ��������Ϣ
 *	@note	 
 */
struct HIK_AUDIO_INFO
{
	TPR_BOOL     bAudio;
	TPR_UINT32   uFrameLen;			//��Ƶ֡����
	TPR_UINT32   uAudioNum;			//��Ƶ������ 0 ��������1 ˫����
	TPR_UINT32   uSampleRate;		//��Ƶ������
	TPR_UINT32   uBitRate;			//��Ƶ����  bps
	TPR_UINT32   uBitsPerSample;    //��λ
};

/**	@struct	 ExtensionBasicStreamInfo 
 *	@brief   �����BasicStream��չͷ��Ϣ
 *	@note	 
 */
struct ExtensionBasicStreamInfo
{
	HIK_AUDIO_INFO audioInfo;
};

/**	@struct	 RTPHeaderExtension 
 *	@brief   rtp��չͷ��Ϣ
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

	TPR_UINT16 nProfile;					//��չͷ��ʶ
	TPR_UINT16 nLength;						//��չͷ���ȣ���ʾ�ֶγ���4֮���ֵ, ������ǰ��4�ֽ�
	TPR_BOOL hasTransportSequenceNumber;	//�Ƿ��д�������
	TPR_UINT16 transportSequenceNumber;		//��������
	TPR_UINT8  byRes[2];
	TPR_BOOL hasHikBasicStream;				//�Ƿ���stream��Ϣ
	ExtensionBasicStreamInfo HikBasicStreaminfo; //�����BasicStream��չͷ��Ϣ
	TPR_BOOL hasHikEncypt;					//�Ƿ��������������
};

struct  VideoCodecInfo
{
	VIDEO_ENCODE_TYPE enVideoType;		//��Ƶ����
	TPR_BOOL bKeyFrame;					//��ƵI֡��־����Ƶ����
	unsigned int uWidth;				//��(ֻ��sps�и���Ϣ)
	unsigned int uHeight;				//��(ֻ��sps�и���Ϣ)
};

struct  AudioCodecInfo
{
	AUDIO_ENCODE_TYPE enAudioType;		//��Ƶ���ͣ���Ƶ����
	TPR_UINT32 nAuidoBitRate;			//��Ƶ�������ʣ���Ƶ����
};

union CodecInfo
{
	VideoCodecInfo struVideo;
	AudioCodecInfo struAudio;
};

/**	@struct	 RTPHeader 
 *	@brief   rtpͷ��Ϣ
 *	@note	 
 */
struct RTPHeader 
{
	RTPHeader();
	~RTPHeader();

	TPR_BOOL bMarkerBit;			//֡�������
	TPR_UINT8 iPayloadType;			//payload����
	TPR_UINT16 sSeqNo;				//���
	TPR_UINT32 nTimestamp;			//ʱ���
	TPR_UINT32 nSsrc;				//ssrc
	TPR_BOOL bPadding;				//padding���
	TPR_INT32 iPayLoadFrequence;	//ʱ��Ƶ��
	TPR_BOOL bExtension;			//��չͷ���
	PaddingInfo paddingInfo;		//padding��Ϣ
	RTPHeaderExtension struExtension;	//��չͷ��Ϣ
};

/**	@class	 CRtpPacketizer 
 *	@brief   �����ݴ��RTP��
 *	@note	 
 */
class CRtpPacketizer
{
public:
	/*���캯��*/
	CRtpPacketizer();
	/*��������*/
	virtual ~CRtpPacketizer();
	/*���RTP*/
	virtual unsigned char* Pack( unsigned char* pData,unsigned int nLen,RTPHeader& struRTPHeader);
	
private:
	unsigned char m_pData[MAX_RTP_LEN];		 //���ݻ����ַ
	unsigned int m_nLen;					 //���ݳ���
};


/**	@class	 CRtpPacket 
 *	@brief   RTP����Ϣ
 *	@note	 
 */
class CRtpPacket
{
public:
	/*���캯��*/
	CRtpPacket();

	/*��������*/
	~CRtpPacket();

	/*����RTP����*/
	int Parse(unsigned char* pData,unsigned int nLen,SdpInfo* pSdp);
	
	/*����playloadȷ����������*/
	RTP_DATA_TYPE ConfirmType(TPR_UINT8 byPayloadType);

	/*����playloadȷ����������*/
	void ConfirmAudioType(TPR_UINT8 byPayloadType);

	/*�����ڲ���������ȷ��NPQ��������*/
	NPQ_DATA_TYPE CovertType(RTP_DATA_TYPE enRtpType);
	
	/*�Ƿ���FEC��*/
	TPR_BOOL IsFecPacket();

	/*�Ƿ����ش���*/
	TPR_BOOL IsRetransPacket();

	/*�Ƿ���Fec�ָ��İ�*/
	TPR_BOOL IsFecRecoveryPacket();

	/*�Ƿ�����ƵRed��*/
	TPR_BOOL IsAudioRedPacket();

	/*�Ƿ���Padding������ע:Padding����Ȼ���ش���*/
	TPR_BOOL IsPaddingPacket();
	
	/*��������Դ*/
	void FreeBuffer();

	RTPHeader m_struHeader;		//rtpͷ��Ϣ
	CodecInfo m_unioCodecInfo;	//�������Ϣ
	unsigned char* m_pData;		//�����׵�ַ
	unsigned int m_nLen;		//RTP������
	unsigned char* m_pRawData;	//�������׵�ַ(����ʾֻȥ��RTPͷ������ݣ�H264δ�ָ���ʼ��,aacҲδ����)
	unsigned int m_nRawLen;		//�����ݳ���

	RTP_DATA_TYPE m_enType;		//�������ͣ���Ƶ/��Ƶ/˽������
	TPR_TIME_T m_nArriveTimeUs;	//���ݰ��ﵽʱ�� ��λus
	TPR_BOOL m_bNeedFree;		//��Ҫ�����ͷ�m_pData�ڴ���
	TPR_BOOL m_bHaveCallback;	//�Ƿ��ѻص���־
	TPR_BOOL m_bPaddingPacket;	//�Ƿ���padding������ֻ���ڷ��Ͷˣ����ն˸�ֵΪTPR_FLASE��padding����ȻҲ���ش�����
	TPR_BOOL m_bFecRecoveryPacket; //FEC�ָ�����ǣ����նˣ�

private:
	/*����padding*/
	int ParsePadding(unsigned char* pData,unsigned int nLen);

	/*������չͷ*/
	int ParseExtension(unsigned char* pData,unsigned int nLen);

	int ParseAvcCodecInfo(unsigned char* pData,unsigned int nLen,VIDEO_ENCODE_TYPE enVideoType);
	
	/*����1�ֽ����͵���չͷ*/
	int ParseOneByteExtensionHeader(unsigned char* pData,unsigned int nLen);
	
	/*����2�ֽ����͵���չͷ*/
	int ParseTwoByteExtensionHeader(unsigned char* pData,unsigned int nLen);

	int ParseHikBasicstreaminfoExtensionHeader(unsigned char* pData,unsigned int nLen);

	int ParseHikAudioDescriptor(unsigned char* pData, unsigned int nLen);

	int ParseHikEncryptExtensionHeader(unsigned char* pData,unsigned int nLen);
};

typedef list<CRtpPacket> CRtpPackets;

/**	@class	 CRtpAddPadding 
 *	@brief   ԭ����������padding
 *	@note	 
 */
class CRtpPadding
{
public:
	/*���캯��*/
	CRtpPadding(){}

	/*��������*/
	virtual ~CRtpPadding(){}
	
	/*����padding*/
	virtual int Padding(CRtpPacket& rtpPacket, PaddingInfo& paddingInfo, unsigned char* pBuf,unsigned int nBufLen);
};


/**	@class	 CRetransPadding 
 *	@brief   �ش�������padding����
 *	@note	 �̳�CRtpPadding
 */
class CRetransPadding :private CRtpPadding
{
public:
	/*���캯��*/
	CRetransPadding();

	/*��������*/
	~CRetransPadding(){}

	/*���ش��������padding*/
	virtual int RetransPadding(CRtpPacket& rtpPacket);

private:
	unsigned char m_pData[MAX_RTP_LEN+12];	 //���ݻ����ַ
};


#endif

