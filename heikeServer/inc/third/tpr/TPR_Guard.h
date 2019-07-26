
#ifndef __TPR_GUARD_H__
#define __TPR_GUARD_H__

#include "TPR_Mutex.h"

//////////////////////////////////////////////////////////////////////////
//c++ ±àÒëÆ÷²ÅÖ§³Ö
class TPR_DECLARE_CLASS TPR_Guard
{
public:
	TPR_Guard(TPR_Mutex* pMutex)
	:m_pMutex(pMutex)
	{
		pMutex->Lock();	
	}
	
	~TPR_Guard()
	{
		Release();
	}

	void Release()
	{
		if (m_pMutex)
		{
			m_pMutex->Unlock();
			m_pMutex = NULL;
		}
	}

private:
	TPR_Mutex* m_pMutex;
};


#endif

