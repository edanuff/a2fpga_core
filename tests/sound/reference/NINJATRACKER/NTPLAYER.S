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
* Some changes by
* Brutal Deluxe Software, 2015
* Visit brutaldeluxe.fr
*
* 4soniq support by
* Brutal Deluxe Software, 2016
*

Reference_Freq = $00F0   ; Interrupt Frequence
Nb_Track  = 14           ; Nb defined Tracks
Nb_PlayedTrack = 8       ; Nb played Tracks

*

          mx %00

Get_Effects1 = *
Effects1  ldal 0,X
          rts

Get_Effects2 = *
Effects2  ldal 0,X
          rts

          mx %11

SoundIRQrtn = *

          php            ; AV..
          phd
          phb
          phk
          plb

          rep #$30
          lda ST_ZPage
          tcd

          sep #$30       ; ..AV 201506

          ldal SonCTRL
          and #%10011111 ; Disable auto-inc. and access DOC reg.
          stal SonCTRL

          lda #$E0
          stal SonREG    ; On lit le registre d'interruptions
          ldal SonDATA   ; pour savoir quel osc. a genere
          ldal SonDATA   ; l'interruption.

          and #%00111110
          lsr
          cmp #$1E
          beq TimerInterrupt ; c'est l'interruption 50Hz.

          sta OscNumber
          lsr            ;Osc*2  (of Track which generated the int.)
                         ; dec
          asl
          tax

          lda #0
          sep #$20

]lp       ldal SonCTRL
          bmi ]lp
          ora #%00100000 ; Auto-incrementation
          and #%10111111
          stal SonCTRL

          lda OscNumber
          clc
          adc #$80
          stal SonREG
          lda NinjaWaveAdr,x
          stal SonDATA   ; Wave Adress pair
          lda OscNumber
          clc
          adc #$C0
          stal SonREG
          lda NinjaWaveSiz,x
          stal SonDATA   ; Wave Size pair
          lda OscNumber
          clc
          adc #$A0
          stal SonREG
          lda NinjaDocReg,x
          and #%1111_0111 ; int. off
          stal SonDATA   ; Control register pair

          rep #$20
          brl EndInterrupt


TimerInterrupt lda Performing ; Les interruptions 50Hz etant generees
          bne WeCanPlay  ; en permanance, on utilise Performing
          jmp EndInterrupt ; pour savoir si on doit jouer les notes

WeCanPlay stz Temporary  ; compteur contenant le numero du track
                         ;                                   courant

          inc Timer
          lda Timer
          cmp Tempo      ; on joue les notes lorsque Timer=Tempo
          beq PlayTracks
          jmp HandleEffects ; les effets sont mis a jour tous les
                         ;                                   1/50 de secondes


PlayTracks stz Timer     ; remise a zero du Timer

NewTrack  rep #$30
          sep #$20

          ldx NoteIndex
Patch_Music ldal $000000,x ; AV Lit la note a jouer
*mpatch011 ldal Music_File+600,x ; Lit la note a jouer

          rep #$20

          and #$7f
                         ; cmp #128 ; Si la note estl 128, c'est une
          bra NoteFound  ; commande

NotValid  inc NoteIndex

NotSTP    = *
NotACommand sep #$20

          inc Temporary  ; On passe au track suivant.
          lda Temporary
          cmp #Nb_PlayedTrack
          beq NextTrack
          jmp NewTrack

          mx %00
NextTrack rep #$20

          jmp EndPlay

NoteFound sta Semitone   ; On sauvegarde la note lue
          cmp #0
          beq DontSave0
          lda Temporary
          asl
          tay
          lda Semitone
          sta SemitoneTbl,y

DontSave0 sep #$20
          jsr Get_Effects1 ; si 0
          ldy Temporary  ; que l'on doit reutiliser le dernier
          and #$f0       ; sample joue.
          sta curr_instnum
          bne ThereIsASample
          lda SampleTable,y
