DATA2MEM = /opt/Xilinx/14.4/ISE_DS/ISE/bin/lin64/data2mem

all:
	make -C tools
	make -C firmware
	cat firmware/image.mem config/*.mem > combined.mem
	$(DATA2MEM) -bm fpga/progmem_bd.bmm -bt fpga/HdmilightTop.bit -bd combined.mem -o b hdmilight.bit
