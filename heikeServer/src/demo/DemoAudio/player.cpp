
#include <vector>
#include "player.h"

using std::vector;

Player::Player(Config& config)
:m_iPlayPort(-1)
,m_iDataLen(0)
,m_iBufferPacketCount(0)
,m_buf(NULL)
,m_config(config)
,m_bFirst(TPR_TRUE)
{

}

Player::~Player()
{
	if(m_buf)
	{
		delete []m_buf;
	}
}

int Player::Open()
{	
#if 0
	m_buf = new unsigned char[4*1500];
	if(!m_buf)
	{
		return -1;
	}

	int iRet;
	char  cPlayDevName[256]={0};///<设备名称
	
	AudioParam stAudioParamQos;
	AudioParam stAudioParamNoQos;
	
	memcpy(&stAudioParamQos,&m_config.m_stAudioParam,sizeof(AudioParam));
	stAudioParamQos.nBitRate = stAudioParamQos.nSampleRate*2*8;
	stAudioParamQos.enAudioEncodeTypeEx = AUDIO_TYPE_PCM;

	memcpy(&stAudioParamNoQos,&m_config.m_stAudioParam,sizeof(AudioParam));

	unsigned int dwDeviceNum = 0;
	iRet = AUDIOCOM_GetSoundCardNum(&dwDeviceNum);

	printf("dwDeviceNum = %d\n",dwDeviceNum);

	AUDIOCOM_GetPlayDeviceNum(&dwDeviceNum);

	SoundCardInfo info={0};

	//printf("dwDeviceNum = %d\n",dwDeviceNum);
	for (unsigned int i = 0; i < dwDeviceNum; i++)
	{
		AUDIOCOM_GetOneSoundCardInfo(i,&info);
		//memset(cPlayDevName,0,sizeof(cPlayDevName));
		//AUDIOCOM_GetOnePlayDeviceName(i, cPlayDevName);
		printf("%s\n",info.byDeviceName);
		break;
	}

	AUDIOCOM_GetOnePlayDeviceName(0, cPlayDevName);
	iRet = AUDIOCOM_CreatePlayHandleEx(&m_iPlayPort, (const char*)cPlayDevName);
	if(iRet != 1)
	{
		printf("AUDIOCOM_CreatePlayHandleEx failed iRet=%d\n",iRet);
	}
	
	if(m_config.m_bUseQos)
	{
		iRet = AUDIOCOM_OpenStreamEx(m_iPlayPort, &stAudioParamQos);
		if(iRet != 1)
		{
			printf("AUDIOCOM_OpenStreamEx failed iRet=%d\n",iRet);
		}
	}
	else
	{
		iRet = AUDIOCOM_OpenStreamEx(m_iPlayPort, &stAudioParamNoQos);
		if(iRet != 1)
		{
			printf("AUDIOCOM_OpenStreamEx failed iRet=%d\n",iRet);
		}
	}

	//iRet = AUDIOCOM_RegisterDecodeDataCallBackEx(m_iPlayPort, g_OutputDataCallBackPrintEx, NULL);
	//if(iRet != 1)
	//{
	//	printf("AUDIOCOM_RegisterDecodeDataCallBackEx failed\n");
	//}

	iRet = AUDIOCOM_StartPlay(m_iPlayPort);
	if(iRet != 1)
	{
		printf("AUDIOCOM_StartPlay failed iRet=%d\n",iRet);
	}
	
	if(m_config.m_bUseQos)
	{
		HIK_MEDIAINFO stInfo;
		memset(&stInfo, 0, sizeof(HIK_MEDIAINFO));

		stInfo.media_fourcc = 0x484B4D49;
		stInfo.media_version = 0x0101;
		stInfo.system_format = 0x0004;
		stInfo.video_format = 0;
		stInfo.audio_format = 0x7001;//AUDIO_RAW_DATA16;
		stInfo.audio_channels = stAudioParamQos.nChannel;
		stInfo.audio_bits_per_sample = stAudioParamQos.nBitWidth;
		stInfo.audio_samplesrate = stAudioParamQos.nSampleRate;
		stInfo.audio_bitrate = stAudioParamQos.nBitRate;

		int len = 40;
		memcpy(m_buf,&len,4);
		memcpy(m_buf+4,&stInfo,sizeof(HIK_MEDIAINFO));

		iRet = AUDIOCOM_InputStreamData(m_iPlayPort, TPR_TRUE,m_buf, 44);
		if(iRet!=1)
		{
			printf("AUDIOCOM_InputStreamData err iRet=%d\n",AUDIOCOM_GetLastError(m_iPlayPort));
		}
	}
#endif

	return 0;
}

