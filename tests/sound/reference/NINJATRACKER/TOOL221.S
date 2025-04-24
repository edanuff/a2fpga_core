*
* Tool221
* by Ninjaforce
* Visit ninjaforce.com
*
* Based on Tool219
* by FTA & ESP, 1990
* See disclaimer below
* Visit freetoolsassociation.com
*
* Tool adaptation by
* Brutal Deluxe Software, 2015
* Visit brutaldeluxe.fr
*
* 4soniq support by
* Brutal Deluxe Software, 2016
*
* v2.1 - AV - 4/Sep/2018 - Thank you Scott D.
*  Stop/Start behaved badly
*  Error codes returns with carry clear
*

          lst off
          rel
          typ TOL
          dsk Tool221.L
          lst off

*------------------------------------------------- Tool221

Version   = $0201        ; v2.1
HTool     = $DD00

          Dum $C
Wave_Ptr  ds 4
Music_Ptr ds 4
Buf_Ptr   ds 4
BatchPos  ds 2
ListPlay_Ptr ds 4
ListPlay_Pos ds 2
Music_Hdl ds 4
BatchList ds 2
          Dend

*Nb_Track = 14
SonREG    Equ $E1C03E
SonDATA   Equ $E1C03D
SonCTRL   Equ $E1C03C

* ST, SoundTrack Tool

Max_Batch Equ 26

Err_AlreadyInit = HTool+$1
Err_STNotInit = HTool+$2
Err_NoMusInMem = HTool+$3
Err_BadMusic = HTool+$4
Err_STMusicExist = HTool+$5
Err_STBatchNumber = HTool+$6
Err_STNoBatchMusic = HTool+$7
Err_BadPlayList = HTool+$8
Err_BadWave = HTool+$9

_STErr    MAC
          bcc No_Err
          brl STDeath
No_Err    <<<

*-------------------------------------------------

STCallTable adrl STEndTable-STCallTable/4

          adrl STBootInit-1 ; 1
          adrl STStartUp-1 ; 2
          adrl STShutDown-1 ; 3
          adrl STVersion-1 ; 4
          adrl STReset-1 ; 5
          adrl STStatus-1 ; 6
          adrl STReset-1 ; 7
          adrl STReset-1 ; 8

          adrl STLoadOneMusic-1 ; $09
          adrl STPlayMusic-1 ; $0A
          adrl STStopMusic-1 ; $0B
          adrl STGetEndOfMusic-1 ; $0C
          adrl STAddToBatch-1 ; $0D
          adrl STSelectBatch-1 ; $0E
          adrl STKillBatch-1 ; $0F
          adrl STGetPlayingMusic-1 ; $10
          adrl STPlayBatch-1 ; $11
          adrl STGetTrackVU-1 ; $12
          adrl STPauseMusic-1 ; $13
          adrl STContinueMusic-1 ; $14
          adrl STSetDecompactMus-1 ; $15
          adrl STSetDecompactWave-1 ; $16

*-------------------------------------------------

STEndTable asc "-* NinjaTracker Tool by Ninjaforce *-"
          asc "-* (c) Ninjaforce, 2015 *-"
          asc "-* Based on Tool219 by Olivier GOGUEL *-"
          asc "-* (c) FTA, October 1990 *-"
          asc "-* (c) 1990 ToolBox-Mag *-"
          asc "-* 6 Rue H. Barbusse *-"
          asc "-* 95100 - Argenteuil - France *-"
          asc "-* revision 2.1, 4-Sep-2018 *-",00

*-------------------------------------------------

          mx %00

STDeath   pha
          pea ^Death_Msg
          pea Death_Msg
          ldx #$1503
          jsl $E10000
Death_Msg str 'NINJATRACKER Tool Error : $'

STReset
STBootInit lda #$FFFF    ; AV 201506 was #$FFF
          stal Current_Music
          lda #0
          stal ST_OpenFlag ; Outil Ferme
          stal Performing
          stal OldPerforming
          clc
          rtl

*-------------------------------------------------

