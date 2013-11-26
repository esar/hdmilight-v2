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

entity ws2811Driver is
    Port ( clk : in  STD_LOGIC;
           load : in  STD_LOGIC;
           datain : in  STD_LOGIC_VECTOR (23 downto 0);
           busy : out  STD_LOGIC := '0';
           dataout : out  STD_LOGIC);
end ws2811Driver;

architecture Behavioral of ws2811Driver is

signal bitcount : std_logic_vector(4 downto 0);
signal subbitcount : std_logic_vector(4 downto 0);

--signal count       : std_logic_vector(9 downto 0) := "1011111000";
signal countEnable : std_logic;

signal shiftData   : std_logic_vector(23 downto 0);
signal shiftEnable : std_logic;
signal shiftOutput : std_logic;

signal nextOutput  : std_logic;

begin

process(clk)
begin
	if(rising_edge(clk)) then
		if(load = '1') then
			bitcount <= (others => '0');
			subbitcount <= (others => '0');
		elsif(countEnable = '1') then
			subbitcount <= std_logic_vector(unsigned(subbitcount) + 1);
			if(subbitcount = "10011") then
				bitcount <= std_logic_vector(unsigned(bitcount) + 1);
				subbitcount <= (others => '0');
			end if;
		end if;
	end if;
end process;

--process(clk)
--begin
--	if(rising_edge(clk)) then
--		if(load = '1') then
--			count <= (others => '0');
--		elsif(countEnable = '1') then
--			count <= std_logic_vector(unsigned(count) + 1);
--		end if;
--	end if;
--end process;

process(clk)
begin
	if(rising_edge(clk)) then
		if(load = '1') then
			shiftData <= datain;
		elsif(shiftEnable = '1') then
			shiftData <= shiftData(22 downto 0) & "0";
		end if;
	end if;
end process;

process(clk)
begin
	if(rising_edge(clk)) then
		dataout <= nextOutput;
	end if;
end process;

process(clk)
begin
	if(rising_edge(clk)) then
		if(load = '1') then
			busy <= '1';
		elsif (bitcount = "10111" and subbitcount = "10001") then
			busy <= '0';
		end if;
	end if;
end process;

-- freeze counter when it reaches 24 bytes (24*4 clocks)
countEnable <= '0' when bitcount = "10111" and subbitcount = "10011" else '1';

-- enable shift every 4 clocks
shiftEnable <= '1' when subbitcount = "10011" else '0';

shiftOutput <= shiftData(23);

-- over a 4 clock period:
--     for a 1 output 1 1 1 0
--     for a 0 output 1 0 0 0
nextOutput <= '1' when subbitcount(4 downto 2) = "000" else
              '0' when subbitcount(4 downto 2) = "100" else
              shiftOutput;


end Behavioral;

