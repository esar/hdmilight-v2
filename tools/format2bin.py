#!/usr/bin/python

import sys, os

if sys.version_info < (3,):
	def tobyte(x): return chr(x)
else:
	def tobyte(x): return bytes([x])

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

	os.write(sys.stdout.fileno(), tobyte(width & 0xff))
	os.write(sys.stdout.fileno(), tobyte(width >> 8))
	os.write(sys.stdout.fileno(), tobyte(height & 0xff))
	os.write(sys.stdout.fileno(), tobyte(height >> 8))
	os.write(sys.stdout.fileno(), tobyte(ratio & 0xff))
	os.write(sys.stdout.fileno(), tobyte(ratio >> 8))
	os.write(sys.stdout.fileno(), tobyte(config & 0xff))
	os.write(sys.stdout.fileno(), tobyte(config >> 8))

for i in range(8):
	os.write(sys.stdout.fileno(), tobyte(0))
