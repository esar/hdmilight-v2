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
use ieee.numeric_std.all;


entity flashDMAController is
    Port (
        clk            : in std_logic;
        flashStartAddr : in std_logic_vector(23 downto 0);
	sramStartAddr  : in std_logic_vector(15 downto 0);
	copySize       : in std_logic_vector(15 downto 0);
	write          : in std_logic;
	start          : in std_logic;
	busy           : out std_logic;

	we    : out std_logic;
	addr  : out std_logic_vector(15 downto 0);
	din   : in  std_logic_vector( 7 downto 0);
	dout  : out std_logic_vector( 7 downto 0);

	spiClk    : out std_logic;
	spiCs     : out std_logic;
	spiSi     : out  std_logic;
	spiSo     : in std_logic
    );
end flashDMAController;



architecture Behavioral of flashDMAController is

	type StateType is (
		STATE_IDLE,
		STATE_READ_INIT, STATE_READ_CMD, STATE_READ_ADDR1, STATE_READ_ADDR2, STATE_READ_ADDR3, STATE_READ_ADDR4, STATE_READ_DATA,
		STATE_WRITE_INIT
	);

	signal state     : StateType;
	signal nextState : StateType;
	signal shiftOut  : std_logic_vector(7 downto 0);
	signal shiftIn   : std_logic_vector(7 downto 0);
	signal count     : std_logic_vector(2 downto 0);
	signal flashAddr : std_logic_vector(23 downto 0);
	signal bytesLeft : std_logic_vector(15 downto 0);
	signal intAddr : std_logic_vector(15 downto 0);
	signal intWe   : std_logic;
	signal intSpiCs    : std_logic;
begin

process(clk)
begin
	if(falling_edge(clk)) then
		state <= nextState;
	end if;
end process;

process(clk)
begin
	if(falling_edge(clk)) then
		intWe   <= '0';
	
		shiftOut <= shiftOut(6 downto 0) & '0';
		shiftIn  <= shiftIn(6 downto 0) & spiSo;
		count    <= std_logic_vector(unsigned(count) + 1);
	
		if(start = '1') then
			intAddr  <= sramStartAddr;
			flashAddr <= flashStartAddr;
			bytesLeft <= copySize;
			count     <= (others => '0');

			if(write = '1') then
				nextState <= STATE_WRITE_INIT;
			else
				nextState <= STATE_READ_INIT;
			end if;
		else
			case state is
				when STATE_IDLE =>
					intSpiCs <= '1';
				when STATE_READ_INIT =>
					if(nextState = STATE_READ_CMD) then
						intSpiCs  <= '0';
					end if;
					shiftOut  <= x"0B";
					count     <= "000";
					nextState <= STATE_READ_CMD;
				when STATE_READ_CMD =>
					if(count = "111") then
						shiftOut  <= flashAddr(23 downto 16);
						nextState <= STATE_READ_ADDR1;
					end if;
				when STATE_READ_ADDR1 =>
					if(count = "111") then
						shiftOut  <= flashAddr(15 downto 8);
						nextState <= STATE_READ_ADDR2;
					end if;
				when STATE_READ_ADDR2 =>
					if(count = "111") then
						shiftOut  <= flashAddr(7 downto 0);
						nextState <= STATE_READ_ADDR3;
					end if;
				when STATE_READ_ADDR3 =>
					if(count = "111") then
						shiftOut  <= x"00";
						nextState <= STATE_READ_ADDR4;
					end if;
				when STATE_READ_ADDR4 =>
					if(count = "111") then
						nextState <= STATE_READ_DATA;
					end if;
				when STATE_READ_DATA =>
					if(count = "000") then
						dout <= shiftIn;
						intWe  <= '1';
						
						if(bytesLeft = x"0001") then
							nextState <= STATE_IDLE;
						end if;
							
						bytesLeft <= std_logic_vector(unsigned(bytesLeft) - 1);
					end if;

					if(intWe = '1') then
						intAddr <= std_logic_vector(unsigned(intAddr) + 1);
					end if;


				when STATE_WRITE_INIT =>
					nextState <= STATE_IDLE;
				--when STATE_WRITE_INIT =>
				--	intSpiCs     <= '0';
				--	shiftOut  <= x"0B";
				--	count     <= (others => '0');
				--	nextState <= STATE_WRITE_ENABLE_CMD;
				--when STATE_WRITE_ENABLE_CMD =>
				--	if(count = "111") then
				--		intSpiCs     <= '1';
				--		nextState <= STATE_WRITE_ERASE_INIT;
				--	end if;
				--when STATE_WRITE_ERASE_INIT =>
				--	intSpiCs     <= '0';
				--	shiftOut  <= x"0E";
				--	nextState <= STATE_WRITE_ERASE_CMD;
				--when STATE_WRITE_ERASE_CMD =>
				--	if(count = "111") then
				--		shiftOut  <= flashAddr(23 downto 0);
				--		nextState <= STATE_WRITE_ERASE_ADDR1;
				--	end if;
				--when STATE_WRITE_ERASE_ADDR1 =>
				--	if(count = "111") then
				--		shiftOut  <= flashAddr(16 downto  8);
				--		nextState <= STATE_WRITE_ERASE_ADDR2;
				--	end if;
				--when STATE_WRITE_ERASE_ADDR2 =>
				--	if(count = "111") then
				--		shiftOut  <= flashAddr( 7 downto  0);
				--		nextState <= STATE_WRITE_ERASE_ADDR3;
				--	end if;
				--when STATE_WRITE_ERASE_ADDR3 =>
				--	if(count = "111") then
				--		intSpiCs     <= '1';
				--		nextState <= STATE_WRITE_PROG_INIT;
				--	end if;
				--when STATE_WRITE_PROG_INIT =>
				--	intSpiCs <= '0';
				--	shiftOut <= x"13";
				--	nextState <= STATE_WRITE_PROG_CMD;
				--when STATE_WRITE_PROG_CMD =>
				--	if(count = "111") then
						
			end case;
		end if;
	end if;
    end process;


busy     <= not intSpiCs;
we   <= intWe;
addr <= intAddr;
spiClk   <= clk or intSpiCs; --when intSpiCs = '1' else '1';
spiCs    <= intSpiCs;
spiSi    <= shiftOut(7);

end Behavioral;

