/**	@RtpAddExtension.cpp
*	@note 111.
*	@brief rtp��չͷ��ش���
*
*	@author		222
*	@date		2018/1/7
*
*	@note 
*
*	@warning 
*/

#include "Util.h"
#include "timeUtil.h"
#include "RtpAddExtension.h"
#define new(x)  new(x,NPQ_NEW_FLAG)

/**	@fn CRtpAddExtension::AddExtension
*	@brief ������չͷ
*	@param  CRtpPacket& rtpPacket  ԭ��RTP��Ϣ
*	@param  RTPHeaderExtension& stuExtenInfo ������չͷ��Ϣ
*	@param  unsigned char* pBuf  ������ݵ�ַ
*	@param  unsigned int nBufLen ������ݳ���
*	@return �������붨��
*/
int CRtpAddExtension::AddExtension( CRtpPacket& rtpPacket, const RTPHeaderExtension& stuExtenInfo, unsigned char* pBuf,unsigned int nBufLen )
{
    unsigned char aExtension[256]={0};
    unsigned char* pDsttr = NULL;
	RTPHeaderExtension stuNewExtenInfo = stuExtenInfo;
	int iNewExtenLen = 0;
	int iPayloadLen = 0;

    if(!pBuf || nBufLen< rtpPacket.m_nLen + stuExtenInfo.nLength+4)
    {
        NPQ_ERROR("AddExtension para err nBufLen=%d,rtpPacket.m_nLen=%d,stuExtenInfo.nLength=%d",nBufLen,rtpPacket.m_nLen,stuExtenInfo.nLength);
        return NPQERR_PARA;
    }

    if(!rtpPacket.m_struHeader.bExtension)
    {
		iPayloadLen = rtpPacket.m_nLen - RTP_HEAD_LEN;

		//��ȡ��չͷ��������
		iNewExtenLen = FormExtenStrSingle(aExtension,stuNewExtenInfo);
		if(iNewExtenLen == 0)
		{
			NPQ_ERROR("iExtenLen 0 err");
			return NPQERR_GENRAL;
		}
    }
    else
    {
		//���س���
		iPayloadLen =  rtpPacket.m_nLen - (rtpPacket.m_struHeader.struExtension.nLength + 4) - RTP_HEAD_LEN;
		
		NPQ_DEBUG("FormExtenStrMuti");
		//��ȡ��չͷ��������
		iNewExtenLen = FormExtenStrMuti(rtpPacket.m_pData+RTP_HEAD_LEN ,rtpPacket.m_struHeader.struExtension, aExtension, stuNewExtenInfo);
		if(iNewExtenLen == 0)
		{
			NPQ_ERROR("2 iExtenLen 0 err");
			return NPQERR_GENRAL;
		}
    }

	//����12�ֽ�ͷ
	pDsttr = pBuf;
	memcpy(pDsttr,rtpPacket.m_pData, RTP_HEAD_LEN);
	pDsttr+=RTP_HEAD_LEN;

	if(!rtpPacket.m_struHeader.bExtension)
	{
		//����ͷ����־
		rtpPacket.m_struHeader.bExtension = TPR_TRUE;
		pBuf[0] |= 0x10;
	}

	//������չͷ�ṹ��Ϣ
	rtpPacket.m_struHeader.struExtension = stuNewExtenInfo;

	if((unsigned int)iNewExtenLen>=256)
	{
		NPQ_ERROR("iExtenLen len %d err",iNewExtenLen);
		return NPQERR_GENRAL;
	}

	//������չͷ
	memcpy(pDsttr ,aExtension, (unsigned int)iNewExtenLen);
	pDsttr+= iNewExtenLen;
	
	//��������
	memcpy(pDsttr ,rtpPacket.m_pRawData, iPayloadLen);
	rtpPacket.m_pRawData = pDsttr;
	pDsttr+= iPayloadLen;
	
	//���µ�ַ�ͳ���
	rtpPacket.m_pData = pBuf;
	rtpPacket.m_nLen = (pDsttr-pBuf);

    return NPQ_OK;
}

