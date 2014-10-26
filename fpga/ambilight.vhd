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
			  cfgaddr : in STD_LOGIC_VECTOR (15 downto 0);
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


signal vblanklast : std_logic;
signal startDistribution : std_logic;
signal delayedStartDistribution : std_logic;
signal delayedResultVblank : std_logic;
signal delayedResultAddr : std_logic_vector(8 downto 0);
signal delayedResultData : std_logic_vector(31 downto 0);

signal driverReady        : std_logic_vector(7 downto 0);
signal driverStart        : std_logic_vector(7 downto 0);
signal driverData         : std_logic_vector(23 downto 0);
signal outputMapAddr      : std_logic_vector(11 downto 0);
signal outputMapData      : std_logic_vector(15 downto 0);
signal outputMapCfgWr     : std_logic;
signal outputMapCfgAddr   : std_logic_vector(12 downto 0);
signal outputMapCfgDin    : std_logic_vector(7 downto 0);
signal outputMapCfgDout   : std_logic_vector(7 downto 0);
signal areaResultR        : std_logic_vector(7 downto 0);
signal areaResultG        : std_logic_vector(7 downto 0);
signal areaResultB        : std_logic_vector(7 downto 0);
signal colourCoefAddr     : std_logic_vector(8 downto 0);
signal colourCoefData     : std_logic_vector(63 downto 0);
signal colourCoefCfgWr    : std_logic;
signal colourCoefCfgAddr  : std_logic_vector(11 downto 0);
signal colourCoefCfgDin   : std_logic_vector(7 downto 0);
signal colourCoefCfgDout  : std_logic_vector(7 downto 0);
signal gammaTableRAddr    : std_logic_vector(10 downto 0);
signal gammaTableRData    : std_logic_vector(7 downto 0);
signal gammaTableRCfgWr   : std_logic;
signal gammaTableRCfgAddr : std_logic_vector(10 downto 0);
signal gammaTableRCfgDin  : std_logic_vector(7 downto 0);
signal gammaTableRCfgDout : std_logic_vector(7 downto 0);
signal gammaTableGAddr    : std_logic_vector(10 downto 0);
signal gammaTableGData    : std_logic_vector(7 downto 0);
signal gammaTableGCfgWr   : std_logic;
signal gammaTableGCfgAddr : std_logic_vector(10 downto 0);
signal gammaTableGCfgDin  : std_logic_vector(7 downto 0);
signal gammaTableGCfgDout : std_logic_vector(7 downto 0);
signal gammaTableBAddr    : std_logic_vector(10 downto 0);
signal gammaTableBData    : std_logic_vector(7 downto 0);
signal gammaTableBCfgWr   : std_logic;
signal gammaTableBCfgAddr : std_logic_vector(10 downto 0);
signal gammaTableBCfgDin  : std_logic_vector(7 downto 0);
signal gammaTableBCfgDout : std_logic_vector(7 downto 0);
signal resultDelayCfgAddr : std_logic_vector(1 downto 0);
signal resultDelayCfgDin  : std_logic_vector(7 downto 0);
signal resultDelayCfgDout : std_logic_vector(7 downto 0);
signal resultDelayCfgWe   : std_logic;
signal resultDelayFrameCount : std_logic_vector(7 downto 0);
signal resultDelayTickCount  : std_logic_vector(23 downto 0);

signal cfgArea   : std_logic;
signal cfgOutput : std_logic;
signal cfgCoef   : std_logic;
signal cfgGammaR : std_logic;
signal cfgGammaG : std_logic;
signal cfgGammaB : std_logic;
signal cfgResult : std_logic;
signal cfgStatus : std_logic;
signal cfgDelay  : std_logic;
signal cfgVect   : std_logic_vector(8 downto 0);

begin

hscale4 : entity work.hscale4 port map(vidclk, hblank, vblank, viddata_r, viddata_g, viddata_b,
                                       hblank_delayed, vblank_delayed, ce2, ce4, ravg, gavg, bavg);
  
scaler : entity work.scaler port map(vidclk, ce2, hblank_delayed, vblank_delayed, ravg, gavg, bavg, 
										       vidclk, lineBufferAddr, lineBufferData, lineReady, yPos);

lightAverager : entity work.lightAverager port map(vidclk, ce2, lineReady, yPos,
                                                   lineBufferAddr, lineBufferData,
															      cfgclk, lightCfgWe, lightCfgAddr, lightCfgDin, lightCfgDout,
															      cfgclk, resultAddr, resultData);

