
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "config.h"

class Player
{
public:
	Player(Config& config);
	~Player();

	int Open();

	int Close();

	int InputData(unsigned char* pData, int iDataLen);

	int GetAvailableNodeNum(unsigned int* pNum);
	int GetBufferTimeMs(unsigned int* pTimeMs);
private:

	int ParseRtp( unsigned char* pData, int nLen ,unsigned char** ppRaw, int* pDataLen);
	int ParseExtension( unsigned char* pData,unsigned int nLen );
	int ParsePadding( unsigned char* pData,unsigned int nLen );
	int ParseRed( unsigned char* pData, int nLen ,unsigned char** ppRaw, int* pDataLen);

	int m_iPlayPort;
	unsigned char* m_buf;
	int m_iDataLen;
	int m_iBufferPacketCount;
	Config& m_config;
	TPR_BOOL m_bFirst;
};
#endif
