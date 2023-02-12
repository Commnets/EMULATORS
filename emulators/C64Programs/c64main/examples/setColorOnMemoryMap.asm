; ==========================================================
; COMMODORE 64 - Examples in 6502 Assembly language
; © Digitsensitive; digit.sensitivee@gmail.com
; How to set the color on the memory map
; ==========================================================

#../C64Programs/c64main/include/constants.asm
#../C64Programs/c64main/include/macros-general.asm

; ----------------------------------------------------------
; Labels
; ----------------------------------------------------------

border = $d020

; ----------------------------------------------------------
; Main Loop
; ----------------------------------------------------------

*=$02a7                 		; sys 679

init:   .poke BORDER_COLOR_REGISTER,CYAN
        .poke COLOR_MEMORY_MAP,GREEN
        .poke COLOR_MEMORY_MAP+1,YELLOW
        lda #$04                ; D
        sta SCREEN_MEMORY
        lda #$05                ; E
        sta SCREEN_MEMORY+1

		rts