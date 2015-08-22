#!/usr/bin/python

import sys, os

if sys.version_info < (3,):
	def tobyte(x): return chr(x)
else:
	def tobyte(x): return bytes([x])

class Values:
	def __init__(self):
		self.frames = 0
		self.ticks  = 0
		self.smooth = 0.0
		
values = Values()

input = open(sys.argv[1])
for (linenum, line) in enumerate(input):
	line = line[:line.find('#')]
	parts = line.split()
	if len(parts) == 0:
		continue

	if len(parts) != 3 or parts[1] != '=':
		sys.stderr.write('expected name = value pair at line %s\n' % linenum)
		sys.exit(1)
	
	if not hasattr(values, parts[0]):
		sys.stderr.write('unknown attribute "%s" at line %s\n' % (parts[0], linenum))
		sys.exit(1)

	t = type(getattr(values, parts[0]))
	setattr(values, parts[0], t(parts[2]))

os.write(sys.stdout.fileno(), tobyte(values.frames & 0xff))
os.write(sys.stdout.fileno(), tobyte((values.ticks >> 16) & 0xff))
os.write(sys.stdout.fileno(), tobyte((values.ticks >>  8) & 0xff))
os.write(sys.stdout.fileno(), tobyte(values.ticks & 0xff))

smooth = int(values.smooth * 512)
si = (smooth >> 9) & 0x1ff
sf = smooth & 0x1ff
os.write(sys.stdout.fileno(), tobyte((smooth >> 8) & 3))
os.write(sys.stdout.fileno(), tobyte(smooth & 0xff))
