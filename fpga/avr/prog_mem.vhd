-------------------------------------------------------------------------------
-- 
-- Copyright (C) 2009, 2010 Dr. Juergen Sauermann
-- 
--  This code is free software: you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation, either version 3 of the License, or
--  (at your option) any later version.
--
--  This code is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this code (see the file named COPYING).
--  If not, see http://www.gnu.org/licenses/.
--
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
--
-- Module Name:    prog_mem - Behavioral 
-- Create Date:    14:09:04 10/30/2009 
-- Description:    the program memory of a CPU.
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

library unisim;
use unisim.vcomponents.all;

-- the content of the program memory.
--
use work.prog_mem_content.all;

entity prog_mem is
    port (  I_CLK       : in  std_logic;

            I_WAIT      : in  std_logic;
            I_PC        : in  std_logic_vector(15 downto 0); -- word address
            I_PM_ADR    : in  std_logic_vector(15 downto 0); -- byte address

            Q_OPC       : out std_logic_vector(31 downto 0);
            Q_PC        : out std_logic_vector(15 downto 0);
            Q_PM_DOUT   : out std_logic_vector( 7 downto 0));
end prog_mem;

architecture Behavioral of prog_mem is

constant zero_256 : bit_vector := X"00000000000000000000000000000000"
                                & X"00000000000000000000000000000000";

signal M_OPC_E1      : std_logic_vector(15 downto 0);
signal M_OPC_E2      : std_logic_vector(15 downto 0);
signal M_OPC_O1      : std_logic_vector(15 downto 0);
signal M_OPC_O2      : std_logic_vector(15 downto 0);
signal M_PMD_E1      : std_logic_vector(15 downto 0);
signal M_PMD_E2      : std_logic_vector(15 downto 0);
signal M_PMD_O1      : std_logic_vector(15 downto 0);
signal M_PMD_O2      : std_logic_vector(15 downto 0);

signal L_WAIT_N     : std_logic;
signal L_PC_0       : std_logic;
signal L_PC_11      : std_logic;
signal L_PC_E       : std_logic_vector(10 downto 1);
signal L_PC_O       : std_logic_vector(10 downto 1);
signal L_PMD        : std_logic_vector(15 downto 0);
signal L_PM_ADR_1_0 : std_logic_vector( 1 downto 0);
signal L_PM_ADR_12  : std_logic;