int Player::Close()
{
#if 0
	if(m_iPlayPort == -1)
	{
		return -1;
	}
	AUDIOCOM_StopPlay(m_iPlayPort);
	AUDIOCOM_ReleasePlayHandle(m_iPlayPort);
	m_iPlayPort = -1;
#endif

	return 0;
}


//FILE* g_filePcmRtp = fopen("receiver_rtp_pcm","wb+");
int g_kBufferTime = 30;

int Player::InputData( unsigned char* pData, int iDataLen )
{
#if 0
	int iRet;
	static TPR_UINT64 lllast = 0, llnow = 0,llDiff = 0;
	
	//printf("iDataLen=%d\n",iDataLen);
	if(m_iPlayPort != -1)
	{
		//fwrite((unsigned char*)pData,1,iDataLen,g_filePcmRtp);

		memcpy(m_buf,&iDataLen,4);
		memcpy(m_buf+4,pData,iDataLen);

		llnow = TPR_GetTimeTick64();
		llDiff = (TPR_UINT64)(llnow - lllast);
		//NPQ_DEMO("@@@@@ diff = %lld,iDataLen=%d",llDiff,iDataLen);
		lllast = llnow;

		if(llDiff>g_kBufferTime)
		{
			//NPQ_DEMO("warning!!! diff = %d\n",llDiff);
		}
		
		//第一次
		if(m_bFirst)
		{
			unsigned char zeroData[256]={0};
			memcpy(zeroData,&iDataLen,4); //长度
			memcpy(zeroData+4,pData,12); //RTP头
			
			for(int i=0;i<g_kBufferTime/10;i++)
			{
				iRet = AUDIOCOM_InputStreamData(m_iPlayPort,TPR_TRUE, zeroData, iDataLen+4);
				if(iRet!=1)
				{
					printf("AUDIOCOM_InputStreamData zero err iRet=%d\n",AUDIOCOM_GetLastError(m_iPlayPort));
				}
			}
			
			m_bFirst = TPR_FALSE;
		}

		iRet = AUDIOCOM_InputStreamData(m_iPlayPort,TPR_TRUE, m_buf, iDataLen+4);
		if(iRet!=1)
		{
			printf("AUDIOCOM_InputStreamData err iRet=%d\n",AUDIOCOM_GetLastError(m_iPlayPort));
		}
	}
#endif

	return 0;
}

int Player::ParseRtp( unsigned char* pData, int nLen ,unsigned char** ppRaw, int* pDataLen)
{
	int iRet;
	if (!pData || nLen<12 || !ppRaw || !pDataLen) 
	{
		printf("nLen err %d\n",nLen);
		return -1;
	}

	// 版本
	const TPR_UINT8 v  = pData[0] >> 6;
	// Padding
	const TPR_BOOL p  = ((pData[0] & 0x20) == 0) ? TPR_FALSE : TPR_TRUE;
	// 扩展字段
	const TPR_BOOL x = ((pData[0] & 0x10) == 0) ? TPR_FALSE : TPR_TRUE;
	const TPR_UINT8 c = pData[0] & 0x0f;
	//markbit
	const TPR_BOOL m  = ((pData[1] & 0x80) == 0) ? TPR_FALSE : TPR_TRUE;
	const TPR_UINT8 pt = pData[1] & 0x7f;
	const TPR_UINT16 sequenceNumber = (pData[2] << 8) + pData[3];

	if (v != 2) 
	{
		printf("v err %d pData=%p,nLen=%d\n",v,pData,nLen);
		return -1;
	}

	*ppRaw  = pData+12;
	*pDataLen = nLen-12;

	if(x)
	{
		iRet = ParseExtension(pData+12,nLen-12);
		*ppRaw += iRet;
		*pDataLen -= iRet;
	}

	if(p)
	{
		iRet = ParsePadding(pData,nLen);
		*pDataLen -= iRet;
	}

	if(pt == 127)
	{
		ParseRed(*ppRaw, *pDataLen, ppRaw, pDataLen);
	}

	return 0;
}

