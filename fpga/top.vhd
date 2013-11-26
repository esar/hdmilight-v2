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
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity HdmilightTop is
	Port 
	(
		ADV_P : in STD_LOGIC_VECTOR(23 downto 0);
		ADV_LLC : in STD_LOGIC;
		ADV_AP : in STD_LOGIC;
		ADV_SCLK : in STD_LOGIC;
		ADV_LRCLK : in STD_LOGIC;
		ADV_MCLK : in STD_LOGIC;
		ADV_SCL : inout STD_LOGIC;
		ADV_SDA : inout STD_LOGIC;
		ADV_INT1 : in STD_LOGIC;
		ADV_RST : out STD_LOGIC;
		ADV_HS : in STD_LOGIC;
		ADV_VS : in STD_LOGIC;
		ADV_DE : in STD_LOGIC;
		OUTPUT : out STD_LOGIC_VECTOR(7 downto 0);
		RX : in STD_LOGIC;
		TX : inout STD_LOGIC;
		CLK : in STD_LOGIC
	);
end HdmilightTop;

architecture Behavioral of HdmilightTop is

-----------------------------------------------
-- Component Definitions
-----------------------------------------------
COMPONENT DCM32to16
PORT(
	CLKIN_IN : IN std_logic;          
	CLKFX_OUT : OUT std_logic;
	CLKIN_IBUFG_OUT : OUT std_logic;
	CLK0_OUT : OUT std_logic
	);
END COMPONENT;

component cpu_core
    port (  I_CLK       : in  std_logic;
            I_CLR       : in  std_logic;
            I_INTVEC    : in  std_logic_vector( 5 downto 0);
            I_DIN       : in  std_logic_vector( 7 downto 0);

            Q_OPC       : out std_logic_vector(15 downto 0);
            Q_PC        : out std_logic_vector(15 downto 0);
            Q_DOUT      : out std_logic_vector( 7 downto 0);
            Q_ADR_IO    : out std_logic_vector( 7 downto 0);
            Q_RD_IO     : out std_logic;
            Q_WE_IO     : out std_logic);
end component;

component uart
    generic (
            divisor   : integer := 139 );
    port (
            clk       : in  std_logic;
            reset     : in  std_logic;
            --
            txdata    : in  std_logic_vector(7 downto 0);
            rxdata    : out std_logic_vector(7 downto 0);
            wr        : in  std_logic;
            rd        : in  std_logic;
            tx_avail  : out std_logic;
            tx_busy   : out std_logic;
            rx_avail  : out std_logic;
            rx_full   : out std_logic;
            rx_error  : out std_logic;
            --
            uart_rxd  : in  std_logic;
            uart_txd  : out std_logic 
    );
end component;

-----------------------------------------------
-- Signals
-----------------------------------------------

signal RST: std_logic:= '1';
signal RST_COUNT: std_logic_vector(1 downto 0):="00";

signal CLK16: std_logic;

-- UART
signal UART_TX_DATA : std_logic_vector(7 downto 0):=X"00";
signal UART_RX_DATA : std_logic_vector(7 downto 0):=X"00";
signal UART_WR : std_logic := '0';
signal UART_RD : std_logic := '0';
signal UART_TX_AVAIL : std_logic := '0';
signal UART_TX_BUSY : std_logic := '0';
signal UART_RX_AVAIL : std_logic := '0';
signal UART_RX_FULL : std_logic := '0';
signal UART_RX_ERROR : std_logic := '0';

-- MCU
signal MCU_RST: std_logic:= '1';
signal MCU_RUN: std_logic:= '0';
signal MCU_CLK: std_logic:= '0';

signal MCU_INST : std_logic_vector(16-1 downto 0):=(others=>'0');
signal MCU_PC : std_logic_vector(16-1 downto 0):=(others=>'0');

-- MCU IO bus control
signal MCU_IO_RD: std_logic:= '0';
signal MCU_IO_WR: std_logic:= '0';
signal MCU_IO_ADDR : std_logic_vector(8-1 downto 0):=(others=>'0');
signal MCU_IO_DATA_READ : std_logic_vector(8-1 downto 0):=(others=>'0');
signal MCU_IO_DATA_WRITE : std_logic_vector(8-1 downto 0):=(others=>'0');

