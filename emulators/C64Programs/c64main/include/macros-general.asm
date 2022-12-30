; ==========================================================
; COMMODORE 64 - Examples in 6502 Assembly language
; © Digitsensitive; digit.sensitivee@gmail.com
; General Macros
; ==========================================================

; ----------------------------------------------------------
; Poke selected address with value
; Example: #poke BORDER_COLOR_REGISTER,RED
; ----------------------------------------------------------

{ poke ?1,?2                   
                        lda #?2
                        sta ?1
}

; ----------------------------------------------------------
; Draw a character on the screen
; ----------------------------------------------------------
{ drawCharacterAt ?1,?2,?3,?4         
                        ldx #?3*40+?2       ; calculate position from x and y
                        lda #?1             ; load the character
                        sta $0400,x         ; set the character at location
                        lda #?4             ; set color
                        sta $d800,x         ; set foreground color at location
}