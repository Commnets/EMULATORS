; General routines
; By Ignacio Cea Forniés
; Copyright Community Networks 2022 - 2023

; TEMPLATES used in many parts of the program

; To save the main registers in the stack
{ SAVEREGISTERS
							PHA									; Starting from the accumulator...
							TXA
							PHA
							TXY
							PHA
}
							
; To recover the main registers from the stack
{ RECOVERREGISTERS
							PLA
							TAY
							PLA
							TAX
							PLA									; Ending with the accumulator...
}

; End