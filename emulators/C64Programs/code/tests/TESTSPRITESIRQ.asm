; C64 Assembler framework.
; To test how sprites works with IRQ, including collision!
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

; The base has to be defined before doing anything...
; and also to include the general file!
BASE = $c000
#../C64Programs/code/routines/RGENERAL.asm
; The rest of the files needed
#../C64Programs/code/routines/RSCREENCLEAR.asm
#../C64Programs/code/routines/RSPRITES.asm
#../C64Programs/code/routines/RIRQ.asm

* = $ca00
IRQPRG1_ADDRESS				= $ca00
IRQPRG1:					.SAVEREGISTERS
							jsr ACTUALIZESPRITES
							.RECOVERREGISTERS
							rti

; Define the sprite
* = $0800
BINARY	000000000000000000000000
BINARY	000000000000000000000000
BINARY	000001100000000001100000
BINARY	000001100000000001100000
BINARY	000000011000000110000000
BINARY	000000011000000110000000
BINARY	000001111111111111100000
BINARY	000001111111111111100000
BINARY	000111100011110001111000
BINARY	000111100011110001111000
BINARY	011111110011111001111100
BINARY	011111111111111111111110
BINARY	011001111111111111100110
BINARY	011001111111111111100110
BINARY	011001110000000011100110
BINARY	011001110000000011100110
BINARY	000000011110011110000000
BINARY	000000011110011110000000
BINARY	000000000000000000000000
BINARY	000000000000000000000000
BINARY	000000000000000000000000
* = $0840
BINARY	000000000000000000000000
BINARY	000000000000000000000000
BINARY	000000011000000110000000
BINARY	000000011000000110000000
BINARY	000000011000000110000000
BINARY	000000011000000110000000
BINARY	000001111111111111100000
BINARY	000001111111111111100000
BINARY	000111100011110001111000
BINARY	000111100011110001111000
BINARY	011111100111110011111110
BINARY	011111111111111111111110
BINARY	011001111111111111100110
BINARY	011001111111111111100110
BINARY	011001110000000011100110
BINARY	011001110000000011100110
BINARY	000001110000000011100000
BINARY	000001110000000011100000
BINARY	000000000000000000000000
BINARY	000000000000000000000000
BINARY	000000000000000000000000

; The program to move it using an interruption
* = $cb00
; A very simple program to test everything works...
; Starts setting up the background and foreground colors and cleanning up the screen.
MAIN:						lda #$06
							sta VICIIBACKGROUND
							lda #$0e
							sta VICIIFOREGROUND
							jsr CLEARSCREEN
; Set the behaviour attributes linked to the image of the sprite 0
							lda #$01
							sta SPRITE_ENABLEVAR			; Only the sprite 0 will be active.
							lda #$20						; Initial block (definition) for of the sprite 0.
							sta SPRITE_BLOCKVAR + 0
							lda #$00						; Initial X position of the sprite 0.
							sta SPRITE_XPOSVAR + 0
							lda #$00						; No MSB in X position for sprite 0.
							sta SPRITE_XMSBPOSVAR + 0
							lda #$80						; Initial Y position of the sprite 0. Never changes.
							sta SPRITE_YPOSVAR + 0			
							lda #$03
							sta TEMP03_DATA					; To count cycles before chainging the form of the alien.
; Set the behaviour attributes not linked to the image of the sprite 0.
							lda #$01
							sta SPRITEBASECOLOR
							lda #$01
							sta SPRITEPRIORITY
; Set the IRQ.
							lda $01							; Changes to see the RAM instead the Kernel (and be able to modify it).
							and #$fd
							sta $01
							lda #<IRQPRG1_ADDRESS
							sta SETVICIIRIRQ_PRGHVAR
							lda #>IRQPRG1_ADDRESS
							sta SETVICIIRIRQ_PRGLVAR
							lda #$00						; Last screen row...
							sta SETVICIIRIRQ_ROWLVAR
							lda #$00
							sta SETVICIIRIRQ_ROWHVAR
							jsr SETVICIIRASTERIRQ
; Now the program starts.
MOVEALIEN:					ldx #$40
							ldy #$05
							jsr DELAY
; Deals with the form of the alien.
							dec TEMP03_DATA					; The form is changed every 5 pixels moved.
							bne MOVE
							lda #$03
							sta TEMP03_DATA
							inc SPRITE_BLOCKVAR + 0
							lda SPRITE_BLOCKVAR + 0
							cmp #$22
							bne MOVE
							lda #$20
							sta SPRITE_BLOCKVAR + 0
; Now it is moved.
MOVE:						lda SPRITE_XMSBPOSVAR + 0
							bne MOVE_ABOVE255				; Is it the above position 255?
							inc SPRITE_XPOSVAR + 0			; No. So increment the current position below 255.
							bne MOVE_AGAIN					; If it is not getting then the 255 position, set the movement.
							inc SPRITE_XMSBPOSVAR + 0		; But if it is, then point out the MSB location..
							jmp MOVE_AGAIN					; ...and the set the movement.
MOVE_ABOVE255:				inc SPRITE_XPOSVAR + 0			; The sprite is behond the 255 position
							lda SPRITE_XPOSVAR + 0
							cmp #$40						; but at the last visible one?	
							bne MOVE_AGAIN					; No. so let's go to set the position.
							lda #$00						; It is at the last visible position...
							sta SPRITE_XPOSVAR + 0			; so put everything back at the beginning...
							sta SPRITE_XMSBPOSVAR + 0
MOVE_AGAIN:					jmp MOVEALIEN					; Starts back again.

; End