ThereIsASample sta SampleTable,y ; Sinon, on sauve le numero du
          lsr            ; sample.
          lsr
          lsr
          lsr
          dec
          asl
          tay
          lda VolumeTable,y
          lsr
                         ; and #$7f
          sta VolumeInt
          rep #$20
          lda NinjaFineT,y
          sta FineTune

          lda Temporary
          asl
          tay
          lda FineTune   ; Save Finetune for this track
          sta TrackTune,y

          jsr Get_Effects1
          and #$f

          bne NotArpegiatto ; Si l'effet est 0, c'est peut-etre
          jsr Get_Effects2
          and #$ff
          beq NttArpegiatto

          lda Temporary
          asl
          tay
          lda SemitoneTbl,y
          sta Semitone_

          sep #$20
          ldy Temporary  ; set arp.
          lda #2
          sta ArpegiattoTbl,y
          lda Temporary
          asl
          clc
          adc Temporary
          tay
          lda Semitone_
          sta ArpegeToneTbl,y
          jsr Get_Effects2
          pha
          and #$f0
          lsr
          lsr
          lsr
          lsr
          clc
          adc Semitone_
          cmp #60
          blt arp_toobig1
          lda #60-1
arp_toobig1 sta ArpegeToneTbl+1,y
          pla
          and #$f
          clc
          adc Semitone_
          cmp #60
          blt arp_toobig2
          lda #60-1
arp_toobig2 sta ArpegeToneTbl+2,y
          rep #$20
          jmp clear_eff  ; Fin de la preparation de l'Arpegiatto

NttArpegiatto sep #$20
          ldy Temporary
          lda #0
          sta ArpegiattoTbl,y
          rep #$20
          jmp clear_eff

NotArpegiatto = *
          pha

          sep #$20

          ldy Temporary  ; Il faut arreter l'effet d'Arpegiatto
          lda #0
          sta ArpegiattoTbl,y

          rep #$20

          lda Temporary  ;set to no fineslide as default
          asl
          tay
          lda #0
          sta Fineslider,y

          lda 1,s
          asl
          phx
          tax
          lda jumptbl-2,x
          sta the_jump+1
          plx
          pla
the_jump  jmp jumptbl

nin_1     dw 0
nin_next_pos dw 0

jumptbl   dw the_tonePup
          dw the_tonePdwn
          dw the_toneP
          dw the_vibrato
          dw the_tonePvols
          dw the_vibravols
          dw clear_eff   ;7
          dw clear_eff   ;8
          dw clear_eff   ;9
          dw the_volslide
          dw the_posjump
          dw the_setvol
          dw the_pattbreak
          dw e_effects
          dw the_setspeed

jumptbl2  dw 0
          dw e_finslideup
          dw e_finslidedwn
          dw 0
          dw 0
          dw 0
          dw 0
          dw 0
          dw 0
          dw 0
          dw e_finvslidup
          dw e_finvsliddwn
          dw 0
          dw 0
          dw 0
          dw 0

e_effects = *            ;e-commands jumper
          jsr Get_Effects2
          and #$ff
          pha
          and #$f0
          lsr
          lsr
          lsr
          tax
          lda jumptbl2,x
          beq e_undefined
          sta the_jump2+1
          pla
          and #$f
the_jump2 jmp the_jump2
e_undefined pla
          jmp clear_eff

e_finvslidup = *         ;slide up only once (Volume)
          pha
          lda Temporary
          asl
          tax
          stz IncFreqTbl,x
          stz NinVslidedwn,x
          pla
          sta NinVslideup,x
          lda #1
          sta Fineslider,y
          jmp NoTempoChange2
e_finvsliddwn = *        ;slide up only once (Volume)
          pha
          lda Temporary
          asl
          tax
          stz IncFreqTbl,x
          stz NinVslideup,x
          pla
          sta NinVslidedwn,x
          lda #1
          sta Fineslider,y
          jmp NoTempoChange2

e_finslidPR dw 0
e_finslidedwn = *        ;slide down only once (Portamento)
          eor #$ffff
          inc
e_finslideup = *         ;slide up only once
          sta e_finslidPR
          lda #0
          sta NinVslideup,y
          sta NinVslidedwn,y
          lda e_finslidPR
          sta IncFreqTbl,y
          lda #1
          sta Fineslider,y
          jmp NoTempoChange2

the_vibrato jsr Get_Effects2 ;Vibrato (sine only)
                         ; stz vibra_tbl
                         ; stz vibra_speed
          and #$ff
          beq the_vib_keep
          pha
          and #$f
          beq the_vibra1
          asl
          tay
          lda vib_tab,y
          sta vibra_tbl
