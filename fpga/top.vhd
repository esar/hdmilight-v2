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
		CLK : in STD_LOGIC;
		FLASH_CK : out STD_LOGIC;
		FLASH_CS : out STD_LOGIC;
		FLASH_SI : out  STD_LOGIC;
		FLASH_SO : in STD_LOGIC
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

component data_mem
    port (  I_CLK       : in  std_logic;

            I_ADR       : in  std_logic_vector(10 downto 0);
            I_DIN       : in  std_logic_vector(15 downto 0);
            I_WE        : in  std_logic_vector( 1 downto 0);

            Q_DOUT      : out std_logic_vector(15 downto 0));
end component;

component cpu_core
    port (  I_CLK       : in  std_logic;
            I_CE        : in  std_logic;
            I_CLR       : in  std_logic;
            I_INTVEC    : in  std_logic_vector( 5 downto 0);
            I_DIN       : in  std_logic_vector(15 downto 0);

            Q_OPC       : out std_logic_vector(15 downto 0);
            Q_PC        : out std_logic_vector(15 downto 0);
            Q_DOUT      : out std_logic_vector(15 downto 0);
            Q_ADR       : out std_logic_vector(15 downto 0);
            Q_RD_IO     : out std_logic;
            Q_WE_IO     : out std_logic;
            Q_WE_SRAM   : out std_logic_vector(1 downto 0));
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

component flashDMAController
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
	spiSi     : out std_logic;
	spiSo     : in  std_logic
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
signal MCU_INTVEC: std_logic_vector(5 downto 0);

signal MCU_INST : std_logic_vector(16-1 downto 0):=(others=>'0');
signal MCU_PC : std_logic_vector(16-1 downto 0):=(others=>'0');

-- MCU IO bus control
signal MCU_IO_RD: std_logic:= '0';
signal MCU_IO_WR: std_logic:= '0';
signal MCU_SRAM_WR : std_logic_vector(1 downto 0);
signal MCU_ADDR : std_logic_vector(15 downto 0):=(others=>'0');
signal MCU_DOUT : std_logic_vector(15 downto 0);
signal MCU_DIN  : std_logic_vector(15 downto 0);
signal MCU_IO_DATA_READ : std_logic_vector(8-1 downto 0):=(others=>'0');

signal MASTER_WE    : std_logic_vector(1 downto 0);
signal MASTER_ADDR  : std_logic_vector(15 downto 0);
signal MASTER_DOUT  : std_logic_vector(15 downto 0);
signal MASTER_DIN   : std_logic_vector(15 downto 0);

signal SRAM_WE   : std_logic_vector(1 downto 0);
signal SRAM_DOUT : std_logic_vector(15 downto 0);
signal SRAM_DIN  : std_logic_vector(15 downto 0);

-- MCU TMR
signal MCU_TIMER_VAL : std_logic_vector(32-1 downto 0):=(others=>'0');
signal MCU_TIMER_CNT : std_logic_vector(32-1 downto 0):=(others=>'0');
signal MCU_TIMER_LATCHED : std_logic_vector(32-1 downto 0):=(others=>'0');

signal DDRD : std_logic_vector(7 downto 0);
signal PIND : std_logic_vector(7 downto 0);
signal PORTD : std_logic_vector(7 downto 0);

signal DMA_SRAM_ADDR  : std_logic_vector(15 downto 0);
signal DMA_FLASH_ADDR : std_logic_vector(23 downto 0);
signal DMA_BYTE_COUNT : std_logic_vector(15 downto 0);
signal DMA_WRITE      : std_logic;
signal DMA_START      : std_logic;
signal DMA_BUSY       : std_logic;
signal DMA_WE         : std_logic;
signal DMA_ADDR       : std_logic_vector(15 downto 0);
signal DMA_DOUT       : std_logic_vector(7 downto 0);
signal DMA_DIN        : std_logic_vector(7 downto 0);
signal DMA_IN_PROGRESS: std_logic;
signal DMA_BUSY_PREV  : std_logic;

signal vidclk : std_logic;
signal viddata_r : std_logic_vector(7 downto 0);
signal viddata_g : std_logic_vector(7 downto 0);
signal viddata_b : std_logic_vector(7 downto 0);
signal hblank : std_logic_vector(1 downto 0);
signal vblank : std_logic_vector(1 downto 0);
signal AMBILIGHT_CFG_WE   : std_logic;
signal AMBILIGHT_CFG_ADDR : std_logic_vector(15 downto 0);
signal AMBILIGHT_CFG_DIN  : std_logic_vector(7 downto 0);
signal AMBILIGHT_CFG_DOUT : std_logic_vector(7 downto 0);
signal driverOutput : std_logic_vector(7 downto 0);

