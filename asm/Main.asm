.open "work/DIGIMON/SLUS_010.32",0x80090000
.psx

.org 0x80090010
  .word EntryPoint

.org 0x801130d8
  jal recalculatePPandArena

; Retained text preparation now composes one bounded row with shared font metrics.
.org drawString2
.area 0x80100e40-., 0
  j drawScriptTextLine
  nop

  .notice "Empty space left: " + (0x80100e40-.) + " bytes"
.endarea

; drawScriptTextLine: pass the caller's full-row or half-row pixel extent.
.org 0x800ffb30
  jal drawScriptTextLine
  lw a3,0x2c(sp)

.org 0x8010ab1c
  jal drawScriptTextLine
  li a3,255

.org 0x80111ba4
  li.u a3,renderMainMenuBackground
.org 0x80111bb8
  li.l a3,renderMainMenuBackground

.close

.open "work/DIGIMON/BTL_REL.BIN",0x80052ae0
.psx

.org 0x800653d4
  jal updateTMDTextureData


.org 0x80056d60
  jal initializeAttackObjects

.org 0x80066714
  jal addAttackObject
.org 0x8006696c
  jal addAttackObject
.org 0x80066c90
  jal addAttackObject
.org 0x8006d320
  jal addAttackObject
.org 0x80070ddc
  jal addAttackObject

.org 0x8005c804
  jal popAttackObject
.close

.open "work/DIGIMON/VS_REL.BIN",0x80052ae0
.psx

.org 0x80063b8c
  jal updateTMDTextureData

.org 0x80064ddc
  jal addAttackObject
.org 0x80065034
  jal addAttackObject
.org 0x80065358
  jal addAttackObject
.org 0x8006b9cc
  jal addAttackObject
.org 0x8006f44c
  jal addAttackObject

.org 0x8005c448
  jal popAttackObject

.close

.open "work/DIGIMON/STD_REL.BIN",0x80052ae0
.psx

.org 0x8006dabc
  jal updateTMDTextureData

.org 0x8005e97c
  jal initializeAttackObjects

.org 0x8006ed0c
  jal addAttackObject
.org 0x8006ef64
  jal addAttackObject
.org 0x8006f288
  jal addAttackObject
.org 0x800758fc
  jal addAttackObject
.org 0x8007937c
  jal addAttackObject

.org 0x80064864
  jal popAttackObject

.close
