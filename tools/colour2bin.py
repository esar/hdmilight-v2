#!/usr/bin/python

import sys, os
import numpy as np

if sys.version_info < (3,):
	def tobyte(x): return chr(x)
else:
	long = int
	def tobyte(x): return bytes([x])

# weights taken from http://graficaobscura.com/matrix/index.html
WR = 0.3086
WG = 0.6094
WB = 0.0820

class Values:
	def __init__(self):
		self.matrixR = [1.0, 0.0, 0.0]
		self.matrixG = [0.0, 1.0, 0.0]
		self.matrixB = [0.0, 0.0, 1.0]
		self.matrixC = [0.0, 0.0, 0.0]
		self.brightR = 1.0
		self.brightG = 1.0
		self.brightB = 1.0
		self.contrast = 1.0
		self.saturation = 1.0
		
values = Values()

input = open(sys.argv[1])
for (linenum, line) in enumerate(input):
	line = line[:line.find('#')]
	parts = line.split(None, 2)
	if len(parts) == 0:
		continue

	if len(parts) != 3 or parts[1] != '=':
		sys.stderr.write('expected name = value pair at line %s\n' % linenum)
		sys.stderr.write('    %s\n' % line)
		sys.exit(1)
	
	if not hasattr(values, parts[0]):
		sys.stderr.write('unknown attribute "%s" at line %s\n' % (parts[0], linenum))
		sys.exit(1)

	t = type(getattr(values, parts[0]))
	if t == list:
		lt = type(getattr(values, parts[0])[0])
		setattr(values, parts[0], [lt(x) for x in parts[2].split()])
	else:
		setattr(values, parts[0], t(parts[2]))


values.matrixR.append(0.0)
values.matrixG.append(0.0)
values.matrixB.append(0.0)
values.matrixC.append(1.0)
mat = np.matrix([values.matrixR,
                 values.matrixG,
                 values.matrixB,
                 values.matrixC])

br = values.brightR
bg = values.brightG
bb = values.brightB
bmat = np.matrix([[br, 0,  0,  0], 
                  [0,  bg, 0,  0], 
                  [0,  0,  bb, 0], 
                  [0,  0,  0,  1]])

c = values.contrast
t = (1.0 - c) / 2.0
cmat = np.matrix([[c, 0, 0, 0],
                  [0, c, 0, 0],
                  [0, 0, c, 0],
                  [t, t, t, 1]])


s = values.saturation
si = 1.0 - s
smat = np.matrix([[si * WR + s, si * WR,     si * WR,     0],
                  [si * WG,     si * WG + s, si * WG,     0],
                  [si * WB,     si * WB,     si * WB + s, 0],
                  [0,           0,           0,           1]])

result = mat * bmat * cmat * smat

#sys.stderr.write(str(result) + '\n')

for row in result.tolist():
	(r, g, b, c) = [long(float(x) * 512) & 0x3ffff for x in row]

	data = (b << 36) | (g << 18) | r

	os.write(sys.stdout.fileno(), tobyte(data & 0xff))
	os.write(sys.stdout.fileno(), tobyte((data >>  8) & 0xff))
	os.write(sys.stdout.fileno(), tobyte((data >> 16) & 0xff))
	os.write(sys.stdout.fileno(), tobyte((data >> 24) & 0xff))
	os.write(sys.stdout.fileno(), tobyte((data >> 32) & 0xff))
	os.write(sys.stdout.fileno(), tobyte((data >> 40) & 0xff))
	os.write(sys.stdout.fileno(), tobyte((data >> 48) & 0xff))
	os.write(sys.stdout.fileno(), tobyte((data >> 56) & 0xff))
