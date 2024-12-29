  daddi r5, r0, 9
  daddi r3, r0, -3
begin:
  beq r5, r0, end
  dadd r5, r5, r3
  dmul r1, r3, r4
  j begin
end:
  slt r4, r1, r4