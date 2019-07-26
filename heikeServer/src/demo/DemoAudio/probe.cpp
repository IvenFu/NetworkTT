#include "probe.h"
#include "config.h"
#include "audio.h"

TPR_BOOL g_bInit = TPR_FALSE;
Audio* g_cLostRTT = NULL;

int ProbingLostrateAndRTT()
{
	Config config;
	config.DoConfig();

	Audio audio(config);
	audio.Start();
}

int GetLostrateAndRTT()
{

}

int ProbingBw()
{

}

int GetBw()
{

}

int main()
{
	Config config;

	config.DoConfig();

	Audio audio(config);
	audio.Start();

	while (1)
	{
		TPR_Sleep(10 * 1000);
	}
	return 0;
}
