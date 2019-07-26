

#include "NPQosImpl.h"
#include "Manage.h"
#include "receiver.h"
#include "sender.h"

NPQosImpl::NPQosImpl()
:m_pQosBase(NULL)
{

}

NPQosImpl::~NPQosImpl()
{

}

int NPQosImpl::Create(NPQ_QOS_ROLE enType)
{
	int id;

	Manager* pManager = Manager::GetInstance();
	if(pManager == NULL)
	{
		return NPQERR_SYSTEM;
	}
	
	id = pManager->Register((void*)this);
	if(id < 0)
	{
		return NPQERR_GENRAL;
	}

	m_struOuterPram.id = id;
	m_struOuterPram.enType = enType;

	Element* pEle = pManager->GetElement(id);
	if(pEle  == NULL)
	{
		/*不可能为NULL*/
		return NPQERR_GENRAL;
	}

	TPR_Guard locker(pEle->GetLock());

	m_pQosBase = CreateQos(enType);
	if(m_pQosBase == NULL)
	{
		pManager->UnRegister(id);
		return NPQERR_GENRAL;
	}
	return id;
}

int NPQosImpl::Destroy(int id)
{
	Manager* pManager = Manager::GetInstance();
	if(pManager == NULL)
	{
		return NPQERR_SYSTEM;
	}

	pManager->UnRegister(id);

	DestroyQos(m_pQosBase);	
	return NPQ_OK;
}

int NPQosImpl::Start()
{
	if(m_pQosBase)
	{
		return m_pQosBase->Start();
	}
	else
	{
		return NPQERR_PRECONDITION;
	}
	return NPQ_OK;
}

int NPQosImpl::Stop()
{
	if(m_pQosBase)
	{
		return m_pQosBase->Stop();
	}
	else
	{
		return NPQERR_PRECONDITION;
	}
	return NPQ_OK;
}

int NPQosImpl::RegisterDataCallBack( NPQCb pCallback, void* pUser )
{
	m_struOuterPram.pCallback = pCallback;
	m_struOuterPram.pUser = pUser;
	return NPQ_OK;
}

int NPQosImpl::InputData(int iDataType, unsigned char* pData, unsigned int iDataLen )
{
	if(m_pQosBase)
	{
		return m_pQosBase->InputData(iDataType,pData,iDataLen);
	}
	else
	{
		return NPQERR_PRECONDITION;
	}
}


int NPQosImpl::SetParam( NPQ_PARAM* pParam )
{
	int iRet;
	if(!pParam)
	{
		return NPQERR_PARA;
	}

	if(pParam->m_type == 0)
	{
		NPQ_ERROR("pParam->m_type err %#x",pParam->m_type);
		return NPQERR_PARA;
	}

	m_struOuterPram.m_enQosConfig = pParam->m_type;
	m_struOuterPram.m_bVideo = pParam->bVideo;
	m_struOuterPram.m_bAudio = pParam->bAudio;
    NPQ_ERROR("m_struOuterPram.m_enQosConfig=%d  bAudio=%d",m_struOuterPram.m_enQosConfig, m_struOuterPram.m_bAudio);
	

	return NPQ_OK;
}

int NPQosImpl::SetNotifyParam(NPQ_SET_NOTIFY_PARAM* pNotifyParam)
{
	return NPQ_OK;
}

Base* NPQosImpl::CreateQos(NPQ_QOS_ROLE enType)
{
	switch(enType)
	{
	case NPQ_QOS_RECEIVER:
		{
			return Receiver::Create(m_struOuterPram);
		}
	case NPQ_QOS_SENDER:
		{
			return Sender::Create(m_struOuterPram);
		}
	default:
		break;
	}

	return NULL;
}

void NPQosImpl::DestroyQos(Base* pBase)
{
	SAFE_DEL(pBase);
}

int NPQosImpl::GetStat(NPQ_MAIN_TYPE enMainType, NPQ_STAT* pStat )
{
	if(m_pQosBase)
	{
		return m_pQosBase->GetStat(enMainType,pStat);
	}
	else
	{
		return NPQERR_PRECONDITION;
	}

	return NPQ_OK;
}