STPauseMusic ldal Performing
* v2.1
          beq No_SaveSoundRegister
          stal OldPerforming

          lda #0
          stal Performing

          php
          sep $30
          sei

]lp       ldal SonCTRL
          bmi ]lp
          ldal $E100CA
          and #$0F
          stal SonCTRL

          ldx #0
]loop     txa
          clc
          adc #$A0
          stal SonREG
          ldal SonDATA
          ldal SonDATA
          stal SoundRegisters,X ; Sauvegarde les
          txa
          clc
          adc #$A1
          stal SonREG
          ldal SonDATA
          ldal SonDATA
          stal SoundRegisters+1,X

          ldy #2         ; 2fois pour arreter
]lp       txa            ; les sons qui bouclent
          clc
          adc #$A0
          stal SonREG
          lda #%0000_0001 ; Arret de l'oscillo
          stal SonDATA   ; en mode free run...
          txa
          clc
          adc #$A1
          stal SonREG
          lda #%0000_0001
          stal SonDATA
          dey
          bne ]lp

          inx
          inx
          cpx #30
          bne ]loop

          plp
          lda #0         ; v2.1
No_SaveSoundRegister clc
          rtl

SoundRegisters ds 32

*-------------------------------------------------

          mx %00

STContinueMusic lda #0
OldPerforming = *-2
          beq DontModifyPerforming
          stal Performing

          php

          sep $30
          sei

]lp       ldal SonCTRL
          bmi ]lp

          ldal $E100CA
          and #$0F
          stal SonCTRL

          ldx #0
]lp       txa
          clc
          adc #$A0
          stal SonREG
          ldal SoundRegisters,X
          stal SonDATA
          inx
          cpx #30
          bne ]lp

          plp
          lda #0         ; v2.1
DontModifyPerforming clc
          rtl

          mx %00

*-------------------------------------------------

STStartUp lda 7,S        ; Prend UserId
          stal UserId
          lda 5,S
          sta 7,S
          lda 3,S
          sta 5,S
          lda 1,S
          sta 3,S
          pla
          lda #0
ST_OpenFlag = *-2
          beq NotYet
          lda #Err_AlreadyInit
          sec
          rtl
NotYet    phb
          phd
          sei
          phk
          plb
          pea $0000
          pea $0000
          pea $0000
          pea $0100      ; Une Page Zero
          lda UserId
          pha
          pea $C001
          pea $0000
          pea $0000
          ldx #$0902     ; New Handle
          jsl $E10000
          _STErr
          pla
          pla
          tsc
          sec
          sbc #4
          tcs
          inc
          tcd
          lda [$00]
          sta ST_ZPage
          lda $00
          sta ZPage_Hdl
          lda $02
          sta ZPage_Hdl2
          tsc
          clc
          adc #4
          tcs
          lda #0
ST_ZPage  = *-2
          tcd
          ldx #$100-2
]lp       stz $00,x
          dex
          dex
          bpl ]lp

          ldx #$20-2     ; AV 201506 Added the InstVu init
]lp       stz TrackVu,x
          stz InstVu,x   ; to prevent any crash if the buffers
          dex            ; are not one after the other
          dex
          bpl ]lp

          stz Performing
          stz OldPerforming

          sep $30

          lda #$00
          stal SonCTRL

          ldx #0
]lp       txa
          clc
          adc #$A0
          stal SonREG
          lda #$03
          stal SonDATA
          ldal SonDATA
          ldal SonDATA
          cmp #3
          bne ]lp
          inx
          cpx #32
          bne ]lp

          lda #%01100000
          stal SonCTRL
          lda #0
          stal $E1C03E
          stal $E1C03F

          ldx #0
]lp       lda #$80
          stal SonDATA
          inx
          bne ]lp

          rep #$30

          ldal $E1002C
          sta Save_SoundIrq
          ldal $E1002E
          sta Save_SoundIrq2

          dfb $A9,$5C,#<SoundIRQrtn
          stal $E1002C
          dfb $A9,#>SoundIRQrtn,#^SoundIRQrtn
          stal $E1002E

          sep $30

          lda #$00
          stal SonCTRL

          ldy #0
]loop     lda Table_Son,Y
          stal SonREG
          lda Table_Son+1,Y
          stal SonDATA
          iny
          iny
          cpy #7*2
          bne ]loop

          ldal $E100CA   ; Mets le volume
          and #$0F
          stal SonCTRL

          rep #$30

          pea ^Normal_RTL
          pea Normal_RTL
          ldx #$15DD     ; AV
          jsl $E10000    ; Adresse decompacteur
                         ; musique
          pea ^Normal_RTL
          pea Normal_RTL
          ldx #$16DD     ; AV Adresse decompacteur
          jsl $E10000    ; wave

          lda #1
          sta ST_OpenFlag ; InitFlag
          lda #$FFFF
          sta Current_Music

          lda #0
