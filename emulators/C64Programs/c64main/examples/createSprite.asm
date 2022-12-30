; ==========================================================
; COMMODORE 64 - Examples in 6502 Assembly language
; © Digitsensitive; digit.sensitivee@gmail.com
; How to create a sprite
; ==========================================================

#../C64Programs/c64main/include/constants.asm

; ----------------------------------------------------------
; Labels
; ----------------------------------------------------------

; each sprite is 3 bytes wide with 21 lines = 63 bytes required for shape
; but it actually uses 64 bytes
; the value @ the pointer times 64 equals
; the starting location of the sprite shape data table
spr0ShapeData = $0340                           ; = 13 * 64 = 832

; ----------------------------------------------------------
; Init and create sprite
; ----------------------------------------------------------

*=$4000                         				; sys 16384

init:           jsr $e544                       ; clear the screen
                            
                lda #z00001101                  ; use block 13 for sprite 0
                sta SPRITE_POINTERS
                lda #z00000001                  ; load accumulator with value 1
                sta SPRITE_ENABLE_REGISTER      ; enable sprite 0
                lda #CYAN                       ; use cyan for sprite 0
                sta SPRITE_0_COLOR_REGISTER     ; load cyan to color register 0

                ldx #0
                lda #0
                jmp build

build:          lda spr0,x                      ; get byte from sprite0+x
                sta spr0ShapeData,x             ; store byte at spr0ShapeData+x
                inx
                cpx #63
                bne build

                ; set position
                lda #z00000000                  ; restrict horizontal position to 0-255
                sta MSIGX

                ; starting sprite location
                ldx #100
                ldy #70
                stx SP0X
                sty SP0Y
                jmp loop

; ----------------------------------------------------------
; Main Loop
; ----------------------------------------------------------

loop:           jmp loop

; ----------------------------------------------------------
; Data
; ----------------------------------------------------------

*=$5000
spr0 = $5000
BYTES $00 $7F $00
BYTES $01 $FF $C0
BYTES $03 $FF $E0
BYTES $03 $E3 $E0
BYTES $07 $D9 $F0
BYTES $07 $DF $F0
BYTES $07 $D9 $F0
BYTES $03 $E7 $E0
BYTES $03 $FF $E0
BYTES $03 $FF $E0
BYTES $02 $FF $A0
BYTES $01 $7F $40
BYTES $01 $3E $40
BYTES $00 $9C $80
BYTES $00 $9C $80
BYTES $00 $49 $00
BYTES $00 $49 $00
BYTES $00 $3E $00
BYTES $00 $3E $00
BYTES $00 $3E $00
BYTES $00 $1C $00
