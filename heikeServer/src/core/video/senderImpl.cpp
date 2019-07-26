
/**	@sendImpl.cpp
*	@note 111.
*	@brief 发送端实现类 相关实现
*
*	@author		333
*	@date		2016/10/08
*
*	@note 
*
*	@warning 
*/
#include "TPR_Time.h"
#include "TPR_Utils.h"
#include "TPR_Time.h"
#include "TPR_Thread.h"
#include "senderImpl.h"
#define new(x)  new(x,NPQ_NEW_FLAG)

/**	@fn SenderImpl::SenderImpl
*	@brief 发送端实现类构造函数
*	@param OuterParam& struOuterParam  外部参数信息
*	@return 
*/
SenderImpl::SenderImpl(OuterParam& struOuterParam,NPQ_QOS_MAIN_TYPE enMainType)
:SenderBase(struOuterParam,enMainType)
{

}

/**	@fn SenderImpl::~SenderImpl
*	@brief 发送端实现类析构函数
*	@param 
*	@return 
*/
SenderImpl::~SenderImpl()
{

}

/**	@fn SenderImpl::Create(OuterParam& struOuterParam)
*	@brief 创建发送端实现类
*	@param  OuterParam& struOuterParam 外部参数信息
*	@return SenderImpl* 发送端实现类
*/
SenderImpl* SenderImpl::Create(OuterParam& struOuterParam, NPQ_QOS_MAIN_TYPE enMainType)
{
    int iRet;

    SenderImpl* pSenderIml = new (std::nothrow)SenderImpl(struOuterParam,enMainType);
    if(NULL == pSenderIml)
    {
        return NULL;
    }

    iRet = pSenderIml->Init();
    if(NPQ_OK != iRet)
    {
        SAFE_DEL(pSenderIml);
        return NULL;
    }

    return pSenderIml;
}

/**	@fn SenderImpl::Destroy(SenderImpl* pSenderImpl)
*	@brief 销毁发送端实现类
*	@param SenderImpl* pSenderImpl 发送端实现类
*	@return int 错误码
*/
int SenderImpl::Destroy(SenderImpl* pSenderImpl)
{
    if(!pSenderImpl)
    {
        return NPQERR_PARA;
    }
    pSenderImpl->Fini();

    SAFE_DEL(pSenderImpl);

    return NPQ_OK;
}

/**	@fn SenderImpl::Init()
*	@brief  初始化函数
*	@param SenderImpl* pSenderImpl 发送端实现类
*	@return int 错误码
*/
int SenderImpl::Init()
{
    int iRet = NPQ_OK;
	
	iRet = SenderBase::Init();
	if(iRet!=NPQ_OK)
	{
		goto ERR;
	}

	m_pRtcpCallback->SetSenderInterface((void*)this);
	
	return NPQ_OK;
ERR:
	Fini();

    return NPQERR_SYSTEM;
}

/**	@fn SenderImpl::Fini()
*	@brief  反初始化函数
*	@param 
*	@return int 错误码
*/
int SenderImpl::Fini()
{	
	int iRet = NPQ_OK;

	SenderBase::Fini();


    return NPQ_OK;
}

/**	@fn SenderImpl::Start()
*	@brief 开始RTCP
*	@param 
*	@return int 错误码
*/
int SenderImpl::Start()
{
	SenderBase::Start();
	

    return NPQ_OK;
}

/**	@fn SenderImpl::Stop()
*	@brief 停止RTCP
*	@param 
*	@return int 错误码
*/
int SenderImpl::Stop()
{
    SenderBase::Stop();

    return NPQ_OK;
}

/**	@fn SenderImpl::InputData(CRtpPacket& RtpPacket)
*	@brief 输入数据
*	@param CRtpPacket& RtpPacket rtp数据包
*	@return int 错误码
*/
int SenderImpl::InputData(CRtpPacket& RtpPacket)
{
    int iRet = NPQ_OK;

	SenderBase::InputData(RtpPacket);

    m_struOuterParam.DoCallback(RtpPacket);

    return NPQ_OK;
}
