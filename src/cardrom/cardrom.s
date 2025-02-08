; ******************************************************************************
; A2FPGA - STARTROM.S
;
; /INH ROM for A2FPGA startup
;
; This ROM code is used to initialize the A2FPGA board.  It is mapped into
; the 6502 address space at $F800-$FFFF.  The code is executed by the 6502
; at startup time by the A2FPGA board asserting the /INH signal prior to
; releasing the 6502 from reset.  The primary purpose of this code is to
; poll the Apple II keyboard and wait for the FPGA to signal that it is ready
; for the 6502 to resume the normal Apple II boot process.
; ******************************************************************************
;
NMI      =        $FB03        ; NON-MASKABLE INTERRUPT VECTOR      
RESET    =        $FA62        ; RESET VECTOR
IRQ      =        $FA40        ; INTERRUPT REQUEST VECTOR
KBD      =        $C000        ; APPLE KEYBOARD DATA
KSTRB    =        $C010        ; KEYBOARD DATA CLEAR
FPGADONE =        $F7FF        ; TBD - SOME MEMORY LOCATION
RESETVEC =        $FFFC        ; JUMP TARGET
;
; **************************  INITIALIZE ***************************************
;
        ORG $F800              ; PROGRAM START ADDRESS

MAIN    
        LDA KBD                ; TEST KEYBOARD
                               ; TBD - MAYBE DO SOMETHING @ KEYPRESS
        LDA FPGADONE           ; FETCH FPGADONE
        BNE MAIN               ; CONTINUE TO LOOP IF FPGADONE IS NOT 0  
        JMP (RESETVEC)        ; JUMP TO RESET VECTOR

ISR     PHA
        TXA
        PHA
                               ; TBD - Interrupt code goes here
        PLA
        TAX
        PLA
        RTI

; Dynamically pad from current address up to $FFFA
        ORG *                  ; Ensure we are at the current location

PAD_SIZE = $FFFA - *           ; Calculate the number of bytes needed to reach $FFFA
        DS PAD_SIZE            ; Reserve the required number of padding bytes

        ORG $FFFA              ; Set up interrupt vectors at the exact memory location
VECTORS DW ISR                 ; Set NMI vector
        DW RESET               ; Set RESET vector
        DW ISR                 ; Set IRQ vector
;
; <<EoF>>
;