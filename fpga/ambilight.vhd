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
			  cfglight : in STD_LOGIC_VECTOR (7 downto 0);
           cfgcomponent : in  STD_LOGIC_VECTOR (3 downto 0);
           cfgdatain : in  STD_LOGIC_VECTOR (7 downto 0);
           cfgdataout : out  STD_LOGIC_VECTOR (7 downto 0);
			  
			  output : out STD_LOGIC_VECTOR(7 downto 0));
end ambilight;

architecture Behavioral of ambilight is

COMPONENT configRam
  PORT (
    clka : IN STD_LOGIC;
    wea : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    addra : IN STD_LOGIC_VECTOR(8 DOWNTO 0);
    dina : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    douta : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    clkb : IN STD_LOGIC;
    web : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    addrb : IN STD_LOGIC_VECTOR(8 DOWNTO 0);
    dinb : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    doutb : OUT STD_LOGIC_VECTOR(31 DOWNTO 0)
  );
END COMPONENT;

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

signal configAddrA : std_logic_vector(8 downto 0);
signal configDataA : std_logic_vector(31 downto 0);
signal configWeB : std_logic_vector(0 downto 0);
signal configAddrB : std_logic_vector(8 downto 0);
signal configDataOutB : std_logic_vector(31 downto 0);
signal configDataInB : std_logic_vector(31 downto 0);
signal configDataLatched : std_logic_vector(31 downto 0);

signal resultAddr : std_logic_vector(8 downto 0);
signal resultData : std_logic_vector(31 downto 0);
signal resultLatched : std_logic_vector(31 downto 0);
signal statusLatched : std_logic_vector(7 downto 0);

signal outputStart : std_logic;
signal outputBusy  : std_logic;
signal outputAddr  : std_logic_vector( 7 downto 0);
signal outputData  : std_logic_vector(23 downto 0);

signal driverOutput : std_logic;

begin

conf : configRam
  PORT MAP (
    clka => vidclk,
    wea => "0",
    addra => configAddrA,
    dina => (others => '0'),
    douta => configDataA,
    clkb => cfgclk,
    web => configWeB,
    addrb => configAddrB,
    dinb => configDataInB,
    doutb => configDataOutB
  );

hscale4 : entity work.hscale4 port map(vidclk, hblank, vblank, viddata_r, viddata_g, viddata_b,
                                       hblank_delayed, vblank_delayed, ce2, ce4, ravg, gavg, bavg);
  
scaler : entity work.scaler port map(vidclk, ce2, hblank_delayed, vblank_delayed, ravg, gavg, bavg, 
										       vidclk, lineBufferAddr, lineBufferData, lineReady, yPos);

lightAverager : entity work.lightAverager port map(vidclk, ce2, lineReady, yPos,
                                                   lineBufferAddr, lineBufferData,
															      configAddrA, configDataA,
															      cfgclk, resultAddr, resultData);



resultDistributor : entity work.resultDistributor port map(cfgclk, vblank, 
                                                           resultAddr, resultData, 
																			  outputBusy, outputStart,
																			  outputAddr, outputData);

ws2811Driver : entity work.ws2811Driver port map(cfgclk, outputStart, outputData, outputBusy, driverOutput);														

process(cfgclk)
begin
	if(rising_edge(cfgclk)) then
		configDataLatched <= configDataOutB;
		statusLatched <= "000000" & hblank & vblank;
		if(resultAddr(7 downto 0) = cfglight) then
			resultLatched <= resultData;
		end if;
	end if;
end process;

configWeB(0) <= cfgwe;
configAddrB <= "0" & cfglight;
--resultAddr <= "0" & cfglight;

with cfgcomponent select configDataInB <= 
	configDataLatched(31 downto 6) & cfgdatain(5 downto 0) when "0000",
	configDataLatched(31 downto 12) & cfgdatain(5 downto 0) & configDataLatched(5 downto 0) when "0001",
	configDataLatched(31 downto 18) & cfgdatain(5 downto 0) & configDataLatched(11 downto 0) when "0010",
	configDataLatched(31 downto 24) & cfgdatain(5 downto 0) & configDataLatched(17 downto 0) when "0011",
	configDataLatched(31 downto 28) & cfgdatain(3 downto 0) & configDataLatched(23 downto 0) when "0100",
	configDataLatched(31 downto 31) & cfgdatain(2 downto 0) & configDataLatched(27 downto 0) when "0101",
	configDataLatched when others;
	
with cfgcomponent select cfgdataout <= 
	"00" & configDataLatched( 5 downto  0) when "0000",
	"00" & configDataLatched(11 downto  6) when "0001",
	"00" & configDataLatched(17 downto 12) when "0010",
	"00" & configDataLatched(23 downto 18) when "0011",
	"0000" & configDataLatched(27 downto 24) when "0100",
	"00000" & configDataLatched(30 downto 28) when "0101",
	resultLatched( 7 downto  0) when "1000",
	resultLatched(15 downto  8) when "1001",
	resultLatched(23 downto 16) when "1010",
	statusLatched when "1111",
	(others => '0') when others;
	
	
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

