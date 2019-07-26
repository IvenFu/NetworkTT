
#include "TPR_Guard.h"
#include "TPR_Thread.h"
#include "TPR_Utils.h"
#include "TPR_Time.h"
#include "receiverImpl.h"
#define new(x)  new(x,NPQ_NEW_FLAG)

ReceiverImpl::ReceiverImpl(OuterParam& struOuterParam, NPQ_QOS_MAIN_TYPE enMainType)
:m_bStart(TPR_FALSE)
,ReceiverBase(struOuterParam,enMainType)
{
}

ReceiverImpl::~ReceiverImpl()
{

}

ReceiverImpl* ReceiverImpl::Create(OuterParam& struOuterParam,NPQ_QOS_MAIN_TYPE enMainType)
{
	int iRet;

	ReceiverImpl* pReceiver = new (std::nothrow)ReceiverImpl(struOuterParam,enMainType);
	if(pReceiver == NULL)
	{
		return NULL;
	}

	iRet = pReceiver->Init();
	if(iRet != NPQ_OK)
	{
		SAFE_DEL(pReceiver);
		return NULL;
	}

	return pReceiver;
}

int ReceiverImpl::Destroy(ReceiverImpl* pReceiver)
{
	if(!pReceiver)
	{
		return NPQERR_PARA;
	}
	pReceiver->Fini();

	SAFE_DEL(pReceiver);

	return NPQ_OK;
}

int ReceiverImpl::Start()
{
	m_bStart = TPR_TRUE;
	
	
	return ReceiverBase::Start();
}

int ReceiverImpl::Stop()
{
	if(m_bStart == TPR_FALSE)
	{
		return NPQ_OK;
	}

	m_bStart = TPR_FALSE;
	

	return ReceiverBase::Stop();
}

int ReceiverImpl::Init()
{
	int iRet;

	iRet = ReceiverBase::Init();
	if(iRet!=NPQ_OK)
	{
		goto ERR;
	}

	return NPQ_OK;
ERR:
	Fini();
	return NPQERR_SYSTEM;
}

int ReceiverImpl::Fini()
{	
	ReceiverBase::Fini();
	return NPQ_OK;
}

int ReceiverImpl::InputData(CRtpPacket& RtpPacket)
{
	int iRet;

	if(!m_bStart)
	{
		NPQ_ERROR("impl do not start");
		return NPQERR_PRECONDITION;
	}

	ReceiverBase::InputData(RtpPacket);

	m_struOuterParam.DoCallback(RtpPacket);

	return NPQ_OK;
}


int ReceiverImpl::GetStat( NPQ_STAT* pStat)
{
	TPR_INT64 llnow = OneTime::OneNowMs();
	

	NPQ_DEBUG("memroy used =%lld",MemoryUsed());
	return ReceiverBase::GetStat(pStat);
}

