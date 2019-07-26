/**	@logCtrl.h
*	@note 111.
*	@brief ��־��ģ�鿪�ؿ���
*
*	@author		222
*	@date		2017/12/07
*
*	@note  ��ͷ�ļ�������new��delete,�����ڴ�ͳ��
*
*	@warning 
*/
#ifndef _LOGCTRL_H_
#define _LOGCTRL_H_

#include <string>

#define LOG_FUNC_JITTER   0  
#define LOG_FUNC_NACK   0  
#define LOG_FUNC_FEC   0  
#define LOG_FUNC_BW   0 
#define LOG_FUNC_PACING   0 
#define LOG_FUNC_PLI   0  
#define LOG_FUNC_NETEQ   0  

//���ص�new������������ϵͳ�İ汾����¼���������к�
void* operator new(size_t size, const char* func, const size_t line);
void* operator new(size_t size, const std::nothrow_t& no, const char* func, const size_t line);
void* operator new[](size_t size, const char* func, const size_t line);
void* operator new[](size_t size, const std::nothrow_t& no,const char* func, const size_t line);

//�������غ���ֻ����ϵͳ�Զ����ã������������ã�����new��ʱ��ʼ��ʧ�ܣ��Զ�����delete�ͷ��ڴ棬�������й©����
void operator delete(void* ptr,const char* func, const size_t line);
void operator delete[](void* ptr,const char* func, const size_t line);
void operator delete(void* ptr,const std::nothrow_t& no,const char* func, const size_t line);
void operator delete[](void* ptr,const std::nothrow_t& no,const char* func, const size_t line);

/*��ȡ��ǰʹ�õ��ڴ��С*/
TPR_INT64 MemoryUsed();

/*ɾ���ͷŵ��ڴ�*/
void MemoryDelete(void* ptr);

/*��ȫ�ͷ��ڴ�궨��*/
#define SAFE_DEL(ptr)           \
	if(ptr != NULL)             \
{                               \
	delete ptr;                 \
	MemoryDelete(ptr);			\
	ptr = NULL;                 \
}

/*��ȫ�ͷ��ڴ�궨��*/
#define SAFE_ARR_DEL(ptr)       \
	if (ptr != NULL)            \
{                               \
	delete[] ptr;               \
	MemoryDelete(ptr);			\
	ptr = NULL;                 \
}

/*��ȫ�ͷ��ڴ�궨��*/
#define SAFE_ARR_FREE(ptr)      \
	if (ptr != NULL)            \
{                               \
	free(ptr);                  \
	ptr = NULL;                 \
}

#define NPQ_FUCNTION __FUNCTION__				//������ϵͳ��
#define NPQ_LINE __LINE__						//�к�ϵͳ��
#define NPQ_NEW_FLAG __FUNCTION__,__LINE__		//�������ص�new�ӿڱ�ʶ
#endif