StartupErr pld
          plb
          cli
          cmp #1
          rtl

*-------------------------------------------------

STShutDown phb
          phd
          php
          sei
          phk
          plb

          lda ST_OpenFlag
          beq NotInit

          lda ST_ZPage
          tcd

          jsl StopMusic

          sep $30

]lp       ldal SonCTRL
          bmi ]lp
          ldal $E100CA
          and #$0F
          stal SonCTRL

]lp       lda #$BE
          stal SonREG
          lda #%0000_0011
          stal SonDATA
          ldal SonDATA
          ldal SonDATA
          cmp #%0000_0011
          bne ]lp

          rep $30

          lda #0
Save_SoundIrq = *-2
          stal $E1002C
          lda #0
Save_SoundIrq2 = *-2
          stal $E1002E

          jsr Dispose_OneMusic
          jsr Dispose_BatchMusic

          pea #0
ZPage_Hdl2 = *-2
          pea #0
ZPage_Hdl = *-2
          ldx #$1002     ; Dispose Handle
          jsl $E10000
          _STErr

NotInit   jsl STReset    ; AV 201506
*NotInit  lda   #$FFFF
*         sta   Current_Music
*         lda   #0
*         sta   Performing
*         sta   OldPerforming
*         sta   ST_OpenFlag

          plp
          pld
          plb
          lda #0
          clc
          rtl

UserId    ds 2

*-------------------------------------------------

STVersion lda #Version
          sta 7,S
          lda #0
          clc
          rtl


*-------------------------------------------------

STStatus  ldal ST_OpenFlag
          sta 7,S
          lda #0
          clc
          rtl

*-------------------------------------------------

STGetTrackVU lda #TrackVu
          sta 7,s
          lda #^TrackVu
          sta 9,s
          lda #0
          clc
          rtl

*-------------------------------------------------

STStopMusic jsl StopMusic
b
          ldal ST_OpenFlag
          beq No_Init0
          phd
          ldal ST_ZPage
          tcd
          stz ListPlay_Ptr
          stz ListPlay_Ptr+2
          pld
          lda #0
No_Init0  clc
          rtl

*-------------------------------------------------

StopMusic php
          sei
          rep $30

          lda #0
          stal Performing
          stal OldPerforming
          lda #$FFFF
          stal Current_Music

          sep $30

]lp       ldal SonCTRL
          bmi ]lp
          ldal $E100CA
          and #$0F
          stal SonCTRL

          ldx #0
]loop     ldy #2
]lp       txa
          clc
          adc #$A0
          stal SonREG
          lda #%0000_0011
          stal SonDATA
          txa
          clc
          adc #$A1
          stal SonREG
          lda #%0000_0011
          stal SonDATA
          dey
          bne ]lp
          inx
          inx
          cpx #30
          bne ]loop

          plp
          lda #0
          clc
          rtl

          mx %00

*-------------------------------------------------

STGetEndOfMusic lda #0
Finished_Flag = *-2
          sta 7,s
          lda #0
          clc
          rtl

STGetPlayingMusic lda #0
Current_Music = *-2
          sta 7,s
          lda #0
          clc
          rtl

*-------------------------------------------------

STAddToBatch phb
          phk
          plb
          phd
          lda ST_ZPage
          tcd
          lda ST_OpenFlag
          bne Al4
          lda #Err_STNotInit
          bra Batch_Return

Music_Still_Exist lda #Err_STMusicExist
          bra Batch_Return
Bad_BatchNumber lda #Err_STBatchNumber
          bra Batch_Return

Al4       lda 10,S
          beq Bad_BatchNumber
          cmp #Max_Batch
          bcs Bad_BatchNumber

          dec
          asl
          asl
          asl
          sta BatchPos
          tax
          lda BatchList,x ; Sauvegarde Hdl
          ora BatchList+2,x
          ora BatchList+4,x
          ora BatchList+6,x
          bne Music_Still_Exist

          lda 14,S       ; Music_Path
          tax
          lda 12,S
          jsr ReadFile
          bcs Batch_Return

