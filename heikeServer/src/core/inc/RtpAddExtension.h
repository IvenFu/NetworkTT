/**	@rtp.h
*	@note 111.
*	@brief rtp��ش���
*
*	@author		222
*	@date		2016/11/14
*
*	@note 
*
*	@warning 
*/
#ifndef _RTP_ADDEXTENSION_H_
#define _RTP_ADDEXTENSION_H_
#include "rtp.h"


/**	@class	 CRtpAddExtension 
 *	@brief   ������չͷ
 *	@note	
 */
class CRtpAddExtension
{
public:
	/*���캯��*/
	CRtpAddExtension(){}

	/*��������*/
	virtual ~CRtpAddExtension(){}

	/*������չͷ*/
	int AddExtension(CRtpPacket& rtpPacket, const RTPHeaderExtension& stuExtenInfo, unsigned char* pBuf,unsigned int nBufLen);
private:
	/*�γɵ�����Ϣ����չͷ����*/
	int FormExtenStrSingle(unsigned char* aExtension,const RTPHeaderExtension& stuExtenInfo);
	
	/*�γɶ����Ϣ����չͷ����*/
	int FormExtenStrMuti(const unsigned char* aOldExt,const RTPHeaderExtension& stuOldExtenInfo,unsigned char* aNewExt,RTPHeaderExtension& stuNewExtenInfo);
};



#endif