process(cfgclk)
begin
	if(rising_edge(cfgclk)) then
		startDistribution <= '0';
		if(vblank = '1' and vblanklast = '0') then
			startDistribution <= '1';
		end if;
		vblanklast <= vblank;
	end if;
end process;
		
resultDelay : entity work.resultDelay port map(cfgclk, 
															  startDistribution, resultAddr, resultData, 
															  delayedStartDistribution, delayedResultAddr, delayedResultData,
															  resultDelayFrameCount, resultDelayTickCount);


areaResultR <= delayedResultData(23 downto 16);
areaResultG <= delayedResultData(15 downto 8);
areaResultB <= delayedResultData(7 downto 0);

resultDistributor : entity work.resultDistributor port map(
	cfgclk, delayedStartDistribution, 
	driverReady, driverStart, driverData,
	outputMapAddr, outputMapData,
	delayedResultAddr, areaResultR, areaResultG, areaResultB,
	colourCoefAddr, colourCoefData,
	gammaTableRAddr, gammaTableRData, 
	gammaTableGAddr, gammaTableGData,
	gammaTableBAddr, gammaTableBData
); 

ws2811Driver0 : entity work.ws2811Driver port map(cfgclk, driverReady(0), driverStart(0), driverData, output(0));
ws2811Driver1 : entity work.ws2811Driver port map(cfgclk, driverReady(1), driverStart(1), driverData, output(1));
ws2811Driver2 : entity work.ws2811Driver port map(cfgclk, driverReady(2), driverStart(2), driverData, output(2));
ws2811Driver3 : entity work.ws2811Driver port map(cfgclk, driverReady(3), driverStart(3), driverData, output(3));
ws2811Driver4 : entity work.ws2811Driver port map(cfgclk, driverReady(4), driverStart(4), driverData, output(4));
ws2811Driver5 : entity work.ws2811Driver port map(cfgclk, driverReady(5), driverStart(5), driverData, output(5));
ws2811Driver6 : entity work.ws2811Driver port map(cfgclk, driverReady(6), driverStart(6), driverData, output(6));
ws2811Driver7 : entity work.ws2811Driver port map(cfgclk, driverReady(7), driverStart(7), driverData, output(7));


outputMapRam : entity work.outputconfigRam
	 port map(
		a_clk  => cfgclk,
		a_wr   => outputMapCfgWr,
		a_addr => outputMapCfgAddr,
		a_din  => outputMapCfgDin,
		a_dout => outputMapCfgDout,
		b_clk  => cfgclk,
		b_addr => outputMapAddr,
		b_data => outputMapData
	);

colourCoefRam : entity work.colourTransformerConfigRam PORT MAP(
	a_clk  => cfgclk,
	a_wr   => colourCoefCfgWr,
	a_addr => colourCoefCfgAddr,
	a_din  => colourCoefCfgDin,
	a_dout => colourCoefCfgDout,
	b_clk  => cfgclk,
	b_addr => colourCoefAddr,
	b_data => colourCoefData
);

gammaTableRRam : entity work.blockram
	generic map(
		DATA => 8,
		ADDR => 11
	)
	port map(
		a_clk  => cfgclk,
		a_en   => '1',
		a_wr   => gammaTableRCfgWr,
		a_rst  => '0',
		a_addr => gammaTableRCfgAddr,
		a_din  => gammaTableRCfgDin,
		a_dout => gammaTableRCfgDout,
		b_clk  => cfgclk,
		b_en   => '1',
		b_wr   => '0',
		b_rst  => '0',
		b_addr => gammaTableRAddr,
		b_din  => (others => '0'),
		b_dout => gammaTableRData
	);

gammaTableGRam : entity work.blockram
	generic map(
		DATA => 8,
		ADDR => 11
	)
	port map(
		a_clk  => cfgclk,
		a_en   => '1',
		a_wr   => gammaTableGCfgWr,
		a_rst  => '0',
		a_addr => gammaTableGCfgAddr,
		a_din  => gammaTableGCfgDin,
		a_dout => gammaTableGCfgDout,
		b_clk  => cfgclk,
		b_en   => '1',
		b_wr   => '0',
		b_rst  => '0',
		b_addr => gammaTableGAddr,
		b_din  => (others => '0'),
		b_dout => gammaTableGData
	);

