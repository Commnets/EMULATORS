; This program is a simple joystick check routine for control port 2.
; Joystick up/down = border color +/-
; Joystick left/right = background color +/-
; Fire button = end
; Start with SYS 49152

*=$c000   ; start address of the program
			lda #$06
			sta $d020
			lda #$01
			sta $d021
			
start:		lda $02
			cmp $dc01
			beq start      										; loop until the joystick register changes.
			
			lda $dc01      										; store new value in memory location 2.
			sta $02		
			
			lda #z00000001 										; mask joystick up movement 
			bit $dc01      										; bitwise AND with address 56321
			bne cont1      										; no movement up -> do not increase border color
			inc $d020      										; border color + 1

cont1:		lda #z00000010 										; mask joystick down movement 
			bit $dc01      										; bitwise AND with address 56321
			bne cont2      										; no movement down -> do not decrease border color
			dec $d020      										; border color- 1

cont2:		lda #z00000100 										; mask joystick left movement 
			bit $dc01      										; bitwise AND with address 56321
			bne cont3      										; no movement left -> do not increase background color
			inc $d021      										; background color + 1

cont3:		lda #z00001000 										; mask joystick right movement 
			bit $dc01      										; bitwise AND with address 56321
			bne cont4      										; no movement right -> do not decrease background color
			dec $d021      										; background color - 1

cont4:		lda #z00010000 										; mask joystick button push 
			bit $dc01      										; bitwise AND with address 56321
			bne start      										; button not pressed -> enter loop again 
			
loop:		jmp loop       										; forever

; End.