the_vibra1 pla
          and #$f0
          lsr
          lsr
          lsr
          sta vibra_speed
          lda Temporary
          asl
          tay
          lda vibra_tbl
          beq *+5
          sta Vibrato_Tbl,y
          sta Vibrato_Tbl_,y
          lda vibra_speed
          beq *+5
          sta VibratoAdd_Tbl,y
          lda #0
          sta VibratoPtr_Tbl,y
          sta NinVslideup,y
          sta NinVslidedwn,y
          sta IncFreqTbl,y
          brl NoTempoChange2
the_vib_keep lda Vibrato_Tbl_,y
          sta Vibrato_Tbl,y
          lda Temporary
          asl
          tay
          lda #0
          sta NinVslideup,y
          sta NinVslidedwn,y
          sta IncFreqTbl,y
          brl NoTempoChange2

vibra_tbl dw 0
vibra_speed dw 0


the_toneP jsr Get_Effects2 ;Tone Portamento, slide to another note

          pha
          lda Temporary
          asl
          tay
          pla

          and #$ff
          bne the_toneP5
          lda ninTonePsp,y
the_toneP5 asl
          sta ninTonePsp,y

          lda Semitone
          bne the_toneP4
          lda ninTonePsp,y
          beq no_toneP
                         ; lda StartFreqTbl,y
                         ; cmp EndFreqTbl,y
                         ; beq no_toneP
                         ; lda ninTonePsp,y
          ldx IncFreqTbl,y
          bmi the_toneP3
          bra the_toneP1
the_toneP4 stz Semitone
the_toneP2 asl
          tax
          lda Temporary
          asl
          tay
          lda ZeroTunOffset,x
          clc
          adc FineTune   ; Add the Instrument's FineTune
          sta EndFreqTbl,y
          cmp StartFreqTbl,y
                         ; beq no_toneP
          lda ninTonePsp,y
          bge the_toneP1
the_toneP3 eor #$ffff
          inc
the_toneP1 sta IncFreqTbl,y
no_toneP  lda #0
          sta NinVslidedwn,y
          sta NinVslideup,y
          sta Vibrato_Tbl,y
          brl NoTempoChange2

ninTonePspX dw 0
the_tonePup = *          ;max. $269
          jsr Get_Effects2 ;Tone Portamento, slide up
          and #$ff
          asl
          sta ninTonePspX
          lda #5*12

the_tonePup1 asl
          tax
          lda Temporary
          asl
          tay
          lda ZeroTunOffset,x
          clc
          adc FineTune   ; Add the Instrument's FineTune
          sta EndFreqTbl,y
          cmp StartFreqTbl,y
          lda ninTonePspX
          bge the_toneP1X
          eor #$ffff
          inc
the_toneP1X sta IncFreqTbl,y
          lda #0
          sta NinVslidedwn,y
          sta NinVslideup,y
          sta Vibrato_Tbl,y
          brl NoTempoChange2


the_tonePdwn = *         ;min. $52
          jsr Get_Effects2 ;Tone Portamento, slide down
          and #$ff
          asl
          sta ninTonePspX
          lda #2*12
          bra the_tonePup1


the_setvol JSR Get_Effects2 ;Change play volume
          and #$ff
          asl
          cmp #$81
          blt the_setvol_1
          lda #$80
the_setvol_1 sta VolumeInt

ChangeVol lda Semitone
          beq ChangeIt
          jmp clear_eff

ChangeIt  lda Temporary
          asl
          sta OscNumber

          sep #$20

]lp       ldal SonCTRL
          bmi ]lp
          ora #%00100000 ; Auto-incrementation
          and #%10111111
          stal SonCTRL

          lda OscNumber
          clc
          adc #$40
          stal SonREG
          ldx VolumeInt
          lda VolumeConversion,x
          stal SonDATA   ; Volume pair
          stal SonDATA   ; Volume impair

          rep #$20

          brl clear_eff


the_volslide jsr Get_Effects2 ; slide volume
          pha
          lda Temporary
          asl
          tay
          lda #0
          sta IncFreqTbl,y
          sta Vibrato_Tbl,y
          pla
          and #$ff
          beq volslide_2
          sta nin_1
          and #$f
          beq volslide_1
          asl
          sta nin_1
          lda Temporary
          asl
          tay
          lda nin_1
          sta NinVslidedwn,y
          lda #0
          sta NinVslideup,y
