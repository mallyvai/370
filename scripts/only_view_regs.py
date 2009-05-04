#!/usr/bin/env python

import sys

fh = open(sys.argv[1], 'r')
lines = fh.readlines()
i = 0
for line in lines:
	if "reg[" in line:
		print line,
	elif "register" in line:
		print i
		i+=1