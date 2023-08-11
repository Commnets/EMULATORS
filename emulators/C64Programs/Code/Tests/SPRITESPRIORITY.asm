; C64 Assembler framework.
; To test whether an sprite is well drawn among the different graphical modes.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

; The base has to be defined before doing anything...
; and also to include the general file!
BASE = $c000
#../C64Programs/code/routines/RGENERAL.asm
; The rest of the files needed
#../C64Programs/code/routines/RSCREENCLEAR.asm
#../C64Programs/code/routines/RSPRITES.asm
#../C64Programs/code/routines/RCOPYCHARSET.asm
#../C64Programs/code/routines/RIRQ.asm

* = $ca00
; This interruption happens at line 175
IRQPRG1_ADDRESS				= $ca00
IRQPRG1:					
;-----
							lda #$05
							sta VICIIBACKGROUND
; Here there is nothing special to do...
;-----
							lda VICIIIRQ
							sta VICIIIRQ						; To desactivate the reasons of the interruption
							lda #$b6
							sta VICIIRASTER						; Next raster line to interrupt is b6 = 182
							lda #>IRQPRG2_ADDRESS
							sta $0314
							lda #<IRQPRG2_ADDRESS
							sta $0315
							
IRQPRG1_EXIT:				.RECOVERREGISTERS
							cli
							rti

; This interruption happens at line 182
* = $ca64
IRQPRG2_ADDRESS				= $ca64
IRQPRG2:
;-----
; The background is grey...
							lda #$08
							sta VICIIBACKGROUND
; Activate the multichar mode = 1...
							jsr MULTICOLOR_SET
; Set the colors used in the this mode...
							lda #$07
							sta VICIIBACKGROUND1				
							lda #$0a
							sta VICIIBACKGROUND2
							lda #$09
							sta VICIIBACKGROUND3
; Desactivate the sprite 0
							lda SPRITEENABLE
							and #$fe
							ora #$00
							sta SPRITEENABLE
;-----
							lda VICIIIRQ
							sta VICIIIRQ						; To desactivate the reasons of the interruption
							lda #$fb
							sta VICIIRASTER						; Next raster line to interrupt is bf = 251, out of the screen
							lda #>IRQPRG3_ADDRESS
							sta $0314
							lda #<IRQPRG3_ADDRESS
							sta $0315
							
IRQPRG2_EXIT:				.RECOVERREGISTERS
							cli
							rti

; This interruption happens at line 251 (out of the screen)
* = $cac8
IRQPRG3_ADDRESS				= $cac8
IRQPRG3:
;-----
; The background is black...
							lda #$00
							sta VICIIBACKGROUND
; Activate the enhaced text mode = 4...
							jsr ENHANCEDCOLOR_SET
; Set the colors used in the this mode...
							lda #$07
							sta VICIIBACKGROUND1				
							lda #$08
							sta VICIIBACKGROUND2
							lda #$09
							sta VICIIBACKGROUND3
; Activate the sprite 0
							lda SPRITEENABLE
							and #$fe
							ora #$01
							sta SPRITEENABLE
;-----
							lda VICIIIRQ
							sta VICIIIRQ						; To desactivate the reasons of the interruption
							lda #$af
							sta VICIIRASTER						; Next raster line to interrupt is af = 172, so back to the beginning...
							lda #>IRQPRG1_ADDRESS
							sta $0314
							lda #<IRQPRG1_ADDRESS
							sta $0315

IRQPRG3_EXIT:				.RECOVERREGISTERS							
							cli
							rti

* = $cb2c
; This routine is to activate the multicolor char text mode
MULTICOLOR_SET:				lda VICIISCROLLY					
							and #$9f
							ora #$00							; Bits 1 and 2 high nibble to 0 (no graphics mode, no extended mode)
							sta VICIISCROLLY
							lda VICIISCROLLX
							and #$ef
							ora #$10							; Bit 0 high nibble to 1 (multicolor text/graphics)
							sta VICIISCROLLX
							rts
; This routine is to activate the enhanced color text mode
ENHANCEDCOLOR_SET:			lda VICIISCROLLY					
							and #$9f
							ora #$40							; Bits 1 high nibble to 0 (no graphics mode), and bit 2 to 1 (extended mode)
							sta VICIISCROLLY
							lda VICIISCROLLX
							and #$ef
							ora #$00							; Bit 0 high nibble to 0 (no multicolor text/graphics)
							sta VICIISCROLLX
							rts
; This routine is to activate the enhanced color text mode
NORMALCOLOR_SET:			lda VICIISCROLLY					
							and #$9f
							ora #$00							; Bits 1 high nibble to 0 (no graphics mode), and bit 2 to 0 (no extended mode)
							sta VICIISCROLLY
							lda VICIISCROLLX
							and #$ef
							ora #$00							; Bit 0 high nibble to 0 (no multicolor text/graphics)
							sta VICIISCROLLX
							rts

