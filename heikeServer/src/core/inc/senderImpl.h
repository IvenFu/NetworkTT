
/**	@sendImpl.h
*	@note 111.
*	@brief ���Ͷ�ʵ���� ��ض���
*
*	@author		333
*	@date		2016/10/08
*
*	@note 
*
*	@warning 
*/

#ifndef _SENDERIMPL_H_
#define _SENDERIMPL_H_

#include "TPR_Semaphore.h"
#include "baseImpl.h"
#include "Util.h"
#include "rtp.h"
#include "RtpAddExtension.h"
#include "buffer.h"
#include "base.h"
#include "senderBase.h"

	
class Fec;
class CbwManager;
/**	@class	 SenderImpl 
 *	@brief   ����ʵ����
 *	@note	 
 */
class SenderImpl:public SenderBase
{
public:
    /*�������Ͷ�ʵ����*/
    static SenderImpl* Create(OuterParam& struOuterParam,NPQ_QOS_MAIN_TYPE enMainType);

    /*���ٷ��Ͷ�ʵ����*/
    static int Destroy(SenderImpl* pReceiver);

    /*����rtp����*/
    int InputData(CRtpPacket& RtpPacket);


    /*��ʼrtcp*/
    int Start();

    /*ֹͣrtcp*/
    int Stop();

protected:
    /*���캯��*/
    SenderImpl(OuterParam& struOuterParam,NPQ_QOS_MAIN_TYPE enMainType);

    /*��������*/
    ~SenderImpl();

private:

    /*��ʼ������*/
    int Init();

    /*����ʼ������*/
    int Fini();
};

#endif


