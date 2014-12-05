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

entity lightAverager is
	port ( 
		CLK :   in std_logic;
		CE  :   in std_logic;

		START : in std_logic;
		YPOS  : in std_logic_vector(5 downto 0);

		LINE_BUF_ADDR : out std_logic_vector(6 downto 0);
		LINE_BUF_DATA : in  std_logic_vector(23 downto 0);

		CFG_CLK : in std_logic;
		CFG_WE : in std_logic;
		CFG_ADDR : in std_logic_vector(11 downto 0);
		CFG_DIN : in std_logic_vector(7 downto 0);
		CFG_DOUT : out std_logic_vector(7 downto 0);

		RESULT_CLK  : in  std_logic;
		RESULT_ADDR : in  std_logic_vector(8 downto 0);
		RESULT_DATA : out std_logic_vector(31 downto 0)
	);
end lightAverager;


architecture Behavioral of lightAverager is

                         -- count is overflow (1), xpos (6), light (8), read/write (1) = 16 bits
signal COUNT           : std_logic_vector(15 downto 0);
signal FRAME           : std_logic := '0';
signal RUNNING         : std_logic;
signal WRITE_CYCLE     : std_logic;
signal XPOS            : std_logic_vector(5 downto 0);
signal LIGHT_ADDR      : std_logic_vector(7 downto 0);
signal WRITE_ENABLE    : std_logic;
signal WRITE_ADDR      : std_logic_vector(7 downto 0);
signal WRITE_DATA      : std_logic_vector(71 downto 0);

signal CONFIG_ADDR     : std_logic_vector(8 downto 0);
signal CONFIG_DATA     : std_logic_vector(31 downto 0);

signal RESULT_RAM_ADDR   : std_logic_vector(8 downto 0);
signal RESULT_RAM_WE     : std_logic;
signal RESULT_RAM_D      : std_logic_vector(71 downto 0);
signal RESULT_RAM_Q      : std_logic_vector(71 downto 0);

signal RESULT_RAM_B_ADDR : std_logic_vector(8 downto 0);
signal RESULT_RAM_B_Q    : std_logic_vector(71 downto 0);

signal XMIN_p0         : std_logic_vector(5 downto 0);
signal XMAX_p0         : std_logic_vector(5 downto 0);
signal YMIN_p0         : std_logic_vector(5 downto 0);
signal YMAX_p0         : std_logic_vector(5 downto 0);
signal SHIFT_p0        : std_logic_vector(3 downto 0);
signal R_TOTAL_p0      : std_logic_vector(20 downto 0);
signal G_TOTAL_p0      : std_logic_vector(20 downto 0);
signal B_TOTAL_p0      : std_logic_vector(20 downto 0);
signal R_p0            : std_logic_vector(7 downto 0);
signal G_p0            : std_logic_vector(7 downto 0);
signal B_p0            : std_logic_vector(7 downto 0);
signal WRITE_ADDR_p0   : std_logic_vector(7 downto 0);
signal XPOS_p0         : std_logic_vector(5 downto 0);
signal YPOS_p0         : std_logic_vector(5 downto 0);
signal RUNNING_p0      : std_logic;

signal SHIFT_p1        : std_logic_vector(3 downto 0);
signal R_TOTAL_p1      : std_logic_vector(20 downto 0);
signal G_TOTAL_p1      : std_logic_vector(20 downto 0);
signal B_TOTAL_p1      : std_logic_vector(20 downto 0);
signal WRITE_ADDR_p1   : std_logic_vector(7 downto 0);
signal WRITE_ENABLE_p1 : std_logic;

signal R_TOTAL_p2      : std_logic_vector(20 downto 0);
signal G_TOTAL_p2      : std_logic_vector(20 downto 0);
signal B_TOTAL_p2      : std_logic_vector(20 downto 0);
signal WRITE_ADDR_p2   : std_logic_vector(7 downto 0);
signal WRITE_ENABLE_p2 : std_logic;

begin

resultBuffer : entity work.blockram
  GENERIC MAP(
    ADDR => 9,
	 DATA => 72
  )
  PORT MAP (
    a_clk => CLK,
    a_en => CE,
    a_wr => RESULT_RAM_WE,
	 a_rst => '0',
    a_addr => RESULT_RAM_ADDR,
    a_din => RESULT_RAM_D,
    a_dout => RESULT_RAM_Q,
    b_clk => RESULT_CLK,
	 b_en => '1',
    b_wr => '0',
	 b_rst => '0',
    b_addr => RESULT_RAM_B_ADDR,
    b_din => (others=> '0'),
    b_dout => RESULT_RAM_B_Q
  );

configRam : entity work.lightConfigRam
	PORT MAP (
		a_clk => CFG_CLK,
		a_wr => CFG_WE,
		a_addr => CFG_ADDR,
		a_din => CFG_DIN,
		a_dout => CFG_DOUT,
		b_clk => CLK,
		b_addr => CONFIG_ADDR,
		b_dout => CONFIG_DATA
	);
	
process(CLK)
begin
	if(rising_edge(CLK)) then
		if(CE = '1') then
			if(START = '1') then
				COUNT <= (others => '0');
				RUNNING <= '1';
				
				if(YPOS = "000000") then
					FRAME <= not FRAME;
				end if;
			elsif(COUNT(15) = '1') then
					RUNNING <= '0';
			else
				COUNT <= std_logic_vector(unsigned(COUNT) + 1);
			end if;
		end if;
	end if;