-- MCU TMR
signal MCU_TIMER_VAL : std_logic_vector(32-1 downto 0):=(others=>'0');
signal MCU_TIMER_CNT : std_logic_vector(32-1 downto 0):=(others=>'0');
signal MCU_TIMER_LATCHED : std_logic_vector(32-1 downto 0):=(others=>'0');

signal DDRD : std_logic_vector(7 downto 0);
signal PIND : std_logic_vector(7 downto 0);
signal PORTD : std_logic_vector(7 downto 0);

signal vidclk : std_logic;
signal viddata_r : std_logic_vector(7 downto 0);
signal viddata_g : std_logic_vector(7 downto 0);
signal viddata_b : std_logic_vector(7 downto 0);
signal hblank : std_logic;
signal vblank : std_logic;
signal ambilightCfgWe : std_logic;
signal ambilightCfgLight : std_logic_vector(7 downto 0);
signal ambilightCfgComponent : std_logic_vector(3 downto 0);
signal ambilightCfgDataIn : std_logic_vector(7 downto 0);
signal ambilightCfgDataOut : std_logic_vector(7 downto 0);
signal driverOutput : std_logic_vector(7 downto 0);

begin

-----------------------------------------------
-- Instantiation
-----------------------------------------------

ambilight : entity ambilight port map(vidclk, viddata_r, viddata_g, viddata_b, hblank, vblank,
                                      CLK16,
												  ambilightCfgWe,
												  ambilightCfgLight,
												  ambilightCfgComponent,
												  ambilightCfgDataIn,
												  ambilightCfgDataOut,
												  driverOutput);

												  
Inst_DCM32to16: DCM32to16 PORT MAP(
	CLKIN_IN => CLK,
	CLKFX_OUT => CLK16,
	CLKIN_IBUFG_OUT => open,
	CLK0_OUT => open
);

-- Simple fixed baud UART             
U2_UART: uart port map ( CLK16, RST, UART_TX_DATA, UART_RX_DATA, UART_WR, UART_RD, 
						 UART_TX_AVAIL, UART_TX_BUSY, UART_RX_AVAIL, UART_RX_FULL, UART_RX_ERROR, 
						 rx, tx);						

-- AVR Core
U3_AVR_MCU:  cpu_core port map (
					 I_CLK       => MCU_CLK,
                I_CLR       => MCU_RST,
                I_DIN       => MCU_IO_DATA_READ,
                I_INTVEC    => "000000",

                Q_ADR_IO    => MCU_IO_ADDR,
                Q_DOUT      => MCU_IO_DATA_WRITE,
                Q_OPC       => MCU_INST,
                Q_PC        => MCU_PC,
                Q_RD_IO     => MCU_IO_RD,
                Q_WE_IO     => MCU_IO_WR);
 			
-----------------------------------------------
-- Implementation
-----------------------------------------------

-- Reset Generator
process (CLK16)
	
begin
	if (rising_edge(CLK16)) then
		if (RST_COUNT = X"3") then
			RST <= '0';
		else
			RST_COUNT <= RST_COUNT + 1;
		end if;
	end if;
end process;

-- IO memory space handler
process (RST,CLK16)

begin
	if (RST = '1') then 
		
		UART_TX_DATA <= X"00";
		UART_WR <= '0';
		UART_RD <= '0';	
		
		MCU_TIMER_LATCHED <= (others=>'0');
		
	elsif (rising_edge(CLK16)) then

		UART_WR <= '0';
		UART_RD <= '0';
		ambilightCfgWe <= '0';
		
		-- IO Read Cycle
		if (MCU_IO_RD = '1') then
								
			case MCU_IO_ADDR is
			
				-- 0x21 -> Uart - UDR - TX BUF
				when X"41"  => 
            		UART_RD <= '1';
									
				when others => 
				
			end case;												
		
		end if;				
		
		-- IO Write Cycle
		if (MCU_IO_WR = '1') then
								
			case MCU_IO_ADDR is
			
				-- 0x21 -> Uart - UDR - TX BUF
				when X"41"  => 
            	UART_TX_DATA <= MCU_IO_DATA_WRITE;
					UART_WR <= '1';
					
				-- 0x22 -> 32-bit Timer Control
				when X"42"  =>		
					-- Take snapshot of current timer value				
					MCU_TIMER_LATCHED <= MCU_TIMER_VAL;
					
				when X"46"  =>
					ambilightCfgLight <= MCU_IO_DATA_WRITE;
				when X"47"  =>
					ambilightCfgComponent <= MCU_IO_DATA_WRITE(3 downto 0);
				when X"48"  =>
					ambilightCfgDataIn <= MCU_IO_DATA_WRITE;
					ambilightCfgWe <= '1';
					
				when X"49"  =>
					DDRD <= MCU_IO_DATA_WRITE;
				when X"4b"  =>
					PORTD <= MCU_IO_DATA_WRITE;
				
				when others => 
				
			end case;												
		
		end if;				
		
	end if;
