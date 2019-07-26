#include "probe.h"
#include "config.h"
#include "audio.h"



int main()
{
	Audio audio;
	audio.Start();

	while (1)
	{
		TPR_Sleep(10 * 1000);
	}
	return 0;
}
