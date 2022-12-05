; C64 Assembler framework.
; To test the graphical modes.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

; The base has to be defined before doing anything...
; and also to include the general file!
BASE = $c000
#../C64Programs/Code/RGENERAL.asm

#../C64Programs/Code/RSCREENCLEAR.asm
#../C64Programs/Code/RSCREENBOX.asm
#../C64Programs/Code/RSCREENRECT.asm
#../C64Programs/Code/RSCREENTEXT.asm

; Data
* = $ca10
BYTES $00 $01 $02 $03 $04 $05 $06 $07 $08 $09 $0a $0b $0c $0d $0e $0f
BYTES $40 $41 $42 $43 $44 $45 $46 $47 $48 $49 $4a $4b $4c $4d $4e $4f
BYTES $80 $81 $82 $83 $84 $85 $86 $87 $88 $89 $8a $8b $8c $8d $8e $8f
BYTES $c0 $c1 $c2 $c3 $c4 $c5 $c6 $c7 $c8 $c9 $ca $cb $cc $cd $ce $cf
TEXTDEF1					= $ca10
TEXTDEF2					= $ca20
TEXTDEF3					= $ca30
TEXTDEF4					= $ca40
COLORLETTERS				= $09

* = $cb00
; A very simple program to test everything works...
MAIN:						lda #$06
							sta VICIIBACKGROUND
							lda #$0e
							sta VICIIFOREGROUND
							lda #$01
							sta VICIIBACKGROUND1
							lda #$00
							sta VICIIBACKGROUND2
							lda #$05
							sta VICIIBACKGROUND3
							jsr CLEARSCREEN

; Change the char set
SETCHARSET:					lda VICIICTRLMEMREG
							and #$f1
							ora #$06								; Put the second character set (lowercase)
							sta VICIICTRLMEMREG

; Change the graphics mode...
; Char multicolor extended
;SETGRAPHICMODE:				lda VICIISCROLLY
;							and #$bf
;							ora #$40								; Sets the bit 6 on... enhaced multimode...
;							sta VICIISCROLLY
;							lda VICIISCROLLX						; Just to test if the shift doesn't affect!
;							and #$f8
;							ora #$07
;							sta VICIISCROLLX
; Char multicolor
SETGRAPHICMODE:			lda VICIISCROLLX						; Sets the bit 4 on... multicolor text mode...
							and #$ef
							ora #$10
							sta VICIISCROLLX
							lda VICIISCROLLX						; Just to test if the shift doesn't affect!
							and #$f8
							ora #$07
							sta VICIISCROLLX

; Test to draw a text on the screen.
; The elements of the text are defined at TEXTDEF.
TEXTEXAMPLE1:				lda #>TEXTDEF1
							sta DRAWTEXT_TEXTLOWVAR
							lda #<TEXTDEF1
							sta DRAWTEXT_TEXTHIGHVAR
							lda #$0f								; Text length.
							sta DRAWTEXT_TEXTLENVAR
							lda #$02								; Pos X.
							sta DRAWTEXT_XPOSVAR
							lda #$08								; Pos Y.
							sta DRAWTEXT_YPOSVAR
							lda #COLORLETTERS
							sta DRAWTEXT_COLORVAR
							jsr DRAWTEXT

TEXTEXAMPLE2:				lda #>TEXTDEF2
							sta DRAWTEXT_TEXTLOWVAR
							lda #<TEXTDEF2
							sta DRAWTEXT_TEXTHIGHVAR
							lda #$0f								; Text length.
							sta DRAWTEXT_TEXTLENVAR
							lda #$02								; Pos X.
							sta DRAWTEXT_XPOSVAR
							lda #$09								; Pos Y.
							sta DRAWTEXT_YPOSVAR
							lda #COLORLETTERS
							sta DRAWTEXT_COLORVAR
							jsr DRAWTEXT

TEXTEXAMPLE3:				lda #>TEXTDEF3
							sta DRAWTEXT_TEXTLOWVAR
							lda #<TEXTDEF3
							sta DRAWTEXT_TEXTHIGHVAR
							lda #$0f								; Text length.
							sta DRAWTEXT_TEXTLENVAR
							lda #$02								; Pos X.
							sta DRAWTEXT_XPOSVAR
							lda #$0a								; Pos Y.
							sta DRAWTEXT_YPOSVAR
							lda #COLORLETTERS
							sta DRAWTEXT_COLORVAR
							jsr DRAWTEXT

TEXTEXAMPLE4:				lda #>TEXTDEF4
							sta DRAWTEXT_TEXTLOWVAR
							lda #<TEXTDEF4
							sta DRAWTEXT_TEXTHIGHVAR
							lda #$0f								; Text length.
							sta DRAWTEXT_TEXTLENVAR
							lda #$02								; Pos X.
							sta DRAWTEXT_XPOSVAR
							lda #$0b								; Pos Y.
							sta DRAWTEXT_YPOSVAR
							lda #COLORLETTERS
							sta DRAWTEXT_COLORVAR
							jsr DRAWTEXT

FOREVER:					jmp FOREVER								; For ever...

; End.