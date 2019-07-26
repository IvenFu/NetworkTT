
/**	@sendImpl.cpp
*	@note 111.
*	@brief ���Ͷ�ʵ���� ���ʵ��
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
*	@brief ���Ͷ�ʵ���๹�캯��
*	@param OuterParam& struOuterParam  �ⲿ������Ϣ
*	@return 
*/
SenderImpl::SenderImpl(OuterParam& struOuterParam,NPQ_QOS_MAIN_TYPE enMainType)
:SenderBase(struOuterParam,enMainType)
{

}

/**	@fn SenderImpl::~SenderImpl
*	@brief ���Ͷ�ʵ������������
*	@param 
*	@return 
*/
SenderImpl::~SenderImpl()
{

}

/**	@fn SenderImpl::Create(OuterParam& struOuterParam)
*	@brief �������Ͷ�ʵ����
*	@param  OuterParam& struOuterParam �ⲿ������Ϣ
*	@return SenderImpl* ���Ͷ�ʵ����
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
*	@brief ���ٷ��Ͷ�ʵ����
*	@param SenderImpl* pSenderImpl ���Ͷ�ʵ����
*	@return int ������
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
*	@brief  ��ʼ������
*	@param SenderImpl* pSenderImpl ���Ͷ�ʵ����
*	@return int ������
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
*	@brief  ����ʼ������
*	@param 
*	@return int ������
*/
int SenderImpl::Fini()
{	
	int iRet = NPQ_OK;

	SenderBase::Fini();


    return NPQ_OK;
}

/**	@fn SenderImpl::Start()
*	@brief ��ʼRTCP
*	@param 
*	@return int ������
*/
int SenderImpl::Start()
{
	SenderBase::Start();
	

    return NPQ_OK;
}

/**	@fn SenderImpl::Stop()
*	@brief ֹͣRTCP
*	@param 
*	@return int ������
*/
int SenderImpl::Stop()
{
    SenderBase::Stop();

    return NPQ_OK;
}

/**	@fn SenderImpl::InputData(CRtpPacket& RtpPacket)
*	@brief ��������
*	@param CRtpPacket& RtpPacket rtp���ݰ�
*	@return int ������
*/
int SenderImpl::InputData(CRtpPacket& RtpPacket)
{
    int iRet = NPQ_OK;

	SenderBase::InputData(RtpPacket);

    m_struOuterParam.DoCallback(RtpPacket);

    return NPQ_OK;
}
