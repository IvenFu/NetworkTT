/**	@NPQos.h
*	@note 111.
*	@brief NPQ库接口定义
*
*	@author		222
*	@date		2016/11/14
*
*	@note
*
*	@warning 
*/
#ifndef _NPQOS_H_
#define _NPQOS_H_

#if defined(WIN32)
#if defined(NPQOS_EXPORTS)
#define NPQ_API extern "C" __declspec(dllexport)
#else
#define NPQ_API extern "C" __declspec(dllimport)
#endif
#else

#ifndef __stdcall
#define __stdcall
#endif

#ifndef NPQ_API
#define NPQ_API extern "C"
#endif

#endif


#define NPQ_OK              0x00000000  //成功
#define NPQERR_PARA         0x80000001  //参数错误
#define NPQERR_SYSTEM       0x80000002  //操作系统调用错误(包括资源申请失败或内部错误等））
#define NPQERR_GENRAL       0x80000003  //内部通用错误
#define NPQERR_PRECONDITION 0x80000004  //调用顺序错误
#define NPQERR_NOTSUPPORT   0x80000005  //功能不支持
#define NPQERR_DATAFORMAT   0x80000006  //rtp或者rtcp数据错误

/**	@enum	 NPQ_QOS_TYPE 
 *	@brief   Qos策略类型
 *	@note	 
 */
enum NPQ_QOS_TYPE
{
	QOS_TYPE_NACK = (1<<0),          //Nack		
	QOS_TYPE_FEC = (1<<1),			 //FEC
	QOS_TYPE_DEJITTER = (1<<2),		 //去抖动
	QOS_TYPE_BW = (1<<3),			 //拥塞控制
	QOS_TYPE_PLI = (1<<4),			 //PLI
	QOS_TYPE_SYNCHRONOUS = (1<<5),	 //音视频同步
	QOS_TYPE_ALL = 0xff,
};

/**	@enum	 NPQ_QOS_ROLE 
 *	@brief   Qos角色，分接收端和发送端
 *	@note	 
 */
enum NPQ_QOS_ROLE
{
	NPQ_QOS_RECEIVER,	//接收端
	NPQ_QOS_SENDER,		//发送端
};

/**	@enum	 NPQ_MAIN_TYPE 
 *	@brief   Qos主类型
 *	@note	 
 */
enum NPQ_MAIN_TYPE
{
	NPQ_MAIN_VEDIO,		//视频Qos
	NPQ_MAIN_AUDIO,		//音频Qos
	NPQ_MAIN_PRIVATE,	//私有数据Qos
};

/**	@enum	 NPQ_DATA_TYPE 
 *	@brief   数据类型
 *	@note	 
 */
enum NPQ_DATA_TYPE
{
	NPQ_DATA_RTP_VIDEO,			//视频RTP数据
	NPQ_DATA_RTP_AUDIO,			//音频RTP数据
	NPQ_DATA_RTP_PRIVATE,		//私有包RTP数据
	NPQ_DATA_RTCP_VIDEO,		//视频RTCP
	NPQ_DATA_RTCP_AUDIO,		//音频RTCP
	NPQ_DATA_MULITIPLEX,	    //未知类型数据（外部不确定数据类型，由NPQ内部识别,需要结合sdp）
	NPQ_DATA_CMD,				//命令回调，对应NPQ_CMD_TYPE 需要外部做相应处理
};

/**	@enum	 NPQ_CMD_TYPE 
 *	@brief   命令类型
 *	@note	 
 */
enum NPQ_CMD_TYPE
{
	NPQ_CMD_ENCODE,			//音视频编码参数
	NPQ_CMD_FORCE_I_FRAME,	//强制I帧
};

/**	@struct	 NPQ_ECODE_PARAM 
 *	@brief   编码参数定义
 *	@note	 
 */
struct NPQ_ECODE_PARAM
{
	int iVersion;				//结构体版本，用于以后兼容
	int iMaxBitRate;			//最大码率   单位 bps
	int iResolution;			//分辨率  按照网络SDK协议定义的索引值
	unsigned char res[244]; 
};

/**	@struct	 NPQ_CMD 
 *	@brief   Qos命令回调, 只适用于发送端
 *	@note	 
 */
struct NPQ_CMD
{
	int iVersion;					//结构体版本，用于以后兼容
	NPQ_CMD_TYPE enInfoType;		//命令类型
	union INFO_U
	{
		NPQ_ECODE_PARAM struEncode;
		unsigned char res[256]; 
	}unCmd;
};

/**	@struct	 NPQ_STAT 
 *	@brief   Qos状态
 *	@note	 
 */
struct NPQ_STAT
{
	int iVersion;					//结构体版本，用于以后兼容
	unsigned int nRttUs;			//rtt，单位us
	unsigned int nRealRttUs;		//实时rtt，单位us
	unsigned int nBitRate;			//当前所有数据实际码率,单位 bps
	unsigned char cLossFraction;	//丢包率，单位1/256
	unsigned char cLossFraction2;	//经过恢复之后的丢包率，只能在接收端获取，单位1/256
	unsigned char res[246];
};

/**	@enum	 NPQ_PARAM_VALUE_TYPE 
 *	@brief   参数类型
 *	@note	 
 */
enum NPQ_PARAM_VALUE_TYPE
{
	NPQ_PARAM_NULL,		//空参数，从私有信息中解析
	NPQ_PARAM_SDP,		//sdp信息
};

/**	@struct	 NPQ_SENDER_PARAM 
 *	@brief   发送端Qos配置参数
 *	@note	总大小128字节固定
 */
