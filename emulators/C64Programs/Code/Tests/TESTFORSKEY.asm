; C64 Assembler framework.
; Wait until S is pressed
; By Ignacio Cea Fornies.
; Copyright Community Networks 2022 - 2023.

; The base has to be defined before doing anything...
; and also to include the general file!
BASE = $c000
#../C64Programs/code/routines/RGENERAL.asm
; The rest of the files needed
#../C64Programs/code/routines/RSCREENCLEAR.asm

PRA							=  $dc00            				; CIA#1 (Port Register A)
DDRA						=  $dc02            				; CIA#1 (Data Direction Register A)
PRB							=  $dc01            				; CIA#1 (Port Register B)
DDRB						=  $dc03            				; CIA#1 (Data Direction Register B)
* = $cb00
; A very simple program to test everything works...
MAIN:						lda #$06
							sta VICIIBACKGROUND
							lda #$0e
							sta VICIIFOREGROUND
							jsr CLEARSCREEN
; Just to test when the S key is pressed, then the program exists!
							sei             					; Interrupts deactivated
							lda #$ff							; CIA#1 port A = outputs 
							sta DDRA             
							lda #$00							; CIA#1 port B = inputs
							sta DDRB             
							lda #z11111101  					; Testing column 1 (COL1) of the matrix
							sta PRA
LOOP:     					lda PRB
							and #z00100000						; Masking row 5 (ROW5) 
							bne LOOP							; Wait until key "S" 
; Then the program exists!
							cli             					; Interrupts activated
							brk

; End.