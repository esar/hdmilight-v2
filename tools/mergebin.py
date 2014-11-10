#!/usr/bin/python

import sys, os

conf = open(sys.argv[1])
output = open(sys.argv[2], 'wb')

for line in conf:
	line = line[:line.find('#')].strip()
	if len(line) == 0:
		continue

	(address, filename) = line.split()
	if os.path.isdir(filename):
		filename = os.path.join(filename, 'merged.bin')
	output.seek(int(address, 0))
	output.write(open(filename).read())
