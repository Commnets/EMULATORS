; ==========================================================
; COMMODORE 64 - Examples in 6502 Assembly language
; © Digitsensitive; digit.sensitivee@gmail.com
; Binary Addition
; ==========================================================

; ----------------------------------------------------------
; Main Loop
; ----------------------------------------------------------

*=$02a7                 		; sys 679

init:   jsr $e544               ; Clear the screen
        lda #z0001              ; 0001 = decimal 1
        clc                     ; Clear Carry flag
        adc #z0010              ; Add with carry (1+2), so accumulator is 3 now
        sta $0400               ; store the result (= 3)
loop:   jmp loop
