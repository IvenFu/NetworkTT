/**	@file  avc_sps_parse.h
 *	@note  
 *	@brief avc_sps_parse头文件
 *
 *	@author	Media Playback SDK Team
 *	@date	
 *
 *	@note   2013/03/02,新增注释
 */
#ifndef _H265_PARSE_H_
#define _H265_PARSE_H_

namespace HEVC_PARSE
{

	typedef enum _H265_SLICE_TYPE
	{
		NAL_UNIT_CODED_SLICE_TRAIL_N = 0, // 0
		NAL_UNIT_CODED_SLICE_TRAIL_R,     // 1
		NAL_UNIT_CODED_SLICE_TSA_N,       // 2
		NAL_UNIT_CODED_SLICE_TLA_R,       // 3
		NAL_UNIT_CODED_SLICE_STSA_N,      // 4
		NAL_UNIT_CODED_SLICE_STSA_R,      // 5
		NAL_UNIT_CODED_SLICE_RADL_N,      // 6
		NAL_UNIT_CODED_SLICE_RADL_R,      // 7
		NAL_UNIT_CODED_SLICE_RASL_N,      // 8
		NAL_UNIT_CODED_SLICE_RASL_R,      // 9
		NAL_UNIT_CODED_SLICE_BLA_W_LP,    // 16
		NAL_UNIT_CODED_SLICE_BLA_W_RADL,  // 17
		NAL_UNIT_CODED_SLICE_BLA_N_LP,    // 18
		NAL_UNIT_CODED_SLICE_IDR_W_RADL,  // 19
		NAL_UNIT_CODED_SLICE_IDR_N_LP,    // 20
		NAL_UNIT_CODED_SLICE_CRA,         // 21
	};

	int OPENHEVC_GetPicSizeFromSPS(unsigned char *nalu_buf, int buf_size, int *p_width, int *p_height);
	int OPENHEVC_GetFrameTypeFromSlice(unsigned char *nalu_buf, int buf_size);

}

#endif//