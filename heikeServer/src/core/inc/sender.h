
#ifndef _SENDER_H_
#define _SENDER_H_

#include "Util.h"
#include "base.h"
#include "senderImpl.h"

class Sender: public Base
{
public:
    static Base* Create(OuterParam& struOuterParam);
    ~Sender();

    int Start();

    int Stop();

    int InputData(int iDataType,unsigned char* pData, unsigned int nDataLen);


	int GetStat (NPQ_MAIN_TYPE enMainType,NPQ_STAT* pStat);
protected:
    Sender(OuterParam& struOuterParam);

private:

    SenderImpl* m_pSendVideo;
	TPR_BOOL m_bFirst;
};

#endif


