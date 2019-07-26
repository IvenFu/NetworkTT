

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

	/*��ȡ�Ự��*/
	TPR_Mutex* GetLock()
	{
		return &m_lock;
	}

private:
	void* m_ptr;
	TPR_Mutex  m_lock;		//Ԫ��������������Ԫ�صĶ���д
};



class Manager
{
public:

	/*��ȡʵ��*/
	static Manager* GetInstance();

	/*����*/
	static void Destroy(); 
	
	int Register(void* ptr);

	int UnRegister(int id);

	Element* GetElement(int id);

	TPR_BOOL IsEmpty();

protected:
	Manager(){}

	~Manager(){}

private:

	static Manager*  s_pInstance;		//���������ַ
	static TPR_Mutex s_hRegisterLock;	//ȫ����������Manager�Ķ���д
	Element m_elements[NPQ_MAX_NUM];	//Ԫ����Ϣ
};


#endif

