

#include "rtcpParser.h"
#include "rtcpCallback.h"

RtcpParser::RtcpParser( RtcpCallback& rtcpCallback )
:m_rtcpCallback(rtcpCallback)
{

}

RtcpParser::~RtcpParser()
{

}

int RtcpParser::InputRTCP(unsigned char* pData, unsigned int uDataLen,unsigned char** ppOutData,unsigned int* pOutLen)
{
	int iRet;
	unsigned char* p = pData;

	if (!pData || uDataLen<4)
	{
		NPQ_ERROR("RTCP packet lenth %d", uDataLen);
		return NPQERR_PARA;
	}

	unsigned int iRtcpHdr = ntohl(*(unsigned int*)p);

	unsigned int rcFmt = (iRtcpHdr>>24)&0x1F;
	unsigned int pt = (iRtcpHdr>>16)&0xFF;
	unsigned int len = 4*((iRtcpHdr&0xFFFF) + 1);//表示整个RTCP包的长度

	if(pt!= RTCP_PT_SR 
		&& pt!= RTCP_PT_XR
		&& pt!= RTCP_PT_SDES
		&& pt!= RTCP_PT_RR
		&& pt!= RTCP_PT_FEEDBACK
		&& pt!= RTCP_PT_SPEC_FEEDBACK)
	{
		NPQ_ERROR("rtcp pt error %d length %d",pt, uDataLen);
		return NPQERR_I_RTCPFORMAT;
	}

	//要求版本是2、不能有padding
	if ((iRtcpHdr & 0xE0000000) != 0x80000000) 
	{
		NPQ_ERROR("iRtcpHdr %x length %d",iRtcpHdr, uDataLen);
		return NPQERR_I_RTCPFORMAT;
	}

	if (uDataLen < 8)
	{
		NPQ_ERROR("RTCP packet lenth %d", uDataLen);
		return NPQERR_PARA;
	}
	p+=4; //跳过hdr
	p+=4; //跳过ssrc

	switch(pt)
	{
	case RTCP_PT_SR:
		{
			RTCP_SR_INFO srInfo={0};
			//绝对时间
			srInfo.nLastSRNTPmsw = ntohl(*(unsigned int*)p);
			p+=4;
			srInfo.nLastSRNTPlsw = ntohl(*(unsigned int*)p);
			p+=4;
			//相对时间
			unsigned int rtpTimestamp = ntohl(*(unsigned int*)p);
			p+=4;
			//packet count
			p+=4;
			//octet count
			p+=4;
            srInfo.nLastRtpTimeStamp = rtpTimestamp;
			srInfo.nlastReceiveSRTime = TPR_TimeNow();

			m_rtcpCallback.TriggerRtcpCallback(INDEX_SR_INFO,&srInfo);
			break;
		}
	case RTCP_PT_RR:
		{	
			RTCP_RR_INFO rrInfo;
			memset(&rrInfo,0,sizeof(RTCP_RR_INFO));

			p+=ParseRR(p,&rrInfo);
			m_rtcpCallback.TriggerRtcpCallback(INDEX_RR_INFO,&rrInfo);
			break;
		}
	case RTCP_PT_FEEDBACK:
		{
			switch (rcFmt)
			{
			case RTCP_FB_FMI_NACK:
				{
					NPQ_NACK_INFO nackInfo={0};
					memset(&nackInfo,0,sizeof(NPQ_NACK_INFO));

					//去掉ssrc 和 hdr的8字节
					iRet = ParseNack(p, len-8 , &nackInfo);
					if(iRet == 0)
					{
						NPQ_ERROR("ParseNack err");
						return NPQERR_I_RTCPFORMAT;
					}
					p+=iRet;
					m_rtcpCallback.TriggerRtcpCallback(INDEX_FB_NACK_INFO, &nackInfo);
				}
				break;
			case RTCP_FB_FMI_TRANSPORT:
				{

                    p += (len - 8);
				}
				break;
			default:
				break;
			}
			break;
		}
	case RTCP_PT_SPEC_FEEDBACK:
		{
			switch (rcFmt)
			{
			case RTCP_SPEC_FB_FMI_PLI:
				{
					p+=4;//跳过 media ssrc
					m_rtcpCallback.TriggerRtcpCallback(INDEX_SPEC_FB_PLI_INFO,NULL);
				}
				break;
			case RTCP_SPEC_FB_FMI_FIR:
				{
					p+=4;//跳过 media ssrc
					p+=8;//跳过 FCI
					m_rtcpCallback.TriggerRtcpCallback(INDEX_SPEC_FB_FIR_INFO,NULL);
				}
				break;
			default:
				break;
			}
			break;
		}
	case RTCP_PT_SDES:
		{	
			p+=32;//TBD
			break;
		}
	case RTCP_PT_XR:
		{
			//NPQ_INFO("receive RTCP XR");
			unsigned int nFir = 0;
			nFir  = ntohl(*(unsigned int*)p);
			p+=4;//nFir

			//p+=4; //ssrc

			unsigned char enXRBT = (nFir>>24)&0xFF;
			unsigned int lenXR = 4*(nFir&0xFFFF);
			if(enXRBT == RTCP_XR_BT_DLRR)
			{
				RTCP_XR_DLRR_INFO xrInfo;

				p+=4; //ssrc
				xrInfo.nLrr = ntohl(*(unsigned int*)p);
				p+=4;
				xrInfo.nDlrr = ntohl(*(unsigned int*)p);
				p+=4;

				m_rtcpCallback.TriggerRtcpCallback(INDEX_XR_DLRR_INFO,&xrInfo);
			}
			else if(enXRBT == RTCP_XR_BT_REFERENCETIME)
			{
				RTCP_XR_REFERENCETIME_INFO xrInfo;
				//绝对时间
				xrInfo.nLastXRNTPmsw = ntohl(*(unsigned int*)p);
				p+=4;
				xrInfo.nLastXRNTPlsw = ntohl(*(unsigned int*)p);
				p+=4;
				xrInfo.nlastReceiveXRTime = TPR_TimeNow();

				m_rtcpCallback.TriggerRtcpCallback(INDEX_XR_REFERENCETIME_INFO,&xrInfo);
			}

			break;
		}
	default:
		break;
	}

	if(p-pData <= uDataLen)
	{
		uDataLen-= (p-pData);
	}
	else
	{
		uDataLen = 0;
	}

	if(ppOutData && pOutLen)
	{
		*ppOutData = p;
		*pOutLen = uDataLen;
		//NPQ_DEBUG("uDataLen %d", uDataLen);
	}

	return NPQ_OK;
}

