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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity ambilight is
    Port ( vidclk : in  STD_LOGIC;
           viddata_r : in  STD_LOGIC_VECTOR (7 downto 0);
           viddata_g : in  STD_LOGIC_VECTOR (7 downto 0);
           viddata_b : in  STD_LOGIC_VECTOR (7 downto 0);
			  hblank : in STD_LOGIC;
			  vblank : in STD_LOGIC;
			  
           cfgclk : in  STD_LOGIC;
			  cfgwe : in STD_LOGIC;
			  cfgaddr : in STD_LOGIC_VECTOR (12 downto 0);
           cfgdatain : in  STD_LOGIC_VECTOR (7 downto 0);
           cfgdataout : out  STD_LOGIC_VECTOR (7 downto 0);
			  
			  output : out STD_LOGIC_VECTOR(7 downto 0));
end ambilight;

architecture Behavioral of ambilight is

signal ce2 : std_logic;
signal ce4 : std_logic;
signal hblank_delayed : std_logic;
signal vblank_delayed : std_logic;
signal ravg : std_logic_vector(7 downto 0);
signal gavg : std_logic_vector(7 downto 0);
signal bavg : std_logic_vector(7 downto 0);

signal lineBufferAddr : std_logic_vector(6 downto 0);
signal lineBufferData : std_logic_vector(23 downto 0);
signal yPos : std_logic_vector(5 downto 0);
signal lineReady : std_logic;

signal lightCfgWe : std_logic;
signal lightCfgAddr : std_logic_vector(11 downto 0);
signal lightCfgDin : std_logic_vector(7 downto 0);
signal lightCfgDout : std_logic_vector(7 downto 0);

signal resultAddr : std_logic_vector(8 downto 0);
signal resultData : std_logic_vector(31 downto 0);
signal resultLatched : std_logic_vector(31 downto 0);
signal resultCfgDout : std_logic_vector(7 downto 0);
signal statusLatched : std_logic_vector(7 downto 0);

signal resultDelayCfgWe : std_logic;
signal resultDelayCfgAddr : std_logic_vector(1 downto 0);
signal resultDelayCfgDin : std_logic_vector(7 downto 0);
signal resultDelayCfgDout : std_logic_vector(7 downto 0);
signal delayedResultVblank : std_logic;
signal delayedResultAddr : std_logic_vector(7 downto 0);
signal delayedResultData : std_logic_vector(31 downto 0);

signal outputStart : std_logic;
signal outputBusy  : std_logic;
signal outputAddr  : std_logic_vector( 7 downto 0);
signal outputData  : std_logic_vector(23 downto 0);

signal driverOutput : std_logic;

begin

hscale4 : entity work.hscale4 port map(vidclk, hblank, vblank, viddata_r, viddata_g, viddata_b,
                                       hblank_delayed, vblank_delayed, ce2, ce4, ravg, gavg, bavg);
  
scaler : entity work.scaler port map(vidclk, ce2, hblank_delayed, vblank_delayed, ravg, gavg, bavg, 
										       vidclk, lineBufferAddr, lineBufferData, lineReady, yPos);

lightAverager : entity work.lightAverager port map(vidclk, ce2, lineReady, yPos,
                                                   lineBufferAddr, lineBufferData,
															      cfgclk, lightCfgWe, lightCfgAddr, lightCfgDin, lightCfgDout,
															      cfgclk, resultAddr, resultData);

--resultDelay : entity work.resultDelay port map(cfgclk, 
--                                               resultDelayCfgWe, resultDelayCfgAddr, 
--															  resultDelayCfgDin, resultDelayCfgDout,
--															  vblank, resultAddr, resultData, 
--															  delayedResultVblank, delayedResultAddr, delayedResultData);

resultDistributor : entity work.resultDistributor port map(cfgclk, vblank, 
                                                           resultAddr(7 downto 0), resultData, 
																			  outputBusy, outputStart,
																			  outputAddr, outputData);

ws2811Driver : entity work.ws2811Driver port map(cfgclk, outputStart, outputData, outputBusy, driverOutput);														

process(cfgclk)
begin
	if(rising_edge(cfgclk)) then
		statusLatched <= "000000" & hblank & vblank;
		if(resultAddr(7 downto 0) = cfgaddr(9 downto 2)) then
			resultLatched <= resultData;
		end if;
	end if;
end process;

with cfgaddr(1 downto 0) select resultCfgDout <=
	resultLatched(7 downto 0) when "00",
	resultLatched(15 downto 8) when "01",
	resultLatched(23 downto 16) when "10",
	resultLatched(31 downto 24) when "11";

with cfgaddr(12 downto 11) select cfgdataout <= 
	lightCfgDout when "00",
	resultDelayCfgDout when "01",
	resultCfgDout when "10",
	statusLatched when "11";
	
lightCfgAddr <= "0" & cfgaddr(10 downto 0);
lightCfgWe <= cfgwe when cfgaddr(12 downto 11) = "00" else '0';
resultDelayCfgAddr <= cfgaddr(1 downto 0);
resultDelayCfgWe <= cfgwe when cfgaddr(12 downto 11) = "01" else '0';

lightCfgDin <= cfgdatain;
resultDelayCfgDin <= cfgdatain;

	
with outputAddr select output <= 
	"0000000" & driverOutput when x"00",
	"000000" & driverOutput & "0" when x"01",
	"00000" & driverOutput & "00" when x"02",
	"0000" & driverOutput & "000" when x"03",
	"000" & driverOutput & "0000" when x"04",
	"00" & driverOutput & "00000" when x"05",
	"0" & driverOutput & "000000" when x"06",
	driverOutput & "0000000" when x"07",
	"00000000" when others;
															 
end Behavioral;
