#include "TPR_Guard.h"
#include "Manage.h"
#define new(x)  new(x,NPQ_NEW_FLAG)

Manager* Manager::s_pInstance = NULL;
TPR_Mutex Manager::s_hRegisterLock;

Manager* Manager::GetInstance()
{
	TPR_Guard guard(&s_hRegisterLock);

	if(s_pInstance == NULL)
	{
		s_pInstance = new (std::nothrow)Manager();
		if(s_pInstance == NULL)
		{
			return NULL;
		}
	}

	return s_pInstance;
}

void Manager::Destroy()
{
	TPR_Guard guard(&s_hRegisterLock);
	SAFE_DEL(s_pInstance);
}


int Manager::Register(void* ptr)
{
	TPR_Guard guard(&s_hRegisterLock);

	int id = -1;

	for(int i=0;i<NPQ_MAX_NUM;i++)
	{
		if(NULL == m_elements[i].GetPtr())
		{
			m_elements[i].SetPtr(ptr);
			id = i;
			break;
		}
	}

	return id;
}

int Manager::UnRegister( int id )
{
	TPR_Guard guard(&s_hRegisterLock);

	if(id < 0 || id>=NPQ_MAX_NUM)
	{
		return NPQERR_PARA;
	}

	m_elements[id].SetPtr(NULL);

	return NPQ_OK;
}

Element* Manager::GetElement( int id )
{
	TPR_Guard guard(&s_hRegisterLock);

	if(id < 0 || id>=NPQ_MAX_NUM)
	{
		return NULL;
	}

	return &m_elements[id];
}

TPR_BOOL Manager::IsEmpty()
{
	TPR_Guard guard(&s_hRegisterLock);

	for(int i=0;i<NPQ_MAX_NUM;i++)
	{
		if(NULL != m_elements[i].GetPtr())
		{
			return TPR_FALSE;
		}
	}
	return TPR_TRUE;
}

