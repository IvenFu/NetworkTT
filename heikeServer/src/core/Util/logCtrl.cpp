/**	@logCtrl.cpp
*	@note 111.
*	@brief �ڴ������־�����
*
*	@author		222
*	@date		2018/4/14
*
*	@note 
*
*	@warning 
*/
#include <string>
#include <map>
#include "TPR_Mutex.h"
#include "TPR_Guard.h"
#include "Util.h"

using std::map;
using std::string;

/**	@struct	 MEM_RECORD_T 
 *	@brief  �ڴ�ڵ��¼
 *	@note	 
 */
struct MEM_RECORD_T
{
	string szFunc;  //�����ڴ�ĺ���
	size_t iLine;	//�����ڴ�Ĵ����к�
	size_t iSize;	//������ڴ��С
};

//��¼�ڴ��map����
typedef map<void* , MEM_RECORD_T> MEMOY_MAP;

static TPR_Mutex g_lock;			//�����ڴ��¼������ȫ����
static TPR_INT64 g_llMemoryLen = 0; //ʹ���ڴ泤��ͳ��
static MEMOY_MAP g_mMemoryMap;		//��¼�ڴ�ʹ�ýڵ��б�

/**	@fn MemoryUsed
*	@brief ��ȡ��ǰʹ�õ��ڴ��С
*	@return �ڴ��С
*/
TPR_INT64 MemoryUsed()
{
	TPR_Guard gurad(&g_lock);

	//MEMOY_MAP::iterator it = mMemoryMap.begin();
	//for(;it!=mMemoryMap.end();++it)
	//{
	//	NPQ_DEBUG("MemoryUsed size=%d,func=%s,line=%d",it->second.iSize,it->second.szFunc.c_str(),it->second.iLine);
	//}
	return g_llMemoryLen;
}

/**	@fn MemoryNew
*	@brief ��¼��������ڴ�
*	@param  void* ptr  �ڴ���ʼ��ַ
*	@param size_t size �ڴ泤��
*	@param const char* func �����ڴ�ĺ���
*	@param const size_t lin �����ڴ�ĺ���������
*	@return void
*/
void MemoryNew(void* ptr,size_t size,const char* func, const size_t line)
{
	TPR_Guard gurad(&g_lock);

	MEM_RECORD_T struMem;
	struMem.szFunc.assign(func);
	struMem.iLine = line;
	struMem.iSize = size;

	g_llMemoryLen+=size;
	g_mMemoryMap[ptr] = struMem;

	//if(size > 1000)
	{
		//NPQ_DEBUG("MemoryNew size=%d,func=%s,line=%d",size,func,line);
	}
}

/**	@fn MemoryDelete
*	@brief ɾ���ͷŵ��ڴ�
*	@param  void* ptr  �ڴ���ʼ��ַ
*	@return void
*/
void MemoryDelete(void* ptr)
{
	TPR_Guard gurad(&g_lock);

	MEMOY_MAP::iterator it;

	it = g_mMemoryMap.find(ptr);
	if(it == g_mMemoryMap.end())
	{
		return;
	}
	
	int iSize = it->second.iSize;
	//NPQ_DEBUG("MemoryDelete size=%d,func=%s,line=%d",iSize,it->second.szFunc.c_str(),it->second.iLine);

	g_mMemoryMap.erase(ptr);
	g_llMemoryLen -= iSize;
	g_llMemoryLen = npq_max(0,g_llMemoryLen);
}

/**	@fn operator new
*	@brief ����new������
*	@param  size_t size �����ڴ��С
*	@param  const char* fun �����ڴ����ڵĺ���
*	@param  const size_t line �����ڴ����ڵ��к�
*	@return ������ڴ��ַ
*/
void* operator new(size_t size, const char* func, const size_t line) 
{
	return ::operator new(size);
}

/**	@fn operator new
*	@brief ����nothrow�汾new������
*	@param  size_t size �����ڴ��С
*	@param  onst std::nothrow_t& no  nothrow��ʶ��
*	@param  const char* fun �����ڴ����ڵĺ���
*	@param  const size_t line �����ڴ����ڵ��к�
*	@return ������ڴ��ַ
*/
void* operator new(size_t size, const std::nothrow_t& no, const char* func, const size_t line) 
{
	void* ptr;

	ptr = ::operator new(size,no);
	if(ptr == NULL)
	{
		return NULL;
	}

	MemoryNew(ptr,size,func,line);
	return ptr;
}

/**	@fn operator new
*	@brief ��������汾new������
*	@param  size_t size �����ڴ��С
*	@param  const char* fun �����ڴ����ڵĺ���
*	@param  const size_t line �����ڴ����ڵ��к�
*	@return ������ڴ��ַ
*/
void* operator new[](size_t size, const char* func, const size_t line) 
{
	return ::operator new[](size);
}

/**	@fn operator new
*	@brief ����nothrow ����汾new������
*	@param  size_t size �����ڴ��С
*	@param  onst std::nothrow_t& no  nothrow��ʶ��
*	@param  const char* fun �����ڴ����ڵĺ���
*	@param  const size_t line �����ڴ����ڵ��к�
*	@return ������ڴ��ַ
*/
void* operator new[](size_t size, const std::nothrow_t& no,const char* func, const size_t line) 
{
	void* ptr;

	ptr = ::operator new[](size,no);
	if(ptr == NULL)
	{
		return NULL;
	}

	MemoryNew(ptr,size,func,line);
	return ptr;
}

//�������غ���ֻ����ϵͳ�Զ����ã������������ã�����new��ʱ��ʼ��ʧ�ܣ��Զ�����delete�ͷ��ڴ棬�������й©����
/**	@fn operator delete
*	@brief ��������汾delete������
*	@param  void* ptr �ڴ��׵�ַ
*	@param  const char* fun �����ڴ����ڵĺ���
*	@param  const size_t line �����ڴ����ڵ��к�
*	@return 
*/
void operator delete(void* ptr,const char* func, const size_t line) 
{
	::operator delete(ptr);
}

/**	@fn operator delete
*	@brief ��������汾delete������
*	@param  void* ptr �ڴ��׵�ַ
*	@param  const char* fun �����ڴ����ڵĺ���
*	@param  const size_t line �����ڴ����ڵ��к�
*	@return 
*/
void operator delete[](void* ptr,const char* func, const size_t line) 
{
	::operator delete[](ptr);
}

/**	@fn operator delete
*	@brief ����nothrow �汾delete������
*	@param  void* ptr �ڴ��׵�ַ
*	@param  onst std::nothrow_t& no  nothrow��ʶ��
*	@param  const char* fun �����ڴ����ڵĺ���
*	@param  const size_t line �����ڴ����ڵ��к�
*	@return void
*/
void operator delete(void* ptr,const std::nothrow_t& no,const char* func, const size_t line) 
{
	::operator delete(ptr);
}

/**	@fn operator delete
*	@brief ����nothrow ����汾delete������
*	@param  void* ptr �ڴ��׵�ַ
*	@param  onst std::nothrow_t& no  nothrow��ʶ��
*	@param  const char* fun �����ڴ����ڵĺ���
*	@param  const size_t line �����ڴ����ڵ��к�
*	@return void
*/
void operator delete[](void* ptr,const std::nothrow_t& no,const char* func, const size_t line) 
{
	::operator delete[](ptr);
}