volslide_2 brl NoTempoChange2
volslide_1 lda nin_1
          and #$f0
          lsr
          lsr
          lsr
          sta nin_1
          lda Temporary
          asl
          tay
          lda nin_1
          sta NinVslideup,y
          lda #0
          sta NinVslidedwn,y
          brl NoTempoChange2

the_tonePvols jsr Get_Effects2 ; slide volume during Portamento
          pha
          lda Temporary
          asl
          tay
          lda #0
          sta Vibrato_Tbl,y
          pla
          and #$ff
          beq Pvolslide_2
          sta nin_1
          and #$f
          beq Pvolslide_1
          asl
          sta nin_1
          lda Temporary
          asl
          tay
          lda nin_1
          sta NinVslidedwn,y
          lda #0
          sta NinVslideup,y
Pvolslide_2 brl NoTempoChange2
Pvolslide_1 lda nin_1
          and #$f0
          lsr
          lsr
          lsr
          sta nin_1
          lda Temporary
          asl
          tay
          lda nin_1
          sta NinVslideup,y
          lda #0
          sta NinVslidedwn,y
          brl NoTempoChange2


the_vibravols jsr Get_Effects2 ; slide volume during Vibrato
          pha
          lda Temporary
          asl
          tay
          lda #0
          sta IncFreqTbl,y
          pla
          and #$ff
          beq Vvolslide_2
          sta nin_1
          and #$f
          beq Vvolslide_1
          asl
          sta nin_1
          lda Temporary
          asl
          tay
          lda nin_1
          sta NinVslidedwn,y
          lda #0
          sta NinVslideup,y
Vvolslide_2 brl NoTempoChange2
Vvolslide_1 lda nin_1
          and #$f0
          lsr
          lsr
          lsr
          sta nin_1
          lda Temporary
          asl
          tay
          lda nin_1
          sta NinVslideup,y
          lda #0
          sta NinVslidedwn,y
          brl NoTempoChange2


the_setspeed jsr Get_Effects2 ; Le tempo est code sur un nibble
          and #$f
          beq clear_eff
          sta Tempo
clear_eff lda Temporary
          asl
          tay
          lda #0
          sta NinVslideup,y
          sta NinVslidedwn,y
          sta IncFreqTbl,y
          sta Vibrato_Tbl,y
          brl NoTempoChange2

the_pattbrk dw 0
the_pattbreak jsr Get_Effects2
          and #$ff
          cmp #64
          bge the_pattb2
          asl
          sta the_pattbrk
          asl
          asl
          asl
          sec
          sbc the_pattbrk
          sta nin_next_pos
the_pattb2 lda #63
          sta NotePlayed
          bra clear_eff

the_posjump jsr Get_Effects2
          and #$ff
          dec
          sta BlockIndex
          lda #63
          sta NotePlayed
          bra clear_eff

NoTempoChange2 = *
          lda Semitone
          bne NoTempoChange

          lda curr_instnum
          beq NoTempoChange3

          lda Temporary
          asl
          tax
          lda VolumeInt
          sta TrueVolumeTbl,x
          stx OscNumber

          sep #$20

]lp       ldal SonCTRL
          bmi ]lp
          ora #%00100000 ; Auto-incrementation
          and #%10111111
          stal SonCTRL

          lda OscNumber
          clc
          adc #$40
          stal SonREG
          ldx VolumeInt
          lda VolumeConversion,x
          stal SonDATA   ; Volume pair
          stal SonDATA   ; Volume impair

          rep #$20

NoTempoChange3 inc NoteIndex
          jmp NotSTP

NoTempoChange lda Temporary
          asl
          tax
          lda VolumeInt
          sta TrueVolumeTbl,x

          lda Semitone
          bne PlayIt

          inc NoteIndex
          jmp NotSTP