Patch_DecompMus0 jsl Normal_RTL

          jsr Verify_Music
          bcs Batch_Return

          ldx BatchPos
          lda $00
          sta BatchList,x
          lda $02
          sta BatchList+2,x

          lda 14,S       ; Music_Path
          tax
          lda 12,S
          jsr ReadWave
          bcc No_ErrPath

          ldx BatchPos
          stz BatchList,x
          stz BatchList+2,x
          bra Batch_Return

No_ErrPath = *

Patch_DecompWave0 jsl Normal_RTL

          jsr Verify_Wave
          bcs Batch_Return

          ldx BatchPos
          lda $00
          sta BatchList+4,x
          lda $02
          sta BatchList+6,x

          lda #0
Batch_Return tax
          pld
          plb
          lda 5,S
          sta 11,S
          lda 3,S
          sta 9,S
          lda 1,S
          sta 7,S
          pla
          pla
          pla
          txa
          cmp #1
          rtl

*-------------------------------------------------

ReadWave  stx $0A
          sta $08

          lda [$08]
          and #$00FF
          inc
          tay
          inc
          sep $20
          sta [$08]
          rep $20
          lda [$08],y
          pha
          lda #".W"
          sta [$08],y

          ldx $0A        ; Music_Path
          lda $08
          jsr ReadFile
          tax
          lda [$08]
          and #$00FF
          dec
          tay
          dec
          sep $20
          sta [$08]
          rep $20
          pla
          sta [$08],y
          txa
          cmp #1
          rts

*-------------------------------------------------

STKillBatch phb
          phk
          plb
          phd
          lda ST_ZPage
          tcd

          lda ST_OpenFlag
          bne Al6
          lda #Err_STNotInit
          brl KillBatch_Return

Al6       lda 10,s
          beq GoKill_All

          cmp #Max_Batch
          bcc Maybe_Good1
Bad_BatchNumber1 lda #Err_STBatchNumber
          brl KillBatch_Return
No_BatchMusic1 lda #Err_STNoBatchMusic
          brl KillBatch_Return

GoKill_All brl Kill_All

Maybe_Good1 dec
          asl
          asl
          asl
          sta BatchPos
          tax
          lda BatchList,x ; Sauvegarde Hdl
          ora BatchList+2,x
          ora BatchList+4,x
          ora BatchList+6,x
          beq No_BatchMusic1

          lda BatchList,x
          stz BatchList,x
          sta $00
          lda BatchList+2,x
          stz BatchList+2,x
          sta $02

          lda [$00]
          cmp Music_Ptr
          bne No_CurrentMusic
          ldy #2
          lda [$00],y
          cmp Music_Ptr+2
          bne No_CurrentMusic

          jsl StopMusic

          stz Music_Ptr
          stz Music_Ptr+2

No_CurrentMusic pei $02  ; \a, c'{tait une faute!
          pei $00
          ldx #$1002     ; Dispose Hdl
          jsl $E10000
          _STErr

          ldx BatchPos
          lda BatchList+6,x
          stz BatchList+6,x
          pha
          lda BatchList+4,x
          stz BatchList+4,x
          pha
          ldx #$1002
          jsl $E10000
          _STErr
          bra KillBatch_End

Kill_All  lda Music_Hdl
          ora Music_Hdl+2
          bne OneMusicPlaying

          jsl StopMusic
          stz Music_Ptr
          stz Music_Ptr+2

OneMusicPlaying jsr Dispose_BatchMusic

KillBatch_End lda #0
KillBatch_Return tax
          pld
          plb
          lda 5,s
          sta 7,s
          lda 3,s
          sta 5,s
          lda 1,s
          sta 3,s
          pla
          txa
          cmp #1
          rtl

*-------------------------------------------------

STPlayBatch phb
          phk
          plb
          phd
          lda ST_ZPage
          tcd
          lda ST_OpenFlag
          bne Al7
          lda #Err_STNotInit
          bra PlayBatch_Return

Al7       jsl StopMusic

          lda 10,s
          sta ListPlay_Ptr
          lda 12,s
          sta ListPlay_Ptr+2

          ldy #0
          sty ListPlay_Pos

]lp       lda [ListPlay_Ptr],y
          cmp #0
          beq EndCheckList
          cmp #$FFFF
          beq EndCheckList
          dec
          asl
          asl
          asl
          tax
          lda BatchList,x ; Sauvegarde Hdl
          ora BatchList+2,x
          ora BatchList+4,x
          ora BatchList+6,x
          beq BadList
          iny
          iny
          bra ]lp

