#!/usr/bin/python

import sys, os

if sys.version_info < (3,):
	def tobyte(x): return chr(x)
else:
	def tobyte(x): return bytes([x])

if sys.platform == 'win32':
	import msvcrt
	msvcrt.setmode(sys.stdout.fileno(), os.O_BINARY)

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
	
	os.write(sys.stdout.fileno(), tobyte(data & 0xff))
	os.write(sys.stdout.fileno(), tobyte((data >>  8) & 0xff))
	os.write(sys.stdout.fileno(), tobyte((data >> 16) & 0xff))
	os.write(sys.stdout.fileno(), tobyte((data >> 24) & 0xff))