end process;

-- Asynchronous IO Read Process
process (MCU_IO_RD, MCU_IO_ADDR, UART_RX_ERROR, UART_TX_BUSY, UART_RX_FULL, UART_TX_AVAIL, UART_RX_AVAIL, UART_RX_DATA, MCU_TIMER_LATCHED,
         ambilightCfgDataOut, DDRD, PIND, PORTD)

begin

	-- Read cycle?
	if (MCU_IO_RD = '1') then 
							
		case MCU_IO_ADDR is
		
			-- 0x20 -> Uart - USR - Status Reg
			when X"40"  => 
				MCU_IO_DATA_READ <= "000" & UART_RX_ERROR & UART_TX_BUSY & UART_RX_FULL & UART_TX_AVAIL & UART_RX_AVAIL;					
			-- 0x21 -> Uart - UDR - RX BUF
			when X"41"  =>			
				MCU_IO_DATA_READ <= UART_RX_DATA;
									
			-- 0x22,23,24,25 -> 32-bit Timer 
			when X"42"  =>	
				MCU_IO_DATA_READ <= MCU_TIMER_LATCHED(7 downto 0);
			when X"43"  =>	
				MCU_IO_DATA_READ <= MCU_TIMER_LATCHED(15 downto 8);
			when X"44"  =>	
				MCU_IO_DATA_READ <= MCU_TIMER_LATCHED(23 downto 16);
			when X"45"  =>	
				MCU_IO_DATA_READ <= MCU_TIMER_LATCHED(31 downto 24);
								
			when X"48"  =>
				MCU_IO_DATA_READ <= ambilightCfgDataOut;
				
			when X"49"  =>
				MCU_IO_DATA_READ <= DDRD;
			when X"4a"  =>
				MCU_IO_DATA_READ <= PIND;
			when X"4b"  => 
				MCU_IO_DATA_READ <= PORTD;
				
			when others => 
				MCU_IO_DATA_READ <= X"00";
		end case;
		
	else
		MCU_IO_DATA_READ <= X"00";
	end if;
	
end process;

-- Timer (1 ms resolution)
process (RST,CLK16)

begin
	if (RST = '1') then 
		
		MCU_TIMER_VAL <= (others=>'0');
		MCU_TIMER_CNT <= (others=>'0');
		
	elsif (rising_edge(CLK16)) then

		-- 16000/0x3E80 = 1ms @ 16MHz
		if (MCU_TIMER_CNT = X"3E80") then 
			MCU_TIMER_VAL <= MCU_TIMER_VAL + 1;
			MCU_TIMER_CNT <= (others=>'0');
		else
			MCU_TIMER_CNT <= MCU_TIMER_CNT + 1;
		end if;
		
	end if;
end process;


-----------------------------------------------
-- Combinatorial
----------------------------------------------- 
MCU_CLK <= CLK16;
MCU_RST <= RST;
MCU_RUN <= '1';


ADV_RST <= '1';
OUTPUT <= driverOutput;

ADV_SCL <= PORTD(7) when DDRD(7) = '1' else 'Z';
ADV_SDA <= PORTD(6) when DDRD(6) = '1' else 'Z';
PIND <= ADV_SCL & ADV_SDA & "000000";

vidclk <= ADV_LLC;
viddata_g <= ADV_P(23 downto 16);
viddata_b <= ADV_P(15 downto 8);
viddata_r <= ADV_P(7 downto 0);
hblank <= not ADV_HS;
vblank <= not ADV_VS;

end Behavioral;
