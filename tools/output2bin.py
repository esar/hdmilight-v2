#!/usr/bin/python

import sys

input = open(sys.argv[1])
for (linenum, line) in enumerate(input):
	line = line[:line.find('#')]
	cols = line.split()
	if len(cols) == 0:
		continue
	
	if len(cols) != 5:
		sys.stderr.write('wrong column count at line %s\n' % linenum)
		sys.exit(1)

	try:
		(index, area, colour, gamma, enabled) = [int(x, 0) for x in cols]
		if enabled != 0:
			enabled = 1
	except ValueError:
		sys.stderr.write('bad value at line %s\n' % linenum)
		sys.exit(1)

	data = (area & 0x1ff) | ((colour & 7) << 9) | ((gamma & 7) << 12) | (enabled << 15)
	
	sys.stdout.write(chr(data & 0xff))
	sys.stdout.write(chr(data >> 8))
