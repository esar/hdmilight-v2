Serial Console Commands
=======================


Get Area
--------

    GA index

* index (0-255): The index or range of indices of the area definition(s) that should be retrieved 




Set Area
--------

    SA index xmin xmax ymin ymax divshift

* index (0-255): The index or range of indices of the area definition(s) that should be set
* xmin (0-63): The left most column of the area's rectangle
* xmax (0-63): The right most column of the area's rectangle
* ymin (0-63): The upper row of the area's rectangle
* ymax (0-63): The lower row of the area's rectangle
* divshift (0-): The number of places to right-shift the accumulated R, G and B value for the area, or in other words the divisor used to calculate the average
 * 1 = divide by 2
 * 2 = divide by 4
 * 3 = divide by 8
 * etc.



Reset Area
----------

    RA



Get Colour Correction Matrix
----------------------------

    GC matrix row 

* matrix (0-15): The index or range of indices of the matrix(s) whose row(s) should be retrieved
* row (0-3): The index or range of indices of the row(s) that should be retrieved:
  * Row 0: defines how red input influences the output R, G and B
  * Row 1: defines how green input influences the output R, G and B
  * Row 2: defines how blue input influences the output R, G and B
  * Row 3: defines the constant value that should be applied to the output R, G and B



Set Colour Correction Matrix
----------------------------

    SC matrix row r g b

* matrix (0-15): The index or range of indices of the matrix(s) whose row(s) should be set
* row (0-3): The index or range of indices of the row(s) that should be set
* r (-255.999-255.999): The coefficient for R
* g (-255.999-255.999): The coefficient for G
* b (-255.999-255.999): The coefficient for B



Reset Colour Correction Matrix
------------------------------

    RC

The reset colour command resets all colour correction matrices to the identity matrix.


Get Delay
---------

    GD



Set Delay
---------

    SD frames usec smooth_ratio

* frames (0-7): The number of whole frames that the output should be delayed by
* usec (0-65535): The number of micro-seconds that the output should be delayed by
* smooth_ratio (0.000-1.000): A number that specifies how much of the new output colour comes from the previous output colour and how much comes from the current frame:
 * 0.000 = 100% from current frame
 * 1.000 = 100% from previous output



Reset Delay
-----------

    RD

The reset delay command disables all delay and temporal smoothing



Disable Format Switching
------------------------

    DF

The disable format switching command stops configurations from being loaded
from flash whenever the format of the incoming video changes.



Enable Format Switching
-----------------------

    EF

The enable format switching command enables loading of the configurations
specified in the format table when the input video format changes.



Get Gamma
---------

    GG table channel index

* table (0-7): The index or range of indices of the gamma correction table(s) whose values should be retrieved
* channel (0-2): The index or range of indices of the colour channel(s) whose values should be retrieved: 
 * 0 = red
 * 1 = green
 * 2 = blue
* index: The index or range of indices of the row(s) that should be retrieved (0-255)



Set Gamma
---------

    SG table channel index value

* table (0-7): The index or range of indices of the gamma correction table(s) whose values should be set
* channel (0-2): The index or range of indices of the colour channel(s) whose values should be set
* index (0-255): The index or range of indices of the row(s) that should be set



Reset Gamma
-----------

    RG

The reset gamma command resets all gamma tables to a linear ramp (gamma 1.0)


Get I2C
-------

    GI addr subaddr

* addr (0-255): The address of the component within the ADV7611 that should be queried:
 * IO: 0x98
 * CP: 0x44
 * HDMI: 0x68
 * Repeater: 0x64
 * EDID: 0x6C
 * InfoFrame: 0x7C
 * CEC: 0x80
 * DPLL: 0x4C
* subaddr (0-255): The address within the component that should be queried



Set I2C
-------

    SI addr subaddr value

* addr (0-255): The address of the component within the ADV7611 that should be queried:
 * IO: 0x98
 * CP: 0x44
 * HDMI: 0x68
 * Repeater: 0x64
 * EDID: 0x6C
 * InfoFrame: 0x7C
 * CEC: 0x80
 * DPLL: 0x4C
* subaddr (0-255): The address within the component that should be queried
* value (0-255): The value that should be written to the register


Reset I2C
---------

    RI

The reset I2C command sets the registers as described in the table
in config_hdmi.h


Get Memory
----------

    GM index

* index (0-63): The index of the configuration that should be retrieved from flash



Get Output Map
--------------

    GO output light

* output (0-7): The index or indices of the output channels whose mapping should be retrieved
* light (0-511): The index or indices of the LED(s) whose mapping should be retrieved



Set Output Map
--------------

    SO output light area colour gamma enable

* output (0-7): The index or indices of the output channels whose mapping should be set
* light (0-511): The index or indices of the LED(s) whose mapping should be set
* area (0-255): The index of the area definition that the LED(s) should use
* colour (0-15): The index of the colour matrix that the LED(s) shoudl use
* gamma (0-7): The index of the gamma table that the LED(s) should use
* enable (0-1): 
 * 1 = enabled
 * 0 = disabled


Reset Output
------------

    RO

The reset output command sets the first 256 LEDs for all outputs to use the 256 area definitions in order, so LED 0 uses area 0, LED 1 uses area 1, and so on. All LEDs are set to use colour matrix 0 and gamma table 0. The remaining 256 LEDs on each output are disabled.







