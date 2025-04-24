*
* Tool222 demo
* by Ninjaforce
* Visit ninjaforce.com
*
* Based on Tool219
* by FTA & ESP, 1990
* See disclaimer below
* Visit freetoolsassociation.com
*
* Adaptation by
* Brutal Deluxe Software, 2018
* Visit brutaldeluxe.fr
*

          lst off
          rel
          typ S16
          dsk Tool222Demo.L
          lst off

          use 4/Locator.Macs
          use 4/Misc.Macs
          use 4/Tool222.Macs
          use 4/Util.Macs

*------------------------------------------------- Tool221

IRQ_SCAN  EQU $E10028
GSOS      EQU $E100A8

*-------------

          CLC
          XCE
          REP #$30
          PHK
          PLB
          SEP #$20
L0008     LDAL $E1C02E
          CMP #$F8
          BCC L0008
          LDA #$41
          STAL $E1C029
          LDAL $E1C034
          STA L03AF+1
          LDA #$00
          STAL $E1C034
          LDAL $E1C022
          PHA
          LDA #$00
          STAL $E1C022
          REP #$20
          LDX #$0000
          TXA
L0034     STAL $E12000,X
          INX
          INX
          BPL L0034
          LDX #$0000
          LDA #$0F00
L0042     STAL $E19E02,X
          PHA
          LDA #$0FFF
          STAL $E19E1E,X
L004E     LDA #$0F00
          STAL $E19E1C,X
          TXA
          CLC
          ADC #$0020
          TAX
          PLA
          CLC
          ADC #$0010
          CPX #$0200
          BCC L0042
          SEP #$30
          LDX #$00
L0069     TXA
          LSR
          STAL $E19D47,X
          INX
          CPX #$20
          BCC L0069
          REP #$30
          LDX #$0324
          JSR L04D3
          ASC 0C
          ASC "        DEMONSTRATION TOOL222."8D8D
          ASC 0C
          ASC "n       NINJATRACKERPLUS PLAYER."8D
          ASC 0C
          ASC ""8D
* ASC "         WRITTEN BY O.GOGUEL."8D
* ASC " WRITTEN BY O.GOGUEL & BRUTAL DELUXE."8D
          ASC "        WRITTEN BY NINJAFORCE.       "8D
          ASC "     (C)OPYRIGHT NINJAFORCE 2018.    "8D
          ASC "      TOOL 222 BY BRUTAL DELUXE."00

          LDAL IRQ_SCAN
          STA L03C1+1

          LDA #$4F5C
          dfb $A9,$5C,#<L044F
          STAL IRQ_SCAN
          LDAL IRQ_SCAN+2
          STA L03C8+1
          LDA #>L044F
          STAL IRQ_SCAN+2

          SEP #$20
L0122     LDAL $E1C02E
          CMP #$F8
          BCC L0122
          LDA #$C1
          STAL $E1C029
          LDAL $E1C023
          AND #$FD
          STAL $E1C023
          PLA
          STAL $E1C022
          REP #$20
          PEA $00DE      ; AV
          PEA $0000
          _LoadOneTool
          BCC L017B

          LDX #^L0159
          LDY #L0159
          BRL L0820
L0159     STR "Can't Load NinjaTrackerPlus Tool222 : $"
L017B     PEA $DEAD
          _NTPStartUp
          BCC L018A
          BRL L081A
L018A     LDA #$0001
          STA L02E4+1
L0190     JSR L035C
          LDA L02E4+1
          DEC
          ASL
          ASL
          TAY
          LDA L0781,Y
          JSR L04DF
L01A0     LDA L02E4+1
          DEC
          ASL
          ASL
          TAY
          PEA ^L0781
          LDA L0783,Y
          PHA
          LDA L02E4+1
          PHA
          _NTPAddToBatch
          CMP #$0045
          BNE L020A
          LDX #$76E3
          JSR L04D3
          ASC 0C
          ASC "nINSERT PROGRAM DISK..."00
          SEP #$20
          STAL $E1C010
L01E3     LDAL $E1C000
          BPL L01E3
          REP #$20
          LDX #$76E3
          JSR L04D3
          ASC "                      "00
          BRA L01A0
L020A     CMP #$0001
          BCC L0232
          LDX #^L0218
          LDY #L0218
          BRL L0820
L0218     STR "Can't load music file : $"
L0232     JSR L0372
          LDA L02E4+1
          INC L02E4+1
          ASL
          ASL
          TAY
          LDA L0781,Y
          CMP #$FFFF
          BEQ L0249
          BRL L0190
L0249     LDA L02E4+1
          STA L033B+1
          LDX #$76C0
          JSR L04D3
          ASC "PRESS "0C
          ASC "nQ"0C
          ASC " TO QUIT OR "0C
          ASC "nSPACE"0C
          ASC " TO CHANGE MUSIC"00

          PHA
          PHA
          _NTPGetTrackVu
          BCC L0294
          BRL L081A