gammaTableBRam : entity work.blockram
	generic map(
		DATA => 8,
		ADDR => 11
	)
	port map(
		a_clk  => cfgclk,
		a_en   => '1',
		a_wr   => gammaTableBCfgWr,
		a_rst  => '0',
		a_addr => gammaTableBCfgAddr,
		a_din  => gammaTableBCfgDin,
		a_dout => gammaTableBCfgDout,
		b_clk  => cfgclk,
		b_en   => '1',
		b_wr   => '0',
		b_rst  => '0',
		b_addr => gammaTableBAddr,
		b_din  => (others => '0'),
		b_dout => gammaTableBData
	);
	
resultDelayRegisters : entity work.resultDelayRegisters
		port map(
			clk => cfgclk,
			addr => resultDelayCfgAddr,
			din  => resultDelayCfgDin,
			dout => resultDelayCfgDout,
			we   => resultDelayCfgWe,
			frameCount => resultDelayFrameCount,
			tickCount  => resultDelayTickCount
		);


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

cfgOutput <= '1' when cfgaddr(15 downto 11) = "00000" or         -- 0x0000 - 0x1FFF
                      cfgaddr(15 downto 11) = "00001" or
                      cfgaddr(15 downto 11) = "00010" or
                      cfgaddr(15 downto 11) = "00011" else '0';
cfgCoef   <= '1' when cfgaddr(15 downto 11) = "00100" or         -- 0x2000 - 0x2FFF
                      cfgaddr(15 downto 11) = "00101" else '0';
cfgArea   <= '1' when cfgaddr(15 downto 11) = "00110" or         -- 0x3000 - 0x3FFF
                      cfgaddr(15 downto 11) = "00111" else '0';
cfgGammaR <= '1' when cfgaddr(15 downto 11) = "01000" else '0';  -- 0x4000 - 0x47FF
cfgGammaG <= '1' when cfgaddr(15 downto 11) = "01001" else '0';  -- 0x4800 - 0x4FFF
cfgGammaB <= '1' when cfgaddr(15 downto 11) = "01010" else '0';  -- 0x5000 - 0x57FF
cfgResult <= '1' when cfgaddr(15 downto 11) = "01011" else '0';  -- 0x5800 - 0x5FFF
cfgStatus <= '1' when cfgaddr(15 downto 11) = "01100" else '0';  -- 0x6000 - 0x67FF
cfgDelay  <= '1' when cfgaddr(15 downto 11) = "01101" else '0';  -- 0x6800 - 0x6FFF

cfgVect <= cfgOutput & cfgCoef & cfgArea & cfgGammaR & cfgGammaG & cfgGammaB & cfgResult & cfgStatus & cfgDelay;
with cfgVect select cfgdataout <= 
	outputMapCfgDout   when "100000000",
	colourCoefCfgDout  when "010000000",
	lightCfgDout       when "001000000",
	gammaTableRCfgDout when "000100000",
	gammaTableGCfgDout when "000010000",
	gammaTableBCfgDout when "000001000",
	resultCfgDout      when "000000100",
	statusLatched      when "000000010",
	resultDelayCfgDout when "000000001",
	"00000000"         when others;

outputMapCfgWr     <= cfgwe when cfgOutput = '1' else '0';
colourCoefCfgWr    <= cfgwe when cfgCoef   = '1' else '0';
lightCfgWe         <= cfgwe when cfgArea   = '1' else '0';
gammaTableRCfgWr   <= cfgwe when cfgGammaR = '1' else '0';
gammaTableGCfgWr   <= cfgwe when cfgGammaG = '1' else '0';
gammaTableBCfgWr   <= cfgwe when cfgGammaB = '1' else '0';
resultDelayCfgWe   <= cfgwe when cfgDelay  = '1' else '0';
	
outputMapCfgDin    <= cfgdatain;
colourCoefCfgDin   <= cfgdatain;
lightCfgDin        <= cfgdatain;
gammaTableRCfgDin  <= cfgdatain;
gammaTableGCfgDin  <= cfgdatain;
gammaTableBCfgDin  <= cfgdatain;
resultDelayCfgDin  <= cfgdatain;

outputMapCfgAddr   <= cfgaddr(12 downto 0);
colourCoefCfgAddr  <= cfgaddr(11 downto 0);
lightCfgAddr       <= cfgaddr(11 downto 0);
gammaTableRCfgAddr <= cfgaddr(10 downto 0);
gammaTableGCfgAddr <= cfgaddr(10 downto 0);
gammaTableBCfgAddr <= cfgaddr(10 downto 0);
resultDelayCfgAddr <= cfgaddr(1 downto 0);
															 
end Behavioral;

