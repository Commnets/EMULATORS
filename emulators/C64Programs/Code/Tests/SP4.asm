; C64 Assembler framework.
; To test whether an sprite is well drawn among the different graphical modes.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

; The base has to be defined before doing anything...
; and also to include the general file!
BASE = $c000

; ------------------------------
; MCROS DEFINITION
SCREENBASE					= $0400								; Where usually the screen starts.
SCREEN1_256BLK				= $0400								; The screen can be easly divided in blocks of 256K...
SCREEN2_256BLK				= $0500								; ...the next variables referes to those blocks.
SCREEN3_256BLK				= $0600
SCREEN4_256BLK				= $0700
COLORRAMBASE				= $d800								; Where the ram color starts.
COLORRAM1_256BLK			= $d800								; The color RAM can be easly dividev in blocks of 256K.
COLORRAM2_256BLK			= $d900								; ...the next variables referes to those blocks.
COLORRAM3_256BLK			= $da00
COLORRAM4_256BLK			= $db00
ROMCHARSET					= $d000								; Where the CPU sees the default char set.
ROMCTRLAVAILABLE			= $01								; To control whether roms are or not visible from CPU.
SPRITEBASELOCATION			= $07F8								; Position where to find the 64 block (1 bank) where the sprites are.
; ------------------------------

; ------------------------------
; MCROS DEFINITION
; Sprites
SPRITEENABLE				= $d015								; Sprites on - off.
SPRITEBASEXPOS				= $d000								; Sprite base coordinate X (+ 2*n sprite).
SPRITEBASEYPOS				= $d001								; Sprite base coordinate Y (+ 2*n sprite).
SPRITEMSBXPOS				= $d010								; The most significant bit of the X position for sprites.
SPRITEDWIDTH				= $d01d								; To set double width for sprites.
SPRITEDHEIGHT				= $d017								; To set double height for sprites.
SPRITEBASECOLOR				= $d027								; Sprite base color (+ n sprite).
SPRITEPRIORITY				= $d01b								; To manage the priority of the sprites over the foreground.
SPRITEMCOLOR				= $d01c								; To set the multicolor mode for sprites.
SPRITESCOLOR1				= $d025								; Sprite Shared Color 1 (once above is set).
SPRITESCOLOR2				= $d026								; Sprite Shared Color 2 (once above is set).
; Control of the screen
VICIIFOREGROUND				= $d020								; Foreground.
VICIIBACKGROUND				= $d021								; Background 0.
VICIIBACKGROUND1			= $d022								; Background 1, used in multicolor and extended background modes.
VICIIBACKGROUND2			= $d023								; Background 2, used also in those modes.
VICIIBACKGROUND3			= $d024								; Background 3, used also in those modes.
VICIISCROLLY				= $d011								; SCROLL in the Y axis, and also type of graphic mode.
VICIISCROLLX				= $d016								; SCROLL in the X axis, and also activation of multicolor possibilities.
VICIICTRLMEMREG				= $d018								; Control the location of the graphical memory.
; IRQ
VICIIIRQ					= $d019								; The active sources of the last IRQ (if any)
VICIICTRLIRQ				= $d01a								; To control the source of VICII IRQs.
VICIIRASTER					= $d012								; To manage the raster line (set and know)
; ------------------------------

; ------------------------------
CIA1IRQ						= $dc0d								; Control sources IRQ (different behaviour when read/write).
CIA2IRQ						= $dd0d								; Same for CIA2.
; ------------------------------

; ------------------------------
; TEMPLATES DEFINITION
; To save the main registers in the stack.
{ SAVEREGISTERS
							pha								; Starting from the accumulator...
							txa
							pha
							tya
							pha
}
							
; To recover the main registers from the stack.
{ RECOVERREGISTERS
							pla
							tay
							pla
							tax
							pla								; Ending with the accumulator...
}
; ------------------------------

* = $ca00
; This interruption happens at line 175
IRQPRG1_ADDRESS				= $ca00
IRQPRG1:					
;-----
; The background is green...
							lda #$05
							sta VICIIBACKGROUND
							sta VICIIFOREGROUND
;-----
							lda #z00000001
							sta VICIIIRQ						; To desactivate the reasons of the interruption
							lda #$b6
							sta VICIIRASTER						; Next raster line to interrupt is b6 = 182
							lda VICIISCROLLY					; The bit 7 of the raster definition must be 0
							and #$7f
							sta VICIISCROLLY
							lda #>IRQPRG2_ADDRESS
							sta $0314
							lda #<IRQPRG2_ADDRESS
							sta $0315
							
IRQPRG1_EXIT:				.RECOVERREGISTERS
							rti

; This interruption happens at line 182 (out of the screen)
* = $ca64
IRQPRG2_ADDRESS				= $ca64
IRQPRG2:
;-----
; The background is brown...
							lda #$08
							sta VICIIBACKGROUND
							sta VICIIFOREGROUND
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
							lda #z00000001
							sta VICIIIRQ						; To desactivate the reasons of the interruption
							lda #$fb
							sta VICIIRASTER						; Next raster line to interrupt is af = 251, so back to the beginning...
							lda VICIISCROLLY					; The bit 7 of the raster definition must be 0
							and #$7f
							sta VICIISCROLLY
							lda #>IRQPRG3_ADDRESS
							sta $0314
							lda #<IRQPRG3_ADDRESS
							sta $0315

IRQPRG2_EXIT:				.RECOVERREGISTERS
							rti


; This interruption happens at line 251 (out of the screen)
* = $cac8
IRQPRG3_ADDRESS				= $cac8
IRQPRG3:
;-----
; The background is black...
							lda #$00
							sta VICIIBACKGROUND
							sta VICIIFOREGROUND
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
							lda #z00000001
							sta VICIIIRQ						; To desactivate the reasons of the interruption
							lda #$af
							sta VICIIRASTER						; Next raster line to interrupt is af = 172, so back to the beginning...
							lda VICIISCROLLY					; The bit 7 of the raster definition must be 0
							and #$7f
							sta VICIISCROLLY
							lda #>IRQPRG1_ADDRESS
							sta $0314
							lda #<IRQPRG1_ADDRESS
							sta $0315

IRQPRG3_EXIT:				.RECOVERREGISTERS
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
* = $0340
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

; The main program. It has to be invokes from BASIC using sys 52224
* = $cc00
MAIN:						lda #$00
							sta VICIIBACKGROUND
							sta VICIIFOREGROUND
							jsr $e544

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
MAIN_SETSPRITE:				lda #$0d
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
							lda #$7f
							sta CIA1IRQ							; Desactivate interruptions in CIA1
							lda CIA1IRQ  
							sta CIA2IRQ							; Desactivate interruptions in CIA2
							lda CIA2IRQ  
							lda #z00000001
							sta VICIICTRLIRQ					; Activate the interruptions for raster line
							lda #$af
							sta VICIIRASTER						; First raster line to interrupt is at = 172...
							lda VICIISCROLLY
							and #$7f
							sta VICIISCROLLY

MAIN_IRQON:					lda #>IRQPRG1_ADDRESS				; ...and the first IRQ interruption has to be the one related with 172...
							sta $0314
							lda #<IRQPRG1_ADDRESS
							sta $0315
							cli
							
FOREVER:					jmp FOREVER

; End