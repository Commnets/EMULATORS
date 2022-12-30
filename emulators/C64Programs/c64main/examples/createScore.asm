; ==========================================================
; COMMODORE 64 - Examples in 6502 Assembly language
; © Digitsensitive; digit.sensitivee@gmail.com; 24.04.2021
; Raster time
; ==========================================================

#../C64Programs/c64main/include/constants.asm

; ----------------------------------------------------------
; Main Loop
; ----------------------------------------------------------

*=$c000                         			; sys 49152


                lda #147
;                jsr $ffd2
                jsr $f1ca

loop:           lda #100

                ; compares contents of accumulator against the specified operand 
wait1:          cmp RASTER                       
                bne wait1

                inc BORDER_COLOR_REGISTER

                ; do operations here
                clc
                lda score
                adc #1
                sta score
                lda score+1
                adc #0
                sta score+1

                lda #19
;                jsr $ffd2
                jsr $f1ca

                ; print score
                ldx score
                lda score+1
                jsr $bdcd

                dec BORDER_COLOR_REGISTER

                jmp loop


*=$c100
score = $c100
BYTES 0 0                       ; assign low and high byte
