

#ifndef _MANAGE_H_
#define _MANAGE_H_


#include "Util.h"
#include "TPR_Mutex.h"
#include "TPR_Guard.h"

#define NPQ_MAX_NUM  500

class Element
{
public:
	Element():m_ptr(NULL){}
	~Element(){}

	void SetPtr(void* ptr)
	{
		m_ptr = ptr;
	}

	void* GetPtr()
	{
		return m_ptr;
	}

	/*获取会话锁*/
	TPR_Mutex* GetLock()
	{
		return &m_lock;
	}

private:
	void* m_ptr;
	TPR_Mutex  m_lock;		//元素锁，保护单个元素的读和写
};



class Manager
{
public:

	/*获取实例*/
	static Manager* GetInstance();

	/*销毁*/
	static void Destroy(); 
	
	int Register(void* ptr);

	int UnRegister(int id);

	Element* GetElement(int id);

	TPR_BOOL IsEmpty();

protected:
	Manager(){}

	~Manager(){}

private:

	static Manager*  s_pInstance;		//单例对象地址
	static TPR_Mutex s_hRegisterLock;	//全局锁，保护Manager的读和写
	Element m_elements[NPQ_MAX_NUM];	//元素信息
};


#endif

