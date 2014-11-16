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

entity formatDetector is
	Port (
		clk    : in std_logic;
		ce2    : in std_logic;
		hblank : in std_logic;
		vblank : in std_logic;
		r      : in std_logic_vector(7 downto 0);
		g      : in std_logic_vector(7 downto 0);
		b      : in std_logic_vector(7 downto 0);

		cfgclk      : in std_logic;
		threshold   : in std_logic_vector(7 downto 0);
		xSize       : out std_logic_vector(11 downto 0);
		xPreActive  : out std_logic_vector(11 downto 0);
		xPostActive : out std_logic_vector(11 downto 0);
		ySize       : out std_logic_vector(10 downto 0);
		yPreActive  : out std_logic_vector(10 downto 0);
		yPostActive : out std_logic_vector(10 downto 0);

		formatChanged : out std_logic
	);
end formatDetector;

architecture Behavioral of formatDetector is

signal xSizeReg       : std_logic_vector(11 downto 0);
signal xPreActiveReg  : std_logic_vector(11 downto 0);
signal xPostActiveReg : std_logic_vector(11 downto 0);
signal ySizeReg       : std_logic_vector(10 downto 0);
signal yPreActiveReg  : std_logic_vector(10 downto 0);
signal yPostActiveReg : std_logic_vector(10 downto 0);

signal xCount           : std_logic_vector(11 downto 0);
signal xPreActiveCount  : std_logic_vector(11 downto 0);
signal xPostActiveCount : std_logic_vector(11 downto 0);
signal xTotalPreActiveCount : std_logic_vector(11 downto 0);
signal xTotalPostActiveCount : std_logic_vector(11 downto 0);
signal yCount           : std_logic_vector(10 downto 0);
signal yPreActiveCount  : std_logic_vector(10 downto 0);
signal yPostActiveCount : std_logic_vector(10 downto 0);

signal xStepCount : std_logic_vector(5 downto 0);
signal xTotalR    : std_logic_vector(17 downto 0);
signal xTotalG    : std_logic_vector(17 downto 0);
signal xTotalB    : std_logic_vector(17 downto 0);
signal xTotal     : std_logic_vector(19 downto 0);

signal yStepCount : std_logic_vector(4 downto 0);


signal lastvblank   : std_logic;
signal lasthblank   : std_logic;
signal startOfFrame : std_logic;
signal endOfFrame   : std_logic;
signal startOfLine  : std_logic;
signal endOfLine    : std_logic;

signal vblankCfgClk     : std_logic_vector(1 downto 0);
signal lastvblankCfgClk : std_logic;
signal endOfFrameCfgClk : std_logic;

signal xSizeCfgClk           : std_logic_vector(11 downto 0);
signal xPreActiveCfgClk      : std_logic_vector(11 downto 0);
signal xPostActiveCfgClk     : std_logic_vector(11 downto 0);
signal ySizeCfgClk           : std_logic_vector(10 downto 0);
signal yPreActiveCfgClk      : std_logic_vector(10 downto 0);
signal yPostActiveCfgClk     : std_logic_vector(10 downto 0);
signal xSizeCfgClkPrev       : std_logic_vector(11 downto 0);
signal xPreActiveCfgClkPrev  : std_logic_vector(11 downto 0);
signal xPostActiveCfgClkPrev : std_logic_vector(11 downto 0);
signal ySizeCfgClkPrev       : std_logic_vector(10 downto 0);
signal yPreActiveCfgClkPrev  : std_logic_vector(10 downto 0);
signal yPostActiveCfgClkPrev : std_logic_vector(10 downto 0);



signal gotFirstActiveColumn  : std_logic;
signal gotFirstActiveLine    : std_logic;
signal pixelIsBelowThreshold : std_logic;
signal lineIsBelowThreshold  : std_logic;

begin

process(clk)
begin
	if(rising_edge(clk)) then
		if(ce2 = '1' and vblank = '0') then
			startOfLine <= '0';
			endOfLine   <= '0';
			if(hblank = '1' and lasthblank = '0') then
				endOfLine <= '1';
			elsif(hblank = '0' and lasthblank = '1') then
				startOfLine <= '1';
			end if;
			lasthblank <= hblank;
		end if;
	end if;
end process;