/**	@fn CRtpAddExtension::FormExtenStr
*	@brief ������չͷ����
*	@param  unsigned char* aExtension �������չͷ���ݵ�ַ
*	@param  const RTPHeaderExtension& stuExtenInfo Ҫ��ӵ���չͷ��Ϣ��ֻ��
*	@return ��չͷ����
*/
int CRtpAddExtension::FormExtenStrSingle(unsigned char* aExtension,const RTPHeaderExtension& stuExtenInfo)
{
    if(!aExtension)
    {
        return 0;
    }

    if(stuExtenInfo.nLength%4)
    {
        NPQ_ERROR("exteninfo len err %d",stuExtenInfo.nLength);
        return 0;
    }

    unsigned char* pExtentr = aExtension;

    WriteBigEndianWord(pExtentr, stuExtenInfo.nProfile);
    pExtentr+=2;

    WriteBigEndianWord(pExtentr, stuExtenInfo.nLength>>2);
    pExtentr+=2;
	
    if(stuExtenInfo.nProfile == RTP_EXTENSION_ONE_BIT_ID)
    {
        if(stuExtenInfo.hasTransportSequenceNumber)
        {
            *pExtentr |= (kRtpExtensionTransportSequenceNumberId<<4);
            *pExtentr |= 1; //len-1

            pExtentr++;

            WriteBigEndianWord(pExtentr, stuExtenInfo.transportSequenceNumber);
            pExtentr+=2;

            pExtentr++;//padding;
        }
    }

    int iExtenLen = pExtentr - aExtension;
    if(iExtenLen != stuExtenInfo.nLength+4)
    {
        NPQ_ERROR("extension len err stuExtenInfo.nLength=%d,%d",stuExtenInfo.nLength, iExtenLen);
        return 0;
    }

    return iExtenLen;
}

/**	@fn CRtpAddExtension::FormExtenStrMuti
*	@brief ���ɸ�������չͷ����
*	@param  unsigned char* aOldExt  ԭ����չͷ���ݵ�ַ
*	@param  const RTPHeaderExtension& stuOldExtenIn ԭ����չͷ��Ϣ
*	@param  unsigned char* aNewExt  ����ɸ��Ϻ��������չͷ����
*	@param  RTPHeaderExtension& stuNewExtenInfo  ��Ϊ��������������չͷ��Ϣ����Ϊ����Ǵ���ɸ��Ϻ����չͷ��Ϣ
*	@return ����ɸ����͵���չͷ����
*/
int CRtpAddExtension::FormExtenStrMuti(const unsigned char* aOldExt,const RTPHeaderExtension& stuOldExtenInfo,unsigned char* aNewExt,RTPHeaderExtension& stuNewExtenInfo)
{
	//�ο���RTPͷ��չ��ʽ.docx��
	//MUTI ��ʽͳһ���two bit

	if(!aOldExt || !aNewExt)
	{
		return 0;
	}

	const RTPHeaderExtension struHeadNewTemp = stuNewExtenInfo;
	if(struHeadNewTemp.nLength%4)
	{
		NPQ_ERROR("1 exteninfo len err %d",struHeadNewTemp.nLength);
		return 0;
	}

	stuNewExtenInfo = stuOldExtenInfo;
	stuNewExtenInfo.nProfile = RTP_EXTENSION_TWO_BIT_ID;

	int nOldExtenLen = stuOldExtenInfo.nLength + 4;

	unsigned char* pExtentr = aNewExt;
	unsigned char* pLenPosition = NULL;

	WriteBigEndianWord(pExtentr, RTP_EXTENSION_TWO_BIT_ID);
	pExtentr+=2;
	
	//��ʱ��¼����λ�ã���������д
	pLenPosition = pExtentr;
	pExtentr+=2;
	
	//�µĴ������չͷ
	if(struHeadNewTemp.hasTransportSequenceNumber)
	{
		*pExtentr |= kRtpExtensionTransportSequenceNumberId;
		pExtentr++;

		*pExtentr |= 2; //len
		pExtentr++;

		WriteBigEndianWord(pExtentr, struHeadNewTemp.transportSequenceNumber);
		pExtentr+=2;
		
		//��鳤��
		int nNewAddExtenLen = pExtentr - aNewExt;
		if(nNewAddExtenLen != struHeadNewTemp.nLength+4)
		{
			NPQ_ERROR("muti extension len err stuExtenInfo.nLength=%d,%d",struHeadNewTemp.nLength, nNewAddExtenLen);
			return 0;
		}

		stuNewExtenInfo.hasTransportSequenceNumber = TPR_TRUE;
		stuNewExtenInfo.transportSequenceNumber = struHeadNewTemp.transportSequenceNumber;
	}
	
	//�����ϵ�˽��ͷ
	*pExtentr |= kRtpExtensionHikPrivateId;
	pExtentr++;
	*pExtentr |= nOldExtenLen; //len
	pExtentr++;
	memcpy(pExtentr, aOldExt, nOldExtenLen);
	pExtentr+= nOldExtenLen;

	//padding
	int iNewExtenLen = pExtentr - aNewExt;
	while(iNewExtenLen%4)
	{
		pExtentr++;
		iNewExtenLen++;
	}

	//��д�ܳ���
	WriteBigEndianWord(pLenPosition,(iNewExtenLen - 4)>>2);
	stuNewExtenInfo.nLength = iNewExtenLen - 4;

	return iNewExtenLen;
}


