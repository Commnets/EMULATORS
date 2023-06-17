; C64 Assembler framework.
; To test the effect of a sprite under the see.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

; The base has to be defined before doing anything...
; and also to include the general file!
BASE = $c000
#../C64Programs/code/routines/RGENERAL.asm
; The rest of the files needed
#../C64Programs/code/routines/RSCREENCLEAR.asm
#../C64Programs/code/routines/RSCREENBOX.asm
#../C64Programs/code/routines/RSCREENRECT.asm
#../C64Programs/code/routines/RSCREENTEXT.asm
#../C64Programs/code/routines/RSPRITES.asm
#../C64Programs/code/routines/RIRQ.asm

* = $ca00
IRQPRG1_ADDRESS				= $ca00
IRQPRG1:					.SAVEREGISTERS
							lda #$ff
							sta VICIIBACKGROUND

							lda #$ff						; Meaning the VICII IRQs are treated 
							sta VICIIIRQ					; and another different one might come later...

							lda #$ff						; All sprites have the same priority level
							sta SPRITEPRIORITY
							jsr ACTUALIZESPRITES			; Just to move the sprites
												
							lda #<IRQPRG2_ADDRESS
							sta SETVICIIRIRQ_PRGHVAR
							lda #>IRQPRG2_ADDRESS
							sta SETVICIIRIRQ_PRGLVAR
							lda #$cb
							sta SETVICIIRIRQ_ROWLVAR
							lda #$00
							sta SETVICIIRIRQ_ROWHVAR
							jsr SETVICIIRASTERIRQ
							
							.RECOVERREGISTERS
							rti

* = $ca64
IRQPRG2_ADDRESS				= $ca64
IRQPRG2:					.SAVEREGISTERS
							lda #$05
							sta VICIIBACKGROUND

							lda #$ff						; Meaning the VICII IRQs are treated 
							sta VICIIIRQ					; and another different one might come later...

							lda #$00						; All sprites have the same priority level
							sta SPRITEPRIORITY
							
							lda #<IRQPRG1_ADDRESS
							sta SETVICIIRIRQ_PRGHVAR
							lda #>IRQPRG1_ADDRESS
							sta SETVICIIRIRQ_PRGLVAR
							lda #$00
							sta SETVICIIRIRQ_ROWLVAR
							lda #$00
							sta SETVICIIRIRQ_ROWHVAR
							jsr SETVICIIRASTERIRQ
							
							.RECOVERREGISTERS
							rti

* = $cac8
BYTES $08													; Number of sprites managed by the program
BYTES $40 $55 $6a $80 $95 $aa $c0 $d5						; Initial positions for the sprites (Y coordinate)
BYTES $01 $02 $03 $04 $05 $07 $01 $09						; Color of the sprites
NUMBERSPRITES				= $cac8
SPRITESPOSITION				= $cac9
SPRITESCOLOR				= $cad1

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

* = $cb00
; A very simple program to test everything works...
MAIN:						lda #$06
							sta VICIIBACKGROUND
							lda #$0e
							sta VICIIFOREGROUND
							jsr CLEARSCREEN

							lda $01							; The IRQ vector is going to be managed direclty...
							and #$fd						; ...so the Kernel is desactivated from 6510 view!
							sta $01							; This shouldn't be done, but it just for testing purposes!
							
SCROLLPOS:					lda VICIISCROLLY					
							and #$f7
							ora #$03
							STA VICIISCROLLY
							
SEA:						lda #$00						; Pos X.
							sta FILLSCR_XPOSVAR
							lda #$13						; Pos Y.
							sta FILLSCR_YPOSVAR
							lda #$28						; Size X.
							sta FILLSCR_XLENVAR
							lda #$06						; Size Y.
							sta FILLSCR_YLENVAR
							lda #$a0						; The block to make the sea
							sta FILLSCR_BYTEVAR
							lda #$06						; Blue
							sta FILLSCR_COLORVAR
							jsr FILLBOXSCREENIN

