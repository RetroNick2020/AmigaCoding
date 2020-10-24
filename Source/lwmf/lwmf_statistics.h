#ifndef LWMF_STATISTICS_H
#define LWMF_STATISTICS_H

#include <stdio.h>
#include <string.h>

// Some global variables for our statistics...
WORD FPS = 0;
BOOL FastCPUFlag = FALSE;

ULONG lwmf_GetSystemTime(void);
void lwmf_FPSCounter(void);
void lwmf_DisplayFPSCounter(const int PosX, const int PosY, const int Color);
void lwmf_CheckCPU(void);

ULONG lwmf_GetSystemTime(void)
{
	static struct timeval tt;
	struct timeval a;
	struct timeval b;

	GetSysTime(&a);
	b = a;
	SubTime(&b, &tt);
	tt = a;

	return (b.tv_secs * 1000 + b.tv_micro / 1000);
}

void lwmf_FPSCounter(void)
{
	const ULONG SystemTime = lwmf_GetSystemTime();
	
	// Calculate fps
	static WORD FPSFrames = 0;
	static ULONG FPSUpdate = 0;

	FPSUpdate += SystemTime;

	if (FPSUpdate >= 1000)
	{
		FPS = FPSFrames;
		FPSFrames = 0;
		FPSUpdate = SystemTime;
	}

	++FPSFrames;
}

void lwmf_DisplayFPSCounter(const int PosX, const int PosY, const int Color)
{
	UBYTE FPSStr[4];
	sprintf(FPSStr, "%d", FPS);
								
	SetAPen(&RenderPort, Color);
	Move(&RenderPort, PosX, PosY);
	Text(&RenderPort, FPSStr, strlen(FPSStr));
}

void lwmf_CheckCPU(void)
{
	struct ExecBase *SysBase = *((struct ExecBase**)4L);

	// Check if CPU is a 68020, 030, 040, 060 (this is the "0x80")
	// If yes, we can calculate more stuff...
	if (SysBase->AttnFlags & AFF_68020 || SysBase->AttnFlags & AFF_68030 || SysBase->AttnFlags & AFF_68040 || SysBase->AttnFlags & 0x80)
	{
		FastCPUFlag = TRUE;
	}
}


#endif /* LWMF_STATISTICS_H */