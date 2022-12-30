; ==========================================================
; COMMODORE 64 - Examples in 6502 Assembly language
; © Digitsensitive; digit.sensitivee@gmail.com
; How to select a color from an include file
; ==========================================================

#../C64Programs/c64main/include/constants.asm

; ----------------------------------------------------------
; Labels
; ----------------------------------------------------------

BORDER = $d020

; ----------------------------------------------------------
; Main Loop
; ----------------------------------------------------------

*=$02a7                 ; sys 679

loop:   lda #BLUE
        sta BORDER
        jmp loop