BadList   lda #Err_BadPlayList
          bra PlayBatch_Return

EndCheckList cpy #0
          beq NothingInList

          lda [ListPlay_Ptr]
          sta Patch_Current
          pha
          ldx #$0EDD     ; AV
          jsl $E10000
          _STErr
          pea $0000      ; NoLoop
          ldx #$0ADD     ; AV
          jsl $E10000
          _STErr

          inc ListPlay_Pos
          inc ListPlay_Pos

NothingInList lda #0
PlayBatch_Return tax
          pld
          plb
          lda 5,s
          sta 9,s
          lda 3,s
          sta 7,s
          lda 1,s
          sta 5,s
          pla
          pla
          txa
          cmp #1
          rtl

*-------------------------------------------------

STSelectBatch phb
          phk
          plb
          phd
          lda ST_ZPage
          tcd

          lda ST_OpenFlag
          bne Al5
          lda #Err_STNotInit
          bra SelectBatch_Return

Al5       jsl StopMusic

          lda 10,s
          beq Bad_BatchNumber0
          cmp #Max_Batch
          bcc Maybe_Good
Bad_BatchNumber0 lda #Err_STBatchNumber
          bra SelectBatch_Return
No_BatchMusic lda #Err_STNoBatchMusic
          bra SelectBatch_Return

Maybe_Good dec
          asl
          asl
          asl
          sta BatchPos
          tax
          lda BatchList,x ; Sauvegarde Hdl
          ora BatchList+2,x
          ora BatchList+4,x
          ora BatchList+6,x
          beq No_BatchMusic

          lda 10,s
          sta Patch_Current

          jsr Dispose_OneMusic

          ldx BatchPos

          lda BatchList,x
          sta $00
          lda BatchList+2,x
          sta $02

          ldy #2
          lda [$00]
          sta Music_Ptr
          lda [$00],y
          sta Music_Ptr+2

          lda BatchList+4,x
          sta $00
          lda BatchList+6,x
          sta $02

          lda [$00]
          sta Wave_Ptr
          lda [$00],y
          sta Wave_Ptr+2

          jsr Init_Music

          lda #0

SelectBatch_Return tax
          pld
          plb
          lda 5,s
          sta 7,s
          lda 3,s
          sta 5,s
          lda 1,s
          sta 3,s
          pla
          txa
          cmp #1
          rtl

*-------------------------------------------------

STLoadOneMusic phb
          phk
          plb
          phd
          lda ST_ZPage
          tcd

          lda ST_OpenFlag
          bne Al3
          lda #Err_STNotInit
          bra LoadOne_Return

Al3       jsl StopMusic  ; Arrete la music
          jsr Dispose_OneMusic

          stz ListPlay_Ptr
          stz ListPlay_Ptr+2

          lda 12,S       ; Music_Path
          tax
          lda 10,S
          jsr ReadFile
          bcs LoadOne_Return

Patch_DecompMus1 jsl Normal_RTL

          lda $00
          sta Music_Hdl
          lda $02
          sta Music_Hdl+2

          lda $04
          sta Music_Ptr
          lda $06
          sta Music_Ptr+2

          jsr Verify_Music
          bcs LoadOne_Return

          lda 12,S
          tax
          lda 10,S
          jsr ReadWave
          bcs LoadOne_Return

Patch_DecompWave1 jsl Normal_RTL

          JSR Verify_Wave
          bcs LoadOne_Return

          lda $04
          sta Wave_Ptr
          lda $06
          sta Wave_Ptr+2 ; Directement

          stz Patch_Current

          jsr Init_Music

          pei $02        ; Libere la wave
          pei $00
          ldx #$1002
          jsl $E10000
          _STErr
          lda #0
LoadOne_Return tax
          pld
          plb

          lda 5,s
          sta 9,s
          lda 3,s
          sta 7,s
          lda 1,s
          sta 5,s
          pla
          pla
          txa
          cmp #1
          rtl


*-------------------------------------------------

STPlayMusic phb
          phd
          phk
          plb

          lda ST_ZPage
          tcd

          lda ST_OpenFlag
          bne al2
          lda #Err_STNotInit
          brl PlayOne_Return ; AV201506 was BRA

