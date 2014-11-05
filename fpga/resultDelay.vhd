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

entity resultDelay is
    Port ( clk : in  STD_LOGIC;
           in_vblank : in  STD_LOGIC;
           in_addr : out  STD_LOGIC_VECTOR (8 downto 0);
           in_data : in  STD_LOGIC_VECTOR (31 downto 0);
           out_vblank : out  STD_LOGIC;
           out_addr : in  STD_LOGIC_VECTOR (8 downto 0);
           out_data : out  STD_LOGIC_VECTOR (31 downto 0);
           delayFrames : in std_logic_vector(7 downto 0);
           delayTicks  : in std_logic_vector(23 downto 0);
	   temporalSmoothingRatio : in std_logic_vector(8 downto 0)
    );
end resultDelay;

architecture Behavioral of resultDelay is

signal lastvblank : std_logic;
signal start : std_logic;
signal enable : std_logic;
signal count : std_logic_vector(10 downto 0);
signal tickcount : std_logic_vector(23 downto 0);
signal count_ram_in : std_logic_vector(2 downto 0) := "000";
signal count_ram_in_prev : std_logic_vector(2 downto 0);
signal count_ram_out : std_logic_vector(2 downto 0);
signal done : std_logic;
signal lastdone : std_logic;

signal coef  : std_logic_vector(9 downto 0);
signal Rin   : std_logic_vector(7 downto 0);
signal Gin   : std_logic_vector(7 downto 0);
signal Bin   : std_logic_vector(7 downto 0);
signal Rprod : std_logic_vector(35 downto 0);
signal Gprod : std_logic_vector(35 downto 0);
signal Bprod : std_logic_vector(35 downto 0);
signal Radd  : std_logic_vector(35 downto 0) := (others => '0');
signal Gadd  : std_logic_vector(35 downto 0) := (others => '0');
signal Badd  : std_logic_vector(35 downto 0) := (others => '0');

signal ram_wr_in : std_logic;
signal ram_addr_in : std_logic_vector(10 downto 0);
signal ram_data_in : std_logic_vector(31 downto 0);
signal ram_addr_out : std_logic_vector(10 downto 0);
signal ram_data_out : std_logic_vector(31 downto 0);

begin
delayRam : entity work.blockram
  GENERIC MAP(
    ADDR => 11,
	 DATA => 32
  )
  PORT MAP (
    a_clk => clk,
    a_en => '1',
    a_wr => ram_wr_in,
    a_rst => '0',
    a_addr => ram_addr_in,
    a_din => ram_data_in,
    a_dout => open,
    b_clk => clk,
    b_en => '1',
    b_wr => '0',
    b_rst => '0',
    b_addr => ram_addr_out,
    b_din => (others=> '0'),
    b_dout => ram_data_out
  );


-- generate start pulse when incoming vblank goes high
process(clk)
begin
	if(rising_edge(clk)) then
		if(in_vblank = '1' and lastvblank = '0') then
			start <= '1';
		else
			start <= '0';
		end if;

		lastvblank <= in_vblank;
	end if;
end process;

-- increment write address once per frame (when we get the start pulse)
process(clk)
begin
	if(rising_edge(clk)) then
		if(start = '1') then
			count_ram_in_prev <= count_ram_in;
			count_ram_in <= std_logic_vector(unsigned(count_ram_in) + 1);
		end if;
	end if;
end process;

-- set the read address to the write address minus the required delay (in whole frames)
count_ram_out <= std_logic_vector(unsigned(count_ram_in) - unsigned(delayFrames(2 downto 0)));

-- counter for copying the 256 values from the current set of results to the delay ram
-- while applying temporal smoothing. There are four counts per item copied:
--     1) start read of incoming value and prev value
--     2) multiply incoming value with ratio
--     3) multiply previous value with inverse ratio
--     4) write result
process(clk)
begin
	if(rising_edge(clk)) then
		if(start = '1') then
			count <= (others => '0');
		elsif(enable = '1') then
			count <= std_logic_vector(unsigned(count) + 1);
		else
			count <= count;
		end if;
	end if;
end process;


-- select the inputs for the multiplies
coef  <= std_logic_vector(512 - unsigned('0' & temporalSmoothingRatio)) when count(1 downto 0) = "01" else ('0' & temporalSmoothingRatio);
with count(1 downto 0) select Rin <=
     in_data( 7 downto  0) when "01",
     ram_data_out( 7 downto  0) when "10",
     (others => '0') when others;
with count(1 downto 0) select Gin <=
     in_data(15 downto  8) when "01",
     ram_data_out(15 downto  8) when "10",
     (others => '0') when others;
with count(1 downto 0) select Bin <=
     in_data(23 downto  16) when "01",
     ram_data_out(23 downto 16) when "10",
     (others => '0') when others;
Radd  <= (others => '0') when count(1 downto 0) /= "10" else Rprod;
Gadd  <= (others => '0') when count(1 downto 0) /= "10" else Gprod;
Badd  <= (others => '0') when count(1 downto 0) /= "10" else Bprod;

process(clk)
begin
	if(rising_edge(clk)) then
		Rprod <= std_logic_vector(unsigned("0" & Rin & "000000000") * unsigned("00000000" & coef) + unsigned(Radd));
		Gprod <= std_logic_vector(unsigned("0" & Gin & "000000000") * unsigned("00000000" & coef) + unsigned(Gadd));
		Bprod <= std_logic_vector(unsigned("0" & Bin & "000000000") * unsigned("00000000" & coef) + unsigned(Badd));
	end if;
end process;


-- counter for tick delay, start counting down toward zero when copying of current results finishes
process(clk)
begin
	if(rising_edge(clk)) then
		if(enable = '1') then
			tickcount <= delayTicks;
		elsif(unsigned(tickcount) /= 0) then
			tickcount <= std_logic_vector(unsigned(tickcount) - 1);
		end if;
	end if;
end process;

enable <= not count(10);

-- signal out_vblank after copy has finished and tickcount has reached zero
done <= '1' when unsigned(tickcount) = 0 and enable = '0' else '0';
process(clk)
begin
	if(rising_edge(clk)) then
		out_vblank <= '0';
		if(done = '1' and lastdone = '0') then
			out_vblank <= '1';
		end if;
		lastdone <= done;
	end if;
end process;

in_addr <= "0" & count(9 downto 2);
ram_addr_in <= count_ram_in & count(9 downto 2);
ram_data_in <= "00000000" & Bprod(25 downto 18) & Gprod(25 downto 18) & Rprod(25 downto 18);
ram_wr_in <= '1' when count(1 downto 0) = "11" else '0';

ram_addr_out <= (count_ram_in_prev & count(9 downto 2)) when enable = '1' else (count_ram_out & out_addr(7 downto 0));
out_data <= ram_data_out;

end Behavioral;

