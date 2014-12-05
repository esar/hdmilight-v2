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

entity colourTransformer is
	Port ( 
		clk : in std_logic;

		start : in  std_logic;
		done  : out std_logic;

		coefIndex : in  std_logic_vector(3 downto 0);
		coefAddr  : out std_logic_vector(8 downto 0);
		coefData  : in  std_logic_vector(63 downto 0);

		Rin : in std_logic_vector(7 downto 0);
		Gin : in std_logic_vector(7 downto 0);
		Bin : in std_logic_vector(7 downto 0);

		Rout : out std_logic_vector(7 downto 0);
		Gout : out std_logic_vector(7 downto 0);
		Bout : out std_logic_vector(7 downto 0)
	);
end colourTransformer;

architecture Behavioral of colourTransformer is

signal count : std_logic_vector(1 downto 0);

signal Xin : std_logic_vector(7 downto 0);

signal Rcoef : std_logic_vector(17 downto 0);
signal Gcoef : std_logic_vector(17 downto 0);
signal Bcoef : std_logic_vector(17 downto 0);

signal Radd : std_logic_vector(35 downto 0);
signal Gadd : std_logic_vector(35 downto 0);
signal Badd : std_logic_vector(35 downto 0);

signal Rprod : std_logic_vector(35 downto 0);
signal Gprod : std_logic_vector(35 downto 0);
signal Bprod : std_logic_vector(35 downto 0);

signal Sshift : std_logic_vector(5 downto 0);

begin

-- create done signal by delaying the start signal by 6 clocks
process(clk)
begin
	if(rising_edge(clk)) then
		Sshift <= Sshift(4 Downto 0) & start;
		done <= Sshift(5);
	end if;
end process;

-- counter to control matrix multiply stages
process(clk)
begin
	if(rising_edge(clk)) then
		if(start = '1') then
			count <= "00";
		else
			count <= std_logic_vector(unsigned(count) + 1);
		end if;
	end if;
end process;

-- select multiply input for each stage: R, G, B, Constant
process(clk)
begin
	if(rising_edge(clk)) then
		case count is
			when "00" => Xin <= Rin;
			when "01" => Xin <= Gin;
			when "10" => Xin <= Bin;
			when "11" => Xin <= "00000001";
			when others => Xin <= "00000000";
		end case;
	end if;
end process;

-- select adder input: zero for first stage, accumulate for remaining stages
Radd <= (others => '0') when count = "01" else Rprod;
Gadd <= (others => '0') when count = "01" else Gprod;
Badd <= (others => '0') when count = "01" else Bprod;

-- get the coefficients
--
-- Rr  Gr  Br
-- Rg  Gg  Bg
-- Rb  Gb  Bb
-- Rx  Gx  Bx

coefAddr <= "000" & coefIndex & count;
Rcoef <= coefData(17 downto 0);
Gcoef <= coefData(35 downto 18);
Bcoef <= coefData(53 downto 36);


-- multiply/accumulate
process(clk)
begin
	if(rising_edge(clk)) then
		Rprod <= std_logic_vector(signed("0" & Xin & "000000000") * signed(Rcoef) + signed(Radd));
		Gprod <= std_logic_vector(signed("0" & Xin & "000000000") * signed(Gcoef) + signed(Gadd));
		Bprod <= std_logic_vector(signed("0" & Xin & "000000000") * signed(Bcoef) + signed(Badd));
	end if;
end process;

-- clamp and deliver
process(clk)
begin
	if(rising_edge(clk)) then
		if(count = "01") then
			if(Rprod(35 downto 26) = "0000000000") then
				Rout <= Rprod(25 downto 18);
			elsif(Rprod(35) = '1') then
				Rout <= (others => '0');
			else
				Rout <= (others => '1');
			end if;
			if(Gprod(35 downto 26) = "0000000000") then
				Gout <= Gprod(25 downto 18);
			elsif(Rprod(35) = '1') then
				Gout <= (others => '0');
			else
				Gout <= (others => '1');
			end if;
			if(Bprod(35 downto 26) = "0000000000") then
				Bout <= Bprod(25 downto 18);
			elsif(Bprod(35) = '1') then
				Bout <= (others => '0');
			else
				Bout <= (others => '1');
			end if;
		end if;
	end if;
end process;

end Behavioral;

