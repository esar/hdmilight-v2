#!/usr/bin/python

import sys, os

if sys.version_info < (3,):
	def tobyte(x): return chr(x)
else:
	def tobyte(x): return bytes([x])

if sys.platform == 'win32':
	import msvcrt
	msvcrt.setmode(sys.stdout.fileno(), os.O_BINARY)

keyNames = {}
actionNames = {}
keyTable = [0 for x in range(256)]

f = open(os.path.join(sys.argv[1], 'cec.h'), 'r')
for line in f:
	if line.startswith('#define CEC_KEY'):
		(name, value) = line.split()[1:3]
		keyNames[name] = int(value, 0)
f.close()

f = open(os.path.join(sys.argv[1], 'ambilight.h'), 'r')
for line in f:
	if line.startswith('#define ACTION_'):
		(name, value) = line.split()[1:3]
		actionNames[name] = int(value, 0)
f.close()

f = open(sys.argv[2], 'r')
for line in f:
	line = line[:line.find('#')].strip()
	if len(line) == 0:
		continue

	if line.find('=>') != -1:
		(key, action) = [x.strip() for x in line.split('=>')]
		keyIndex = keyNames[key]
		actionIndex = actionNames[action]
		keyTable[keyIndex] = actionIndex

for key in keyTable:
	os.write(sys.stdout.fileno(), tobyte(key))