process(clk)
begin
	if(rising_edge(clk)) then
		if(ce2 = '1') then
			startOfFrame <= '0';
			endOfFrame   <= '0';
			if(vblank = '0' and lastvblank = '1') then
				startOfFrame <= '1';
			elsif(vblank = '1' and lastvblank = '0') then
				endOfFrame <= '1';
			end if;
			lastvblank <= vblank;
		end if;
	end if;
end process;

process(clk)
begin
	if(rising_edge(clk)) then
		if(ce2 = '1') then
			if(startOfLine = '1') then
				xPreActiveCount <= (others => '0');
				gotFirstActiveColumn <= '0';
			elsif(pixelIsBelowThreshold = '0') then
				gotFirstActiveColumn <= '1';
			elsif(gotFirstActiveColumn = '0') then
				xPreActiveCount <= std_logic_vector(unsigned(xPreActiveCount) + 1);
			end if;
		end if;
	end if;
end process;

process(clk)
begin
	if(rising_edge(clk)) then
		if(ce2 = '1') then
			if(startOfLine = '1' or pixelIsBelowThreshold = '0') then
				xPostActiveCount <= (others => '0');
			else
				xPostActiveCount <= std_logic_vector(unsigned(xPostActiveCount) + 1);
			end if;
		end if;
	end if;
end process;

process(clk)
begin
	if(rising_edge(clk)) then
		if(ce2 = '1') then
			if(startOfFrame = '1') then
				xTotalPreActiveCount  <= (others => '1');
			elsif(endOfLine = '1' and 
			      unsigned(xPreActiveCount) < unsigned(xTotalPreActiveCount) and
			      unsigned(yCount) > unsigned(ySizeReg(10 downto 2))) then
				xTotalPreActiveCount  <= xPreActiveCount;
			end if;
		end if;
	end if;
end process;

process(clk)
begin
	if(rising_edge(clk)) then
		if(ce2 = '1') then
			if(startOfFrame = '1') then
				xTotalPostActiveCount  <= (others => '1');
			elsif(endOfLine = '1' and 
			      unsigned(xPostActiveCount) < unsigned(xTotalPostActiveCount) and
			      unsigned(yCount) < unsigned(ySizeReg(10 downto 2))) then
				xTotalPostActiveCount  <= xPostActiveCount;
			end if;
		end if;
	end if;
end process;

-- counter to for 1/64th of the horizontal resolution
process(clk)
begin
	if(rising_edge(clk)) then
		if(ce2 = '1') then
			if(startOfLine = '1' or unsigned(xStepCount) = 0) then
				xStepCount <= xSizeReg(11 downto 6);
			else
				xStepCount <= std_logic_vector(unsigned(xStepCount) - 1);
			end if;
		end if;
	end if;
end process;

-- sum together 64 pixels spread evenly across the line
process(clk)
begin
	if(rising_edge(clk)) then
		if(ce2 = '1') then
			if(startOfLine = '1') then
				xTotalR <= (others => '0');
				xTotalG <= (others => '0');
				xTotalB <= (others => '0');
			elsif(unsigned(xStepCount) = 0) then
				xTotalR <= std_logic_vector(unsigned(xTotalR) + unsigned(r));
				xTotalG <= std_logic_vector(unsigned(xTotalG) + unsigned(g));
				xTotalB <= std_logic_vector(unsigned(xTotalB) + unsigned(b));
			end if;
		end if;
	end if;
end process;

process(clk)
begin
	if(rising_edge(clk)) then
		if(ce2 = '1') then
			-- note: 2 times G so we can average with a divide by 4
			xTotal <= std_logic_vector(unsigned("00" & xTotalR) + unsigned("0" & xTotalG & "0") + unsigned("00" & xTotalB));
		end if;
	end if;
end process;

-- at the end of the line, increment the bar height if the average of the 
-- 64 samples are below the threshold
process(clk)
begin
	if(rising_edge(clk)) then
		if(ce2 = '1') then
			if(startOfFrame = '1') then
				yPreActiveCount <= (others => '0');
				gotFirstActiveLine <= '0';
			elsif(endOfLine = '1') then
				if(lineIsBelowThreshold = '0') then
					gotFirstActiveLine <= '1';
				elsif(gotFirstActiveLine = '0') then
					yPreActiveCount <= std_logic_vector(unsigned(yPreActiveCount) + 1);
				end if;
			end if;
		end if;
	end if;
