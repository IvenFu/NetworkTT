
/**	@sendImpl.h
*	@note 111.
*	@brief 发送端实现类 相关定义
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
 *	@brief   发送实现类
 *	@note	 
 */
class SenderImpl:public SenderBase
{
public:
    /*创建发送端实现类*/
    static SenderImpl* Create(OuterParam& struOuterParam,NPQ_QOS_MAIN_TYPE enMainType);

    /*销毁发送端实现类*/
    static int Destroy(SenderImpl* pReceiver);

    /*输入rtp数据*/
    int InputData(CRtpPacket& RtpPacket);


    /*开始rtcp*/
    int Start();

    /*停止rtcp*/
    int Stop();

protected:
    /*构造函数*/
    SenderImpl(OuterParam& struOuterParam,NPQ_QOS_MAIN_TYPE enMainType);

    /*析构函数*/
    ~SenderImpl();

private:

    /*初始化函数*/
    int Init();

    /*反初始化函数*/
    int Fini();
};

#endif


