/*
 * CopyRight:	HangZhou Hikvision System Technology Co., Ltd. All	Right Reserved.
 * FileName:	HRP_Hpr.h
 * Desc:		OS Layer Component.
 * Author:		schina
 * Date:		2009-03-02
 * Contact:		zhaoyiji@hikvision.com.cn
 * History:		Created By Zhaoyiji 2009-03-02
 * */

/*! 
	\file HPR_Hpr.h
	\brief HPR Hpr Manager.

	Details.
*/

#ifndef __HPR_HPR_H__
#define __HPR_HPR_H__

#include "HPR_Config.h"
#include "HPR_Types.h"

/**
 * HPR_Init 初始化HPR库,使用HPR库必须首先调用这个函数
 * @param void
 * @return 0成功，-1失败。
 * @sa HPR_Fini().
 */
HPR_DECLARE HPR_INT32 CALLBACK HPR_Init();
HPR_DECLARE HPR_INT32 CALLBACK HPR_InitEx();

/**
 * HPR_Fini 销毁HPR库,最后调用
 * @param void
 * @return void
 * @sa HPR_Init().
 */
HPR_DECLARE HPR_VOID CALLBACK HPR_Fini();
HPR_DECLARE HPR_VOID CALLBACK HPR_FiniEx();


/**
* HPR_GetHPRVersion 获取HPR当前版本号
* @param void
* @return HPR_UINT32类型，版本号0.5.2.0，返回0x00050200
*/
HPR_DECLARE HPR_UINT32 CALLBACK HPR_GetHPRVersion();

#define LOG_PRINT		0x00000001	//只打印，windows到debug，linux/unix到console
#define LOG_FILE			0x00000002	//只写文件
/**
 * HPR_Log 操作hpr内部日志打印功能
 * @param action-输出行为，置0不打印，置LOG_PRINT，LOG_FILE打印并打印到不同目标
				path-action中指定LOG_FILE时设置输出目录，NULL使用默认在"./hprlog/"，文件名不能指定
 * @return 0-成功，-1失败
 * @sa void.
 */
HPR_DECLARE HPR_INT32 CALLBACK HPR_Log(int action, const char* path = NULL);

#endif
