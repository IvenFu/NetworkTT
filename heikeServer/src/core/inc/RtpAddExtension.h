/**	@rtp.h
*	@note 111.
*	@brief rtp相关代码
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
 *	@brief   增加扩展头
 *	@note	
 */
class CRtpAddExtension
{
public:
	/*构造函数*/
	CRtpAddExtension(){}

	/*析构函数*/
	virtual ~CRtpAddExtension(){}

	/*增加扩展头*/
	int AddExtension(CRtpPacket& rtpPacket, const RTPHeaderExtension& stuExtenInfo, unsigned char* pBuf,unsigned int nBufLen);
private:
	/*形成单个信息的扩展头内容*/
	int FormExtenStrSingle(unsigned char* aExtension,const RTPHeaderExtension& stuExtenInfo);
	
	/*形成多个信息的扩展头内容*/
	int FormExtenStrMuti(const unsigned char* aOldExt,const RTPHeaderExtension& stuOldExtenInfo,unsigned char* aNewExt,RTPHeaderExtension& stuNewExtenInfo);
};



#endif
