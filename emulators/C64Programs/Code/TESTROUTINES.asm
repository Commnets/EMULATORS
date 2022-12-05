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
BYTES $a0 $a0 $89 $87 $8e $81 $83 $89 $8f $a0 $83 $85 $81 $a0 $a0 $a0
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
							lda #$15								; Pos Y.
							sta DRAWRECT_YPOSVAR
							lda #$26								; Size X.
							sta DRAWRECT_XLENVAR
							lda #$03								; Size Y.
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
							lda #$f0								; Pos X.
							sta DRAWTEXT_XPOSVAR
							lda #$09								; Pos Y.
							sta DRAWTEXT_YPOSVAR
							lda #$07								; Yellow.
							sta DRAWTEXT_COLORVAR
							jsr DRAWTEXTIN

; cout all the full character set
COUTCHARSET:				ldx #$00
							ldy #$00
COUTCHARSET_LOOP:			txa
							sta SCREENBASE,y
							lda #$01								; White
							sta COLORRAMBASE,y
							inx
							iny
							bne COUTCHARSET_LOOP

; Test to draw a text crossing the screen (horizontal)...
TEXTEXAMPLEMOVE:			lda #>TEXTDEF
							sta DRAWTEXT_TEXTLOWVAR
							lda #<TEXTDEF
							sta DRAWTEXT_TEXTHIGHVAR
							lda #$10								; Text length.
							sta DRAWTEXT_TEXTLENVAR
							lda #$0a								; Pos Y.
							sta DRAWTEXT_YPOSVAR
							lda #$0a								; Orange.
							sta DRAWTEXT_COLORVAR
							ldx #$f0								; Starting Pos X.
TEXTEXAMPLEMOVELOOP:		stx DRAWTEXT_XPOSVAR
							txa
							pha
							jsr DRAWTEXTIN
							ldx #$00								; Just a delay...
							ldy #$10
							jsr DELAY
							pla
							tax
							inx
							cpx #$28
							bmi TEXTEXAMPLEMOVELOOP
							jmp TEXTEXAMPLEMOVE

FOREVER:					jmp FOREVER								; For ever...

; End.