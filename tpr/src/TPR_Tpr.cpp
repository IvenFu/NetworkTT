#include "TPR_Tpr.h"
#include "TPR_Socket.h"

extern TPR_INT32 TPR_InitNetwork_Inter();

TPR_DECLARE TPR_INT32 CALLBACK TPR_Init()
{
	TPR_InitNetwork_Inter();



	return 0;
}