signal formatChanged : std_logic;

begin

-----------------------------------------------
-- Instantiation
-----------------------------------------------

ambilight : entity work.ambilight port map(vidclk, viddata_r, viddata_g, viddata_b, hblank(1), vblank(1),
                                           CLK16,
                                           AMBILIGHT_CFG_WE,
                                           AMBILIGHT_CFG_ADDR,
                                           AMBILIGHT_CFG_DIN,
                                           AMBILIGHT_CFG_DOUT,
                                           driverOutput,
                                           formatChanged);

												  
Inst_DCM32to16: DCM32to16 PORT MAP(
	CLKIN_IN => CLK,
	CLKFX_OUT => CLK16,
	CLKIN_IBUFG_OUT => open,
	CLK0_OUT => open
);

dma : flashDMAController port map(
	CLK16,

	DMA_FLASH_ADDR,
	DMA_SRAM_ADDR,
	DMA_BYTE_COUNT,
	DMA_WRITE,
	DMA_START,
	DMA_BUSY,

	DMA_WE,
	DMA_ADDR,
	DMA_DIN,
	DMA_DOUT,

	FLASH_CK,
	FLASH_CS,
	FLASH_SI,
	FLASH_SO
);

SRAM : data_mem port map(
	I_CLK   => CLK16,
	I_ADR   => MASTER_ADDR(10 downto 0),
	I_DIN   => SRAM_DIN,
	I_WE    => SRAM_WE,
	Q_DOUT  => SRAM_DOUT
);


-- Simple fixed baud UART             
U2_UART: uart port map ( CLK16, RST, UART_TX_DATA, UART_RX_DATA, UART_WR, UART_RD, 
						 UART_TX_AVAIL, UART_TX_BUSY, UART_RX_AVAIL, UART_RX_FULL, UART_RX_ERROR, 
						 rx, tx);						

-- AVR Core
U3_AVR_MCU:  cpu_core port map (
                I_CLK       => MCU_CLK,
                I_CE        => MCU_RUN,
                I_CLR       => MCU_RST,
                I_DIN       => MCU_DIN,
                I_INTVEC    => MCU_INTVEC,

                Q_ADR       => MCU_ADDR,
                Q_DOUT      => MCU_DOUT,
                Q_OPC       => MCU_INST,
                Q_PC        => MCU_PC,
                Q_RD_IO     => MCU_IO_RD,
                Q_WE_IO     => MCU_IO_WR,
                Q_WE_SRAM   => MCU_SRAM_WR);
 			
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

process(RST,CLK16)
begin
	if(RST = '1') then
		MCU_INTVEC <= (others => '0');
	elsif(rising_edge(CLK16)) then
		MCU_INTVEC <= "000000";

		if(formatChanged = '1') then
			MCU_INTVEC <= "100001";
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

		DMA_START <= '0';
		
		-- IO Read Cycle
		if (MCU_IO_RD = '1') then
								
			case MASTER_ADDR is
			
				-- 0x21 -> Uart - UDR - TX BUF
				when X"0041"  => 
            		UART_RD <= '1';
									
				when others => 
				
			end case;												
		
		end if;				
		
		-- IO Write Cycle
		if (MCU_IO_WR = '1') then
								
			case MASTER_ADDR is
			
				-- 0x21 -> Uart - UDR - TX BUF
				when X"0041"  => 
					UART_TX_DATA <= MASTER_DOUT(7 downto 0);
					UART_WR <= '1';
					
				-- 0x22 -> 32-bit Timer Control
				when X"0042"  =>		
					-- Take snapshot of current timer value				
					MCU_TIMER_LATCHED <= MCU_TIMER_VAL;
					
				when X"0049"  =>
					DDRD <= MASTER_DOUT(7 downto 0);
				when X"004b"  =>
					PORTD <= MASTER_DOUT(7 downto 0);

				when X"004c"  =>
					DMA_FLASH_ADDR(23 downto 16) <= MASTER_DOUT(7 downto 0);
				when X"004d"  =>
					DMA_FLASH_ADDR(15 downto  8) <= MASTER_DOUT(7 downto 0);
				when X"004e"  =>
					DMA_FLASH_ADDR( 7 downto  0) <= MASTER_DOUT(7 downto 0);
				when X"004f"  =>
					DMA_SRAM_ADDR(15 downto 8) <= MASTER_DOUT(7 downto 0);
				when X"0050"  =>
					DMA_SRAM_ADDR( 7 downto 0) <= MASTER_DOUT(7 downto 0);
				when X"0051"  =>
					DMA_BYTE_COUNT(15 downto 8) <= MASTER_DOUT(7 downto 0);
				when X"0052"  =>
					DMA_BYTE_COUNT( 7 downto 0) <= MASTER_DOUT(7 downto 0);
				when X"0053"  =>
					DMA_WRITE <= MASTER_DOUT(0);
					DMA_IN_PROGRESS <= '1';
					DMA_START <= '1';
				
				when others => 
				
			end case;												
		
		end if;	
	
		if(DMA_BUSY = '0' and DMA_BUSY_PREV = '1') then
			DMA_IN_PROGRESS <= '0';
		end if;

		DMA_BUSY_PREV <= DMA_BUSY;
		
	end if;
