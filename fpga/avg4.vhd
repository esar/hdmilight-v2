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

entity hscale4 is
    Port ( CLK : in  STD_LOGIC;
           D_HSYNC : in  STD_LOGIC;
           D_VSYNC : in  STD_LOGIC;
	        D_R : in  STD_LOGIC_VECTOR (7 downto 0);
           D_G : in  STD_LOGIC_VECTOR (7 downto 0);
           D_B : in  STD_LOGIC_VECTOR (7 downto 0);
           Q_HSYNC : out  STD_LOGIC;
           Q_VSYNC : out  STD_LOGIC;
			  CE2 : out STD_LOGIC;
			  CE4 : out STD_LOGIC;
           Q_R : out  STD_LOGIC_VECTOR (7 downto 0);
           Q_G : out  STD_LOGIC_VECTOR (7 downto 0);
           Q_B : out  STD_LOGIC_VECTOR (7 downto 0));
end hscale4;

architecture Behavioral of hscale4 is

signal COUNT : std_logic_vector(1 downto 0);

signal HSYNC_LAST : std_logic;

signal R1    : std_logic_vector(7 downto 0);
signal R2    : std_logic_vector(7 downto 0);
signal R3    : std_logic_vector(7 downto 0);
signal RSUM1 : std_logic_vector(8 downto 0);
signal RSUM2 : std_logic_vector(8 downto 0);
signal RSUM3 : std_logic_vector(8 downto 0);
signal RAVG  : std_logic_vector(7 downto 0);

signal G1    : std_logic_vector(7 downto 0);
signal G2    : std_logic_vector(7 downto 0);
signal G3    : std_logic_vector(7 downto 0);
signal GSUM1 : std_logic_vector(8 downto 0);
signal GSUM2 : std_logic_vector(8 downto 0);
signal GSUM3 : std_logic_vector(8 downto 0);
signal GAVG  : std_logic_vector(7 downto 0);

signal B1    : std_logic_vector(7 downto 0);
signal B2    : std_logic_vector(7 downto 0);
signal B3    : std_logic_vector(7 downto 0);
signal BSUM1 : std_logic_vector(8 downto 0);
signal BSUM2 : std_logic_vector(8 downto 0);
signal BSUM3 : std_logic_vector(8 downto 0);
signal BAVG  : std_logic_vector(7 downto 0);

signal HSYNC1 : std_logic;
signal HSYNC2 : std_logic;
signal HSYNC3 : std_logic;
signal HSYNC4 : std_logic;
signal HSYNC5 : std_logic;
signal HSYNC6 : std_logic;
signal HSYNC7 : std_logic;

signal VSYNC1 : std_logic;
signal VSYNC2 : std_logic;
signal VSYNC3 : std_logic;
signal VSYNC4 : std_logic;
signal VSYNC5 : std_logic;
signal VSYNC6 : std_logic;
signal VSYNC7 : std_logic;

begin
	process(CLK)
	begin
		if(rising_edge(CLK)) then
			if(D_HSYNC = '0' and HSYNC_LAST = '1') then
				COUNT <= (others => '0');
			else
				COUNT <= std_logic_vector(unsigned(COUNT) + 1);
			end if;
			HSYNC_LAST <= D_HSYNC;
		end if;
	end process;

	process(CLK)
	begin
		if(rising_edge(CLK)) then
			R3 <= R2;
			R2 <= R1;
			R1 <= D_R;

			RSUM1 <= std_logic_vector(unsigned('0' & D_R) + unsigned('0' & R1));
			RSUM2 <= std_logic_vector(unsigned('0' & R2) + unsigned('0' & R3));
			RSUM3 <= std_logic_vector(unsigned('0' & RSUM1(8 downto 1)) + unsigned('0' & RSUM2(8 downto 1)));
			
			if(COUNT(1 downto 0) = "01") then
				RAVG <= RSUM3(8 downto 1);
			end if;
		end if;
	end process;

	process(CLK)
	begin
		if(rising_edge(CLK)) then
			G3 <= G2;
			G2 <= G1;
			G1 <= D_G;

			GSUM1 <= std_logic_vector(unsigned('0' & D_G) + unsigned('0' & G1));
			GSUM2 <= std_logic_vector(unsigned('0' & G2) + unsigned('0' & G3));
			GSUM3 <= std_logic_vector(unsigned('0' & GSUM1(8 downto 1)) + unsigned('0' & GSUM2(8 downto 1)));
			
			if(COUNT(1 downto 0) = "11") then
				GAVG <= GSUM3(8 downto 1);
			end if;
		end if;
	end process;

	process(CLK)
	begin
		if(rising_edge(CLK)) then
			B3 <= B2;
			B2 <= B1;
			B1 <= D_B;

			BSUM1 <= std_logic_vector(unsigned('0' & D_B) + unsigned('0' & B1));
			BSUM2 <= std_logic_vector(unsigned('0' & B2) + unsigned('0' & B3));
			BSUM3 <= std_logic_vector(unsigned('0' & BSUM1(8 downto 1)) + unsigned('0' & BSUM2(8 downto 1)));
			
			if(COUNT(1 downto 0) = "11") then
				BAVG <= BSUM3(8 downto 1);
			end if;
		end if;
	end process;

	process(CLK)
	begin
		if(rising_edge(CLK)) then
			HSYNC7 <= HSYNC6;
			HSYNC6 <= HSYNC5;
			HSYNC5 <= HSYNC4;
			HSYNC4 <= HSYNC3;
			HSYNC3 <= HSYNC2;
			HSYNC2 <= HSYNC1;
			HSYNC1 <= D_HSYNC;
		end if;
	end process;

	process(CLK)
	begin
		if(rising_edge(CLK)) then
			VSYNC7 <= VSYNC6;
			VSYNC6 <= VSYNC5;
			VSYNC5 <= VSYNC4;
			VSYNC4 <= VSYNC3;
			VSYNC3 <= VSYNC2;
			VSYNC2 <= VSYNC1;
			VSYNC1 <= D_VSYNC;
		end if;
	end process;

	Q_HSYNC <= HSYNC7;
	Q_VSYNC <= VSYNC7;
	Q_R <= RAVG;
	Q_G <= GAVG;
	Q_B <= BAVG;
	
	CE2 <= COUNT(0);
	CE4 <= COUNT(1);
	
end Behavioral;

