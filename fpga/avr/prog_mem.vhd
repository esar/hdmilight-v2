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

entity prog_mem is
    port (  I_CLK       : in  std_logic;
            I_CE        : in  std_logic;

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
signal M_OPC_E3      : std_logic_vector(15 downto 0);
signal M_OPC_E4      : std_logic_vector(15 downto 0);
signal M_OPC_O1      : std_logic_vector(15 downto 0);
signal M_OPC_O2      : std_logic_vector(15 downto 0);
signal M_OPC_O3      : std_logic_vector(15 downto 0);
signal M_OPC_O4      : std_logic_vector(15 downto 0);
signal M_PMD_E1      : std_logic_vector(15 downto 0);
signal M_PMD_E2      : std_logic_vector(15 downto 0);
signal M_PMD_E3      : std_logic_vector(15 downto 0);
signal M_PMD_E4      : std_logic_vector(15 downto 0);
signal M_PMD_O1      : std_logic_vector(15 downto 0);
signal M_PMD_O2      : std_logic_vector(15 downto 0);
signal M_PMD_O3      : std_logic_vector(15 downto 0);
signal M_PMD_O4      : std_logic_vector(15 downto 0);

signal L_WAIT_N       : std_logic;
signal L_PC_0         : std_logic;
signal L_PC_12_11     : std_logic_vector( 1 downto 0);
signal L_PC_E         : std_logic_vector(10 downto 1);
signal L_PC_O         : std_logic_vector(10 downto 1);
signal L_PMD          : std_logic_vector(15 downto 0);
signal L_PM_ADR_1_0   : std_logic_vector( 1 downto 0);
signal L_PM_ADR_13_12 : std_logic_vector( 1 downto 0);

begin

    pe1 : RAMB16_S18_S18
    generic map(INIT_00 => X"00000000000000000000000000000000000000000000000095082411940c940e")
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
    port map(ADDRA => L_PC_E,                   ADDRB => I_PM_ADR(11 downto 2),
             CLKA  => I_CLK,                    CLKB  => I_CLK,
             DIA   => "0000000000000000",       DIB   => "0000000000000000",
             ENA   => L_WAIT_N,                 ENB   => '1',
             SSRA  => '0',                      SSRB  => '0',
             WEA   => '0',                      WEB   => '0',
             DOA   => M_OPC_E2,                 DOB   => M_PMD_E2,
             DOPA  => open,                     DOPB  => open,
             DIPA  => "00",                     DIPB  => "00");

    pe3 : RAMB16_S18_S18
    port map(ADDRA => L_PC_E,                   ADDRB => I_PM_ADR(11 downto 2),
             CLKA  => I_CLK,                    CLKB  => I_CLK,
             DIA   => "0000000000000000",       DIB   => "0000000000000000",
             ENA   => L_WAIT_N,                 ENB   => '1',
             SSRA  => '0',                      SSRB  => '0',
             WEA   => '0',                      WEB   => '0',
             DOA   => M_OPC_E3,                 DOB   => M_PMD_E3,
             DOPA  => open,                     DOPB  => open,
             DIPA  => "00",                     DIPB  => "00");

    pe4 : RAMB16_S18_S18
    port map(ADDRA => L_PC_E,                   ADDRB => I_PM_ADR(11 downto 2),
             CLKA  => I_CLK,                    CLKB  => I_CLK,
             DIA   => "0000000000000000",       DIB   => "0000000000000000",
             ENA   => L_WAIT_N,                 ENB   => '1',
             SSRA  => '0',                      SSRB  => '0',
             WEA   => '0',                      WEB   => '0',
             DOA   => M_OPC_E4,                 DOB   => M_PMD_E4,
             DOPA  => open,                     DOPB  => open,
             DIPA  => "00",                     DIPB  => "00");

    po1 : RAMB16_S18_S18
    generic map(INIT_00 => X"0000000000000000000000000000000000000000000000000000be1300000004")
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
    port map(ADDRA => L_PC_O,                   ADDRB => I_PM_ADR(11 downto 2),
             CLKA  => I_CLK,                    CLKB  => I_CLK,
             DIA   => "0000000000000000",       DIB   => "0000000000000000",
             ENA   => L_WAIT_N,                 ENB   => '1',
             SSRA  => '0',                      SSRB  => '0',
             WEA   => '0',                      WEB   => '0',
             DOA   => M_OPC_O2,                 DOB   => M_PMD_O2,
             DOPA  => open,                     DOPB  => open,
             DIPA  => "00",                     DIPB  => "00");
				 
    po3 : RAMB16_S18_S18
    port map(ADDRA => L_PC_O,                   ADDRB => I_PM_ADR(11 downto 2),
             CLKA  => I_CLK,                    CLKB  => I_CLK,
             DIA   => "0000000000000000",       DIB   => "0000000000000000",
             ENA   => L_WAIT_N,                 ENB   => '1',
             SSRA  => '0',                      SSRB  => '0',
             WEA   => '0',                      WEB   => '0',
             DOA   => M_OPC_O3,                 DOB   => M_PMD_O3,
             DOPA  => open,                     DOPB  => open,
             DIPA  => "00",                     DIPB  => "00");
				 
    po4 : RAMB16_S18_S18
    port map(ADDRA => L_PC_O,                   ADDRB => I_PM_ADR(11 downto 2),
             CLKA  => I_CLK,                    CLKB  => I_CLK,
             DIA   => "0000000000000000",       DIB   => "0000000000000000",
             ENA   => L_WAIT_N,                 ENB   => '1',
             SSRA  => '0',                      SSRB  => '0',
             WEA   => '0',                      WEB   => '0',
             DOA   => M_OPC_O4,                 DOB   => M_PMD_O4,
             DOPA  => open,                     DOPB  => open,
             DIPA  => "00",                     DIPB  => "00");


    -- remember I_PC0 and I_PM_ADR for the output mux.
    --
    pc0: process(I_CLK)
    begin
        if (I_CE = '1' and rising_edge(I_CLK)) then
            Q_PC <= I_PC;
            L_PM_ADR_1_0 <= I_PM_ADR(1 downto 0);
            L_PM_ADR_13_12 <= I_PM_ADR(13 downto 12);
            if ((I_WAIT = '0')) then
                L_PC_0 <= I_PC(0);
                L_PC_12_11 <= I_PC(12 downto 11);
            end if;
        end if;
    end process;

    L_WAIT_N <= I_CE and (not I_WAIT);

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
    Q_OPC(15 downto  0)  <= M_OPC_E1 when L_PC_12_11 = "00" and L_PC_0 = '0' else
                            M_OPC_E2 when L_PC_12_11 = "01" and L_PC_0 = '0' else
                            M_OPC_E3 when L_PC_12_11 = "10" and L_PC_0 = '0' else
                            M_OPC_E4 when L_PC_12_11 = "11" and L_PC_0 = '0' else
                            M_OPC_O1 when L_PC_12_11 = "00" and L_PC_0 = '1' else
                            M_OPC_O2 when L_PC_12_11 = "01" and L_PC_0 = '1' else
                            M_OPC_O3 when L_PC_12_11 = "10" and L_PC_0 = '1' else
                            M_OPC_O4;
    Q_OPC(31 downto  16) <= M_OPC_E1 when L_PC_12_11 = "00" and L_PC_0 = '1' else
                            M_OPC_E2 when L_PC_12_11 = "01" and L_PC_0 = '1' else
                            M_OPC_E3 when L_PC_12_11 = "10" and L_PC_0 = '1' else
                            M_OPC_E4 when L_PC_12_11 = "11" and L_PC_0 = '1' else
                            M_OPC_O1 when L_PC_12_11 = "00" and L_PC_0 = '0' else
                            M_OPC_O2 when L_PC_12_11 = "01" and L_PC_0 = '0' else
                            M_OPC_O3 when L_PC_12_11 = "10" and L_PC_0 = '0' else
                            M_OPC_O4;
    --Q_OPC(15 downto  0) <= M_OPC_E when L_PC_0 = '0' else M_OPC_O;
    --Q_OPC(31 downto 16) <= M_OPC_E when L_PC_0 = '1' else M_OPC_O;

    L_PMD <= M_PMD_E1 when L_PM_ADR_13_12 = "00" and L_PM_ADR_1_0(1) = '0' else
             M_PMD_O1 when L_PM_ADR_13_12 = "00" and L_PM_ADR_1_0(1) = '1' else
             M_PMD_E2 when L_PM_ADR_13_12 = "01" and L_PM_ADR_1_0(1) = '0' else
             M_PMD_O2 when L_PM_ADR_13_12 = "01" and L_PM_ADR_1_0(1) = '1' else
             M_PMD_E3 when L_PM_ADR_13_12 = "10" and L_PM_ADR_1_0(1) = '0' else
             M_PMD_O3 when L_PM_ADR_13_12 = "10" and L_PM_ADR_1_0(1) = '1' else
             M_PMD_E4 when L_PM_ADR_13_12 = "11" and L_PM_ADR_1_0(1) = '0' else
             M_PMD_O4;
    --L_PMD <= M_PMD_E               when (L_PM_ADR_1_0(1) = '0') else M_PMD_O;
    Q_PM_DOUT <= L_PMD(7 downto 0) when (L_PM_ADR_1_0(0) = '0')
            else L_PMD(15 downto 8);
end Behavioral;