PlayIt    lda Temporary  ; La paire 0-1 d'oscillos etant utilisee
                         ; inc  ; pour generer les interruptions, le
          asl            ; track 0 utilise la paire 2-3, etc.
          sta OscNumber

          sep #$20

          ldal SonCTRL
          and #%10011111
          stal SonCTRL

          lda OscNumber
          clc
          adc #$A0
          stal SonREG
          ldal SonDATA
          ldal SonDATA
          and #%11110111
          ora #%00000001
          stal SonDATA   ; Arrete l'oscillateur pair
          lda OscNumber
          clc
          adc #$A1
          stal SonREG
          ldal SonDATA
          ldal SonDATA
          and #%11110111
          ora #%00000001
          stal SonDATA   ; Arrete l'oscillateur impair

          ldy Temporary
          lda SampleTable,y

          rep #$20

          and #%0000000011110000
          lsr
          lsr
          lsr
          lsr
          dec
          cmp InstIndex
          bcc SampleExists
          jmp IgnoreSample
SampleExists sta CurrInstInt
          asl
          tax
          lda InstIndexTable,x ; Offset du debut de la definition de inst.
          tax

                         ; removed code: we know that we always have only one wavelist, so we take
                         ; the one that's there.

          stx IndexInterrupt
          lda Instrument1+1,x ; On lit la taille et l'adresse de la
          sta TempInterrupt ; wave pour l'osc. pair
          lda Instrument1+3,x ; On lit le mode a utiliser pour l'osc.
          and #$ff       ; pair
          sta Temp2Interrupt

          lda StereoMode ; Si StereoMode vaut zero, on utilise
          beq StereoOk   ; le Mode de l'osc. pour la stereo.

          lda Temp2Interrupt ; Sinon on utilise la table StereoTable
          and #$f        ; pour determiner si le son doit sortir
          sta Temp2Interrupt ; a droite ou a gauche.

          lda Temporary  ; (0=droite $FFFF=gauche)
          asl
          tax
          lda StereoTable,x
* BD 2016 - 4soniq support
* beq StereoOk
* lda Temp2Interrupt
* ora #%0000000000010000
          asl
          asl
          asl
          asl
          ora Temp2Interrupt
          sta Temp2Interrupt

StereoOk  ldx IndexInterrupt ;same for Osc. B
          lda Instrument1+7,x
          sta Temp3Interrupt
          lda Instrument1+9,x
          and #%0000000011111111
          sta Temp4Interrupt

          lda StereoMode
          beq StereoOk2

          lda Temp4Interrupt
          and #%0000000000001111
          sta Temp4Interrupt

          lda Temporary
          asl
          tax
          lda StereoTable,x
* BD 2016 - 4soniq support
* beq StereoOk2
* lda Temp4Interrupt
* ora #%0000000000010000
          asl
          asl
          asl
          asl
          ora Temp4Interrupt
          sta Temp4Interrupt

StereoOk2 = *
          lda Temporary
          asl
          tax
          stz SwapperMode,x

          lda CurrInstInt
          asl
          tax
          lda Ninjaloop,x
          beq conv_note  ;inst is not looped

          cmp #3
          bne swapmode3
          lda Temp2Interrupt
          and #%1111_0000
          ora #%0000_0110
          sta Temp2Interrupt
          lda Temp4Interrupt
          and #%1111_0000
          ora #%0000_0111 ;set swap mode
          sta Temp4Interrupt
          bra swapmode2

swapmode3 ldx IndexInterrupt
          lda Instrument1+7+12,x ;copy wave pos. and len.
          sta Temp3Interrupt
          lda Instrument1+9+12,x ;get doc mode
          and #%1111_0000
          ora #%0000_0111 ;set swap mode
          sta Temp4Interrupt
          lda Temp2Interrupt
          ora #%0000_1000
          sta Temp2Interrupt

swapmode2 lda StereoMode
          beq conv_note

          lda Temp4Interrupt
* BD 2016
* and #%0000000000011111
          and #%0000000000001111
          sta Temp4Interrupt

          lda Temporary
          asl
          tax
          lda StereoTable,x
* BD 2016 - 4soniq support
* beq conv_note
* lda Temp4Interrupt
* ora #%0000000000010000
          asl
          asl
          asl
          asl
          ora Temp4Interrupt
          sta Temp4Interrupt

          lda Temporary
          asl
          tax
          inc SwapperMode,x

