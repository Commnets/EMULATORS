; This program waits until the key "S" was pushed.
; Start with SYS 49152

*=$c000                  ; startaddress 

PRA  =  $dc00            ; CIA#1 (Port Register A)
DDRA =  $dc02            ; CIA#1 (Data Direction Register A)

PRB  =  $dc01            ; CIA#1 (Port Register B)
DDRB =  $dc03            ; CIA#1 (Data Direction Register B)


start:   sei             ; interrupts deactivated

         lda #z11111111  ; CIA#1 port A = outputs 
         sta DDRA             

         lda #z00000000  ; CIA#1 port B = inputs
         sta DDRB             

         lda #z11111101  ; testing column 1 (COL1) of the matrix
         sta PRA
            
loop:    lda PRB
         and #z00100000  ; masking row 5 (ROW5) 
         bne loop        ; wait until key "S" 

         cli             ; interrupts activated

ende:    rts             ; back to BASIC
