; ==========================================================
; COMMODORE 64 - Examples in 6502 Assembly language
; © Digitsensitive; digit.sensitivee@gmail.com
; How to use a macro
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

        *=$02a7                 ; sys 679

init:   .poke BORDER_COLOR_REGISTER,RED
        
		rts