al2       lda Music_Ptr
          ora Music_Ptr+2
          bne Some2
          lda #Err_NoMusInMem
          brl PlayOne_Return ; AV201506 was BRA

Some2     lda 10,s
          sta LoopMode

          stz Timer
          stz Finished_Flag

          ldy #470
          lda [Music_Ptr],y
          and #%0000000011111111
          sta NumberOfBlocks
          beq SkipPlay
          stz NotePlayed
          stz BlockIndex
          iny
          iny
          lda [Music_Ptr],y
          and #%0000000011111111
          asl
          tax
          lda BlockTable,x
          sta NoteIndex
          ldy #8
          lda [Music_Ptr],y
          and #$00FF
          sta Tempo

          ldx #0
          ldy #$2C
]loop     lda [Music_Ptr],y
          sta VolumeTable,x

* AV...
          iny
          iny
          lda [Music_Ptr],y
          and #$ff
          sta Ninjaloop,x
          iny
          lda [Music_Ptr],y
          and #$0f
          phx
          asl
          tax
          lda FineTuneCnvrt,x
          plx
          sta NinjaFineT,x

          tya
          clc
          adc #$1E-3
          tay
          inx
          inx
          cpx #32        ; was 30
          bcc ]loop

* ...AV 201506

          lda #1
          sta Performing
          stz OldPerforming

          lda #0
Patch_Current = *-2
          sta Current_Music

SkipPlay  lda #0

PlayOne_Return tax
          pld
          plb
          lda 5,s
          sta 7,s
          lda 3,s
          sta 5,s
          lda 1,s
          sta 3,s
          pla
          txa
          cmp #1
          rtl

STSetDecompactMus lda 7,s
          stal Patch_DecompMus0+1
          stal Patch_DecompMus1+1
          lda 9,s
          sep $20
          stal Patch_DecompMus0+3
          stal Patch_DecompMus1+3
          rep $20
          lda 5,s
          sta 9,s
          lda 3,s
          sta 7,s
          lda 1,s
          sta 5,s
          pla
          pla
          lda #0
          clc
          rtl

STSetDecompactWave lda 7,s
          stal Patch_DecompWave0+1
          stal Patch_DecompWave1+1
          lda 9,s
          sep $20
          stal Patch_DecompWave0+3
          stal Patch_DecompWave1+3
          rep $20
          lda 5,s
          sta 9,s
          lda 3,s
          sta 7,s
          lda 1,s
          sta 5,s
          pla
          pla
          lda #0
          clc
Normal_RTL rtl

*------------------------------------------------- Sous Routines

          MX %00
Verify_Music ldy #2
          lda [$04]
          cmp #'SO'
          bne NotASoundsmithMusic
          lda [$04],y
          cmp #'NG'
          bne NotASoundsmithMusic
          iny
          iny
          lda [$04],y
          cmp #'OK'
          beq Yes_Itis
NotASoundsmithMusic lda #Err_BadMusic
          sec
          hex 24
Yes_Itis  clc
          rts

Verify_Wave lda [$04]    ; Verification tres sommaire
          cmp #$0010     ; (1er Octet du fichier= Nombre d'instruments...)
          bcs NotASoundsmithWave
          rts
NotASoundsmithWave lda #Err_BadWave
          rts            ; retenue bien positionnee

          mx %00
Dispose_OneMusic lda Music_Hdl
          ora Music_Hdl+2
          beq No_OldOneMusic
          pei Music_Hdl+2
          pei Music_Hdl
          ldx #$1002
          jsl $E10000
          _STErr
          stz Music_Hdl
          stz Music_Hdl+2
No_OldOneMusic rts

          mx %00
Dispose_BatchMusic = *
          ldx #BatchList
]loop     phx
          lda $00,x
          ora $02,x
          beq No_Music
          lda $02,x
          pha
          lda $00,x
          pha
          stz $00,x
          stz $02,x
          ldx #$1002
          jsl $E10000    ; Dispose Hdl
          _STErr
No_Music  pla
          clc
          adc #4
          tax
          cmp #$0100
          bcc ]loop
          rts


          MX %00
ReadFile  sta Parm0
          stx Parm0+2
          jsl $E100A8
          dw $10
          adrl Params
          bcc No_ErrOpen
          rts
