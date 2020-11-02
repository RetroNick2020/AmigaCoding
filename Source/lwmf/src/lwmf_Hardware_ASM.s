; Various assembler functions for lwmf
; Code is compatible with Motorola syntax as provided by vbcc

; ***************************************************************************************************
; * Global                                                                                          *
; ***************************************************************************************************

; Labels

EXECBASE        = $4

ADKCON          = $DFF09E   ; Audio/Disk control read/write
ADKCONR         = $DFF010   ; Audio/Disk control read
BLTCON0 	    = $DFF040   ; Blitter control reg 0
BLTCON1 	    = $DFF042   ; Blitter control reg 1
BLTDPTH		    = $DFF054   ; Blitter pointer to destination D (high 5 bits)
BLTDPTL         = $DFF056   ; Blitter pointer to destination D (low 15 bits)
BLTDMOD 	    = $DFF066   ; Blitter modulo for destination D
BLTSIZE 	    = $DFF058   ; Blitter start and size (win/width, height)
COP1LCH         = $DFF080   ; Coprocessor first location register (high 5 bits) (old-3 bits)
COP1LCL         = $DFF082   ; Coprocessor first location register (low 15 bits)
CUSTOM		    = $DFF000   ; Base address of custom registers
DMACON          = $DFF096   ; DMA control (and blitter status) read/write
DMACONR         = $DFF002   ; DMA control (and blitter status) read
INTENA          = $DFF09A   ; Interrupt enable read/write
INTENAR         = $DFF01C   ; Interrupt enable read
INTREQ          = $DFF09C   ; Interrupt request read/write
INTREQR         = $DFF01E   ; Interrupr request read
VPOSR           = $DFF004   ; Read vert most sig. bits (and frame flop)

DEST 		    = $100      ; Blitter control register / destination

DMAB_BLTDONE    = 14        ; DMACONR bit 14 - blitter busy flag

; Library vector offsets (LVO)

; graphics.library
LVOLoadView     = -222
LVOWaitTOF      = -270
; exec.library
LVOForbid       = -132
LVOPermit       = -138
LVOOpenLibrary  = -552

; Declare external variables

    XDEF _GfxBase

; ***************************************************************************************************
; * Functions                                                                                       *
; ***************************************************************************************************

;
; void lwmf_TakeOverOS(void);
;

_lwmf_TakeOverOS:
    move.w  DMACONR,d0              ; Store current custom registers for later restore
    or.w    #$8000,d0
    move.w  d0,olddma
    move.w  INTENAR,d0
    or.w    #$8000,d0
    move.w  d0,oldintena
    move.w  INTREQR,d0
    or.w    #$8000,d0
    move.w  d0,oldintreq
    move.w  ADKCONR,d0
    or.w    #$8000,d0
    move.w  d0,oldadkcon

    move.l	EXECBASE,a6             ; Use exex base address
    lea     gfxlibname,a1
    moveq   #39,d0                  ; Set required version (39 -> Amiga OS 3.0 and higher)
    jsr     LVOOpenLibrary(a6)      ; Load graphics.library   
    move.l  d0,_GfxBase             ; Store adress of GfxBase in variable
    move.l  d0,a6
    move.l  34(a6),oldview
    move.l  38(a6),oldcopper

    suba.l  a1,a1                   ; Use graphics.library base address
    jsr     LVOLoadView(a6)	        ; LoadView(NULL)
    jsr     LVOWaitTOF(a6)          ; WaitTOF
    jsr     LVOWaitTOF(a6)          ; WaitTOF
    move.l	$4,a6
    jsr     LVOForbid(a6)           ; Forbid
    rts

   	public _lwmf_TakeOverOS

;
; void lwmf_ReleaseOS(void);
;

_lwmf_ReleaseOS:
    move.w  #$7fff,DMACON
    move.w  olddma,DMACON
    move.w  #$7fff,INTENA
    move.w  oldintena,INTENA
    move.w  #$7fff,INTREQ
    move.w  oldintreq,INTREQ
    move.w  #$7fff,ADKCON
    move.w  oldadkcon,ADKCON

    move.l  oldcopper,COP1LCH
    move.l  _GfxBase,a6              ; Use graphics.library base address
    move.l  oldview,a1
    jsr     LVOLoadView(a6)         ; LoadView(oldview)
    jsr     LVOWaitTOF(a6)          ; WaitTOF
    jsr     LVOWaitTOF(a6)          ; WaitTOF
    move.l  EXECBASE,a6             ; Use exex base address
    jsr     LVOPermit(a6)           ; Permit
   	rts

    public _lwmf_ReleaseOS

;
; void _lwmf_WaitBlitter(void)
;

_lwmf_WaitBlitter:
    btst.b  #DMAB_BLTDONE-8,DMACONR 		; check against DMACONR
.loop:                                      ; check twice, bug in A1000
    btst.b 	#DMAB_BLTDONE-8,DMACONR 		
    bne 	.loop
    rts

	public _lwmf_WaitBlitter

;
; void _lwmf_WaitVertBlank(void)
;

_lwmf_WaitVertBlank:
.loop: 
    move.l  VPOSR,d0
	and.l   #$0001FF00,d0
	cmp.l   #303<<8,d0          ; Check if line 303 is reached
	bne.s   .loop
.loop2:                         ; Second check for A4000 compatibility
	move.l  VPOSR,d0
	and.l   #$1FF00,d0
	cmp.l   #303<<8,d0
	beq.s   .loop2
	rts

	public _lwmf_WaitVertBlank
	
;
; void lwmf_ClearMem(__reg("a0") long* Address, __reg("d0") long NumberOfBytes);
;

_lwmf_ClearMem:
    lsr.l   #5,d0               ; Shift right by 5 -> Division by 32
    subq    #1,d0               ; Subtract 1
    moveq   #0,d1
.loop:
    move.l  d1,(a0)+
    move.l  d1,(a0)+
    move.l  d1,(a0)+
    move.l  d1,(a0)+
    move.l  d1,(a0)+
    move.l  d1,(a0)+
    move.l  d1,(a0)+
    move.l  d1,(a0)+
    dbra    d0,.loop
    rts

    public _lwmf_ClearMem

; ***************************************************************************************************
; * Variables                                                                                       *
; ***************************************************************************************************

;
; System take over
;

olddma:
    dc.w    0

oldintena:
    dc.w    0

oldintreq:
    dc.w    0

oldadkcon:
    dc.w    0

oldview:
    dc.l    0

oldcopper:
    dc.l    0

;
; Libraries
;

gfxlibname:
    dc.b "graphics.library",0

_GfxBase:
    dc.l    0