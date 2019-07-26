/** @file    HikFecDecInterface.cpp
*  @date    20106/11/2
*
*  @note    V1.0´´½¨
*/

#include "HikFecInterface.h"
#include "HikFecInterfaceImpl.h"
#include "FecDecoder.h"
#include "FecEncoder.h"

void * HIKFEC_CreateHandle( bool bEncoder,HikFecProtectionParams* pEncoderParam )
{
	if(bEncoder)
	{
		if (NULL == pEncoderParam)
		{
			return NULL;
		}

		CHikFecEncoder *pcHikFecEnc = new CHikFecEncoder(pEncoderParam);
		if (NULL == pcHikFecEnc)
		{
			return NULL;
		}

		HikFecInterfaceImpl *pInterface = new HikFecInterfaceImpl(bEncoder);
		if (NULL == pInterface)
		{
			SAFE_DEL(pcHikFecEnc);
			return NULL;
		}

		pInterface->m_handle = (void*)pcHikFecEnc;
		return (void*)pInterface;
	}
	else
	{
		CHikFecDecoder *pcHikDec = new(std::nothrow)CHikFecDecoder();
		if (NULL == pcHikDec)
		{
			return NULL;
		}

		HikFecInterfaceImpl *pInterface = new HikFecInterfaceImpl(bEncoder);
		if (NULL == pInterface)
		{
			SAFE_DEL(pcHikDec);
			return NULL;
		}

		pInterface->m_handle = (void*)pcHikDec;
		return (void*)pInterface;
	}
}

int HIKFEC_RegisterOutputFecCallBack(void *handle, HIKFEC_OutputDataCallBack pCb, void *pUser)
{
	if(!handle)
	{
		return HIK_FEC_LIB_E_PARA_NULL;
	}

	HikFecInterfaceImpl *pInterface = (HikFecInterfaceImpl*)handle;
	if(pInterface->m_bEncoder)
	{
		CHikFecEncoder *pcEnc = (CHikFecEncoder*)pInterface->m_handle;;
		if (pcEnc)
		{
			return pcEnc->RegisterOutputFecCallBack(pCb,pUser);
		}
	}
	else
	{
		CHikFecDecoder *pcDec = (CHikFecDecoder*)pInterface->m_handle;
		if(pcDec)
		{
			return pcDec->RegisterOutputFecCallBack(pCb, pUser);
		}
	}

	return HIK_FEC_LIB_E_FAIL;
}


int HIKFEC_InputData(void* handle, DATA_PACK_VEC *pstStream,DATA_INFO* pstruInfo)
{
	if(!handle)
	{
		return HIK_FEC_LIB_E_PARA_NULL;
	}

	HikFecInterfaceImpl *pInterface = (HikFecInterfaceImpl*)handle;
	if(pInterface->m_bEncoder)
	{
		CHikFecEncoder *pcEnc = (CHikFecEncoder*)pInterface->m_handle;;
		if (pcEnc)
		{
			return pcEnc->GenerateFEC(pstStream,pstruInfo);
		}
	}
	else
	{
		CHikFecDecoder *pcDec = (CHikFecDecoder*)pInterface->m_handle;
		if(pcDec)
		{
			int iRet = pcDec->DecodeFec(pstStream);
			return iRet;
		}
	}

	return HIK_FEC_LIB_E_FAIL;
}


void  HIKFEC_Release(void *handle)
{
	if(!handle)
	{
		return;
	}

	HikFecInterfaceImpl *pInterface = (HikFecInterfaceImpl*)handle;
	if(pInterface->m_bEncoder)
	{
		CHikFecEncoder *pcEnc = (CHikFecEncoder*)pInterface->m_handle;
		SAFE_DEL(pcEnc);
	}
	else
	{
		CHikFecDecoder *pcDec = (CHikFecDecoder*)pInterface->m_handle;
		SAFE_DEL(pcDec);
	}

	SAFE_DEL(pInterface);
}

int HIKFEC_SetParam( void* handle,HikFecProtectionParams* pEncoderParam )
{
	if(!handle)
	{
		return HIK_FEC_LIB_E_PARA_NULL;
	}

	HikFecInterfaceImpl *pInterface = (HikFecInterfaceImpl*)handle;
	if(pInterface->m_bEncoder)
	{
		CHikFecEncoder *pcEnc = (CHikFecEncoder*)pInterface->m_handle;;
		if (pcEnc)
		{
			return pcEnc->SetFecProtectionParms(pEncoderParam);
		}
	}

	return HIK_FEC_LIB_E_FAIL;
}