SKY:						lda #$00						; Pos X.
							sta FILLSCR_XPOSVAR
							lda #$00						; Pos Y.
							sta FILLSCR_YPOSVAR
							lda #$28						; Size X.
							sta FILLSCR_XLENVAR
							lda #$13						; Size Y.
							sta FILLSCR_YLENVAR
							lda #$a0						; The block to make the sea
							sta FILLSCR_BYTEVAR
							lda #$00						; Black
							sta FILLSCR_COLORVAR
							jsr FILLBOXSCREENIN

; Set the behaviour attributes linked to the image of the sprite 0
							lda #$ff
							sta SPRITE_ENABLEVAR			; Initially all sprites availables
							ldx #$00
LOADSPRITES:				lda #$20						; Initial block (definition) for of the sprite 0.
							sta TEMP02_DATA					; It will be also useful later!
							sta SPRITE_BLOCKVAR,x
							lda #$00						; Initial X position of the sprite 0.
							sta SPRITE_XPOSVAR,x
							lda #$00						; No MSB in X position for sprite 0.
							sta SPRITE_XMSBPOSVAR,x
							lda SPRITESPOSITION,x			; Initial Y position of the sprite 0. Never changes.
							sta SPRITE_YPOSVAR,x			
							lda SPRITESCOLOR,x
							sta SPRITEBASECOLOR,x
							inx
							cpx NUMBERSPRITES 
							bne LOADSPRITES
							lda #$00						; All sprites have the same priority level
							sta SPRITEPRIORITY

;Sets the IRQ							
							lda #<IRQPRG1_ADDRESS			; Stablish the first raster IRQ (the top part of the sea)
							sta SETVICIIRIRQ_PRGHVAR
							lda #>IRQPRG1_ADDRESS
							sta SETVICIIRIRQ_PRGLVAR
							lda #$00						; Almost at the end
							sta SETVICIIRIRQ_ROWLVAR
							lda #$00
							sta SETVICIIRIRQ_ROWHVAR
							jsr SETVICIIRASTERIRQ

							lda #$00
							sta TEMP03_DATA
; To move the scene...
MOVESCENE:					inc TEMP03_DATA
							lda TEMP03_DATA
							cmp #$07
							bne MOVEALLIENS
							lda #$00
							sta TEMP03_DATA
							inc TEMP02_DATA
							lda TEMP02_DATA
							cmp #$22
							bne MOVEALLIENS
							lda #$20
							sta TEMP02_DATA
; To move the aliens
MOVEALLIENS:				ldx #$00
MOVEALLIENS_LOOP:			lda TEMP02_DATA
							sta SPRITE_BLOCKVAR,x
							lda SPRITE_XMSBPOSVAR,x
							bne MOVEALLIENS_MSB
							inc SPRITE_XPOSVAR,x
							bne MOVEALLIENS_NEXT
							lda #$01
							sta SPRITE_XMSBPOSVAR,x
							jmp MOVEALLIENS_NEXT
MOVEALLIENS_MSB:			inc SPRITE_XPOSVAR,x
							lda SPRITE_XPOSVAR,x
							cmp #$5a
							bne MOVEALLIENS_NEXT
							lda #$00
							sta SPRITE_XPOSVAR,x
							sta SPRITE_XMSBPOSVAR,x
MOVEALLIENS_NEXT:			inx
							cpx #$08
							bne MOVEALLIENS_LOOP

; The positions are actualized using the IRQ!
; A set up a delay...
DELAY1:						dec TEMP10_DATA
							bne DELAY1
							lda #$80
							sta TEMP10_DATA
							dec TEMP11_DATA
							bne DELAY1
							lda #$05
							sta TEMP11_DATA

							jmp MOVESCENE

FOREVER:					jmp FOREVER						; For ever...

; End.