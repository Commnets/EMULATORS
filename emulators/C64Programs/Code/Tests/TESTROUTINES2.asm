; C64 Assembler framework.
; To test the box routine with limits
; By Ignacio Cea Fornies.
; Copyright Community Networks 2022 - 2023.

; The base has to be defined before doing anything...
; and also to include the general file!
BASE = $c000
#../C64Programs/code/routines/RGENERAL.asm
; The rest of the files needed
#../C64Programs/code/routines/RSCREENCLEAR.asm
#../C64Programs/code/routines/RSCREENBOX.asm
#../C64Programs/code/routines/RSCREENRECT.asm
#../C64Programs/code/routines/RSCREENTEXT.asm

* = $cb00
; A very simple program to test everything works...
MAIN:						lda #$06
							sta VICIIBACKGROUND
							lda #$0e
							sta VICIIFOREGROUND
							jsr CLEARSCREEN

; Test the creation of a box.
BOXEXAMPLE:					lda #$fa							; Pos X.
							sta FILLSCR_XPOSVAR
							lda #$fa							; Pos Y.
							sta FILLSCR_YPOSVAR
							lda #$05							; Size X.
							sta FILLSCR_XLENVAR
							lda #$05							; Size Y.
							sta FILLSCR_YLENVAR
							lda #$03							; With letter A.
							sta FILLSCR_BYTEVAR
							lda #$01							; White.
							sta FILLSCR_COLORVAR
							ldx #$00
BOXEXAMPLE_LOOP:			txa
							pha
							jsr FILLBOXSCREENIN
							ldx #$ff
							ldy #$0f
							jsr DELAY
							lda #$20
							sta FILLSCR_BYTEVAR
							lda #$06
							sta FILLSCR_COLORVAR
							jsr FILLBOXSCREENIN
							inc FILLSCR_XPOSVAR
							inc FILLSCR_YPOSVAR
							lda #$03							; With letter A.
							sta FILLSCR_BYTEVAR
							lda #$01							; White.
							sta FILLSCR_COLORVAR
							pla
							tax
							inx
							cpx #$20
							bne BOXEXAMPLE_LOOP

FOREVER:					jmp BOXEXAMPLE						; For ever...

; End.