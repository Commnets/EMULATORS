; C64 Assembler framework.
; Routines related with managing interrupts.
; By Ignacio Cea Fornies.
; Copyright Community Networks 2022 - 2023.

#../C64Programs/code/macros/MVICII.asm
#../C64Programs/code/macros/MCIAS.asm

; ------------------------------
; SETVICIIRASTERIRQ
; To set the location of the raster IRQ.
; When the raster reaches the location defined, then a IRQ will be invoked.
; The IRQ routine has to be defined in the memory also passed as parameter.
; The parameters are:
;SETVICIIRIRQ_ROWHVAR	:	The high byte of the row where the IRQ has to happen (1 or 0).
;SETVICIIRIRQ_ROWLVAR	:	The low byte of the row where the IRQ has to happen (from 0 to 255).
;SETVICIIRIRQ_PRGHVAR	:	The high part of the address where the IRQ routine is located.
;SETVICIIRIRQ_PRGLVAR	:	The low part of that address.
SETVICIIRIRQ_ROWHVAR		= VARIABLES_DATAZONE
SETVICIIRIRQ_ROWLVAR		= VARIABLES_DATAZONE + 1
SETVICIIRIRQ_PRGHVAR		= VARIABLES_DATAZONE + 2
SETVICIIRIRQ_PRGLVAR		= VARIABLES_DATAZONE + 3
SETVICIIRASTERIRQ:			.SWITCHOFFIRQ
; Activate the VICII for raster interrupts							
							lda VICIICTRLIRQ
							and #$fe
							ora #$01
							sta VICIICTRLIRQ					; This is how to tell the VICII to generate a raster interrupt.
; Set the line where the interrupt has to happen.							
							lda SETVICIIRIRQ_ROWLVAR			; This is how to tell at which rasterline we want the irq to be triggered.
							sta VICIIRASTER
							lda SETVICIIRIRQ_ROWHVAR		
							beq SETVICIIRASTERIRQ_LOADPRG		; The raster line is below 255.
							lda VICIISCROLLY
							and #$7f
							ora #$80
							sta VICIISCROLLY					; Activate the MSB of the VICIIRASTER that is hold in VICIISCROLLY.
; Set the Kernel to indicate the address where to deal with the IRQ.
; This piece of code understands that there is no system IRQ to deal with.
SETVICIIRASTERIRQ_LOADPRG:	lda SETVICIIRIRQ_PRGLVAR			; Load the address where the program is
							sta $fffe							; This is where the IRQ system looks for where to jump...
							lda SETVICIIRIRQ_PRGHVAR			; When Kernel is disactivated (see address $1)
							sta $ffff

							cli									; Enable maskable interrupts again

							rts   
; ------------------------------

; End