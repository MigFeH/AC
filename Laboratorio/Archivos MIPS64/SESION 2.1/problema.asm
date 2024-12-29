.code
or r1, r0, 2
startf:
beqz r1, endf
daddi r1, r1, -1
j startf
endf:
ld r5, 100(r3)