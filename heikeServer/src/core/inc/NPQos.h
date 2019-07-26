/**	@NPQos.h
*	@note 111.
*	@brief NPQ��ӿڶ���
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


#define NPQ_OK              0x00000000  //�ɹ�
#define NPQERR_PARA         0x80000001  //��������
#define NPQERR_SYSTEM       0x80000002  //����ϵͳ���ô���(������Դ����ʧ�ܻ��ڲ�����ȣ���
#define NPQERR_GENRAL       0x80000003  //�ڲ�ͨ�ô���
#define NPQERR_PRECONDITION 0x80000004  //����˳�����
#define NPQERR_NOTSUPPORT   0x80000005  //���ܲ�֧��
#define NPQERR_DATAFORMAT   0x80000006  //rtp����rtcp���ݴ���

/**	@enum	 NPQ_QOS_TYPE 
 *	@brief   Qos��������
 *	@note	 
 */
enum NPQ_QOS_TYPE
{
	QOS_TYPE_NACK = (1<<0),          //Nack		
	QOS_TYPE_FEC = (1<<1),			 //FEC
	QOS_TYPE_DEJITTER = (1<<2),		 //ȥ����
	QOS_TYPE_BW = (1<<3),			 //ӵ������
	QOS_TYPE_PLI = (1<<4),			 //PLI
	QOS_TYPE_SYNCHRONOUS = (1<<5),	 //����Ƶͬ��
	QOS_TYPE_ALL = 0xff,
};

/**	@enum	 NPQ_QOS_ROLE 
 *	@brief   Qos��ɫ���ֽ��ն˺ͷ��Ͷ�
 *	@note	 
 */
enum NPQ_QOS_ROLE
{
	NPQ_QOS_RECEIVER,	//���ն�
	NPQ_QOS_SENDER,		//���Ͷ�
};

/**	@enum	 NPQ_MAIN_TYPE 
 *	@brief   Qos������
 *	@note	 
 */
enum NPQ_MAIN_TYPE
{
	NPQ_MAIN_VEDIO,		//��ƵQos
	NPQ_MAIN_AUDIO,		//��ƵQos
	NPQ_MAIN_PRIVATE,	//˽������Qos
};

/**	@enum	 NPQ_DATA_TYPE 
 *	@brief   ��������
 *	@note	 
 */
enum NPQ_DATA_TYPE
{
	NPQ_DATA_RTP_VIDEO,			//��ƵRTP����
	NPQ_DATA_RTP_AUDIO,			//��ƵRTP����
	NPQ_DATA_RTP_PRIVATE,		//˽�а�RTP����
	NPQ_DATA_RTCP_VIDEO,		//��ƵRTCP
	NPQ_DATA_RTCP_AUDIO,		//��ƵRTCP
	NPQ_DATA_MULITIPLEX,	    //δ֪�������ݣ��ⲿ��ȷ���������ͣ���NPQ�ڲ�ʶ��,��Ҫ���sdp��
	NPQ_DATA_CMD,				//����ص�����ӦNPQ_CMD_TYPE ��Ҫ�ⲿ����Ӧ����
};

/**	@enum	 NPQ_CMD_TYPE 
 *	@brief   ��������
 *	@note	 
 */
enum NPQ_CMD_TYPE
{
	NPQ_CMD_ENCODE,			//����Ƶ�������
	NPQ_CMD_FORCE_I_FRAME,	//ǿ��I֡
};

/**	@struct	 NPQ_ECODE_PARAM 
 *	@brief   �����������
 *	@note	 
 */
struct NPQ_ECODE_PARAM
{
	int iVersion;				//�ṹ��汾�������Ժ����
	int iMaxBitRate;			//�������   ��λ bps
	int iResolution;			//�ֱ���  ��������SDKЭ�鶨�������ֵ
	unsigned char res[244]; 
};

/**	@struct	 NPQ_CMD 
 *	@brief   Qos����ص�, ֻ�����ڷ��Ͷ�
 *	@note	 
 */
struct NPQ_CMD
{
	int iVersion;					//�ṹ��汾�������Ժ����
	NPQ_CMD_TYPE enInfoType;		//��������
	union INFO_U
	{
		NPQ_ECODE_PARAM struEncode;
		unsigned char res[256]; 
	}unCmd;
};

/**	@struct	 NPQ_STAT 
 *	@brief   Qos״̬
 *	@note	 
 */
struct NPQ_STAT
{
	int iVersion;					//�ṹ��汾�������Ժ����
	unsigned int nRttUs;			//rtt����λus
	unsigned int nRealRttUs;		//ʵʱrtt����λus
	unsigned int nBitRate;			//��ǰ��������ʵ������,��λ bps
	unsigned char cLossFraction;	//�����ʣ���λ1/256
	unsigned char cLossFraction2;	//�����ָ�֮��Ķ����ʣ�ֻ���ڽ��ն˻�ȡ����λ1/256
	unsigned char res[246];
};

/**	@enum	 NPQ_PARAM_VALUE_TYPE 
 *	@brief   ��������
 *	@note	 
 */
enum NPQ_PARAM_VALUE_TYPE
{
	NPQ_PARAM_NULL,		//�ղ�������˽����Ϣ�н���
	NPQ_PARAM_SDP,		//sdp��Ϣ
};

/**	@struct	 NPQ_SENDER_PARAM 
 *	@brief   ���Ͷ�Qos���ò���
 *	@note	�ܴ�С128�ֽڹ̶�
 */
struct NPQ_SENDER_PARAM
{
	int bVaild;	        //�ṹ���Ƿ���Ч
	int iMinBitrate;	//��С���� ��λbps
	int iMaxBitrate;	//������� ��λbps
	char res[116];		//Ԥ���ֶ�
};

