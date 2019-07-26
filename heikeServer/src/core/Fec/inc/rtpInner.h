/**	@rtpInner.h
*	@note 111.
*	@brief rtp��ش���
*
*	@author		222
*	@date		2016/11/14
*
*	@note   ע���ⲿrtp.h �ļ��ĸ���
*
*	@warning 
*/
#ifndef _RTPINNER_H_
#define _RTPINNER_H_

#include "utilInner.h"

#ifndef RTP_HEAD_LEN
#define RTP_HEAD_LEN    (12)  //RTP��׼ͷ����
#define MIN_RTP_LEN    RTP_HEAD_LEN  //��СRTP����
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
 *	@brief   rtp padding��Ϣ
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

	HPR_INT32 iVer;								//padding�����汾��
	HPR_BOOL bRetrans;							//�Ƿ����ش�����־
	HPR_INT32 iPaddingSize;						//���һ���ֽڣ���ʾpadding���ȣ����������ֽ�
	HPR_UINT8 aPadding[RTP_MAX_PADDING_LEN];	//padding���ݣ��������һ���ֽڳ���
};

/**	@struct	 RTPHeaderExtensionIn 
 *	@brief   rtp��չͷ��Ϣ
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

	HPR_UINT16 nProfile;					//��չͷ��ʶ
	HPR_UINT16 nLength;						//��չͷ���ȣ���ʾ�ֶγ���4֮���ֵ, ������ǰ��4�ֽ�
	HPR_BOOL hasTransportSequenceNumber;	//�Ƿ��д�������
	HPR_UINT16 transportSequenceNumber;		//��������
};


/**	@struct	 RTPHeader 
 *	@brief   rtpͷ��Ϣ
 *	@note	 
 */
struct RTPHeaderIn 
{
	RTPHeaderIn();
	~RTPHeaderIn();

	HPR_BOOL bMarkerBit;			//֡�������
	HPR_UINT8 iPayloadType;			//payload����
	HPR_UINT16 sSeqNo;				//���
	HPR_UINT32 nTimestamp;			//ʱ���
	HPR_UINT32 nSsrc;				//ssrc
	HPR_BOOL bPadding;				//padding���
	HPR_BOOL bExtension;			//��չͷ���
	PaddingInfoIn paddingInfo;		//padding��Ϣ
	RTPHeaderExtensionIn struExtension;	//��չͷ��Ϣ
};


/**	@class	 CRtpPacketIn 
 *	@brief   RTP����Ϣ
 *	@note	 
 */
class CRtpPacketIn 
{
public:
	/*���캯��*/
	CRtpPacketIn();

	/*��������*/
	~CRtpPacketIn();

	/*����RTP����*/
	int Parse(unsigned char* pData,unsigned int nLen);
	
	/*�Ƿ���FEC��*/
	HPR_BOOL IsFecPacket();

	/*�Ƿ����ش���*/
	HPR_BOOL IsRetransPacket();

	RTPHeaderIn m_struHeader;	//rtpͷ��Ϣ
	unsigned char* m_pData;		//�����׵�ַ
	unsigned int m_nLen;		//RTP������
	unsigned char* m_pRawData;	//�������׵�ַ(����ʾֻȥ��RTPͷ������ݣ�H264δ�ָ���ʼ��)
	unsigned int m_nRawLen;		//�����ݳ���

private:
	/*����padding*/
	int ParsePadding(unsigned char* pData,unsigned int nLen);

	/*������չͷ*/
	int ParseExtension(unsigned char* pData,unsigned int nLen);
	
	/*����1�ֽ����͵���չͷ*/
	int ParseOneByteExtensionHeader(unsigned char* pData,unsigned int nLen);
};

#endif

