; C64 Assembler framework.
; Routine to copy the set o characters from to a position into rhe RAM.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

#../C64Programs/Code/RGENERAL.asm

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
							pha                             ; Because it is going to be modified...
							lda #$00
							sta COPYCHAR_TEXTLOWVAR
							lda #>$d000						; Where the data is stored...
							sta TEMP00_DATA
							lda #<$d000
							sta TEMP01_DATA
							lda $dc0e
							and #$fe
							sta $dc0e                       ; Interrupts are desactivated.
							lda $01
							and #$fb
							sta $01                         ; Charset ROM available for reading from CPU (char set id behind VICII/CIA1/CIA2 registers).
							ldx #10							; 16 blocks of 256 bytes (32 chars each) = 512 chars copied = 4096 bytes.
COPYCHARSET_LOOPBLOCK:      ldy #$00
COPYCHARSET_LOOPPAGE:       lda (TEMP00_DATA),y             ; Read data from ROM...
							sta (COPYCHAR_TEXTLOWVAR),y     ; ...and copy them into RAM.
							iny
							bne COPYCHARSET_LOOPPAGE
							inc TEMP01_DATA
							inc COPYCHAR_TEXTHIGHVAR
							dex
							bne COPYCHARSET_LOOPBLOCK
							lda $01
							ora #$04
							sta $01                         ; Character ROM not longer available for reading from CPU.
							lda $dc0e
							ora #$01
							sta $dc0e                       ; Interrups are activated back.
							pla
							sta COPYCHAR_TEXTHIGHVAR		; Gets the original value back...
							lsr								; Blocks of 1024 bytes (high byte determines the number of "256 block" so 4 times more is the number of the "1024 block" equivalent).
							lsr
							sta TEMP00_DATA
							lda $d018
							and #$f0
							ora TEMP00_DATA
							sta $d018
							rts
; ------------------------------

;End.