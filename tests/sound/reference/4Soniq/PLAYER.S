*
* 4soniq toolbox demo
* by Brutal Deluxe Software
*
* v1.1 - AV 20170216
* Added shutdown
*

          mx %11
          org $8000
          lst off

          use 4/Locator.Macs
          use 4/Mem.Macs
          use 4/Sound.Macs
          use 4/Util.Macs

*--- Equates

myDP      = $7f00        ; address of the direct page for the Sound toolset
myCHANNEL = $ff          ; zero page address of the channel output

*---
* wav file is loaded at $01/2000
* we are on ProDOS 8
* we assume a file is less than 32KB
* the channel to output is in zero page $FF
*
* The sound tool set requires one direct page
*  We will assign $7F00..$7FFF below our player
*---

          jmp init       ; we init, we don't shutdown...
          jmp play       ; we play a sound on channel dp $FF
          jmp quit       ; we now shutdown...

*---

init      clc            ; 16-bit
          xce
          rep #$30

          _TLStartUp     ; Start the Tool locator

          pha            ; Start the Memory manager
          _MMStartUp
          pla
          sta myID

          PushWord #myDP ; Start the sound tool set
          _SoundStartUp

          sec            ; 8-bit
          xce
          sep #$30
          rts

*---

play      clc            ; 16-bit
          xce
          rep #$30

          PushWord #$ffff ; Stop all sounds
          _FFStopSound

          lda myCHANNEL
          and #%00000000_00001111
          xba            ; we have 00001111_00000000
          asl
          asl
          asl            ; we must have
          asl            ; them in bits 12-15
          ora #%00000000_00000001 ; freeform synthesizer
          pha            ; see toolbox ref vol 2 page 21-16
          PushLong #mySNDBLOCK
          _FFStartSound

          sec            ; 8-bit
          xce
          sep #$30
          rts

*---

quit      clc            ; 16-bit
          xce
          rep #$30

          _SoundShutDown ; Shutdown the sound tool set

          PushWord myID  ; Shutdown the Memory Manager
          _MMShutDown

          sec            ; 8-bit
          xce
          sep #$30
          rts


*--- My numerous data

myID      ds 2           ; memory manager ID

mySNDBLOCK adrl $012000  ; where in memory is my sound?
          dw $0080       ; waveSize - waveform size in pages
          dw 429         ; freqOffset - output sample rate
          dw $0000       ; docBuffer - DOC buffer start address
          dw $8000       ; bufferSize - DOC buffer size
          ds 4           ; next WavePtr - where in mem is the next?
          dw $00ff       ; volSetting - the higher, the noisier

          ds \
