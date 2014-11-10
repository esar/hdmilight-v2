#!/usr/bin/python

import sys

input = open(sys.argv[1])
for line in input:
	line = line[:line.find('#')]
	cols = line.split()
	if len(cols) == 0:
		continue

	(r, g, b) = [long(float(x) * 512) for x in cols]

	r &= 0x3ffff
	g &= 0x3ffff
	b &= 0x3ffff

	data = (b << 36) | (g << 18) | r
	
	sys.stdout.write(chr(data & 0xff))
	sys.stdout.write(chr((data >>  8) & 0xff))
	sys.stdout.write(chr((data >> 16) & 0xff))
	sys.stdout.write(chr((data >> 24) & 0xff))
	sys.stdout.write(chr((data >> 32) & 0xff))
	sys.stdout.write(chr((data >> 40) & 0xff))
	sys.stdout.write(chr((data >> 48) & 0xff))
	sys.stdout.write(chr((data >> 56) & 0xff))
