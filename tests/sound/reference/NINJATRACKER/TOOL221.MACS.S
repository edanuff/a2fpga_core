* NinjaTracker Tool
* FTA, 1991
* Ninjaforce, 1995
* Brutal Deluxe, 2015

_NTBootInit mac
          Tool $01DD
          <<<

~NTStartUp mac
          PHW ]1
_NTStartUp mac
          Tool $02DD
          <<<

_NTShutDown mac
          Tool $03DD
          <<<

~NTVersion mac
          phd            ; WordResult
_NTVersion mac
          Tool $04DD
          <<<

_NTReset  mac
          Tool $05DD
          <<<

~NTStatus mac
          phd            ; WordResult
_NTStatus mac
          Tool $06DD
          <<<

~NTLoadOneMusic mac
          PHL ]1
_NTLoadOneMusic mac
          Tool $09DD
          <<<

~NTPlayMusic mac
          PHW ]1
_NTPlayMusic mac
          Tool $0ADD
          <<<

_NTStopMusic mac
          Tool $0BDD
          <<<

~NTGetEOfMusic mac
          phd            ; WordResult
_NTGetEOfMusic mac
          Tool $0CDD
          <<<

~NTAddToBatch mac
          PHLW ]1        ;]2
_NTAddToBatch mac
          Tool $0DDD
          <<<

~NTSelectBatch mac
          PHW ]1
_NTSelectBatch mac
          Tool $0EDD
          <<<

~NTKillBatch mac
          PHW ]1
_NTKillBatch mac
          Tool $0FDD
          <<<

~NTGetPlayingMusic mac
          phd            ; WordResult
_NTGetPlayingMusic mac
          Tool $10DD
          <<<

~NTPlayBatch mac
          PHL ]1
_NTPlayBatch mac
          Tool $11DD
          <<<

~NTGetTrackVu mac
          phd            ;Long
          phd            ;    Result
_NTGetTrackVu mac
          Tool $12DD
          <<<

_NTPauseMusic mac
          Tool $13DD
          <<<

_NTContinueMusic mac
          Tool $14DD
          <<<

_NTinternal15 mac
          Tool $15DD
          <<<

_NTinternal16 mac
          Tool $16DD
          <<<
