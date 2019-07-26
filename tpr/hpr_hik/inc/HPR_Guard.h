/*
 * CopyRight:	HangZhou Hikvision System Technology Co., Ltd. All	Right Reserved.
 * FileName:	HRP_Guard.h
 * Desc:		OS Layer Component.
 * Author:		schina
 * Date:		2009-03-02
 * Contact:		zhaoyiji@hikvision.com.cn
 * History:		Created By Zhaoyiji 2009-03-02
 * */

#ifndef __HPR_GUARD_H__
#define __HPR_GUARD_H__

#include "HPR_Mutex.h"

//////////////////////////////////////////////////////////////////////////
//c++ ±àÒëÆ÷²ÅÖ§³Ö
#if defined (CXXCOMPILE)
class HPR_DECLARE_CLASS HPR_Guard
{
public:
	HPR_Guard(HPR_Mutex* pMutex)
	:m_pMutex(pMutex)
	{
		pMutex->Lock();	
	}
	
	~HPR_Guard()
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
	HPR_Mutex* m_pMutex;
};
#endif
//////////////////////////////////////////////////////////////////////////

//add by zhaoyiji 2011-10-20
//////////////////////////////////////////////////////////////////////////
namespace hpr
{

	template <typename T>
	class HPR_DECLARE_CLASS hpr_guard
	{
	public:
		hpr_guard(T& mutex)
		//hpr_guard(hprcxx_mutex* mutex)
			: m_mutex (mutex)
		{
			mutex.lock();
			//m_mutex->lock();
		};

		~hpr_guard()
		{
			m_mutex.unlock();
			//m_mutex->unlock();
		};

	private:
		hpr_guard();
		hpr_guard(const hpr_guard&);
		const hpr_guard& operator=(const hpr_guard&);
		
	private:
		T& m_mutex;
		//hprcxx_mutex* m_mutex;
	};

};

#endif

