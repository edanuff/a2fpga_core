* NinjaTrackerPlus Tool
* Ninjaforce, 2018
* Brutal Deluxe, 2018
* FTA, 1991

          MACRO
&lab      _NTPBootInit
&lab      ldx #$01DE
          jsl $E10000
          MEND

          MACRO
&lab      _NTPStartUp
&lab      ldx #$02DE
          jsl $E10000
          MEND

          MACRO
&lab      _NTPShutDown
&lab      ldx #$03DE
          jsl $E10000
          MEND

          MACRO
&lab      _NTPVersion
&lab      ldx #$04DE
          jsl $E10000
          MEND

          MACRO
&lab      _NTPReset
&lab      ldx #$05DE
          jsl $E10000
          MEND

          MACRO
&lab      _NTPStatus
&lab      ldx #$06DE
          jsl $E10000
          MEND

          MACRO
&lab      _NTPLoadOneMusic
&lab      ldx #$09DE
          jsl $E10000
          MEND

          MACRO
&lab      _NTPPlayMusic
&lab      ldx #$0ADE
          jsl $E10000
          MEND

          MACRO
&lab      _NTPStopMusic
&lab      ldx #$0BDE
          jsl $E10000
          MEND

          MACRO
&lab      _NTPGetEndOfMusic
&lab      ldx #$0CDE
          jsl $E10000
          MEND

          MACRO
&lab      _NTPAddToBatch
&lab      ldx #$0DDE
          jsl $E10000
          MEND

          MACRO
&lab      _NTPSelectBatch
&lab      ldx #$0EDE
          jsl $E10000
          MEND

          MACRO
&lab      _NTPKillBatch
&lab      ldx #$0FDE
          jsl $E10000
          MEND

          MACRO
&lab      _NTPGetPlayingMusic
&lab      ldx #$10DE
          jsl $E10000
          MEND

          MACRO
&lab      _NTPPlayBatch
&lab      ldx #$11DE
          jsl $E10000
          MEND

          MACRO
&lab      _NTPGetTrackVu
&lab      ldx #$12DE
          jsl $E10000
          MEND

          MACRO
&lab      _NTPPauseMusic
&lab      ldx #$13DE
          jsl $E10000
          MEND

          MACRO
&lab      _NTPContinueMusic
&lab      ldx #$14DE
          jsl $E10000
