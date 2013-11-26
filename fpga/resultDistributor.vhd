----------------------------------------------------------------------------------
--
-- Copyright (C) 2013 Stephen Robinson
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
use ieee.numeric_std.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity resultDistributor is
    Port ( clk : in  STD_LOGIC;
           vblank : in  STD_LOGIC;
			  resultAddr : out STD_LOGIC_VECTOR (8 downto 0);
           resultData : in  STD_LOGIC_VECTOR (31 downto 0);
			  
           busy : in  STD_LOGIC;
			  outputReady : out STD_LOGIC;
			  outputAddr : out STD_LOGIC_VECTOR (7 downto 0);
           outputData : out STD_LOGIC_VECTOR (23 downto 0));
end resultDistributor;

architecture Behavioral of resultDistributor is

signal lastbusy : std_logic;
signal lastvblank : std_logic;
signal count : std_logic_vector(7 downto 0) := (others => '0');

begin

process(clk)
begin
	if(rising_edge(clk)) then
		outputReady <= '0';
		
		if(vblank = '1' and lastvblank = '0') then
			count <= (others => '0');
			outputReady <= '1';
		elsif(busy = '0' and lastbusy = '1' and count /= "11111111") then --"11111111") then
			count <= std_logic_vector(unsigned(count) + 1);
			outputReady <= '1';
		end if;
		
		lastbusy <= busy;
		lastvblank <= vblank;
	end if;
end process;

resultAddr <= "0" & count;
outputAddr <= resultData(31 downto 24);
outputData <= resultData(23 downto  0);

end Behavioral;

