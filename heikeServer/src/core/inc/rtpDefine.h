/**	@rtpDefine.h
*	@note 111.
*	@brief rtp参数定义
*
*	@author		222
*	@date		2017/5/15
*
*	@note 
*
*	@warning 
*/
#ifndef _RTPDEFINE_H_
#define _RTPDEFINE_H_


/**	@enum	 RTP_DATA_TYPE 
 *	@brief   数据类型
 *	@note	 
 */
enum RTP_DATA_TYPE
{
	RTP_VIDEO,
	RTP_AUDIO,
	RTP_PRIVATE,
	RTP_UNKNOW,
};

/**	@enum	 RTCP_DATA_TYPE 
 *	@brief   rtcp数据类型
 *	@note	 
 */
enum RTCP_DATA_TYPE
{
	RTCP_VIDEO,
	RTCP_AUDIO,
	RTCP_UNKNOW,
};

/**	@enum	 AUDIO_ENCODE_TYPE 
 *	@brief   音频编码类型
 *	@note	 
 */
enum AUDIO_ENCODE_TYPE
{
	NPQ_G711A,
	NPQ_G711U,
	NPQ_G7221,              //G722.1
	NPQ_G726,				//
	NPQ_AAC,
    NPQ_AAC_LD,
	NPQ_MPA,
	NPQ_OPUS,
	NPQ_PCM,
	NPQ_AUDIO_UNKNOW = 0xff,
};

/**	@enum	 VIDEO_ENCODE_TYPE 
 *	@brief   视频编码类型
 *	@note	 
 */
enum VIDEO_ENCODE_TYPE
{
	NPQ_H264,
	NPQ_H265,
	NPQ_MPEG4,
	NPQ_SVAC,
	NPQ_VIDEO_UNKNOW = 0xfe,
};
#endif