L0294     PLA
          PLA
          TSC
          SEC
          SBC #$0004
          TCS
          PHD
          INC
          TCD
          LDA $00
          STA L0458+1
          LDA $02
          STA L045D+1
          PLD
          TSC
          CLC
          ADC #$0004
          TCS
          LDX #$4272
          JSR L04D3
          ASC "TRACK VU-METER."00

          SEP #$20
          LDAL $E1C023
          ORA #$02
          STAL $E1C023
          STAL $E1C010
          LDA #$40
          STAL $E19DC7
          REP #$20
          LDA #$0001
          STA L02E4+1
L02E4     PEA $0000
          _NTPSelectBatch
          BCC L02F3
          BRL L081A
L02F3     JSR L0349

          jsr clearVU

          PEA $0000
          _NTPPlayMusic
          BCC L0305
          BRL L081A
L0305     PHA
          _NTPGetEOfMusic
          PLA
          ORAL $E1BFFF
          BPL L0305
          SEP #$20
          LDAL $E1C000
          STAL $E1C010
          REP #$20
          AND #$00FF
          CMP #$0080
          BCC L0334
          CMP #$00A0
          BEQ L0334
          cmp #$00f1
          beq L039F
          CMP #$00D1
          BNE L0305
          BEQ L039F
L0334     JSR L0372
          LDA L02E4+1
          INC
L033B     CMP #$0004
          BNE L0343
          LDA #$0001
L0343     STA L02E4+1
          BRL L02E4
L0349     JSR L0386
          JSR L04D3
          ASC 0C
          ASC "nPLAYING :"00
          RTS

L035C     JSR L0386
          JSR L04D3
          ASC 0C
          ASC "nLOADING : "0C
          ASC ""00
          RTS

L0372     JSR L0386
          JSR L04D3
          ASC 0C
          ASC "n          "00
          RTS

L0386     LDA L02E4+1
          DEC
          ASL
          ASL
          ASL
          CLC
          ADC #$008D
          ASL
          ASL
          ASL
          ASL
          ASL
          PHA
          ASL
          ASL
          CLC
          ADC $01,S
          PLX
          TAX
          RTS

L039F     SEP #$30
L03A1     LDAL $E1C02E
          CMP #$F8
          BCC L03A1
          LDA #$01
          STAL $E1C029
L03AF     LDA #$00
          STAL $E1C034
          LDAL $E1C023
          AND #$FD
          STAL $E1C023
          REP #$30
L03C1     LDA #$0000
          STAL IRQ_SCAN
L03C8     LDA #$0000
          STAL IRQ_SCAN+2
          _NTPShutDown
          BCC L03DB
          BRL L081A
L03DB     JSL GSOS
          DW $0029       ; Quit
          ADRL L03E5
L03E5     ADRL $00000000 ;  path name
          DW $0000       ;  flags

          BRK $00

L03ED     DW $1400
          DW $1360
          DW $12C0
          DW $1220
          DW $1180
          DW $10E0
          DW $1040
          DW $0FA0
          DW $0F00
          DW $0E60
          DW $0DC0
          DW $0D20
          DW $0C80
          DW $0BE0
          DW $0B40
          DW $0AA0
          DW $0A00
          DW $0960
          DW $08C0
          DW $0820
          DW $0780
          DW $06E0
          DW $0640
          DW $05A0
          DW $0500
          DW $0460
          DW $03C0
          DW $0320
          DW $0280
          DW $01E0
          DW $0140
          DW $00A0
          DW $2000

L042F     DS $20

          mx %00

L044F     PHB
          PHK
          PLB

          REP #$30
          PEI $00
          PEI $02
L0458     LDA #$0000
          STA $00
L045D     LDA #$0000
          STA $02
          LDY #$001c
L0465     LDA L042F,Y
          CMP [$00],Y
          BEQ L04AA
          BCS L0496
          LSR
          LSR
          LSR
          PHA
          LDA [$00],Y
          LSR
          LSR
          LSR
          INC
          STA L048F+1
          PLA
L047C     PHA
          ASL
          TAX
          TYA
          ASL
          CLC
          ADC L03ED,X
          TAX
          LDA #$1111
          STAL $E14BFC,X
          PLA
          INC
L048F     CMP #$0000
          BCC L047C
          BRA L04AA

L0496     LSR
          LSR
          AND #$FFFE
          TAX
          TYA
          ASL
          CLC
          ADC L03ED,X
          TAX
          LDA #$0000
          STAL $E14BFC,X

L04AA     LDA [$00],Y
          STA L042F,Y
          SEC
          SBC #$0006
          BCS L04B8
          LDA #$0000
L04B8     STA [$00],Y
          DEY
          DEY
          BPL L0465

          PLA
          STA $02
          PLA
          STA $00
          SEP #$30
          LDAL $E1C032
          AND #$DF
          STAL $E1C032
          PLB
          CLC
          RTL

          MX %00

