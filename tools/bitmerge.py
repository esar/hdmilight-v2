#!/usr/bin/env python

# Originally from http://code.google.com/p/fpga-sram-bootstrap/source/browse/trunk/bitmerge.py

# To run this script, use Python version 2.7 not version 3.x
import argparse
import struct

def autoint(x):
	return int(x, 0)

#Standard SST25VF040B FLASH size of 4Mbits
flash_size = 8*1024*1024/8
#Standard size of bitstream for a Spartan 6 LX9
bit_size = 340*1024

parser = argparse.ArgumentParser(description='Concatenates an FPGA .bit file and a user supplied binary file together.\nProduces a valid .bit file that can be written to a platform flash using\nstandard tools.')
parser.add_argument('ifile', type=argparse.FileType('rb'), help='source FPGA .bit file')
parser.add_argument('bfile', type=argparse.FileType('rb'), help='source binary file')
parser.add_argument('ofile', type=argparse.FileType('wb'), help='destination merged FPGA .bit + binary file')
parser.add_argument('--pad', type=autoint, dest='pad', default=1, 
                    metavar='X', help='pad the size of the .bit so the user data starts at a boundary that is a multiple of X')
args = parser.parse_args()

#seek to end
args.bfile.seek(0,2)
#get size of user binary file to merge
bsize = args.bfile.tell()
#seek to start
args.bfile.seek(0,0)

data = args.ifile.read(2)
args.ofile.write(data)
(length,) = struct.unpack(">H", data)
assert length == 9, "Invalid .bit file magic length."

#check bit file magic marker
data = args.ifile.read(length)
args.ofile.write(data)
(n1,n2,n3,n4,n5,) = struct.unpack("<HHHHB", data)
assert n1==n2==n3==n4==0xF00F, "Invalid .bit file magic marker"

data = args.ifile.read(2)
args.ofile.write(data)
(length,) = struct.unpack(">H", data)
assert length==1, "Unexpected value."

#loop through the bit file sections 'a' through 'd' and print out stats
section=""
while section != 'd':
	section = args.ifile.read(1)
	args.ofile.write(section)
	data =  args.ifile.read(2)
	args.ofile.write(data)
	(length,) = struct.unpack(">H", data)
	desc = args.ifile.read(length)
	args.ofile.write(desc)
	print "Section '%c' (size %6d) '%s'" % (section, length, desc)

#process section 'e' (main bit file data)
section = args.ifile.read(1)
args.ofile.write(section)
assert section=="e", "Unexpected section"
data =  args.ifile.read(4)
#this is the actual size of the FPGA bit stream contents
(length,) = struct.unpack(">L", data)
print "Section '%c' (size %6d) '%s'" % (section, length, "FPGA bitstream")
#we can't merge a "merged" file, well..., we could, but we won't
assert length<=bit_size, "Section 'e' length of %d seems unreasonably long\nCould this file have already been merged with a binary file?" %length

if length % args.pad != 0:
	padsize = args.pad - (length % args.pad)
	print "Padding by %s" % padsize
else:
	padsize = 0

#check that both files will fit in flash
assert (length+bsize+padsize) <= flash_size, "Combined files sizes of %d would exceed flash capacity of %d bytes" % ((length+bsize), flash_size)
print "Merged user data begins at FLASH address 0x%06X" % (length + padsize)

#write recalculated section length
data = struct.pack(">L", length+bsize+padsize)
args.ofile.write(data)

#read FPGA bitstream and write to output file
data = args.ifile.read(length)
args.ofile.write(data)

args.ofile.write("\x00" * padsize)

#read user provided binary data and append to file
data = args.bfile.read(bsize)
args.ofile.write(data)

#close up files and exit
args.ifile.close()
args.bfile.close()
args.ofile.close()