; Define the sprite multicolor
* = $0800
BINARYDEF A=00 B=01 C=10 D=11
BINARY	AABBBBBBBBAA
BINARY	AABBBBBBBBAA
BINARY	AABBBBBBBBAA
BINARY	AABBBBBBBBAA
BINARY	AABBBBBBBBAA
BINARY	AABBBBBBBBAA
BINARY	AAACCCCCCAAA
BINARY	AAACCCCCCAAA
BINARY	AAACCCCCCAAA
BINARY	AAACCCCCCAAA
BINARY	AAACCCCCCAAA
BINARY	AAACCCCCCAAA
BINARY	AADDDDDDDDAA
BINARY	AADDDDDDDDAA
BINARY	AADDDDDDDDAA
BINARY	AADDDDDDDDAA
BINARY	AADDDDDDDDAA
BINARY	AADDDDDDDDAA
BINARY	AADDDDDDDDAA
BINARY	AADDDDDDDDAA
BINARY	AADDDDDDDDAA

; Where the chars will be looked for...
CHARDESTINATION = $3000
; Defines the character multicolor/extended mode (depending on the code below)
* = $0840
CHARDEF = $0840
BINARY DDDD
BINARY DDDD
BINARY DDDD
BINARY DDDD
BINARY AAAA
BINARY AAAA
BINARY AAAA
BINARY AAAA

; The main program. It has to be invokes from BASIC using sys 52224
* = $cc00
MAIN:						jsr CLEARSCREEN
							lda #$00
							sta VICIIBACKGROUND
							lda #$00
							sta VICIIFOREGROUND

MAIN_SETCHARS:				lda #<CHARDESTINATION
							sta COPYCHAR_TEXTHIGHVAR
							jsr COPYCHARSET
							ldy #$00
MAIN_SETCHARSLOOP:			lda CHARDEF,y
							sta CHARDESTINATION,y
							iny
							cpy #$08							; Only 1 chars to redefine (the number 0)
							bne MAIN_SETCHARSLOOP
							jsr ACTIVATECHARSET

MAIN_SETSCROLLY:			lda VICIISCROLLY					; Put the SCROLLY to 0!
							and #$f8
							ora #$00
							sta VICIISCROLLY

; Draws the separation between the lines 175 and 182 in the raster line...
MAIN_SETSCREEN1:			ldx #$00
MAIN_SETSCREEN1LOOP:		lda #$c0
							sta SCREENBASE + 640,x
							lda #$01
							sta COLORRAMBASE + 640,x
							inx
							cpx #$28
							bne MAIN_SETSCREEN1LOOP

; Draws a simulation of text in the lines behind the 182 (8 lines later even)...
MAIN_SETSCREEN2:			ldx #$00
MAIN_SETSCREEN2LOOP:		lda #$03
							sta SCREENBASE + 720,x
							lda #$09
							sta COLORRAMBASE + 720,x
							inx
							cpx #$28
							bne MAIN_SETSCREEN2LOOP

; Draws the sprite in the screen. It is a multicolor sprite...							
MAIN_SETSPRITE:				lda #$20
							sta $07f8
							lda #$90
							sta SPRITEBASEXPOS
							lda #$a0
							sta SPRITEBASEYPOS
							lda #$03
							sta SPRITEBASECOLOR
							lda #$06
							sta SPRITESCOLOR1
							lda #$0f
							sta SPRITESCOLOR2
							lda SPRITEDWIDTH
							and #$fe
							ora #$01
							sta SPRITEDWIDTH
							lda SPRITEDHEIGHT
							and #$fe
							ora #$01
							STA SPRITEDHEIGHT
							lda SPRITEMCOLOR
							and #$fe
							ora #$01
							sta SPRITEMCOLOR
							lda SPRITEPRIORITY
							and #$fe
							ora #$01
							sta SPRITEPRIORITY
							lda SPRITEENABLE
							and #$fe
							ora #$01
							sta SPRITEENABLE

MAIN_VICRQON:				sei
							lda #z00000001
							sta VICIICTRLIRQ					; Activate the interruptions for raster line
							lda #$af
							sta VICIIRASTER						; First raster line to interrupt is at = 172...

MAIN_IRQON:					lda #>IRQPRG1_ADDRESS				; ...and the first IRQ interruption has to be the one related with 172...
							sta $0314
							lda #<IRQPRG1_ADDRESS
							sta $0315
							cli
							
FOREVER:					jmp FOREVER

; End