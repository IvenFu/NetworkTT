

#ifndef _HIK_FEC_INTTERFACE_H_
#define _HIK_FEC_INTTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif


#define FEC_PACKET_VIDEO_PAYLOAD_TYPE    (0x75)  //��ƵFEC�� PTֵ
#define FEC_PACKET_AUDIO_PAYLOAD_TYPE    (0x76)  //��ƵFEC�� PTֵ
#define FEC_PACKET_AUDIO_RED_PAYLOAD_TYPE    (0x7F)  //��ƵRED�� PTֵ

/* ״̬�룺С�����ʾ�д���(0x80000000��ʼ)�����ʾʧ�ܣ��������ʾ�ɹ�  */
#define HIK_FEC_LIB_S_OK			0x00000000  /* �ɹ�					    */
#define HIK_FEC_LIB_E_FAIL			0x80000001  /* ʧ��					    */
#define HIK_FEC_LIB_E_PARA_NULL		0x80000002	/* ����ָ��Ϊ��			    */
#define HIK_FEC_LIB_E_MEM_OVER		0x80000003	/* �ڴ����				    */
#define HIK_FEC_LIB_E_REPEAT		0x80000004	/* ���ݰ��ظ�����		    */
#define HIK_FEC_LIB_E_ORDER		    0x80000005	/* ���ݰ���Ų����������Ͷˣ�*/

/*************************�ṹ������*****************************************************/

typedef struct _DATA_INFO
{
	bool bMarkbit;		//�Ƿ���markbit���(�����һ�������ʾ���һ����)
	bool bKeyFrame;		//�Ƿ��ǹؼ�֡���(��ƵΪfalse)
}DATA_INFO;

typedef struct _DATA_BASE_VEC
{
	unsigned char*         pBuf;
	unsigned int           nDataLen;
	struct _DATA_BASE_VEC* pNext;
}DATA_PACK_VEC;

typedef struct _HikFecProtectionParams
{
	unsigned char aKeyPrtotectionFactor;	// �ؼ�֡�������� 1 �� 255
	unsigned char aDeltaPrtotectionFactor;	// �ǹؼ�֡�������� 1 �� 255
}HikFecProtectionParams;

/******************************************************************************
* �ӿں�������
******************************************************************************/

typedef void(*HIKFEC_OutputDataCallBack)(DATA_PACK_VEC* pstFecList, void *pUser);

/******************************************************************************
* ��  �ܣ���ȡ���
* ��  ������
* ����ֵ�����
******************************************************************************/
void * HIKFEC_CreateHandle(bool bEncoder,HikFecProtectionParams* pEncoderParam);

/******************************************************************************
* ��  �ܣ�ע�����ݻص�
* ��  ����handle - ���(handle��HIKFEC_CreateHandle����)
*        HIKFEC_OutputDataCallBack   - �ص�����
pUser                - �û�ָ��
* ����ֵ�����ش�����
******************************************************************************/
int HIKFEC_RegisterOutputFecCallBack(void *handle, HIKFEC_OutputDataCallBack pCb, void *pUser);

/******************************************************************************
* ��  �ܣ�����RTP����
* ��  ����handle - ���(handle��HIKFEC_CreateHandle����)
*        pstDataVec   - �����RTP���ݰ�
* ����ֵ�����ش�����
******************************************************************************/
int HIKFEC_InputData(void* handle, DATA_PACK_VEC *pstDataVec,DATA_INFO* pstruInfo);

/******************************************************************************
* ��  �ܣ�����RTP����
* ��  ����handle - ���(handle��HIKFEC_CreateHandle����)
*        pEncoderParam   - FEC����ϵ��
* ����ֵ�����ش�����
******************************************************************************/
int HIKFEC_SetParam(void* handle,HikFecProtectionParams* pEncoderParam);

/******************************************************************************
* ��  �ܣ����پ��
* ��  ����handle - ���(handle��HIKFEC_CreateHandle����)
* ����ֵ����
******************************************************************************/
void HIKFEC_Release(void *handle);


#ifdef __cplusplus
}
#endif 


#endif