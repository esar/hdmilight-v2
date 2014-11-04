----------------------------------------------------------------------------------
--
-- Copyright (C) 2014 Stephen Robinson
--
-- This file is part of HDMI-Light
--
-- HDMI-Light is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 2 of the License, or
-- (at your option) any later version.
--
-- HDMI-Light is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
-- 
-- You should have received a copy of the GNU General Public License
-- along with this code (see the file names COPING).  
-- If not, see <http://www.gnu.org/licenses/>.
--
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.std_logic_unsigned.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity resultDelayRegisters is
    Port ( clk : in  STD_LOGIC;
           addr : in  STD_LOGIC_VECTOR (2 downto 0);
           din : in  STD_LOGIC_VECTOR (7 downto 0);
			  dout : out STD_LOGIC_VECTOR (7 downto 0);
           we : in  STD_LOGIC;
			  
           frameCount : out  STD_LOGIC_VECTOR (7 downto 0);
           tickCount : out  STD_LOGIC_VECTOR (23 downto 0);
           temporalSmoothingRatio : out STD_LOGIC_VECTOR (8 downto 0)
    );
end resultDelayRegisters;

architecture Behavioral of resultDelayRegisters is

constant ADDR_WIDTH : integer := 3;
constant DATA_WIDTH : integer := 8;
type mem_type is array ( (2**ADDR_WIDTH)-1 downto 0 ) of std_logic_vector(DATA_WIDTH-1 downto 0);
shared variable mem : mem_type := ("11101000", "00000011", "00000000", "00000010", "00000000", "00000000", "00000000", "00000000");

begin

process(clk)
begin
	if(rising_edge(clk)) then
		if(we = '1') then
			mem(conv_integer(addr)) := din;
		end if;
		dout <= mem(conv_integer(addr));
	end if;
end process;

frameCount <= mem(0);
tickCount <= mem(1) & mem(2) & mem(3);
temporalSmoothingRatio <= mem(4)(0) & mem(5);

end Behavioral;