int Player::ParseExtension( unsigned char* pData,unsigned int nLen )
{
	unsigned char* ptr = pData;

	if (!pData || nLen < 4) 
	{
		printf("Extension para err 1,nLen=%d\n",nLen);
		return 0;
	}
	
	ptr += 2;

	int iXLen = ptr[0]<<8 | ptr[1];
	ptr += 2;

	iXLen *= 4; 

	if (nLen < (4 + iXLen)) 
	{
		printf("Extension para err 2,nLen=%d\n",nLen);
		return nLen;
	}

	return 4 + iXLen;
}

int Player::ParsePadding( unsigned char* pData,unsigned int nLen )
{
	return pData[nLen-1];
}

int Player::ParseRed( unsigned char* pData, int nLen ,unsigned char** ppRaw, int* pDataLen)
{
	TPR_UINT8* pPayloadPtr = pData;

	vector<int> NewHeaders;

	bool nLastBlock = false;
	int nSumLength = 0;
	int iTemLen = 0;
	while (!nLastBlock) 
	{
		// F == 0,表示是最后一个block
		nLastBlock = ((*pPayloadPtr & 0x80) == 0);
		if (nLastBlock) 
		{
			++nSumLength;  // RED头1字节
			iTemLen = nLen - nSumLength;
			pPayloadPtr += 1;
		} 
		else 
		{
			iTemLen = ((pPayloadPtr[2] & 0x03) << 8) + pPayloadPtr[3];
			pPayloadPtr += 4;  //冗余数据头部4字节
			nSumLength += iTemLen;//跳过payload数据
			nSumLength += 4;  // RED头4字节
		}

		//加入头部列表
		NewHeaders.push_back(iTemLen);
	}

	for (int i = 0; i != NewHeaders.size(); ++i) 
	{
		int iTemLen = NewHeaders[i];
		int  nPayloadLength = iTemLen;
		if (pPayloadPtr + nPayloadLength > pData + nLen) 
		{
			// pPayloadPtr和RED指向的payload长度不一致
			printf("SplitRed length mismatch\n");
			return -1;
		}

		*ppRaw = (unsigned char*)pPayloadPtr;
		*pDataLen = nPayloadLength;
		pPayloadPtr += nPayloadLength;
	}

	return 0;

}

int Player::GetAvailableNodeNum(unsigned int* pNum)
{
#if 0
	int iRet;
	if(m_iPlayPort != -1)
	{
		iRet = AUDIOCOM_GetAvailableNodeNumber(m_iPlayPort,pNum);
		if(iRet!=1)
		{
			printf("AUDIOCOM_InputStreamData err iRet=%d\n",AUDIOCOM_GetLastError(m_iPlayPort));
			return -1;
		}
	}
#endif

	return 0;
}

int Player::GetBufferTimeMs(unsigned int* pTimeMs)
{
#if 0
	int iRet;
	if(m_iPlayPort != -1)
	{
		iRet = AUDIOCOM_GetBufferTime(m_iPlayPort,pTimeMs);
		if(iRet!=1)
		{
			//printf("AUDIOCOM_GetBufferTime err iRet=%d\n",AUDIOCOM_GetLastError(m_iPlayPort));
			return -1;
		}
	}
#endif

	return 0;
}

