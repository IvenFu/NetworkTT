#ifndef _PROBE_H_
#define _PROBE_H_

#if defined(WIN32)

#else

#ifndef __stdcall
#define __stdcall
#endif

#endif

int ProbingLostrateAndRTT();
int GetLostrateAndRTT();

int ProbingBw();
int GetBw();



#endif