conv_note lda Semitone   ; On convertit un semitone en une
          asl            ; frequence comprehensible pour le
          tax            ; DOC.
          lda Temporary  ; Get position in big Finetune Table
          asl            ; so that we can easily do portamento
          tay
          lda ZeroTunOffset,x
          clc
          adc FineTune   ; Add the Instrument's FineTune
          sta StartFreqTbl,y
          tax
          lda FineTuneTbl,x
          sta TempFreqInt

          lda CurrInstInt
          asl
          tax
          lda CompactTable,x
          tax
bcleFreq  cpx #0
          beq EndBcleFreq
          lsr TempFreqInt
          dex
          bra bcleFreq

* AV 201506 - Useful for VU meters

EndBcleFreq = *

          lda Temporary
          asl
          tay
          lda TrackVu,y
          lsr            ; /2
          cmp VolumeInt
          bcs No_Track

          lda VolumeInt
          asl
          cmp #$0100
          bcc No_DepTrack
          lda #$00FF
No_DepTrack sta TrackVu,y

No_Track  = *
          lda CurrInstInt
          asl
          tay
          lda InstVu,y
          lsr            ; /2
          cmp VolumeInt
          bcs No_Inst
          lda VolumeInt
          asl
          cmp #$0100
          bcc No_DepInst
          lda #$00FF
No_DepInst sta InstVu,y
No_Inst   = *

* ..AV 201506

No_Voice  lda #0
          sep #$20


]lp       ldal SonCTRL
          bmi ]lp
          ora #%00100000 ; Auto-incrementation
          and #%10111111
          stal SonCTRL

          lda OscNumber
          stal SonREG
          lda TempFreqInt
          stal SonDATA   ; Frequency low pair
          stal SonDATA   ; Frequency low impair
          lda OscNumber
          clc
          adc #$20
          stal SonREG
          lda TempFreqInt+1
          stal SonDATA   ; Frequency high pair
          stal SonDATA   ; Frequency high impair
          lda OscNumber
          clc
          adc #$40
          stal SonREG
          ldx VolumeInt
          lda VolumeConversion,x
          stal SonDATA   ; Volume pair
          stal SonDATA   ; Volume impair
          lda OscNumber
          clc
          adc #$80
          stal SonREG
          lda TempInterrupt
          stal SonDATA   ; Wave Adress pair
          lda Temp3Interrupt
          stal SonDATA   ; Wave Adress impair
          lda OscNumber
          clc
          adc #$C0
          stal SonREG
          lda TempInterrupt+1
          stal SonDATA   ; Wave Size pair
          lda Temp3Interrupt+1
          stal SonDATA   ; Wave Size impair
          lda OscNumber
          clc
          adc #$A0
          stal SonREG
          lda Temp2Interrupt
          stal SonDATA   ; Control register pair
          lda Temporary
          asl
          tax
          lda SwapperMode,x
          eor #1
          ora Temp4Interrupt
          stal SonDATA   ; Control register impair

          lda CurrInstInt ; if sample is looped, backup
          asl            ; place and length of loop, and control
          tax
          lda Ninjaloop,x
          beq IgnoreSample
          lda Temporary
          asl
          tax
          lda Temp3Interrupt
          sta NinjaWaveAdr,x
          lda Temp3Interrupt+1
          sta NinjaWaveSiz,x
          lda Temp4Interrupt
          sta NinjaDocReg,x

IgnoreSample rep #$20

          inc NoteIndex  ; C'est fini, on passe au track
          inc Temporary  ; suivant...
          lda Temporary
          cmp #Nb_PlayedTrack
          beq EndPlay

          jmp NewTrack

EndPlay   = *
          REP #$20

                         ; DO Nb_PlayedTrack-Nb_Track
          LDA NoteIndex
          CLC
          ADC #Nb_Track-Nb_PlayedTrack
          STA NoteIndex
                         ; FIN

          inc NotePlayed ; Si la position de la ligne jouee
          lda NotePlayed ; vaut 64, on doit lire un nouveau
          cmp #64        ; block
          bge ReadNewBlock
          jmp EndInterrupt

ReadNewBlock stz NotePlayed
          inc BlockIndex ; On verifie si on n'a pas fini
* ldx BlockIndex
* cpx NumberOfBlocks
* bge Finished
          lda BlockIndex ; AV...
          cmp NumberOfBlocks
          bge Finished
          clc
          adc #472
          tay
          lda [Music_Ptr],y ; ...AV
