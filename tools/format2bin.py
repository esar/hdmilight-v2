#!/usr/bin/python

import sys

input = open(sys.argv[1])
for (linenum, line) in enumerate(input):
	line = line[:line.find('#')]
	cols = line.split()
	if len(cols) == 0:
		continue
	
	if len(cols) != 4:
		sys.stderr.write('wrong column count at line %s\n' % linenum)
		sys.exit(1)

	try:
		width  = int(cols[0], 0)
		height = int(cols[1], 0)
		ratio  = float(cols[2])
		config = int(cols[3], 0)
	except ValueError:
		sys.stderr.write('bad value at line %s\n' % linenum)
		sys.exit(1)

	if ratio == 1.77:
		ratio = 0
	elif ratio == 2.40:
		ratio = 1
	elif ratio == 1.33:
		ratio = 2
	else:
		sys.stderr.write('unknown ratio at line %s\n' % linenum)

	sys.stdout.write(chr(width & 0xff))
	sys.stdout.write(chr(width >> 8))
	sys.stdout.write(chr(height & 0xff))
	sys.stdout.write(chr(height >> 8))
	sys.stdout.write(chr(ratio & 0xff))
	sys.stdout.write(chr(ratio >> 8))
	sys.stdout.write(chr(config & 0xff))
	sys.stdout.write(chr(config >> 8))

sys.stdout.write(chr(0))
sys.stdout.write(chr(0))
sys.stdout.write(chr(0))
sys.stdout.write(chr(0))
sys.stdout.write(chr(0))
sys.stdout.write(chr(0))
sys.stdout.write(chr(0))
sys.stdout.write(chr(0))
