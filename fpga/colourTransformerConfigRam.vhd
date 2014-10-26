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

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
 
entity colourTransformerConfigRam is
	port (
		-- Port A
		a_clk  : in  std_logic;
		a_wr   : in  std_logic;
		a_addr : in  std_logic_vector(11 downto 0);
		a_din  : in  std_logic_vector(7 downto 0);
		a_dout : out std_logic_vector(7 downto 0);
     
		-- Port B
		b_clk  : in  std_logic;
		b_addr : in  std_logic_vector(8 downto 0);
		b_data : out std_logic_vector(63 downto 0)
	);
end colourTransformerConfigRam;

architecture Behavioral of colourTransformerConfigRam is
	type mem_t is array (0 to 2047) of std_logic_vector(7 downto 0);
	shared variable memOdd1  : mem_t;
	shared variable memEven1 : mem_t;

	signal a_doutOdd1 : std_logic_vector(7 downto 0);
	signal a_doutEven1 : std_logic_vector(7 downto 0);
begin


-- Port A
process(a_clk)
begin
	if(rising_edge(a_clk)) then
		if(a_addr(0) = '0') then
			if(a_wr = '1') then
				memOdd1(conv_integer(a_addr(11 downto 1))) := a_din;
			end if;
		end if;
		a_doutOdd1 <= memOdd1(conv_integer(a_addr(11 downto 1)));
	end if;
end process;

process(a_clk)
begin
	if(rising_edge(a_clk)) then
		if(a_addr(0) = '1') then
			if(a_wr = '1') then
				memEven1(conv_integer(a_addr(11 downto 1))) := a_din;
			end if;
		end if;
		a_doutEven1 <= memEven1(conv_integer(a_addr(11 downto 1)));
	end if;
end process;

a_dout <= a_doutOdd1 when a_addr(0) = '0' else a_doutEven1;
 
-- Port B
process(b_clk)
begin
	if(rising_edge(b_clk)) then
		b_data( 7 downto  0) <= memOdd1(conv_integer(b_addr(8 downto 0) & "00"));
		b_data(23 downto 16) <= memOdd1(conv_integer(b_addr(8 downto 0) & "01"));
		b_data(39 downto 32) <= memOdd1(conv_integer(b_addr(8 downto 0) & "10"));
		b_data(55 downto 48) <= memOdd1(conv_integer(b_addr(8 downto 0) & "11"));
	end if;
end process;

process(b_clk)
begin
	if(rising_edge(b_clk)) then
		b_data(15 downto  8) <= memEven1(conv_integer(b_addr(8 downto 0) & "00"));
		b_data(31 downto 24) <= memEven1(conv_integer(b_addr(8 downto 0) & "01"));
		b_data(47 downto 40) <= memEven1(conv_integer(b_addr(8 downto 0) & "10"));
		b_data(63 downto 56) <= memEven1(conv_integer(b_addr(8 downto 0) & "11"));
	end if;
end process;
 
end Behavioral;