end process;

process(clk)
begin
	if(rising_edge(clk)) then
		if(ce2 = '1') then
			if(startOfFrame = '1') then
				yPostActiveCount <= (others => '0');
			elsif(endOfLine = '1') then
				if(lineIsBelowThreshold = '0') then
					yPostActiveCount <= (others => '0');
				else
					yPostActiveCount <= std_logic_vector(unsigned(yPostActiveCount) + 1);
				end if;
			end if;
		end if;
	end if;
end process;
				

process(clk)
begin
	if(rising_edge(clk)) then
		if(ce2 = '1') then
			if(startOfLine = '1') then
				xCount <= (others => '0');
			else
				xCount <= std_logic_vector(unsigned(xCount) + 1);
			end if;
		end if;
	end if;
end process;

process(clk)
begin
	if(rising_edge(clk)) then
		if(ce2 = '1') then
			if(startOfFrame = '1') then
				yCount <= (others => '0');
			elsif(endOfLine = '1') then
				yCount <= std_logic_vector(unsigned(yCount) + 1);
			end if;
		end if;
	end if;
end process;

process(clk)
begin
	if(rising_edge(clk)) then
		if(ce2 = '1') then
			if(endOfFrame = '1') then
				xSizeReg        <= xCount;
				xPreActiveReg   <= xTotalPreActiveCount;
				xPostActiveReg  <= xTotalPostActiveCount;
				ySizeReg        <= yCount;
				yPreActiveReg   <= yPreActiveCount;
				yPostActiveReg  <= yPostActiveCount;
			end if;
		end if;
	end if;
end process;

process(cfgclk)
begin
	if(rising_edge(cfgclk)) then
		vblankCfgClk <= vblankCfgClk(0) & vblank;
	end if;
end process;

process(cfgclk)
begin
	if(rising_edge(cfgclk)) then
		endOfFrameCfgClk <= '0';
		if(vblankCfgClk(1) = '1' and lastvblankCfgClk = '0') then
			endOfFrameCfgClk <= '1';
		end if;
		lastvblankCfgClk <= vblankCfgClk(1);
	end if;
end process;

process(cfgclk)
begin
	if(rising_edge(cfgclk)) then
		formatChanged <= '0';

		if(endOfFrameCfgClk = '1') then
			xSizeCfgClkPrev       <= xSizeCfgClk;
			xPreActiveCfgClkPrev  <= xPreActiveCfgClk;
			xPostActiveCfgClkPrev <= xPostActiveCfgClk;
			ySizeCfgClkPrev       <= ySizeCfgClk;
			yPreActiveCfgClkPrev  <= yPreActiveCfgClk;
			yPostActiveCfgClkPrev <= yPostActiveCfgClk;

			xSizeCfgClk        <= xSizeReg;
			xPreActiveCfgClk   <= xPreActiveReg;
			xPostActiveCfgClk  <= xPostActiveReg;
			ySizeCfgClk        <= ySizeReg;
			yPreActiveCfgClk   <= yPreActiveReg;
			yPostActiveCfgClk  <= yPostActiveReg;

			if(xSizeCfgClkPrev       /= xSizeCfgClk or
			   xPreActiveCfgClkPrev  /= xPreActiveCfgClk or
			   xPostActiveCfgClkPrev /= xPostActiveCfgClk or
			   ySizeCfgClkPrev       /= ySizeCfgClk or
			   yPreActiveCfgClkPrev  /= yPreActiveCfgClk or
			   yPostActiveCfgClkPrev /= yPostActiveCfgClk) then
				formatChanged <= '1';
			end if;
		end if;
	end if;
end process;

xSize       <= xSizeCfgClk;
xPreActive  <= xPreActiveCfgClk;
xPostActive <= xPostActiveCfgClk;
ySize       <= ySizeCfgClk;
yPreActive  <= yPreActiveCfgClk;
yPostActive <= yPostActiveCfgClk;

pixelIsBelowThreshold <= '1' when (unsigned(r) < unsigned(threshold)) and 
                                  (unsigned(g) < unsigned(threshold)) and
                                  (unsigned(b) < unsigned(threshold)) else '0';

lineIsBelowthreshold <= '1' when unsigned(xTotal(19 downto 6)) < unsigned(threshold) else '0';

end Behavioral;
