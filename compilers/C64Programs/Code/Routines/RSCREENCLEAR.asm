; C64 Assembler framework.
; Routine to clear u the screen.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

#../C64Programs/code/routines/RSCREENBOX.asm

; ------------------------------
; CLEARSCREEN
; To clear the screen with the color of the foreground.
; No parameters are needed.
CLEARSCREEN:				lda #$00
							sta FILLSCR_XPOSVAR
							sta FILLSCR_YPOSVAR
							lda #$28								; Max number of text columns in the screen.
							sta FILLSCR_XLENVAR
							lda #$19								; Max number of rown in the screen.
							sta FILLSCR_YLENVAR
							lda #$20								; It is the equivalent to a space.
							sta FILLSCR_BYTEVAR
							lda VICIIBACKGROUND
							sta FILLSCR_COLORVAR
							jsr FILLBOXSCREEN
							rts
; ------------------------------

; End.