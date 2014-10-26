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
use ieee.numeric_std.all;

entity resultDistributor is
	PORT ( 
		clk             : in  std_logic;
		start           : in  std_logic;
		driverReadyVect : in  std_logic_vector(7  downto 0);
		driverStartVect : out std_logic_vector(7  downto 0);
		driverData      : out std_logic_vector(23 downto 0);

		outputMapAddr   : out std_logic_vector(11 downto 0);
		outputMapData   : in  std_logic_vector(15 downto 0);

		areaResultAddr  : out std_logic_vector(8 downto 0);
		areaResultR     : in  std_logic_vector(7 downto 0);
		areaResultG     : in  std_logic_vector(7 downto 0);
		arearesultB     : in  std_logic_vector(7 downto 0);

		colourCoefAddr  : out std_logic_vector(8 downto 0);
		colourCoefData  : in  std_logic_vector(63 downto 0);

		gammaTableRAddr : out std_logic_vector(10 downto 0);
		gammaTableRData : in  std_logic_vector(7 downto 0);
		gammaTableGAddr : out std_logic_vector(10 downto 0);
		gammaTableGData : in  std_logic_vector(7 downto 0);
		gammaTableBAddr : out std_logic_vector(10 downto 0);
		gammaTableBData : in  std_logic_vector(7 downto 0)
	);
end resultDistributor;

architecture Behavioral of resultDistributor is

signal laststart   : std_logic;
signal count       : std_logic_vector(11 downto 0);
signal lightIndex  : std_logic_vector(8 downto 0);
signal driverIndex : std_logic_vector(2 downto 0);
signal firstLight  : std_logic;
signal enabled     : std_logic;
signal startOne    : std_logic;
signal startOne_1  : std_logic;
signal resultReady : std_logic;
signal driverReady : std_logic;
signal driverStart : std_logic;
signal colourCoefIndex : std_logic_vector(2 downto 0);
signal colourTransformStart : std_logic;
signal colourTransformDone  : std_logic;
signal transformedR : std_logic_vector(7 downto 0);
signal transformedG : std_logic_vector(7 downto 0);
signal transformedB : std_logic_vector(7 downto 0);

begin

colourTransformer : entity work.colourTransformer port map (
	clk, colourTransformStart, colourTransformDone,
	colourCoefIndex, colourCoefAddr, colourCoefData, 
	areaResultR, areaResultG, areaResultB,
	transformedR, transformedG, transformedB
);


lightIndex  <= count(11 downto 3);
driverIndex <= count(2 downto 0);
driverReady <= driverReadyVect(to_integer(unsigned(driverIndex)));
driverStartVect(0) <= driverStart when driverIndex = "000" else '0';
driverStartVect(1) <= driverStart when driverIndex = "001" else '0';
driverStartVect(2) <= driverStart when driverIndex = "010" else '0';
driverStartVect(3) <= driverStart when driverIndex = "011" else '0';
driverStartVect(4) <= driverStart when driverIndex = "100" else '0';
driverStartVect(5) <= driverStart when driverIndex = "101" else '0';
driverStartVect(6) <= driverStart when driverIndex = "110" else '0';
driverStartVect(7) <= driverStart when driverIndex = "111" else '0';

process(clk)
begin
	if(rising_edge(clk)) then
		startOne <= '0';
		if(start = '1' and laststart = '0') then
			count <= (others => '0');
			startOne <= '1';
		elsif((enabled /= '1' or driverStart = '1') and count /= "111111111111") then
			count <= std_logic_vector(unsigned(count) + 1);
			startOne <= '1';
		end if;
		laststart <= start;
	end if;
end process;

-- signal colour transformer start two clocks after startOne
-- to wait for output map RAM read, and then area result RAM read
process(clk)
begin
	if(rising_edge(clk)) then
		if(enabled = '1') then
			startOne_1 <= startOne;
			colourTransformStart <= startOne_1;
		end if;
	end if;
end process;

process(clk, startOne)
begin
	if(startOne = '1') then
		resultReady <= '0';
	elsif(rising_edge(clk)) then
		if(colourTransformDone = '1') then
			resultReady <= '1';
		end if;
	end if;
end process;

firstLight     <= '1' when lightIndex = "000000000" else '0';
driverStart    <= (driverReady or firstLight) and resultReady;

outputMapAddr   <= driverIndex & lightIndex;
areaResultAddr  <= outputMapData(8  downto 0);
gammaTableRAddr <= outputMapData(11 downto 9) & transformedR;
gammaTableGAddr <= outputMapData(11 downto 9) & transformedG;
gammaTableBAddr <= outputMapData(11 downto 9) & transformedB;
colourCoefIndex <= outputMapData(14 downto 12);
enabled         <= outputMapData(15);
driverData      <= gammaTableRData & gammaTableGData & gammaTableBData;

end Behavioral;