struct NPQ_SENDER_PARAM
{
	int bVaild;	        //结构体是否有效
	int iMinBitrate;	//最小码率 单位bps
	int iMaxBitrate;	//最大码率 单位bps
	char res[116];		//预留字段
};

/**	@struct	 NPQ_RECEIVER_PARAM 
 *	@brief   接收端Qos配置参数
 *	@note	总大小128字节固定
 */
struct NPQ_RECEIVER_PARAM
{
	int  bVaild;	//结构体是否有效
	char res[124];	//预留字段
};

/**	@struct	 NPQ_PARAM 
 *	@brief   Qos配置参数
 *	@note	 
 */
struct NPQ_PARAM
{
	int iVersion;				//结构体版本，用于以后兼容
	int m_type;					//Qos策略类型 NPQ_QOS_TYPE
	int bVideo;					//是否开启视频Qos
	int bAudio;					//是否开启音频Qos
	NPQ_PARAM_VALUE_TYPE enValueType;	//参数类型
	void*  pValue;						//参数值
	union DIF_PARAM
	{
		NPQ_SENDER_PARAM struSender;	//发送端参数
		NPQ_RECEIVER_PARAM struReceiver;//接收端参数
		unsigned char res[128];	
	}unDifParam;
	unsigned char res[120];		
};

/**	@struct	 NPQ_SET_NOTIFY_PARAM 
 *	@brief   配置通知参数，只用于接收端，可随时设置
 *	@note	 
 */
struct NPQ_SET_NOTIFY_PARAM
{
	int iVersion;				//结构体版本，用于以后兼容
	unsigned int nBitRate;		//码率 单位bps
	bool bHaveBitrate;			//码率字段是否有效标志
	bool bHaveForceIframe;		//强制I帧字段是否有效标志
	bool bForceIframe;			//强制I帧
	bool bHaveScale;			//scale字段是否有效标志
	float fScale;				//scale信息  1、2、4、8
	unsigned char res[240];		
};

/**	@fn NPQCb
*	@brief 回调函数
*	@param  int id  id号
*	@param  int iDataType  数据类型  见NPQ_DATA_TYPE定义
*	@param  unsigned char* pData  数据地址
*	@param  unsigned int uDataLen 数据长度
*	@param  void* pUser 回调用户参数
*	@return void
*/
typedef void  (__stdcall *NPQCb)( int id , int iDataType, unsigned char* pData, unsigned int nDataLen, void* pUser);

/**	@fn NPQ_Create
*	@brief 创建一路Qos服务
*	@param  NPQ_QOS_ROLE enType  类型
*	@return NPQ错误码
*/
NPQ_API int __stdcall NPQ_Create(NPQ_QOS_ROLE enType);

/**	@fn NPQ_Destroy
*	@brief 销毁一路Qos服务
*	@param  int id  id号
*	@return NPQ错误码
*/
NPQ_API int __stdcall NPQ_Destroy(int id);

/**	@fn NPQ_Start
*	@brief 启动一路Qos服务
*	@param  int id  id号
*	@return NPQ错误码
*/
NPQ_API int __stdcall NPQ_Start(int id);

/**	@fn NPQ_Stop
*	@brief 停止一路Qos服务
*	@param  int id  id号
*	@return NPQ错误码
*/
NPQ_API int __stdcall NPQ_Stop(int id);

/**	@fn NPQ_RegisterDataCallBack
*	@brief 注册数据回调
*	@param  int id  id号
*	@param  NPQCb pCallback 回调函数指针
*	@param  void* pUser 用户参数
*	@return NPQ错误码
*/
NPQ_API int __stdcall NPQ_RegisterDataCallBack(int id, NPQCb pCallback, void* pUser);

/**	@fn NPQ_InputData
*	@brief 输入RTP和RTCP数据
*	@param  int id  id号
*	@param  int iDataType  数据类型  同NPQ_DATA_TYPE
*	@param  unsigned char* pData  数据地址
*	@param  unsigned int uDataLen 数据长度
*	@param  void* pUser 回调用户参数
*	@return NPQ错误码
*/
NPQ_API int __stdcall NPQ_InputData(int id, int iDataType, unsigned char* pData, unsigned int nDataLen);

/**	@fn NPQ_SetParam
*	@brief 设置参数
*	@param  int id  id号
*	@param  void* pParam   参数，见NPQ_PARAM
*	@return NPQ错误码
*/
NPQ_API int __stdcall NPQ_SetParam(int id,  void* pParam);

/**	@fn NPQ_SetNotifyParam
*	@brief 设置通知参数
*	@param  int id  id号
*	@param  void* pNotifyParam   参数，见NPQ_SET_NOTIFY_PARAM
*	@return NPQ错误码
*/
NPQ_API int __stdcall NPQ_SetNotifyParam(int id,  NPQ_SET_NOTIFY_PARAM* pNotifyParam);

/**	@fn NPQ_GetStat
*	@brief 获取状态
*	@param  int id  id号
*	@param  NPQ_MAIN_TYPE enMainType  Qos主类型
*	@param  void* pStat  状态信息，见NPQ_STAT
*	@return NPQ错误码
*/
NPQ_API int __stdcall NPQ_GetStat(int id,NPQ_MAIN_TYPE enMainType, void* pStat);


/**	@fn NPQ_SetLogFile
*	@brief 设置日志文件路径（不支持hlog的平台）
*	@param  char* szAbsFileName  日志文件路径（包含文件名）
*	@return NPQ错误码
*/
NPQ_API int __stdcall NPQ_SetLogFile(char* szAbsFileName);

#endif

