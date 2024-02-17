; C64 Assembler framework.
; Routine to copy the set o characters from to a position into rhe RAM.
; By Ignacio Cea Fornies.
; Copyright Community Networks 2022 - 2023.

#../C64Programs/code/routines/RGENERAL.asm

; ------------------------------
; COPYCHARSET
; To copy the character set from ROM to RAM.
; The active bank for VICII should be aligned with the destination address.
; This piece of code doesn't actualize the bank, just copy the chars.
; COPYCHAR_TEXTHIGHVAR	:	High byte of the address where the char set hay to be located, no low is required...
; First of all the system has to be prepared to copy...
COPYCHAR_TEXTLOWVAR			= VARIABLES_DATAZONE
COPYCHAR_TEXTHIGHVAR		= VARIABLES_DATAZONE + 1
COPYCHARSET:                lda COPYCHAR_TEXTHIGHVAR
							pha                             	; Because it is going to be modified...
							lda #$00
							sta COPYCHAR_TEXTLOWVAR
							lda #>ROMCHARSET					; Where the char data is stored...
							sta TEMP00_DATA
							lda #<ROMCHARSET
							sta TEMP01_DATA
; Switch off interrupts
							.SWITCHOFFIRQ					
; Charset ROM available for reading from CPU (char set id behind VICII/CIA1/CIA2 registers).
							lda ROMCTRLAVAILABLE
							and #$fb
							sta ROMCTRLAVAILABLE			
; Copy the char set
							ldx #10								; 16 blocks of 256 bytes (32 chars each) = 512 chars copied = 4096 bytes.
COPYCHARSET_LOOPBLOCK:      ldy #$00
COPYCHARSET_LOOPPAGE:       lda (TEMP00_DATA),y             	; Read data from ROM...
							sta (COPYCHAR_TEXTLOWVAR),y     	; ...and copy them into RAM.
							iny
							bne COPYCHARSET_LOOPPAGE
							inc TEMP01_DATA
							inc COPYCHAR_TEXTHIGHVAR
							dex
							bne COPYCHARSET_LOOPBLOCK
							; Character ROM not longer available for reading from CPU.
							lda ROMCTRLAVAILABLE
							ora #$04
							sta ROMCTRLAVAILABLE			
; IRQS allowed again
							cli								
; Restore the original value
							pla
							sta COPYCHAR_TEXTHIGHVAR		
							rts
; ------------------------------

; ------------------------------
; ACTIVATECHARSET
; To activate the char set in a specific memory location.
; The paramaters are the same than in the previous method.
; The previous code doesn't activate the location, 
; because many things can happen from copy to activation!
ACTIVATECHARSET:			lda COPYCHAR_TEXTHIGHVAR			; Gets high value of the address...
							lsr									; Blocks of 1024 bytes...
							lsr									; High byte determines the number of "256 block" so 4 times more is the number of the "1024 block" equivalent).
							sta TEMP00_DATA
							lda VICIICTRLMEMREG
							and #$f0
							ora TEMP00_DATA
							sta VICIICTRLMEMREG
							rts
; ------------------------------

;End.