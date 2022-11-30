; C64 Assembler framework.
; To test the routines.
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
* = $ca00
BYTES $4f $77 $50 $65 $6a $4c $6f $7a
CHARSDEF					= $ca00
* = $ca10
BYTES $89 $87 $8e $81 $83 $89 $8f $57 $83 $85 $81 $57 $57 $57 $57 $57
TEXTDEF						= $ca10

* = $cb00
; A very simple program to test everything works...
MAIN:						lda #$06
							sta VICIIBACKGROUND
							lda #$0e
							sta VICIIFOREGROUND
							jsr CLEARSCREEN

; Test the creation of a box.
BOXEXAMPLE:					lda #$10								; Pos X.
							sta FILLSCR_XPOSVAR
							lda #$0e								; Pos Y.
							sta FILLSCR_YPOSVAR
							lda #$10								; Size X.
							sta FILLSCR_XLENVAR
							lda #$06								; Size Y.
							sta FILLSCR_YLENVAR
							lda #$01								; With letter A.
							sta FILLSCR_BYTEVAR
							lda #$01								; White.
							sta FILLSCR_COLORVAR
							jsr FILLBOXSCREEN

; Test the creation of a rectangle.
; The elements of the rectangle are defined at CHARSDEF.
RECTANGLEEXAMPLE:			lda #$01								; Pos X.
							sta DRAWRECT_XPOSVAR
							lda #$01								; Pos Y.
							sta DRAWRECT_YPOSVAR
							lda #$26								; Size X.
							sta DRAWRECT_XLENVAR
							lda #$04								; Size Y.
							sta DRAWRECT_YLENVAR
							ldx #$07
LOOPCHARS:					lda CHARSDEF,x
							sta DRAWRECT_BYTEVAR,x
							dex
							bpl LOOPCHARS
							lda #$03								; Green.
							sta DRAWRECT_COLORVAR
							jsr DRAWRECTANGLE

; Test to draw a text on the screen.
; The elements of the text are defined at TEXTDEF.
TEXTEXAMPLE:				lda #>TEXTDEF
							sta DRAWTEXT_TEXTLOWVAR
							lda #<TEXTDEF
							sta DRAWTEXT_TEXTHIGHVAR
							lda #$10								; Text length.
							sta DRAWTEXT_TEXTLENVAR
							lda #$02								; Pos X.
							sta DRAWTEXT_XPOSVAR
							lda #$08								; Pos Y.
							sta DRAWTEXT_YPOSVAR
							lda #$07								; Yellow.
							sta DRAWTEXT_COLORVAR
							jsr DRAWTEXT

; Test to draw a text out with some parts of it out of the screen.
; The elements of the text are defined at TEXTDEF.
TEXTEXAMPLEIN:				lda #>TEXTDEF
							sta DRAWTEXT_TEXTLOWVAR
							lda #<TEXTDEF
							sta DRAWTEXT_TEXTHIGHVAR
							lda #$10								; Text length.
							sta DRAWTEXT_TEXTLENVAR
							lda #$fb								; Pos X.
							sta DRAWTEXT_XPOSVAR
							lda #$09								; Pos Y.
							sta DRAWTEXT_YPOSVAR
							lda #$07								; Yellow.
							sta DRAWTEXT_COLORVAR
							jsr DRAWTEXTIN

FOREVER:					jmp FOREVER								; For ever...

; End.