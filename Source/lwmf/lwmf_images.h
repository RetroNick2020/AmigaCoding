#ifndef LWMF_IMAGES_H
#define LWMF_IMAGES_H

struct lwmf_Image
{
	struct BitMap* Image;
	ULONG* CRegs;
	UBYTE NumberOfColors;
};

struct BitMap* lwmf_BitmapCopy(struct BitMap* SourceBM)
{
	struct BitMap* TargetBM = NULL;
	ULONG Width = 0;
	ULONG Height = 0;

	if (!(TargetBM = AllocBitMap(Width = GetBitMapAttr(SourceBM, BMA_WIDTH), Height = GetBitMapAttr(SourceBM, BMA_HEIGHT), GetBitMapAttr(SourceBM, BMA_DEPTH), GetBitMapAttr(SourceBM, BMA_FLAGS), NULL)))
	{
		return NULL;
	}
	
	BltBitMap(SourceBM, 0, 0, TargetBM, 0, 0, Width, Height, 0x0C0, 0xFF, NULL);

	return TargetBM;
}

struct lwmf_Image* lwmf_LoadImage(const char* Filename)
{
	struct lwmf_Image* TempImage = NULL;
	struct BitMap* TempBitmap = NULL;
	ULONG NumberOfColors = 0;
	ULONG* CRegs = NULL;
	APTR dtObject = NULL;

	if (!(dtObject = NewDTObject(Filename, DTA_GroupID, GID_PICTURE, PDTA_Remap, FALSE, TAG_END)))
	{
		return NULL;
	}
	
	DoDTMethod(dtObject, NULL, NULL, DTM_PROCLAYOUT, NULL, TRUE);
	GetDTAttrs(dtObject, PDTA_DestBitMap, &TempBitmap, PDTA_CRegs, &CRegs, PDTA_NumColors, &NumberOfColors, TAG_END);

	if (!(TempImage = AllocMem(sizeof(struct lwmf_Image), MEMF_ANY | MEMF_CLEAR)))
	{
		return NULL;
	}

	if (!(TempImage->Image = lwmf_BitmapCopy(TempBitmap)))
	{
		return NULL;
	}

	TempImage->NumberOfColors = NumberOfColors;
	TempImage->CRegs = CRegs;

	DisposeDTObject(dtObject);
	return TempImage;
}

void lwmf_DeleteImage(struct lwmf_Image* Image)
{
	if (Image->Image)
	{
		FreeBitMap(Image->Image);
		Image->Image = NULL;
	}
}


#endif /* LWMF_IMAGES_H */