*--- Clear VU meters AV201809

clearVU

          ldx #32-2      ; AV201809 Clear the VU meters
]lp       stz L042F,x
          dex
          dex
          bpl ]lp

          LDY #$001c
loopVU    lda #0
]lp       PHA
          ASL
          TAX
          TYA
          ASL
          CLC
          ADC L03ED,X
          TAX
          LDA #$0000
          STAL $E14BFC,X
          PLA
          INC
          CMP #32
          BCC ]lp
          dey
          dey
          bpl loopVU
          rts

L04D3     PLA
          STA L04E9+1
          JSR L04E3
          LDA L04E9+1
          PHA
          RTS

L04DF     DEC
          STA L04E9+1
L04E3     STX L050F+1
L04E6     INC L04E9+1
L04E9     LDA |$0000
          TAY
          AND #$00FF
          BEQ L0519
          CMP #$000C
          BEQ L0501
          CMP #$008D
          BEQ L050F
          JSR L051A
          BRA L04E6
L0501     TYA
          XBA
          SEP #$20
          STA L0550+1
          REP #$20
          INC L04E9+1
          BRA L04E6
L050F     LDA #$0000
          CLC
          ADC #$0500
          TAX
          BRA L04E3
L0519     RTS

L051A     SEC
          SBC #$00A0
          ASL
          ASL
          ASL
          CLC
          ADC #L0571
          STA L052D+1
          LDA #$0008
L052B     PHA
          PHX
L052D     LDA |$0000
          INC L052D+1
          LDY #$0004
L0536     PHY
          PHA
          LDY #$0000
          BIT #$0002
          BEQ L0543
          LDY #$000F
L0543     BIT #$0001
          BEQ L054D
          TYA
          ORA #$00F0
          TAY
L054D     TYA
          SEP #$20
L0550     AND #$FF
          STAL $E12000,X
          REP #$20
          INX
          PLA
          LSR
          LSR
          PLY
          DEY
          BNE L0536
          PLA
          CLC
          ADC #$00A0
          TAX
          PLA
          DEC
          BNE L052B
          TXA
          SEC
          SBC #$04FC
          TAX
          RTS

L0571     HEX 00000000000000000018181800180000
          HEX 003636360000000000263E263E260000
          HEX 003E1E3E383E0000002638181E260000
          HEX 00182618263800000018181800000000
          HEX 00380C0C0C380000001C3030301C0000
          HEX 0026183E182600000018187E18180000
          HEX 0000000018180C000000007E00000000
          HEX 0000000018180000006030180C060000
          HEX 007E6666667E00000018181818180000
          HEX 007E607E067E0000007E6078607E0000
          HEX 0066667E60600000007E067E607E0000
          HEX 007E067E667E0000007E6030180C0000
          HEX 007E667E667E0000007E667E60600000
          HEX 00001800180000000000180018180C00
          HEX 0030180C1830000000003E003E000000
          HEX 000C1830180C0000003E203800000018
          HEX 3F3F3F3F3F3F3F3F003C667E66660000
          HEX 003E663E663E0000007C0606067C0000
          HEX 003E6666663E0000007E063E067E0000
          HEX 007E063E06060000007C0606663C0000
          HEX 0066667E66660000007E1818187E0000
          HEX 00606060667E00000066361E36660000
          HEX 00060606067E000000667E7E66660000
          HEX 00666E7E76660000003C6666663C0000
          HEX 003E663E06060000003C6666366C0000
          HEX 003E661E36660000007C063C603E0000
          HEX 007E18181818000000666666663C0000
          HEX 006666663C180000006666667E660000
          HEX 00663C183C6600000066663C18180000
          HEX 007E6018067E0000001E0606061E0000
          HEX 0038183E183E0000003C3030303C0000
          HEX 00183E18181800000018063E00000000
          HEX 00000000000000000000000000000000

L0781     DA L07BD
L0783     DA L078F
          DA L07DC
          DA L079F
          DA L07FB
          DA L07AD
          DA L07FC
          DA L07AE
          DW $FFFF

L078F     STR '1/Music/Arnesune.NTP'
L079F     STR '1/Music/BlowBlow.NTP'
L07AD     STR '1/Music/Conrad.NTP'
L07AE     STR '1/Music/SpiceItUp.NTP'

L07BD     ASC "ARNESUNE (D-ZIRE SILENTS)"00
L07DC     ASC "BLOW BLOW (JESTER)"00
L07FB     ASC "CONRAD (XTD UNION)"00
L07FC     ASC "SPICEITUP (JESTER)"00

L081A     LDX #^L0836
          LDY #L0836
L0820     PHA
          PHX
          PHY
          SEP #$20
          LDAL L03AF+1
          STAL $E1C034
          REP #$20
          _SysFailMgr

L0836     STR 'Fatal System Error : $'
