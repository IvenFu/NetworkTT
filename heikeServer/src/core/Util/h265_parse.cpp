#include <string.h>
#include "h265_parse.h"

namespace HEVC_PARSE
{

typedef struct _OPEN_BITSTREAM_T
{
    unsigned char  *initbuf;
    unsigned int  bitscnt;
} OPEN_BITSTREAM;

// 指数哥伦布码开方表
static unsigned char OPENHEVC_LOG2_TAB[256] = 
{
    0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};

// 指数哥伦布码长度表
static unsigned char OPENHEVC_GOLOMB_VLC_LEN[512] =                        
{
    19,17,15,15,13,13,13,13,11,11,11,11,11,11,11,11,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

// 无符号指数哥伦布码字表
static unsigned int OPENHEVC_UE_GOLOMB_VLC_CODE[512] = 
{
    32,32,32,32,32,32,32,32,31,32,32,32,32,32,32,32,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
    7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9,10,10,10,10,11,11,11,11,12,12,12,12,13,13,13,13,14,14,14,14,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int OPENHEVC_ebsp_to_rbsp(unsigned char* streamBuffer, int len)
{
    int i, j, count;
    int x03_cnt = 0;

    j = 0;
    count = 0;
    for (i = 0; i < len; i++) 
    { 
        //starting from begin_bytepos to avoid header information
        if ((count == 2) && (streamBuffer[j] == 0x03)) 
        {
            memmove(&streamBuffer[j], &streamBuffer[j + 1], len - i - 1); //hcj20120303
            count = 0;
            i++;
            x03_cnt++;
        }	
        if (streamBuffer[j] == 0x00)
        {
            count++;
        }
        else
        {
            count = 0;
        }
        j++;
    }

    return x03_cnt;
}

static void OPENHEVC_init_bitstream(OPEN_BITSTREAM * bs, unsigned char* buffer, int size)
{
    bs->bitscnt = 0;
    bs->initbuf = buffer;
}

static void OPENHEVC_skip_n_bits(OPEN_BITSTREAM *bs, int n)
{
    bs->bitscnt += n;
    return;
}

#define OPENHEVC_BITSWAP(bitswap_t) {unsigned int bitswap_tmp;                            \
    char *bitswap_src = (char *)&bitswap_t;     \
    char *bitswap_dst = (char *)&bitswap_tmp;   \
    bitswap_dst[0] = bitswap_src[3];		\
    bitswap_dst[1] = bitswap_src[2];		\
    bitswap_dst[2] = bitswap_src[1];		\
    bitswap_dst[3] = bitswap_src[0];		\
    bitswap_t      = bitswap_tmp;  }

static unsigned int OPENHEVC_read_n_bits(OPEN_BITSTREAM *bs, int n)
{
    unsigned int bcnt  = bs->bitscnt;
    unsigned int tmp   = *(unsigned int *)(bs->initbuf + (bcnt >> 3));

    OPENHEVC_BITSWAP(tmp);

    tmp <<= bcnt & 7;
    tmp >>= (32 - n);

    bs->bitscnt += n;

    return tmp;
}

static unsigned int OPENHEVC_show_n_bits(OPEN_BITSTREAM *bs, unsigned int n)
{
    unsigned int bcnt  = bs->bitscnt;
    unsigned int tmp   = *(unsigned int *)(bs->initbuf + (bcnt >> 3));

    OPENHEVC_BITSWAP(tmp);

    tmp <<= bcnt & 7;

    return tmp >> (32 - n);
}

static unsigned int OPENHEVC_show_n_bits_long(OPEN_BITSTREAM *bs, unsigned int n)
{
    if (!n) {
        return 0;
    } else if (n <= 25) {
        return OPENHEVC_show_n_bits(bs, n);
    } else {
        unsigned ret = OPENHEVC_read_n_bits(bs, 16) << (n - 16);
        ret |= OPENHEVC_read_n_bits(bs, n - 16);
        bs->bitscnt -= n;
        return ret;
    }
}

static int OPENHEVC_log2(unsigned int v)
{
    int n = 0;

    if (v & 0xffff0000)
    {
        v >>= 16;
        n += 16;
    }
    if (v & 0xff00)
    {
        v >>= 8;
        n += 8;
    }
    n += OPENHEVC_LOG2_TAB[v];

    return n;
}

static unsigned int OPENHEVC_read_n_bits_long(OPEN_BITSTREAM *bs, unsigned int n)
{
    if (!n) {
        return 0;
    } else if (n <= 25) {
        return OPENHEVC_read_n_bits(bs, n);
    } else {
        unsigned ret = OPENHEVC_read_n_bits(bs, 16) << (n - 16);
        return ret | OPENHEVC_read_n_bits(bs, n - 16);
    }
}

static unsigned int OPENHEVC_read_ue_golomb_long(OPEN_BITSTREAM *bs)
{
    unsigned int buf, log;

    buf = OPENHEVC_show_n_bits_long(bs, 32);
    log = 31 - OPENHEVC_log2(buf);
    OPENHEVC_skip_n_bits(bs, log);

    return OPENHEVC_read_n_bits_long(bs, log + 1) - 1;
    //return HEVCDEC_read_ue_golomb(bs);
}

static unsigned int OPENHEVC_read_ue_golomb(OPEN_BITSTREAM *bs)
{
    int len;
    unsigned int bcnt  = bs->bitscnt;
    unsigned int tmp   = *(unsigned int *)(bs->initbuf + (bcnt >> 3));
    unsigned int re_tmp; 

    OPENHEVC_BITSWAP(tmp);

    tmp <<= (bcnt & 7);

    if (tmp & 0xf8000000)       
    {
        tmp >>= (32 - 9);
        bs->bitscnt += OPENHEVC_GOLOMB_VLC_LEN[tmp];

        return OPENHEVC_UE_GOLOMB_VLC_CODE[tmp];
    }
    else if (tmp & 0xff800000)
    {
        re_tmp = tmp;
        re_tmp >>= (32 - 9);

        len = OPENHEVC_GOLOMB_VLC_LEN[re_tmp];
        bs->bitscnt += len;

        tmp = tmp >> (32 - len);
        tmp--;

        return tmp;
    }
    else
    {
        len = 63 - 2 * OPENHEVC_log2(tmp);

        if ( len > 25 )
        {
            tmp = tmp | ( (bs->initbuf + (bcnt >> 3))[4] >> (8 - (bcnt & 7)) );
        }

        bs->bitscnt += len;
        tmp = tmp >> (32 - len);
        tmp--;

        return tmp;
    }
}

static int OPENHEVC_interpret_profiletilerlevel(OPEN_BITSTREAM *bs)
{
    int i;

    OPENHEVC_skip_n_bits(bs, 2);
    OPENHEVC_skip_n_bits(bs, 1);
    OPENHEVC_skip_n_bits(bs, 5);

    for (i = 0; i < 32; i++)
    {
        OPENHEVC_skip_n_bits(bs, 1);
    }

    OPENHEVC_skip_n_bits(bs, 1);
    OPENHEVC_skip_n_bits(bs, 1);
    OPENHEVC_skip_n_bits(bs, 1);
    OPENHEVC_skip_n_bits(bs, 1);

    OPENHEVC_skip_n_bits(bs, 16); // XXX_reserved_zero_44bits[0..15]
    OPENHEVC_skip_n_bits(bs, 16); // XXX_reserved_zero_44bits[16..31]
    OPENHEVC_skip_n_bits(bs, 12); // XXX_reserved_zero_44bits[32..43]

    return 0;
}

static int OPENHEVC_interpret_ptl(OPEN_BITSTREAM *bs, int max_num_sub_layers)
{
    int i;
    int sub_layer_profile_present_flag[7];
    int sub_layer_level_present_flag[7];
    
    OPENHEVC_interpret_profiletilerlevel(bs);

    OPENHEVC_skip_n_bits(bs, 8);

    for (i = 0; i < max_num_sub_layers - 1; i++) 
    {
        sub_layer_profile_present_flag[i] = OPENHEVC_read_n_bits(bs, 1);
        sub_layer_level_present_flag[i]   = OPENHEVC_read_n_bits(bs, 1);
    }

    if (max_num_sub_layers - 1 > 0)
    {
        for (i = max_num_sub_layers - 1; i < 8; i++)
        {
            OPENHEVC_skip_n_bits(bs, 2); // reserved_zero_2bits[i]
        }
    }

    for (i = 0; i < max_num_sub_layers - 1; i++) 
    {
        if (sub_layer_profile_present_flag[i])
        {
            OPENHEVC_interpret_profiletilerlevel(bs);
        }
        if (sub_layer_level_present_flag[i])
        {
            OPENHEVC_skip_n_bits(bs, 8);
        }
    }

    return 0;
}

static int OPENHEVC_interpret_sps(OPEN_BITSTREAM *bs, int *p_width, int *p_height)
{
    int max_sub_layers, chroma_format_idc;

    OPENHEVC_skip_n_bits(bs, 4);//sps_video_parameter_set_id
    max_sub_layers = OPENHEVC_read_n_bits(bs, 3) + 1;//sps_max_sub_layers_minus1
    OPENHEVC_skip_n_bits(bs, 1);//sps_temporal_id_nesting_flag

    OPENHEVC_interpret_ptl(bs, max_sub_layers);

    OPENHEVC_read_ue_golomb_long(bs);//seq_parameter_set_id
    chroma_format_idc    = OPENHEVC_read_ue_golomb_long(bs);

    if (chroma_format_idc == 3)
    {
        OPENHEVC_skip_n_bits(bs, 1);
    }

    *p_width  = OPENHEVC_read_ue_golomb_long(bs);
    *p_height = OPENHEVC_read_ue_golomb_long(bs);

    return 0;
}

static int OPENHEVC_preread_slice_header(OPEN_BITSTREAM * bs, int nal_unit_type)
{
    int first_slice_in_pic_flag, slice_type;

    first_slice_in_pic_flag = OPENHEVC_read_n_bits(bs, 1);

    if (first_slice_in_pic_flag == 0)
    {
        return -2; // 不是第一个slice，无法确定帧类型
    }

    if (nal_unit_type >= 16 && nal_unit_type <= 23)
    {
        OPENHEVC_read_n_bits(bs, 1);//no_output_of_prior_pics_flag
    }

    OPENHEVC_read_ue_golomb(bs);//pps_id

    //当前标准下，num_extra_slice_header_bits永远为0

    slice_type = OPENHEVC_read_ue_golomb(bs);

    if (slice_type >= 0 && slice_type <= 2)
    {
        return slice_type;
    }
    else
    {
        return -2;//错误的类型（码流有错）
    }
}

/***************************************************************************************************
* 功  能：从SPS的NALU单元中解析图像宽高
* 参  数：nalu_buf    [in]    包含SPS的NALU单元缓冲区（不含起始码）
*         buf_size    [in]    缓冲区长度
*         p_width     [ou]    输出的图像宽
*         p_height    [ou]    输出的图像高
*         
* 返回值：0 ：成功
*         -1：参数错误
* 备  注：
* 修  改: 
***************************************************************************************************/
int OPENHEVC_GetPicSizeFromSPS(unsigned char *nalu_buf, int buf_size, int *p_width, int *p_height)
{
    OPEN_BITSTREAM bitstream;
    int x03_cnt;

    if (nalu_buf == NULL || p_height == NULL || p_width == NULL || buf_size <= 0)
    {
        return -1;
    }

    x03_cnt = OPENHEVC_ebsp_to_rbsp(nalu_buf, buf_size);
    OPENHEVC_init_bitstream(&bitstream, nalu_buf + 2, buf_size - x03_cnt); 
    OPENHEVC_interpret_sps(&bitstream, p_width, p_height);

    return 0;
}

/***************************************************************************************************
* 功  能：从SLICE的NALU单元中解析帧类型
* 参  数：nalu_buf    [in]    包含SPS的NALU单元缓冲区（不含起始码）
*         buf_size    [in]    缓冲区长度
*         
* 返回值：0 ：B帧
*         1 ：P帧
*         2 ：I帧
*         -1：输入参数有误
*         -2：码流中没有帧类型信息
* 备  注：
* 修  改: 
***************************************************************************************************/
int OPENHEVC_GetFrameTypeFromSlice(unsigned char *nalu_buf, int buf_size)
{
    OPEN_BITSTREAM bitstream, *bs = &bitstream;
    int x03_cnt;
    int nal_type, nuh_layer_id, temporal_id;

    if (nalu_buf == NULL || buf_size <= 0)
    {
        return -1;
    }

    x03_cnt = OPENHEVC_ebsp_to_rbsp(nalu_buf, buf_size);
    OPENHEVC_init_bitstream(&bitstream, nalu_buf, buf_size - x03_cnt); 

    OPENHEVC_read_n_bits(bs, 1);
    nal_type = OPENHEVC_read_n_bits(bs, 6);
    nuh_layer_id = OPENHEVC_read_n_bits(bs, 6);
    temporal_id = OPENHEVC_read_n_bits(bs, 3) - 1;

    if (nuh_layer_id == 0)
    {
        if ((nal_type >= 1 && nal_type <= 9) || (nal_type >= 16 && nal_type <= 21))
        {
            return OPENHEVC_preread_slice_header(bs, nal_type);
        }
    }

    return -2;
}

}