int RtcpParser::ParseNack( unsigned char* pData,unsigned int uLen,NPQ_NACK_INFO* pInfo)
{
	if(!pInfo)
	{
		return 0;
	}
	unsigned char* p = pData;
	unsigned int len = uLen;

	//发送端解析丢包信息
	NPQ_NACK_INFO stNackInfo;
	memset(&stNackInfo, 0, sizeof(stNackInfo));
	//跳过ssrc
	p+=4;
	len-=4;
	
	if(len %4)
	{
		NPQ_ERROR("len err %d",len);
		return 0;
	}

	int nNackNum = 0;

	while(p - pData < uLen)
	{
		int nPID = (p[0] << 8) + p[1];
		if (0 > nPID)
		{
			NPQ_ERROR("PID parse error!");
			return 0;
		}
		else
		{
			stNackInfo.iNackNumber++;
			stNackInfo.aNackList[nNackNum++] = nPID;
		}
		p+=2;
		int nbitMask = (p[0] << 8) + p[1];

		if (nbitMask) 
		{
			for (int i=1; i <= 16; ++i) 
			{
				if (nbitMask & 0x01) 
				{
					stNackInfo.iNackNumber++;
					stNackInfo.aNackList[nNackNum++] = nPID + i;
				}
				nbitMask = nbitMask >>1;
			}
		}
		p+=2;
	}

	memcpy(pInfo,&stNackInfo,sizeof(NPQ_NACK_INFO));
	return p - pData;
}

int RtcpParser::ParseRR( unsigned char* pData,RTCP_RR_INFO* pInfo)
{
	if(!pInfo)
	{
		return 0;
	}
	unsigned char* p = pData;

	RTCP_RR_INFO stRRInfo;
	memset(&stRRInfo, 0, sizeof(stRRInfo));

	//跳过ssrc
	p+=4;

	//packet lost
	unsigned int tmp = ntohl(*(unsigned int*)p);
	stRRInfo.cLossFraction = tmp >> 24;
	p+=4;
	
	//sequence number
	tmp = READ_BIG_ENDIAN_DWORD(p);
	stRRInfo.nMaxSeqNumber =  tmp;
	p+=4;
	p+=4;//jitter
	//获取LSR
	stRRInfo.nLsr = ntohl(*(unsigned int*)p);
	p+=4;
	//获取DLSR
	stRRInfo.nDlsr = ntohl(*(unsigned int*)p);
	p+=4;

	memcpy(pInfo,&stRRInfo,sizeof(RTCP_RR_INFO));
	return p - pData;
}

