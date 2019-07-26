
#include "file.h"

#define kFrameRate  25
#define kOneRtpMaxLen 1400


File::File()
:m_bStart(TPR_FALSE)
,m_hThread(TPR_INVALID_HANDLE)
,m_pCallback(NULL)
,m_pUser(NULL)
,m_uTimeStamp(0)
,m_sSeqNo(0)
,m_iBitrate(32*1024*1024)
{
	memset(m_buf, 0, BUF_LEN);
}

File::~File()
{

}

int File::MakeOneRtp(TPR_UINT32 uTimeStamp , TPR_UINT16 sSeqNo, TPR_BOOL bMarkBit, int iLen)
{
	if (iLen <= 12 || iLen > kOneRtpMaxLen)
	{
		return 0;
	}

	TPR_UINT8 v = 2;

	m_buf[0] |= (v << 6); //version
	m_buf[0] &= ~0x20;  //padding
	m_buf[0] &= ~0x10;  //extern
	m_buf[0] |= (0 & 0x0f);  //csrc

	m_buf[1] |= (bMarkBit ? 0x80 : 0);//markbit
	m_buf[1] |= (0x60 & 0x7f); //PT

	//big endian
	m_buf[2] = sSeqNo >> 8;
	m_buf[3] = sSeqNo;

	//big endian nTimeStamp
	m_buf[4] = uTimeStamp >> 24;
	m_buf[5] = uTimeStamp >> 16;
	m_buf[6] = uTimeStamp >> 8;
	m_buf[7] = uTimeStamp;

	//big endian nSsrc
	TPR_UINT32 nSsrc = 0x11223344;
	m_buf[8] = nSsrc >> 24;
	m_buf[9] = nSsrc >> 16;
	m_buf[10] = nSsrc >> 8;
	m_buf[11] = nSsrc;

	int iLeftLen = iLen - 12;

	//传输层扩展头8字节
	if (iLeftLen > 8)
	{
		iLeftLen -= 8;
	}

	//无padding 
	//无扩展头 
	memset(m_buf + 12, 0, iLeftLen);


	//printf("uTimeStamp = %u,sSeqNo=%d,bMarkBit=%d\n", uTimeStamp, sSeqNo, bMarkBit);
	return iLeftLen+12;
}



int File::Open( CallbackFunc pCallback, void* pUser )
{
	if(!pCallback)
	{
		return -1;
	}

	m_hThread = TPR_Thread_Create(FileThread, this, 0,0);
	if (TPR_INVALID_THREAD == m_hThread)
	{
		return -1;
	}

	m_pCallback = pCallback;
	m_pUser = pUser;
	return 0;
}

int File::Close()
{
	m_bStart = TPR_FALSE;
	if(m_hThread!= TPR_INVALID_THREAD)
	{
		TPR_Thread_Wait(m_hThread);
		m_hThread = TPR_INVALID_THREAD;
	}
	return 0;
}

int File::SetBitRate(int iBitrate)
{
	if (iBitrate > 64 * 1024 * 1024)
	{
		return -1;
	}

	m_iBitrate = iBitrate;
}

TPR_VOIDPTR __stdcall File::FileThread( TPR_VOIDPTR pUserData )
{
	File* p = (File*)pUserData;

	if (NULL == p)
	{
		return NULL;
	}

	p->FileThreadRel();
	return NULL;
}

int File::FileThreadRel()
{	
	m_bStart = TPR_TRUE;
	while(m_bStart)
	{
		Process();
	}
	return 0;
}

int File::Process()
{

	int iRtpLen = 0;
	int nReadSize = 0;

	while(m_bStart)
	{
		TPR_TIME_T t1 = TPR_TimeNow();

		//DEMO_DEBUG("m_iBitrate=%d", m_iBitrate);
		SendOneFrame(m_iBitrate);

		TPR_TIME_T t2 = TPR_TimeNow();

		TPR_INT64 delta = t2 - t1;

		int tmp = (1000 / kFrameRate) * 1000;
		tmp -= delta;
		if (tmp > 0)
		{
			TPR_USleep(tmp);
		}
	}

	return 0;
}

int File::SendOneFrame(int iBitrate)
{
	int iRtpCount = ((iBitrate >>3 )/kFrameRate) / kOneRtpMaxLen;
	int iLeft = ((iBitrate >> 3) / kFrameRate) % kOneRtpMaxLen;

	//DEMO_DEBUG("iRtpCount=%d,iLeft=%d", iRtpCount, iLeft);
	for (int i = 0; i < iRtpCount; i++)
	{
		int iMarkbit = (iLeft == 0 && i == iRtpCount - 1) ? 1 : 0;
		int iLen = MakeOneRtp(m_uTimeStamp,m_sSeqNo, iMarkbit, kOneRtpMaxLen);
		m_sSeqNo++;

		if (m_pCallback && iLen>0)
		{
			m_pCallback(m_buf, iLen, m_pUser);
		}
	}

	if (iLeft > 0)
	{
		int iLen = MakeOneRtp(m_uTimeStamp, m_sSeqNo, TPR_TRUE, iLeft);
		m_sSeqNo++;

		if (m_pCallback && iLen > 0)
		{
			m_pCallback(m_buf, iLen, m_pUser);
		}
	}

	m_uTimeStamp += (90*1000/kFrameRate);
	return 0;
}


