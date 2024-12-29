
.microarch
out_of_order_retirement = true
forwarding = true
early_branch_evaluation = true
branch_prediction = always_not_taken
exc_pipelined_cycles = 3
exc_instructions = [dmul]

.code
ori r1, r0, 8 ; r1 = 8
daddi r2, r0, 2 ; r2 = 2
loop:
beqz r1, dest
dmul r3, r2, r7
xor r6, r4, r5
dadd r6, r7, r6
dsub r1, r1, r2
j loop
dest:
dmul r5, r4, r2
