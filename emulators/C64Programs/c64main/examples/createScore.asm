; ==========================================================
; COMMODORE 64 - Examples in 6502 Assembly language
; © Digitsensitive; digit.sensitivee@gmail.com; 24.04.2021
; Raster time
; ==========================================================

* = $C000
#../C64Programs/c64main/include/constants.asm

; ----------------------------------------------------------
; Main Loop
; ----------------------------------------------------------

*=$c100                         			; sys 49152

                lda #147
                jsr $ffd2
				lda #25						; To point to the right place in the queue of stings
				sta $16						; Because the basic is not still loaded... for jsr $bdcd to work.

				lda #$05					; Changing the color of the letter
				jsr $ffd2

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
                jsr $ffd2

                ; print score
                ldx score
                lda score+1
                jsr $bdcd

                dec BORDER_COLOR_REGISTER

                rts


*=$c200
score = $c200
BYTES 0 0                       ; assign low and high byte
