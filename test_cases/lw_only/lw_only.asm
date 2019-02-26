 .data
	x: .word 20
    y: .word 30
    z: .word 90
.text
main:
	lw $t0, x
	lw $t1, y
	lw $t2, z
	add $t3, $t0, $t1
	lw $t4, x
	lw $t5, y
	jr $ra