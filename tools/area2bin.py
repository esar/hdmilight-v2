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
		(xmin, xmax, ymin, ymax, divshift) = [int(x, 0) for x in cols]
	except ValueError:
		sys.stderr.write('bad value at line %s\n' % linenum)
		sys.exit(1)

	data = ((divshift & 15) << 24) | ((ymax & 63) << 18) | ((ymin & 63) << 12) | ((xmax & 63) << 6) | (xmin & 63);
	
	sys.stdout.write(chr(data & 0xff))
	sys.stdout.write(chr((data >>  8) & 0xff))
	sys.stdout.write(chr((data >> 16) & 0xff))
	sys.stdout.write(chr((data >> 24) & 0xff))
