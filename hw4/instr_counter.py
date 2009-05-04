"""
	Program to count number and type of instructions executed based
	on output from
		grep "pc" simulator.out
"""

source_fh = open("one.as")
source_lines = source_fh.readlines()
source_lines = [line.split() for line in source_lines]

pc_fh = open("one.pc.out")
pc_lines = pc_fh.readlines()
pc_lines = [int(line.strip("\tpc")) for line in pc_lines]

instructions = ["lw", "sw", "add", "nand", "beq", "halt", "noop", "jalr", ".fill"]

source_instr_location = {}

for line, i in zip(source_lines, xrange(len(source_lines))):
	instr = None

	if line[0] in instructions:
		instr = line[0]
	elif line[1] in instructions:
		instr = line[1]
	else:
		continue
	source_instr_location[i] = instr

count = {}
for i in instructions:
	count[i] = 0

for pc in pc_lines:
	count[source_instr_location[pc]] += 1

total = 0.0

for i in instructions:
	if i is not ".fill" and i is not "halt":
		total += count[i]

for i in instructions:
	if i is not ".fill" and i is not "halt":
		print i + ":", count[i], "     " +  str((count[i]/total) * 100.0) + "%"
