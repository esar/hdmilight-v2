#!/usr/bin/python

import sys

out = file(sys.argv[1], 'wb')

offset = int(sys.argv[2], 0)

for line in sys.stdin:
	parts = line.split()
	if parts[0].startswith('@'):
		addr = int(parts[0][1:], 16)
		addr += offset
		out.seek(addr)
		for part in parts[1:]:
			out.write(chr(int(part, 16)))

out.close()
		
