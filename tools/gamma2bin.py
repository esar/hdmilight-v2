#!/usr/bin/python

import sys

gamma = float(sys.argv[1])

for i in range(256):
	value = int(((float(i) / 255)**gamma) * 255)
	sys.stdout.write(chr(value))