*mpatch012 ldal Music_File+472,x ; Sinon, on cherche le numero du
          and #$ff       ; block a jouer
          sta BlockPlayed
          asl
          tax
          lda BlockTable,x ; et on actualise NoteIndex en fonction
          clc
          adc nin_next_pos ; (+add pos. in next blk.)
          sta NoteIndex  ; du block a jouer.
          stz nin_next_pos
          bra EndInterrupt

Finished  stz Performing ; Si on a fini, on met Performing a
                         ; zero
* AV 201506
* Old code logic restored

          lda LoopMode
          beq NoLoop

SameMusic stz Timer

          ldy #470
          lda [Music_Ptr],y
          and #%0000000011111111
          sta NumberOfBlocks

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

          ldx #0         ; AV 201506: Should we restore
          ldy #$2C       ; the loop instruments as well?
]loop     lda [Music_Ptr],y
          sta VolumeTable,x
          tya
          clc
          adc #$1E
          tay
          inx
          inx
          cpx #32
          bcc ]loop

          lda #1
          sta Performing
          stz OldPerforming

EndInterrupt sep $30
          pld
          plb
          plp
          clc
          rtl

          mx %00

NoLoop    lda ListPlay_Ptr
          ora ListPlay_Ptr+2
          beq NoNewMusic

]lp       ldy ListPlay_Pos
          lda [ListPlay_Ptr],y
          beq NoNewMusic ; EndList
          bpl Get_NewMusic
          stz ListPlay_Pos ; debut Liste
          bra ]lp

Get_NewMusic inc ListPlay_Pos
          inc ListPlay_Pos

          cmp Current_Music
          beq SameMusic

          pha
          sta Patch_Current
          ldx #$0EDD
          jsl $E10000
          _STErr
          pea $0000      ; NoLoop
          ldx #$0ADD
          jsl $E10000
          _STErr

          bra EndInterrupt

NoNewMusic stz ListPlay_Ptr
          stz ListPlay_Ptr+2
          jsr StopMusic
          bra EndInterrupt

*---

HandleEffects sep #$30   ; Gestion des effets

          stz Temporary
bcleHandleArp ldx Temporary

          lda ArpegiattoTbl,x
          bne *+5
          jmp NoArpegiatto
          cmp #4
          bne *+3
          lsr
          clc
          adc Temporary
          adc Temporary
          adc Temporary
          tay
          lda Temporary
          asl
          tax
arp_get_freq lda ArpegeToneTbl-1,y

          mx %00         ; Merlin 32 not picking this up?
          rep #$30
          and #$ff
          asl
          tay
          lda ZeroTunOffset,y
          clc
          adc TrackTune,x ; Add the Track's FineTune
          bpl *+5
          lda #0
          cmp #$600
          blt *+5
          lda #$600-2
          tay
          lda FineTuneTbl,y
          sta ninfreqInt

          ldx Temporary
          sep #$20
          inc ArpegiattoTbl,x
          lda ArpegiattoTbl,x
          cmp #5
          blt arp_maker_1
          lda #1
          sta ArpegiattoTbl,x
arp_maker_1 = *

          lda Temporary
          asl
          sta OscNumber

]lp       ldal SonCTRL   ; et on modifie les Frequency registers
          bmi ]lp
          ora #%00100000
          and #%10111111
          stal SonCTRL

          lda OscNumber
          stal SonREG
          lda ninfreqInt
          stal SonDATA   ; Frequency low pair
          stal SonDATA   ; Frequency low impair
          lda OscNumber
          clc
          adc #$20
          stal SonREG
          lda ninfreqInt+1
          stal SonDATA   ; Frequency high pair
          stal SonDATA   ; Frequency high impair

          jmp NoVolSlide

NoArpegiatto rep #$30    ;Pitch up/down, (=Portamento, ToneP)

          lda Temporary
          asl
          tax
          lda IncFreqTbl,x
          bne *+5
          jmp NoPitch
                         ; lda Fineslider,x
                         ; beq no_finesl3
                         ; lda Temporary
                         ; bne NoPitch
no_finesl3 lda IncFreqTbl,x
          clc
          adc StartFreqTbl,x
          sta StartFreqTbl,x

          lda IncFreqTbl,x
          bmi tonepslide1

          lda StartFreqTbl,x
          cmp EndFreqTbl,x
          blt tonepslide2
          lda EndFreqTbl,x
          sta StartFreqTbl,x
          stz IncFreqTbl,x
          bra tonepslide2

