  bne r4, r4, target
  daddi r5, r4, 90
target:
  dmul r3, r8, r5
  ld r8, 16(r9)
  andi r3, r8, 40
