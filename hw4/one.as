	lw	0	1	one
	lw	0	4	four
loop	beq	3	4	next
	sw	3	3	array
	add	3	1	3
	beq	0	0	loop
next	beq	0	3	done
	noop
	lw	1	6	array
	beq	6	1	done
	lw	0	3	dAddr
	jalr	3	5
done	halt
dAddr	.fill	done
one	.fill	1
four	.fill	4
array	.fill	0