No_ErrOpen jsl $E100A8
          dw $19
          adrl Params
          bcc No_ErrGetEof
          pha
          jsr Go_Close
          pla
          sec
          rts
No_ErrGetEof pha
          pha
          lda Parm0+2
          sta Parm1+2
          pha
          lda Parm0
          sta Parm1
          pha
          lda UserId
          pha
          pea $C000
          pea $0
          pea $0
          ldx #$0902     ; NewHandle
          jsl $E10000
          _STErr
          pla
          sta $00
          pla
          sta $02
          ldy #2
          lda [$00],Y
          sta Parm0+2
          tax
          lda [$00]
          sta Parm0
          sta $04
          stx $06
          jsl $E100A8
          dw $12         ; Read
          adrl Params
          bcc Go_Close
          pha
          pei $02        ; Libere la wave
          pei $00
          ldx #$1002
          jsl $E10000
          _STErr
          jsr Go_Close
          pla
          sec
          rts
Go_Close  jsl $E100A8
          dw $14         ; Close
          adrl Params
          rts

Params    dw 0
Parm0     adrl 0
Parm1     adrl 0
          adrl 0

Init_Music sep $30       ; D et B bien positionne

          lda #%01100000
          stal SonCTRL
          lda #0
          stal $E1C03E
          stal $E1C03F

          rep #$30

          ldx #0
]lp       stz Clear_Deb,X
          inx
          inx
          cpx #Clear_End-Clear_Deb
          bcc ]lp

          lda [Wave_Ptr]
          and #$00FF
          sta InstIndex

          lda Wave_Ptr
          clc
          adc #2
          sta Wave_Ptr
          lda Wave_Ptr+2
          adc #0
          sta Wave_Ptr+2

          sep $20

          ldy #2
]lp       lda [Wave_Ptr],y
          stal SonDATA
          iny
          bne ]lp

          rep $20

* Sauve la stereo pour chaque track.

          lda Music_Ptr+2
          sta Buf_Ptr+2

          ldy #6
          lda [Music_Ptr],y
          asl
          bcc no_bk
          inc Buf_Ptr+2
no_bk     clc
          adc [Music_Ptr],y
          bcc no_bk0
          inc Buf_Ptr+2
no_bk0    clc
          adc #600
          bcc no_bk1
          inc Buf_Ptr+2
no_bk1    clc
          adc Music_Ptr
          sta Buf_Ptr
          bcc no_bk2
          inc Buf_Ptr+2

* BD - 4soniq
* $0000 > $0000: from right to left channel
* $FFFF > $0001: from left to right channel

no_bk2    ldy #$1E
]loop     lda [Buf_Ptr],y
          bpl no_bk3
          lda #1
no_bk3    sta StereoTable,y
          dey
          dey
          bpl ]loop

* Move les parametres des instruments...

          ldx #0
          ldy #0

          lda Wave_Ptr
          clc
          adc #$20
          sta Buf_Ptr
          lda Wave_Ptr+2
          adc #1
          sta Buf_Ptr+2

          lda InstIndex
]loopaga  pha

          lda #6
]loop     pha
          lda [Buf_Ptr],y
          sta instdef,x
          iny
          iny
          inx
          inx
          pla
          dec
          bne ]loop

          tya
          clc
          adc #$5C-12
          tay

          pla
          dec
          bne ]loopaga

          tya
          clc
          adc #$5E-$20
          tay
          ldx #0
]loop     lda [Buf_Ptr],y
          sta CompactTable,x
          iny
          iny
          inx
          inx
          cpx #$20
          bcc ]loop

          sep $20
          lda Music_Ptr+2
          sta Effects1+3
          sta Effects2+3
          sta Patch_Music+3
          rep $20

          ldy #6
          lda Music_Ptr
          clc
          adc #600
          sta Patch_Music+1
          bcc No_Dep0
          inc Patch_Music+3
          inc Effects1+3
          inc Effects2+3
No_Dep0   clc
          clc
          adc [Music_Ptr],y
          sta Effects1+1
          bcc No_Dep1
          inc Effects1+3
          inc Effects2+3
No_Dep1   clc
          adc [Music_Ptr],y
          sta Effects2+1
          bcc No_Dep2
          inc Effects2+3
No_Dep2   rts

*------------------------------------------------- Tool221

          put NTPlayer
          put NTData

* End of code (AV)
