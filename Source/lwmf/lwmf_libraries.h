#ifndef LWMF_LIBRARIRIES_H
#define LWMF_LIBRARIRIES_H

#include <exec/exec.h>
#include <exec/types.h>
#include <graphics/rastport.h>
#include <datatypes/pictureclass.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/datatypes_protos.h>

//
// Global symbols for our assembler functions
//

// Hardware

UWORD lwmf_LoadGraphicsLib(void);
UWORD lwmf_LoadDatatypesLib(void);
void lwmf_CloseLibraries();
void lwmf_TakeOverOS(void);
void lwmf_ReleaseOS(void);
void lwmf_OwnBlitter(void);
void lwmf_DisownBlitter(void);
void lwmf_WaitVertBlank(void);
void lwmf_WaitBlitter(void);
void lwmf_ClearMemCPU(__reg("a1") long* StartAddress, __reg("d7") long NumberOfBytes);
void lwmf_ClearScreen(__reg("a1") long* StartAddress);
void lwmf_SetPixel(__reg("d0") WORD PosX, __reg("d1") WORD PosY,  __reg("d2") UBYTE Color,  __reg("a0") long* Target);
void lwmf_BlitTile(__reg("a0") long* SrcAddr, __reg("d0") WORD SrcModulo, __reg("d1") long SrcOffset, __reg("a1") long* DstAddr, __reg("d2") WORD PosX, __reg("d3") WORD PosY, __reg("d4") WORD Width, __reg("d5") WORD Height);

// Math

ULONG lwmf_Random(void);

//
// External variables as defined in assembler sources
//

extern long GfxBase;
extern long DataTypesBase;


#endif /* LWMF_LIBRARIRIES_H */