/**	@RtpAddExtension.cpp
*	@note 111.
*	@brief rtp扩展头相关代码
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
*	@brief 增加扩展头
*	@param  CRtpPacket& rtpPacket  原有RTP信息
*	@param  RTPHeaderExtension& stuExtenInfo 新增扩展头信息
*	@param  unsigned char* pBuf  输出数据地址
*	@param  unsigned int nBufLen 输出数据长度
*	@return 见错误码定义
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

		//获取扩展头二进制流
		iNewExtenLen = FormExtenStrSingle(aExtension,stuNewExtenInfo);
		if(iNewExtenLen == 0)
		{
			NPQ_ERROR("iExtenLen 0 err");
			return NPQERR_GENRAL;
		}
    }
    else
    {
		//负载长度
		iPayloadLen =  rtpPacket.m_nLen - (rtpPacket.m_struHeader.struExtension.nLength + 4) - RTP_HEAD_LEN;
		
		NPQ_DEBUG("FormExtenStrMuti");
		//获取扩展头二进制流
		iNewExtenLen = FormExtenStrMuti(rtpPacket.m_pData+RTP_HEAD_LEN ,rtpPacket.m_struHeader.struExtension, aExtension, stuNewExtenInfo);
		if(iNewExtenLen == 0)
		{
			NPQ_ERROR("2 iExtenLen 0 err");
			return NPQERR_GENRAL;
		}
    }

	//拷贝12字节头
	pDsttr = pBuf;
	memcpy(pDsttr,rtpPacket.m_pData, RTP_HEAD_LEN);
	pDsttr+=RTP_HEAD_LEN;

	if(!rtpPacket.m_struHeader.bExtension)
	{
		//更新头部标志
		rtpPacket.m_struHeader.bExtension = TPR_TRUE;
		pBuf[0] |= 0x10;
	}

	//更新扩展头结构信息
	rtpPacket.m_struHeader.struExtension = stuNewExtenInfo;

	if((unsigned int)iNewExtenLen>=256)
	{
		NPQ_ERROR("iExtenLen len %d err",iNewExtenLen);
		return NPQERR_GENRAL;
	}

	//拷贝扩展头
	memcpy(pDsttr ,aExtension, (unsigned int)iNewExtenLen);
	pDsttr+= iNewExtenLen;
	
	//拷贝负载
	memcpy(pDsttr ,rtpPacket.m_pRawData, iPayloadLen);
	rtpPacket.m_pRawData = pDsttr;
	pDsttr+= iPayloadLen;
	
	//更新地址和长度
	rtpPacket.m_pData = pBuf;
	rtpPacket.m_nLen = (pDsttr-pBuf);

    return NPQ_OK;
}

/**	@fn CRtpAddExtension::FormExtenStr
*	@brief 生成扩展头内容
*	@param  unsigned char* aExtension 输出的扩展头内容地址
*	@param  const RTPHeaderExtension& stuExtenInfo 要添加的扩展头信息，只读
*	@return 扩展头长度
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
*	@brief 生成复合型扩展头内容
*	@param  unsigned char* aOldExt  原有扩展头内容地址
*	@param  const RTPHeaderExtension& stuOldExtenIn 原有扩展头信息
*	@param  unsigned char* aNewExt  打包成复合后输出的扩展头内容
*	@param  RTPHeaderExtension& stuNewExtenInfo  作为输入是新增的扩展头信息，作为输出是打包成复合后的扩展头信息
*	@return 打包成复合型的扩展头长度
*/
int CRtpAddExtension::FormExtenStrMuti(const unsigned char* aOldExt,const RTPHeaderExtension& stuOldExtenInfo,unsigned char* aNewExt,RTPHeaderExtension& stuNewExtenInfo)
{
	//参考《RTP头扩展格式.docx》
	//MUTI 方式统一打成two bit

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
	
	//暂时记录长度位置，后面再填写
	pLenPosition = pExtentr;
	pExtentr+=2;
	
	//新的传输层扩展头
	if(struHeadNewTemp.hasTransportSequenceNumber)
	{
		*pExtentr |= kRtpExtensionTransportSequenceNumberId;
		pExtentr++;

		*pExtentr |= 2; //len
		pExtentr++;

		WriteBigEndianWord(pExtentr, struHeadNewTemp.transportSequenceNumber);
		pExtentr+=2;
		
		//检查长度
		int nNewAddExtenLen = pExtentr - aNewExt;
		if(nNewAddExtenLen != struHeadNewTemp.nLength+4)
		{
			NPQ_ERROR("muti extension len err stuExtenInfo.nLength=%d,%d",struHeadNewTemp.nLength, nNewAddExtenLen);
			return 0;
		}

		stuNewExtenInfo.hasTransportSequenceNumber = TPR_TRUE;
		stuNewExtenInfo.transportSequenceNumber = struHeadNewTemp.transportSequenceNumber;
	}
	
	//加上老的私有头
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

	//填写总长度
	WriteBigEndianWord(pLenPosition,(iNewExtenLen - 4)>>2);
	stuNewExtenInfo.nLength = iNewExtenLen - 4;

	return iNewExtenLen;
}


