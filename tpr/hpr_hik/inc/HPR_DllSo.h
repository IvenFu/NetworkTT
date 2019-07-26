/*
 * CopyRight:	HangZhou Hikvision System Technology Co., Ltd. All	Right Reserved.
 * FileName:	HRP_DllSo.h
 * Desc:		OS Layer Component.
 * Author:		schina
 * Date:		2009-03-02
 * Contact:		zhaoyiji@hikvision.com.cn
 * History:		Created By Zhaoyiji 2009-03-02
 * */

/*! 
	\file HPR_DllSo.h
	\brief HPR DLL Manager.

	Details.
*/


#ifndef __HPR_DLLSO_H__
#define __HPR_DLLSO_H__

#include "HPR_Config.h"
#include "HPR_Types.h"

/**
 * HPR_LoadDSo load dll so or other library.
 * @param pPath (IN) dll name hpr.dll or libhpr.so
 * @return HPR_INVALID_HANDEL fail, else succ
 * @sa HPR_UnloadDSo(), HPR_GetDsoSym()
 */
HPR_DECLARE HPR_HANDLE CALLBACK HPR_LoadDSo(const char* pPath);

/**
 * HPR_LoadDSo unload dll so or other library.
 * @param hModule (IN) success ruturned by HPR_LoadDSo.
 * @return 0 success, -1 fail.
 * @sa HPR_LoadDSo(), HPR_GetDsoSym()
 */
HPR_DECLARE HPR_INT32 CALLBACK HPR_UnloadDSo(HPR_HANDLE hModule);

/**
 * HPR_LoadDSo get the symbol of the dll/so by name.
 * @param hModule (IN) success ruturned by HPR_LoadDSo.
 * @param pName (IN) symbol name.
 * @return HPR_INVALID_HANDEL fail, else succ
 * @sa HPR_UnloadDSo(), HPR_UnloadDSo()
 */
HPR_DECLARE HPR_PROC CALLBACK HPR_GetDsoSym(HPR_HANDLE hModule, const char* pName);


///add by zhaoyiji 2011-10-20
//////////////////////////////////////////////////////////////////////////
#define HPR_MAX_DSO_PATH (260) //windows has 260

namespace hpr
{

	class HPR_DECLARE_CLASS hpr_dso
	{
	public:
		hpr_dso();
		hpr_dso(const char* path);
		hpr_dso(hpr_handle module);
		hpr_dso(const hpr_dso&);
		virtual ~hpr_dso();

	public:
		hpr_dso& operator=(const hpr_dso&);

		hpr_handle load(const char* path);
		hpr_int32 unload();
		hpr_handle get_symbol(const char* name);

	private:
		hpr_int32 format_path(char* path);

	private:
		hpr_handle m_hModule;
		char m_szPath[HPR_MAX_DSO_PATH];
	};

};

#endif