end process;


process(CLK)
begin
	if(rising_edge(CLK)) then
		if(CE = '1') then
			-- read address is set when write_cycle = '0'
			-- so read data is available when write_cycle = '1'
			if(WRITE_CYCLE = '1') then
				XMIN_p0     <= CONFIG_DATA(5  downto  0);
				XMAX_p0     <= CONFIG_DATA(11 downto  6);
				YMIN_p0     <= CONFIG_DATA(17 downto 12);
				YMAX_p0     <= CONFIG_DATA(23 downto 18);
				SHIFT_p0    <= CONFIG_DATA(27 downto 24);

				R_TOTAL_p0 <= RESULT_RAM_Q(20 downto  0);
				G_TOTAL_p0 <= RESULT_RAM_Q(41 downto 21);
				B_TOTAL_p0 <= RESULT_RAM_Q(62 downto 42);
				
				R_p0       <= LINE_BUF_DATA(23 downto 16);
				G_p0       <= LINE_BUF_DATA(15 downto  8);
				B_p0       <= LINE_BUF_DATA( 7 downto  0);
				
				WRITE_ADDR_p0 <= LIGHT_ADDR;
				XPOS_p0       <= XPOS;
				YPOS_p0       <= YPOS;
				RUNNING_p0    <= RUNNING;
			end if;
		end if;
	end if;
end process;

process(CLK)
begin
	if(rising_edge(CLK)) then
		if(CE = '1') then
			if(RUNNING_p0 = '1' and 
				unsigned(XPOS_p0) >= unsigned(XMIN_p0) and unsigned(XPOS_p0) <= unsigned(XMAX_p0) and 
				unsigned(YPOS_p0) >= unsigned(YMIN_p0) and unsigned(YPOS_p0) <= unsigned(YMAX_p0)) then
				WRITE_ENABLE_p1 <= '1';
			else
				WRITE_ENABLE_p1 <= '0';
			end if;

			if(XPOS_p0 = XMIN_p0 and YPOS_p0 = YMIN_p0) then
				R_TOTAL_p1 <= "0000000000000" & R_p0;
				G_TOTAL_p1 <= "0000000000000" & G_p0;
				B_TOTAL_p1 <= "0000000000000" & B_p0;
			else
				R_TOTAL_p1 <= std_logic_vector(unsigned(R_TOTAL_p0) + unsigned(R_p0));
				G_TOTAL_p1 <= std_logic_vector(unsigned(G_TOTAL_p0) + unsigned(G_p0));
				B_TOTAL_p1 <= std_logic_vector(unsigned(B_TOTAL_p0) + unsigned(B_p0));
			end if;

			if(XPOS_p0 = XMAX_p0 and YPOS_p0 = YMAX_p0) then
				SHIFT_p1 <= SHIFT_p0;
			else
				SHIFT_p1 <= (others => '0');
			end if;

			WRITE_ADDR_p1 <= WRITE_ADDR_p0;
		end if;
	end if;
end process;

process(CLK)
begin
	if(rising_edge(CLK)) then
		if(CE = '1') then
			R_TOTAL_p2      <= std_logic_vector(unsigned(R_TOTAL_p1) srl to_integer(unsigned(SHIFT_p1)));
			G_TOTAL_p2      <= std_logic_vector(unsigned(G_TOTAL_p1) srl to_integer(unsigned(SHIFT_p1)));
			B_TOTAL_p2      <= std_logic_vector(unsigned(B_TOTAL_p1) srl to_integer(unsigned(SHIFT_p1)));

			WRITE_ENABLE_p2 <= WRITE_ENABLE_p1;
			WRITE_ADDR_p2   <= WRITE_ADDR_p1;
		end if;
	end if;
end process;

WRITE_ENABLE <= '1' when WRITE_ENABLE_p2 = '1' and WRITE_CYCLE = '1' else '0';
WRITE_ADDR   <= WRITE_ADDR_p2;
WRITE_DATA(20 downto  0) <= R_TOTAL_p2;
WRITE_DATA(41 downto 21) <= G_TOTAL_p2;
WRITE_DATA(62 downto 42) <= B_TOTAL_p2;
WRITE_DATA(71 downto 63) <= (others => '0');


WRITE_CYCLE <= COUNT(0);
LIGHT_ADDR  <= COUNT(8  downto 1);
XPOS        <= COUNT(14 downto 9);

CONFIG_ADDR <= "0" & LIGHT_ADDR;

LINE_BUF_ADDR   <= YPOS(0) & XPOS;

RESULT_RAM_ADDR  <= (not FRAME) & LIGHT_ADDR when WRITE_CYCLE = '0' else (not FRAME) & WRITE_ADDR;
RESULT_RAM_WE    <= '0'                      when WRITE_CYCLE = '0' else WRITE_ENABLE;
RESULT_RAM_D     <= WRITE_DATA;

RESULT_RAM_B_ADDR <= FRAME & RESULT_ADDR(7 downto 0);
RESULT_DATA( 7 downto  0) <= RESULT_RAM_B_Q( 7 downto  0);
RESULT_DATA(15 downto  8) <= RESULT_RAM_B_Q(28 downto 21);
RESULT_DATA(23 downto 16) <= RESULT_RAM_B_Q(49 downto 42);
RESULT_DATA(26 downto 24) <= RESULT_RAM_B_Q(65 downto 63);
RESULT_DATA(31 downto 27) <= (others => '0');

end Behavioral;

