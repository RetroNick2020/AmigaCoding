//**********************************************************************
//* Simple sine scroller demo for Amiga with at least OS 3.0           *
//*														 			   *
//* (C) 2020 by Stefan Kubsch                            			   *
//* Project for vbcc 0.9g                                			   *
//*                                                      			   *
//* Compile & link with:                                 			   *
//* vc -O4 SineScroller.c -o SineScroller -lmieee -lamiga  			   *
//*                                                      			   *
//* Quit with mouse click                                  			   *
//**********************************************************************

// Include our own header files
#include "lwmf/lwmf.h"

//
// Screen settings
//

const ULONG WIDTH = 320;
const ULONG HEIGHT = 256;

// Our timing/fps limit is targeted at 25fps
// If you want to use 50fps instead, calc 1000000 / 50
// If you want to use 20fps instead, calc 1000000 / 20 - I guess, you got it...
// Is used in function "DoubleBuffering()"
const int FPSLIMIT = (1000000 / 25);

// Here we define, how many bitplanes we want to use...
// Colors / number of required Bitplanes
// 2 / 1
// 4 / 2
// 8 / 3
// 16 / 4
// 32 / 5
// 64 / 6 (Amiga Halfbrite mode)
const int NUMBEROFBITPLANES = 1;

// ...and here which colors we want to use
// Format: { Index, Red, Green, Blue }, Array must be terminated with {-1, 0, 0, 0}
struct ColorSpec ColorTable[] = 
{ 
	{0, 0, 0, 3}, 
	{1, 15, 15, 15},
	{-1, 0, 0, 0} 
};

//***************************************************************
// Demo stuff                                                   *
//***************************************************************

BOOL InitDemo();
void CleanupDemo();
void DrawDemo();

struct BitMap* ScrollFontBitMap;
const char ScrollText[] = "...WELL,WELL...NOT PERFECT, BUT STILL WORKING ON IT !!!";
const char ScrollCharMap[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ!.,";
const int ScrollCharWidth = 16;
const int ScrollCharHeight = 28;
int YSine[360];
int ScrollTextLength = 0;
int ScrollCharMapLength = 0;
int ScrollLength = 0;
int ScrollX = 0;

BOOL InitDemo()
{
	//
	// Init sine scoller
	//

	// Generate sinus table
	for (int i = 0; i < 360; ++i)
	{
		YSine[i] = (int)(sin(0.05f * i) * 10.0f);
	}

	ScrollX = WIDTH;
	ScrollTextLength = strlen(ScrollText);
	ScrollCharMapLength = strlen(ScrollCharMap);
	ScrollLength = ScrollTextLength * ScrollCharWidth;

	// Generate bitmap for charmap
	ScrollFontBitMap = AllocBitMap(ScrollCharMapLength * ScrollCharWidth, ScrollCharHeight + 4, 1, BMF_STANDARD | BMF_INTERLEAVED | BMF_CLEAR, RenderPort.BitMap);

	if (!ScrollFontBitMap)
	{
		lwmf_ReleaseOS();
		CleanupDemo();
		lwmf_CleanupRastPort();
		lwmf_CleanupScreen();
		lwmf_CloseLibraries();
	}

	RenderPort.BitMap = ScrollFontBitMap;

	// Load font
	struct TextAttr ScrollFontAttrib =
	{
		"topaz.font", 
		16,
		FSF_BOLD,
		0
	};

	struct TextFont* ScrollFont = NULL;
	struct TextFont* OldFont = NULL;

	if (ScrollFont = OpenDiskFont(&ScrollFontAttrib))
   	{
    	// Save current font
		OldFont = RenderPort.Font;
		// Set new font
     	SetFont(&RenderPort, ScrollFont);
	}

	// Draw charmap
	SetAPen(&RenderPort, 1);
	Move(&RenderPort, 0, ScrollCharHeight);
	Text(&RenderPort, ScrollCharMap, ScrollCharMapLength);

	// Load old font
	SetFont(&RenderPort, OldFont);
    CloseFont(ScrollFont);
	
	return TRUE;
}

void CleanupDemo()
{
	if (ScrollFontBitMap)
	{
		FreeBitMap(ScrollFontBitMap);
	}
}

void DrawDemo()
{
	// Clear background
	SetRast(&RenderPort, 0);

	//
	// Sine scroller
	//

	for (int i = 0, XPos = ScrollX; i < ScrollTextLength; ++i)
	{
		for (int j = 0, CharX = 0; j < ScrollCharMapLength; ++j)
		{
			if (*(ScrollText + i) == *(ScrollCharMap + j))
			{
				for (int x1 = 0, x = CharX; x < CharX + ScrollCharWidth; ++x1, ++x)
				{
					const int TempPosX = XPos + x1;

					if ((unsigned int)TempPosX < WIDTH)
					{
						BltBitMap(ScrollFontBitMap, x, 0, RenderPort.BitMap, TempPosX, 100 + YSine[TempPosX], 1, ScrollCharHeight + 4, 0xC0, 0x01, NULL);
					}
				}

				break;
			}

			CharX += ScrollCharWidth;
		}

		if (XPos >= WIDTH)
		{
			break;
		}

		XPos += ScrollCharWidth;
	}

	ScrollX -= 5;

	if (ScrollX < -ScrollLength)
	{
		ScrollX = WIDTH;
	}
}

int main()
{
    // Load libraries
    // Exit with SEVERE Error (20) if something goes wrong
	if (!lwmf_LoadLibraries())
    {
        return 20;
    }

	// Check which CPU is used in your Amiga (or UAE...)
	lwmf_CheckCPU();

	// Gain control over the OS
	lwmf_TakeOverOS();
	
	// Setup screen
	if (!lwmf_CreateScreen(WIDTH, HEIGHT, NUMBEROFBITPLANES, ColorTable))
    {
        return 20;
    }

    // Init the RenderPort (=Rastport)
	if (!lwmf_CreateRastPort(1, 1, 1))
	{
		return 20;
	}

	//
	// Init stuff for demo if needed
	//

	// Init sine scroller
	if (!InitDemo())
	{
		return 20;
	}

    // This is our main loop
    // Call "DoubleBuffering" with the name of function you want to use...
	lwmf_DoubleBuffering(DrawDemo, FPSLIMIT);

	// Cleanup everything
	lwmf_ReleaseOS();
	CleanupDemo();
	lwmf_CleanupRastPort();
	lwmf_CleanupScreen();
	lwmf_CloseLibraries();
	return 0;
}