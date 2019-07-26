#ifndef _FILE_H_
#define _FILE_H_

#include "config.h"

class File
{
public:
	File();
	~File();

	int Open(CallbackFunc pCallback, void* pUser);
	int Close();

	int SetBitRate(int iBitrate);

private:
	static TPR_VOIDPTR __stdcall FileThread(TPR_VOIDPTR pUserData);
	int FileThreadRel();
	int Process();
	int MakeOneRtp(TPR_UINT32 uTimeStamp, TPR_UINT16 sSeqNo, TPR_BOOL bMarkBit, int iLen);
	int SendOneFrame(int iBitrate);

	TPR_BOOL m_bStart;
	TPR_HANDLE m_hThread;
#define BUF_LEN 2048
	unsigned char m_buf[BUF_LEN];
	CallbackFunc m_pCallback;
	void* m_pUser;
	TPR_UINT32 m_uTimeStamp;
	TPR_UINT16 m_sSeqNo;
	TPR_UINT32 m_iBitrate;
};


#endif