/**	@struct	 NPQ_RECEIVER_PARAM 
 *	@brief   ���ն�Qos���ò���
 *	@note	�ܴ�С128�ֽڹ̶�
 */
struct NPQ_RECEIVER_PARAM
{
	int  bVaild;	//�ṹ���Ƿ���Ч
	char res[124];	//Ԥ���ֶ�
};

/**	@struct	 NPQ_PARAM 
 *	@brief   Qos���ò���
 *	@note	 
 */
struct NPQ_PARAM
{
	int iVersion;				//�ṹ��汾�������Ժ����
	int m_type;					//Qos�������� NPQ_QOS_TYPE
	int bVideo;					//�Ƿ�����ƵQos
	int bAudio;					//�Ƿ�����ƵQos
	NPQ_PARAM_VALUE_TYPE enValueType;	//��������
	void*  pValue;						//����ֵ
	union DIF_PARAM
	{
		NPQ_SENDER_PARAM struSender;	//���Ͷ˲���
		NPQ_RECEIVER_PARAM struReceiver;//���ն˲���
		unsigned char res[128];	
	}unDifParam;
	unsigned char res[120];		
};

/**	@struct	 NPQ_SET_NOTIFY_PARAM 
 *	@brief   ����֪ͨ������ֻ���ڽ��նˣ�����ʱ����
 *	@note	 
 */
struct NPQ_SET_NOTIFY_PARAM
{
	int iVersion;				//�ṹ��汾�������Ժ����
	unsigned int nBitRate;		//���� ��λbps
	bool bHaveBitrate;			//�����ֶ��Ƿ���Ч��־
	bool bHaveForceIframe;		//ǿ��I֡�ֶ��Ƿ���Ч��־
	bool bForceIframe;			//ǿ��I֡
	bool bHaveScale;			//scale�ֶ��Ƿ���Ч��־
	float fScale;				//scale��Ϣ  1��2��4��8
	unsigned char res[240];		
};

/**	@fn NPQCb
*	@brief �ص�����
*	@param  int id  id��
*	@param  int iDataType  ��������  ��NPQ_DATA_TYPE����
*	@param  unsigned char* pData  ���ݵ�ַ
*	@param  unsigned int uDataLen ���ݳ���
*	@param  void* pUser �ص��û�����
*	@return void
*/
typedef void  (__stdcall *NPQCb)( int id , int iDataType, unsigned char* pData, unsigned int nDataLen, void* pUser);

/**	@fn NPQ_Create
*	@brief ����һ·Qos����
*	@param  NPQ_QOS_ROLE enType  ����
*	@return NPQ������
*/
NPQ_API int __stdcall NPQ_Create(NPQ_QOS_ROLE enType);

/**	@fn NPQ_Destroy
*	@brief ����һ·Qos����
*	@param  int id  id��
*	@return NPQ������
*/
NPQ_API int __stdcall NPQ_Destroy(int id);

/**	@fn NPQ_Start
*	@brief ����һ·Qos����
*	@param  int id  id��
*	@return NPQ������
*/
NPQ_API int __stdcall NPQ_Start(int id);

/**	@fn NPQ_Stop
*	@brief ֹͣһ·Qos����
*	@param  int id  id��
*	@return NPQ������
*/
NPQ_API int __stdcall NPQ_Stop(int id);

/**	@fn NPQ_RegisterDataCallBack
*	@brief ע�����ݻص�
*	@param  int id  id��
*	@param  NPQCb pCallback �ص�����ָ��
*	@param  void* pUser �û�����
*	@return NPQ������
*/
NPQ_API int __stdcall NPQ_RegisterDataCallBack(int id, NPQCb pCallback, void* pUser);

/**	@fn NPQ_InputData
*	@brief ����RTP��RTCP����
*	@param  int id  id��
*	@param  int iDataType  ��������  ͬNPQ_DATA_TYPE
*	@param  unsigned char* pData  ���ݵ�ַ
*	@param  unsigned int uDataLen ���ݳ���
*	@param  void* pUser �ص��û�����
*	@return NPQ������
*/
NPQ_API int __stdcall NPQ_InputData(int id, int iDataType, unsigned char* pData, unsigned int nDataLen);

/**	@fn NPQ_SetParam
*	@brief ���ò���
*	@param  int id  id��
*	@param  void* pParam   ��������NPQ_PARAM
*	@return NPQ������
*/
NPQ_API int __stdcall NPQ_SetParam(int id,  void* pParam);

/**	@fn NPQ_SetNotifyParam
*	@brief ����֪ͨ����
*	@param  int id  id��
*	@param  void* pNotifyParam   ��������NPQ_SET_NOTIFY_PARAM
*	@return NPQ������
*/
NPQ_API int __stdcall NPQ_SetNotifyParam(int id,  NPQ_SET_NOTIFY_PARAM* pNotifyParam);

/**	@fn NPQ_GetStat
*	@brief ��ȡ״̬
*	@param  int id  id��
*	@param  NPQ_MAIN_TYPE enMainType  Qos������
*	@param  void* pStat  ״̬��Ϣ����NPQ_STAT
*	@return NPQ������
*/
NPQ_API int __stdcall NPQ_GetStat(int id,NPQ_MAIN_TYPE enMainType, void* pStat);


/**	@fn NPQ_SetLogFile
*	@brief ������־�ļ�·������֧��hlog��ƽ̨��
*	@param  char* szAbsFileName  ��־�ļ�·���������ļ�����
*	@return NPQ������
*/
NPQ_API int __stdcall NPQ_SetLogFile(char* szAbsFileName);

#endif