begin

    pe1 : RAMB16_S18_S18
	 generic map(INIT_00 => p0_00, INIT_01 => p0_01, INIT_02 => p0_02, 
                INIT_03 => p0_03, INIT_04 => p0_04, INIT_05 => p0_05,
                INIT_06 => p0_06, INIT_07 => p0_07, INIT_08 => p0_08,
                INIT_09 => p0_09, INIT_0A => p0_0A, INIT_0B => p0_0B, 
                INIT_0C => p0_0C, INIT_0D => p0_0D, INIT_0E => p0_0E,
                INIT_0F => p0_0F,
                INIT_10 => p0_10, INIT_11 => p0_11, INIT_12 => p0_12,
                INIT_13 => p0_13, INIT_14 => p0_14, INIT_15 => p0_15,
                INIT_16 => p0_16, INIT_17 => p0_17, INIT_18 => p0_18,
                INIT_19 => p0_19, INIT_1A => p0_1A, INIT_1B => p0_1B,
                INIT_1C => p0_1C, INIT_1D => p0_1D, INIT_1E => p0_1E,
                INIT_1F => p0_1F,
                INIT_20 => p0_20, INIT_21 => p0_21, INIT_22 => p0_22,
                INIT_23 => p0_23, INIT_24 => p0_24, INIT_25 => p0_25,
                INIT_26 => p0_26, INIT_27 => p0_27, INIT_28 => p0_28,
                INIT_29 => p0_29, INIT_2A => p0_2A, INIT_2B => p0_2B,
                INIT_2C => p0_2C, INIT_2D => p0_2D, INIT_2E => p0_2E,
                INIT_2F => p0_2F,
                INIT_30 => p0_30, INIT_31 => p0_31, INIT_32 => p0_32,
                INIT_33 => p0_33, INIT_34 => p0_34, INIT_35 => p0_35,
                INIT_36 => p0_36, INIT_37 => p0_37, INIT_38 => p0_38,
                INIT_39 => p0_39, INIT_3A => p0_3A, INIT_3B => p0_3B,
                INIT_3C => p0_3C, INIT_3D => p0_3D, INIT_3E => p0_3E,
                INIT_3F => p0_3F)

    port map(ADDRA => L_PC_E,                  ADDRB => I_PM_ADR(11 downto 2),
             CLKA  => I_CLK,                    CLKB  => I_CLK,
             DIA   => "0000000000000000",       DIB   => "0000000000000000",
             ENA   => L_WAIT_N,                 ENB   => '1',
             SSRA  => '0',                      SSRB  => '0',
             WEA   => '0',                      WEB   => '0',
             DOA   => M_OPC_E1,                 DOB   => M_PMD_E1,
				 DOPA  => open,                     DOPB  => open,
				 DIPA  => "00",                     DIPB  => "00");

    pe2 : RAMB16_S18_S18
	 generic map(INIT_00 => p0_00, INIT_01 => p0_01, INIT_02 => p0_02, 
                INIT_03 => p0_03, INIT_04 => p0_04, INIT_05 => p0_05,
                INIT_06 => p0_06, INIT_07 => p0_07, INIT_08 => p0_08,
                INIT_09 => p0_09, INIT_0A => p0_0A, INIT_0B => p0_0B, 
                INIT_0C => p0_0C, INIT_0D => p0_0D, INIT_0E => p0_0E,
                INIT_0F => p0_0F,
                INIT_10 => p0_10, INIT_11 => p0_11, INIT_12 => p0_12,
                INIT_13 => p0_13, INIT_14 => p0_14, INIT_15 => p0_15,
                INIT_16 => p0_16, INIT_17 => p0_17, INIT_18 => p0_18,
                INIT_19 => p0_19, INIT_1A => p0_1A, INIT_1B => p0_1B,
                INIT_1C => p0_1C, INIT_1D => p0_1D, INIT_1E => p0_1E,
                INIT_1F => p0_1F,
                INIT_20 => p0_20, INIT_21 => p0_21, INIT_22 => p0_22,
                INIT_23 => p0_23, INIT_24 => p0_24, INIT_25 => p0_25,
                INIT_26 => p0_26, INIT_27 => p0_27, INIT_28 => p0_28,
                INIT_29 => p0_29, INIT_2A => p0_2A, INIT_2B => p0_2B,
                INIT_2C => p0_2C, INIT_2D => p0_2D, INIT_2E => p0_2E,
                INIT_2F => p0_2F,
                INIT_30 => p0_30, INIT_31 => p0_31, INIT_32 => p0_32,
                INIT_33 => p0_33, INIT_34 => p0_34, INIT_35 => p0_35,
                INIT_36 => p0_36, INIT_37 => p0_37, INIT_38 => p0_38,
                INIT_39 => p0_39, INIT_3A => p0_3A, INIT_3B => p0_3B,
                INIT_3C => p0_3C, INIT_3D => p0_3D, INIT_3E => p0_3E,
                INIT_3F => p0_3F)

    port map(ADDRA => L_PC_E,                   ADDRB => I_PM_ADR(11 downto 2),
             CLKA  => I_CLK,                    CLKB  => I_CLK,
             DIA   => "0000000000000000",       DIB   => "0000000000000000",
             ENA   => L_WAIT_N,                 ENB   => '1',
             SSRA  => '0',                      SSRB  => '0',
             WEA   => '0',                      WEB   => '0',
             DOA   => M_OPC_E2,                 DOB   => M_PMD_E2,
				 DOPA  => open,                     DOPB  => open,
				 DIPA  => "00",                     DIPB  => "00");

    po1 : RAMB16_S18_S18
	 generic map(INIT_00 => p1_00, INIT_01 => p1_01, INIT_02 => p1_02, 
                INIT_03 => p1_03, INIT_04 => p1_04, INIT_05 => p1_05,
                INIT_06 => p1_06, INIT_07 => p1_07, INIT_08 => p1_08,
                INIT_09 => p1_09, INIT_0A => p1_0A, INIT_0B => p1_0B, 
                INIT_0C => p1_0C, INIT_0D => p1_0D, INIT_0E => p1_0E,
                INIT_0F => p1_0F,
                INIT_10 => p1_10, INIT_11 => p1_11, INIT_12 => p1_12,
                INIT_13 => p1_13, INIT_14 => p1_14, INIT_15 => p1_15,
                INIT_16 => p1_16, INIT_17 => p1_17, INIT_18 => p1_18,
                INIT_19 => p1_19, INIT_1A => p1_1A, INIT_1B => p1_1B,
                INIT_1C => p1_1C, INIT_1D => p1_1D, INIT_1E => p1_1E,
                INIT_1F => p1_1F,
                INIT_20 => p1_20, INIT_21 => p1_21, INIT_22 => p1_22,
                INIT_23 => p1_23, INIT_24 => p1_24, INIT_25 => p1_25,
                INIT_26 => p1_26, INIT_27 => p1_27, INIT_28 => p1_28,
                INIT_29 => p1_29, INIT_2A => p1_2A, INIT_2B => p1_2B,
                INIT_2C => p1_2C, INIT_2D => p1_2D, INIT_2E => p1_2E,
                INIT_2F => p1_2F,
                INIT_30 => p1_30, INIT_31 => p1_31, INIT_32 => p1_32,
                INIT_33 => p1_33, INIT_34 => p1_34, INIT_35 => p1_35,
                INIT_36 => p1_36, INIT_37 => p1_37, INIT_38 => p1_38,
                INIT_39 => p1_39, INIT_3A => p1_3A, INIT_3B => p1_3B,
                INIT_3C => p1_3C, INIT_3D => p1_3D, INIT_3E => p1_3E,
                INIT_3F => p1_3F)

    port map(ADDRA => L_PC_O,                   ADDRB => I_PM_ADR(11 downto 2),
             CLKA  => I_CLK,                    CLKB  => I_CLK,
             DIA   => "0000000000000000",       DIB   => "0000000000000000",
             ENA   => L_WAIT_N,                 ENB   => '1',
             SSRA  => '0',                      SSRB  => '0',
             WEA   => '0',                      WEB   => '0',
             DOA   => M_OPC_O1,                 DOB   => M_PMD_O1,
				 DOPA  => open,                     DOPB  => open,
				 DIPA  => "00",                     DIPB  => "00");
				 
    po2 : RAMB16_S18_S18
	 generic map(INIT_00 => p1_00, INIT_01 => p1_01, INIT_02 => p1_02, 
                INIT_03 => p1_03, INIT_04 => p1_04, INIT_05 => p1_05,
                INIT_06 => p1_06, INIT_07 => p1_07, INIT_08 => p1_08,
                INIT_09 => p1_09, INIT_0A => p1_0A, INIT_0B => p1_0B, 
                INIT_0C => p1_0C, INIT_0D => p1_0D, INIT_0E => p1_0E,
                INIT_0F => p1_0F,
                INIT_10 => p1_10, INIT_11 => p1_11, INIT_12 => p1_12,
                INIT_13 => p1_13, INIT_14 => p1_14, INIT_15 => p1_15,
                INIT_16 => p1_16, INIT_17 => p1_17, INIT_18 => p1_18,
                INIT_19 => p1_19, INIT_1A => p1_1A, INIT_1B => p1_1B,
                INIT_1C => p1_1C, INIT_1D => p1_1D, INIT_1E => p1_1E,
                INIT_1F => p1_1F,
                INIT_20 => p1_20, INIT_21 => p1_21, INIT_22 => p1_22,
                INIT_23 => p1_23, INIT_24 => p1_24, INIT_25 => p1_25,
                INIT_26 => p1_26, INIT_27 => p1_27, INIT_28 => p1_28,
                INIT_29 => p1_29, INIT_2A => p1_2A, INIT_2B => p1_2B,
                INIT_2C => p1_2C, INIT_2D => p1_2D, INIT_2E => p1_2E,
                INIT_2F => p1_2F,
                INIT_30 => p1_30, INIT_31 => p1_31, INIT_32 => p1_32,
                INIT_33 => p1_33, INIT_34 => p1_34, INIT_35 => p1_35,
                INIT_36 => p1_36, INIT_37 => p1_37, INIT_38 => p1_38,
                INIT_39 => p1_39, INIT_3A => p1_3A, INIT_3B => p1_3B,
                INIT_3C => p1_3C, INIT_3D => p1_3D, INIT_3E => p1_3E,
                INIT_3F => p1_3F)

    port map(ADDRA => L_PC_O,                   ADDRB => I_PM_ADR(11 downto 2),
             CLKA  => I_CLK,                    CLKB  => I_CLK,
             DIA   => "0000000000000000",       DIB   => "0000000000000000",
             ENA   => L_WAIT_N,                 ENB   => '1',
             SSRA  => '0',                      SSRB  => '0',
             WEA   => '0',                      WEB   => '0',
             DOA   => M_OPC_O2,                 DOB   => M_PMD_O2,
				 DOPA  => open,                     DOPB  => open,
				 DIPA  => "00",                     DIPB  => "00");


    -- remember I_PC0 and I_PM_ADR for the output mux.
    --
    pc0: process(I_CLK)
    begin
        if (rising_edge(I_CLK)) then
            Q_PC <= I_PC;
            L_PM_ADR_1_0 <= I_PM_ADR(1 downto 0);
				L_PM_ADR_12 <= I_PM_ADR(12);
            if ((I_WAIT = '0')) then
                L_PC_0 <= I_PC(0);
					 L_PC_11 <= I_PC(11);
            end if;
        end if;
    end process;

    L_WAIT_N <= not I_WAIT;

    -- we use two memory blocks _E and _O (even and odd).
    -- This gives us a quad-port memory so that we can access
    -- I_PC, I_PC + 1, and PM simultaneously.
    --
    -- I_PC and I_PC + 1 are handled by port A of the memory while PM
    -- is handled by port B.
    --
    -- Q_OPC(15 ... 0) shall contain the word addressed by I_PC, while
    -- Q_OPC(31 ... 16) shall contain the word addressed by I_PC + 1.
    --
    -- There are two cases:
    --
    -- case A: I_PC     is even, thus I_PC + 1 is odd
    -- case B: I_PC + 1 is odd , thus I_PC is even
    --

    L_PC_O <= I_PC(10 downto 1);
    L_PC_E <= I_PC(10 downto 1) + ("000000000" & I_PC(0));
	 Q_OPC(15 downto  0)  <= M_OPC_E1 when L_PC_11 = '0' and L_PC_0 = '0' else
	                         M_OPC_E2 when L_PC_11 = '1' and L_PC_0 = '0' else
									 M_OPC_O1 when L_PC_11 = '0' and L_PC_0 = '1' else
									 M_OPC_O2;
	 Q_OPC(31 downto  16) <= M_OPC_E1 when L_PC_11 = '0' and L_PC_0 = '1' else
	                         M_OPC_E2 when L_PC_11 = '1' and L_PC_0 = '1' else
							  		 M_OPC_O1 when L_PC_11 = '0' and L_PC_0 = '0' else
									 M_OPC_O2;
    --Q_OPC(15 downto  0) <= M_OPC_E when L_PC_0 = '0' else M_OPC_O;
    --Q_OPC(31 downto 16) <= M_OPC_E when L_PC_0 = '1' else M_OPC_O;

    L_PMD <= M_PMD_E1 when L_PM_ADR_12 = '0' and L_PM_ADR_1_0(1) = '0' else
	          M_PMD_O1 when L_PM_ADR_12 = '0' and L_PM_ADR_1_0(1) = '1' else
				 M_PMD_E2 when L_PM_ADR_12 = '1' and L_PM_ADR_1_0(1) = '0' else
				 M_PMD_O2;
    --L_PMD <= M_PMD_E               when (L_PM_ADR_1_0(1) = '0') else M_PMD_O;
    Q_PM_DOUT <= L_PMD(7 downto 0) when (L_PM_ADR_1_0(0) = '0')
            else L_PMD(15 downto 8);
end Behavioral;

