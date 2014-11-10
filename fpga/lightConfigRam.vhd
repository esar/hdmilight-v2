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
 
entity lightConfigRam is
port (
    -- Port A
    a_clk   : in  std_logic;
    a_wr    : in  std_logic;
    a_addr  : in  std_logic_vector(11 downto 0);
    a_din   : in  std_logic_vector(7 downto 0);
    a_dout  : out std_logic_vector(7 downto 0);
     
    -- Port B
    b_clk   : in  std_logic;
    b_addr  : in  std_logic_vector(8 downto 0);
    b_dout  : out std_logic_vector(31 downto 0)
);
end lightConfigRam;
 
architecture Behavioral of lightConfigRam is
	type mem_t is array (0 to 2047) of std_logic_vector(7 downto 0);
	shared variable mem : mem_t := (others => "11111111");
begin
 
-- Port A
process(a_clk)
begin
    if(rising_edge(a_clk)) then
        if(a_wr = '1' and a_addr(11) = '0') then
            mem(conv_integer(a_addr)) := a_din;
        end if;
        a_dout <= mem(conv_integer(a_addr));
    end if;
end process;
 
-- Port B
process(b_clk)
begin
    if(rising_edge(b_clk)) then
        b_dout(31 downto 24) <= mem(conv_integer(b_addr & "11"));
        b_dout(23 downto 16) <= mem(conv_integer(b_addr & "10"));
        b_dout(15 downto  8) <= mem(conv_integer(b_addr & "01"));
        b_dout( 7 downto  0) <= mem(conv_integer(b_addr & "00"));
    end if;
end process;
 
end Behavioral;
