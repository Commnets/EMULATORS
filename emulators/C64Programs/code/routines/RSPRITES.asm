; C64 Assembler framework.
; General elements to manage sprites.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

#../C64Programs/code/macros/MSCREEN.asm
#../C64Programs/code/macros/MVICII.asm
#../C64Programs/code/macros/MCIAS.asm

; ------------------------------
; ACTUALIZESPRITES
; Used for some of the routines to read typical things about the sprites and
; transform them into VICII register information.
; The variables admitted by the routine are:
;SPRITE_ENABLEVAR		:	1 byte to indicate whether the sprite is or not active.
;SPRITE_XPOSVAR			:	8 bytes (1 per sprite) to point the X position (together with the MSB byte).
;SPRITE_YPOSVAR			:	8 bytes (1 per sprite) to point the Y position (together with the MSB byte).
;SPRITE_XMSBPOSVAR		:	8 bytes 81 per sprite) to point whether a sprite is above the $ff position, only the first bit is used.
;SPRITE_BLOCKVAR		:	8 bytes (1 per sprite) to indicate the block of 64 (within the current bank) where the form of the sprite is.
SPRITE_ENABLEVAR			= BLOCK_DATAZONE
SPRITE_XPOSVAR				= BLOCK_DATAZONE + 8
SPRITE_YPOSVAR				= BLOCK_DATAZONE + 16
SPRITE_XMSBPOSVAR			= BLOCK_DATAZONE + 24
SPRITE_BLOCKVAR				= BLOCK_DATAZONE + 32
ACTUALIZESPRITES:			.SAVEREGISTERS
							lda SPRITE_ENABLEVAR
							sta SPRITEENABLE
							ldx #$07
ACTUALIZESPRITES_LOOP:		txa
							asl
							tay								; Y = X * 2
							lda SPRITE_BLOCKVAR,x
							sta SPRITEBASELOCATION,x		; The form of the sprite X
							lda SPRITE_XPOSVAR,x
							sta SPRITEBASEXPOS,y			; The X position of the sprite X.
							lda SPRITE_YPOSVAR,x
							sta SPRITEBASEYPOS,y			; The Y position of the sprite X.
							txa
							tay
							lda #$01
ACTUALIZESPRITES_LOOPB:		cpy #$00
							beq ACTUALIZESPRITES_SETMSB
							asl
							dey
							jmp ACTUALIZESPRITES_LOOPB
ACTUALIZESPRITES_SETMSB:	sta TEMP00_DATA
							eor #$ff
							sta TEMP01_DATA
							lda SPRITE_XMSBPOSVAR,x
							and #$01
							tay
							lda SPRITEMSBXPOS
							and TEMP01_DATA
							cpy #$00
							beq ACTUALIZESPRITES_NEXT
							ora TEMP00_DATA
ACTUALIZESPRITES_NEXT:		sta SPRITEMSBXPOS
							dex
							bpl ACTUALIZESPRITES_LOOP
							.RECOVERREGISTERS
							rts
; ------------------------------

;End.