/*
 * CopyRight:	HangZhou Hikvision System Technology Co., Ltd. All	Right Reserved.
 * FileName:	HPR_Singleton.h
 * Desc:		OS Layer Component.
 * Author:		schina
 * Date:		2009-03-02
 * Contact:		zhaoyiji@hikvision.com.cn
 * History:		Created By Zhaoyiji 2009-03-02
 * */

#ifndef __HPR_SINGLETON_H__
#define __HPR_SINGLETON_H__

#if (defined _WIN32 || defined _WIN64)
#   include <windows.h>
#endif

//////////////////////////////////////////////////////////////////////////

#include <HPR_Mutex.h>

template<typename T>
class singleton
{
public:
	static inline T* instance()
	{
		if (instance_ == 0)
		{
			lock_.lock();
			if (instance_ == 0)
			{
				instance_ = new T;
			}
			lock_.unlock();
		}
		return instance_;
	}

	static inline void free()
	{
		free_ = true;
		lock_.lock();
		if (instance_ != 0)
		{
			delete instance_;
			instance_ = 0;
		}
		lock_.unlock();
	}

protected:
	singleton() {}
	virtual ~singleton()
	{
		if (!free_)
		{
			lock_.lock();
			if (instance_ != 0)
			{
				delete instance_;
				instance_ = 0;
			}
			lock_.unlock();
		}
	}
private:
	singleton(const singleton&) {}
	singleton& operator=(const singleton&) {}

private:
	static T* instance_;
	static hpr::hpr_mutex lock_;
	static bool free_;
};

template<typename T>
T* singleton<T>::instance_ = NULL;

template<typename T>
hpr::hpr_mutex singleton<T>::lock_;

template<typename T>
bool singleton<T>::free_ = false;


#endif