end process;

-- Asynchronous IO Read Process
process (MCU_IO_RD, MASTER_ADDR, UART_RX_ERROR, UART_TX_BUSY, UART_RX_FULL, UART_TX_AVAIL, UART_RX_AVAIL, UART_RX_DATA, MCU_TIMER_LATCHED,
         DDRD, PIND, PORTD)

begin

	-- Read cycle?
	if (MCU_IO_RD = '1') then 
							
		case MASTER_ADDR is
		
			-- 0x20 -> Uart - USR - Status Reg
			when X"0040"  => 
				MCU_IO_DATA_READ <= "000" & UART_RX_ERROR & UART_TX_BUSY & UART_RX_FULL & UART_TX_AVAIL & UART_RX_AVAIL;					
			-- 0x21 -> Uart - UDR - RX BUF
			when X"0041"  =>			
				MCU_IO_DATA_READ <= UART_RX_DATA;
									
			-- 0x22,23,24,25 -> 32-bit Timer 
			when X"0042"  =>	
				MCU_IO_DATA_READ <= MCU_TIMER_LATCHED(7 downto 0);
			when X"0043"  =>	
				MCU_IO_DATA_READ <= MCU_TIMER_LATCHED(15 downto 8);
			when X"0044"  =>	
				MCU_IO_DATA_READ <= MCU_TIMER_LATCHED(23 downto 16);
			when X"0045"  =>	
				MCU_IO_DATA_READ <= MCU_TIMER_LATCHED(31 downto 24);
								
			when X"0049"  =>
				MCU_IO_DATA_READ <= DDRD;
			when X"004a"  =>
				MCU_IO_DATA_READ <= PIND;
			when X"004b"  => 
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

process(ADV_LLC)
begin
	if(rising_edge(ADV_LLC)) then
		viddata_g <= ADV_P(23 downto 16);
		viddata_b <= ADV_P(15 downto  8);
		viddata_r <= ADV_P( 7 downto  0);
	end if;
end process;

process(ADV_LLC)
begin
	if(rising_edge(ADV_LLC)) then
		hblank <= hblank(0) & (not ADV_HS);
		vblank <= vblank(0) & (not ADV_VS);
	end if;
end process;

-----------------------------------------------
-- Combinatorial
----------------------------------------------- 
MCU_CLK <= CLK16;
MCU_RST <= RST;
MCU_RUN <= not DMA_IN_PROGRESS;

MASTER_WE   <= '0' & DMA_WE when DMA_IN_PROGRESS = '1' else MCU_SRAM_WR;
MASTER_ADDR <= DMA_ADDR when DMA_IN_PROGRESS = '1' else MCU_ADDR;
MASTER_DIN  <= "00000000" & MCU_IO_DATA_READ when MCU_IO_RD = '1' else
               SRAM_DOUT when MASTER_ADDR(15) = '0' else
               "00000000" & AMBILIGHT_CFG_DOUT;
MASTER_DOUT <= "00000000" & DMA_DOUT when DMA_IN_PROGRESS = '1' else MCU_DOUT;

MCU_DIN <= MASTER_DIN;
DMA_DIN <= MASTER_DIN(7 downto 0);

AMBILIGHT_CFG_DIN  <= MASTER_DOUT(7 downto 0);
AMBILIGHT_CFG_ADDR <= '0' & MASTER_ADDR(14 downto 0);
AMBILIGHT_CFG_WE   <= MASTER_WE(0) and MASTER_ADDR(15);

SRAM_DIN <= MASTER_DOUT;
SRAM_WE  <= MASTER_WE when MASTER_ADDR(15) = '0' else "00";

ADV_RST <= '1';
OUTPUT <= driverOutput;

ADV_SCL <= PORTD(7) when DDRD(7) = '1' else 'Z';
ADV_SDA <= PORTD(6) when DDRD(6) = '1' else 'Z';
PIND <= ADV_SCL & ADV_SDA & "000000";

vidclk <= ADV_LLC;
--viddata_g <= ADV_P(23 downto 16);
--viddata_b <= ADV_P(15 downto 8);
--viddata_r <= ADV_P(7 downto 0);
--hblank <= not ADV_HS;
--vblank <= not ADV_VS;


end Behavioral;