tonepslide1 lda StartFreqTbl,x
          bmi tonepslide3
          cmp EndFreqTbl,x
          beq *+4
          bge tonepslide2
tonepslide3 lda EndFreqTbl,x
          sta StartFreqTbl,x
          stz IncFreqTbl,x

tonepslide2 = *

          lda StartFreqTbl,x
          tax
          lda FineTuneTbl,x
          sta ninfreqInt

          sep #$20

          lda Temporary
          asl
          sta OscNumber

]lp       ldal SonCTRL   ; et on modifie les Frequency registers
          bmi ]lp
          ora #%00100000
          and #%10111111
          stal SonCTRL

          lda OscNumber
          stal SonREG
          lda ninfreqInt
          stal SonDATA   ; Frequency low pair
          stal SonDATA   ; Frequency low impair
          lda OscNumber
          clc
          adc #$20
          stal SonREG
          lda ninfreqInt+1
          stal SonDATA   ; Frequency high pair
          stal SonDATA   ; Frequency high impair

          jmp NoVibrato

          mx %00
NoPitch   = *            ;Handle Vibrato

          lda Temporary
          asl
          tax
          lda Vibrato_Tbl,x
          beq NoVibrato
          sta vibrato_lokup
          ldy VibratoPtr_Tbl,x
          lda StartFreqTbl,x
          clc
vibrato_lokup = *+1
          adc vib0,y
          bpl *+5
          lda #0
          cmp #$600
          blt *+5
          lda #$600-2
          tay
          lda FineTuneTbl,y
          sta ninfreqInt

          lda VibratoAdd_Tbl,x
          clc
          adc VibratoPtr_Tbl,x
          cmp #32*2
          blt vibrato_ovli
                         ; sec
                         ; sbc #32*2
          lda #0
vibrato_ovli sta VibratoPtr_Tbl,x

          sep #$20

          lda Temporary
          asl
          tax
          sta OscNumber

]lp       ldal SonCTRL   ; et on modifie les Frequency registers
          bmi ]lp
          ora #%00100000
          and #%10111111
          stal SonCTRL

          lda OscNumber
          stal SonREG
          lda ninfreqInt
          stal SonDATA   ; Frequency low pair
          stal SonDATA   ; Frequency low impair
          lda OscNumber
          clc
          adc #$20
          stal SonREG
          lda ninfreqInt+1
          stal SonDATA   ; Frequency high pair
          stal SonDATA   ; Frequency high impair

NoVibrato = *            ;VolumeSlide up/down
          rep #$20

          lda Temporary
          asl
          tax
          lda NinVslidedwn,x
          beq checkVslideup
                         ; lda Fineslider,x
                         ; beq no_finesl1
                         ; lda Temporary
                         ; bne NoVolSlide
no_finesl1 lda TrueVolumeTbl,x
          sec
          sbc NinVslidedwn,x
          bpl changevolsl1
          lda #0
          stz NinVslidedwn,x
          bra changevolsl1

checkVslideup lda NinVslideup,x
          beq NoVolSlide
                         ; lda Fineslider,x
                         ; beq no_finesl2
                         ; lda Temporary
                         ; bne NoVolSlide
no_finesl2 lda NinVslideup,x
          clc
          adc TrueVolumeTbl,x
          cmp #$81
          blt changevolsl1
          lda #$80
          stz NinVslideup,x
changevolsl1 sta TrueVolumeTbl,x

          lda Temporary
          asl
          sta OscNumber

          lda TrueVolumeTbl,x
          tax
          lda VolumeConversion,x
          and #$ff
          sta Volumina

          sep #$20

]lp       ldal SonCTRL
          bmi ]lp
          ora #%00100000 ; Auto-incrementation
          and #%10111111
          stal SonCTRL

          lda OscNumber
          clc
          adc #$40
          stal SonREG
          lda Volumina
          stal SonDATA   ; Volume pair
          stal SonDATA   ; Volume impair

NoVolSlide = *

          sep #$30

          inc Temporary
          lda Temporary
          cmp #Nb_PlayedTrack
          beq Fini
          jmp bcleHandleArp

Fini      BRL EndInterrupt
