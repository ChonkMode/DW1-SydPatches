.open "work/DIGIMON/KAR_REL.BIN",0x80053800
.psx

; KAR_getWallZone: preserve the retail caller's return address and signed arguments.
.org 0x80058430
.area 8
  j karGetWallZone
  nop
.endarea

.close
