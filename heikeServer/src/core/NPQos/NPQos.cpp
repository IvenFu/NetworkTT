

#include "NPQosImpl.h"
#include "Manage.h"
#define new(x)  new(x,NPQ_NEW_FLAG)

/*检查会话iId是否合法*/
#define CHECK_ID_RETURN(id)                                                        \
	if (((id)<0)||((id) >= NPQ_MAX_NUM))          \
	{                                                                               \
		NPQ_ERROR("Parameter is invalid,Id[%d]",(id));                             \
		return (int)NPQERR_PARA;                                                    \
	}

#define LOCALTION(id)           \
	Manager* pManager = Manager::GetInstance();\
	if(pManager == NULL)\
	{\
		NPQ_ERROR("GetInstance err,Id[%d]", id);\
		return NPQERR_SYSTEM;\
	}\
	Element* pEle = pManager->GetElement(id);\
	if(pEle  == NULL)\
	{\
		/*不可能为NULL*/\
		return NPQERR_GENRAL;\
	}\
	TPR_Guard locker(pEle->GetLock());\
	NPQosImpl* pQosImpl = (NPQosImpl*)pEle->GetPtr();\
	if(pQosImpl == NULL)\
	{\
		return NPQERR_PARA;\
	}


NPQ_API int __stdcall NPQ_Create(NPQ_QOS_ROLE enType)
{
	int iRet;
	NPQosImpl* pQosImpl = NULL;


	pQosImpl = new(std::nothrow)NPQosImpl();
	if(!pQosImpl)
	{
		return NPQERR_SYSTEM;
	}

	iRet = pQosImpl->Create(enType);
	if(iRet< 0)
	{
		SAFE_DEL(pQosImpl);
		return iRet;
	}

	return iRet;
}

NPQ_API int __stdcall NPQ_Destroy(int id)
{
	CHECK_ID_RETURN(id);
	LOCALTION(id);

	int iRet = pQosImpl->Destroy(id);
	if(iRet != NPQ_OK)
	{
		NPQ_ERROR("NPQosImpl destroy failed");
	}

	SAFE_DEL(pQosImpl);

	if(pManager->IsEmpty())
	{
		Hlog::Fini();
		InitTPR::Fini();
	}

	NPQ_DEBUG("memory after destroy %lld",MemoryUsed());
	return NPQ_OK;
}

NPQ_API int __stdcall NPQ_Start(int id)
{
	CHECK_ID_RETURN(id);
	LOCALTION(id);

	return pQosImpl->Start();
}


NPQ_API int __stdcall NPQ_Stop(int id)
{
	CHECK_ID_RETURN(id);
	LOCALTION(id);

	return pQosImpl->Stop();
}


NPQ_API int __stdcall NPQ_RegisterDataCallBack( int id, NPQCb pCallback, void* pUser )
{
	CHECK_ID_RETURN(id);
	LOCALTION(id);

	return pQosImpl->RegisterDataCallBack(pCallback,pUser);
}

NPQ_API int __stdcall NPQ_InputData(int id, int iDataType, unsigned char* pData, unsigned int iDataLen)
{
	CHECK_ID_RETURN(id);
	LOCALTION(id);

	return pQosImpl->InputData(iDataType,pData,iDataLen);
}

NPQ_API int __stdcall NPQ_SetParam( int id, void* pParam )
{
	CHECK_ID_RETURN(id);
	LOCALTION(id);

	return pQosImpl->SetParam((NPQ_PARAM*)pParam);
}

NPQ_API int __stdcall NPQ_GetStat( int id,NPQ_MAIN_TYPE enMainType, void* pStat )
{
	CHECK_ID_RETURN(id);
	LOCALTION(id);

	return pQosImpl->GetStat(enMainType,(NPQ_STAT*)pStat);
}

NPQ_API int __stdcall NPQ_SetNotifyParam(int id,  NPQ_SET_NOTIFY_PARAM* pNotifyParam)
{
	CHECK_ID_RETURN(id);
	LOCALTION(id);

	return pQosImpl->SetNotifyParam(pNotifyParam);
}


NPQ_API int __stdcall NPQ_SetLogFile(char* szAbsFileName)
{
	if(!szAbsFileName)
	{
		return NPQERR_PARA;
	}

	return SetLogFilePath(szAbsFileName);
}

