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
-- Module Name:    data_mem - Behavioral 
-- Create Date:    14:09:04 10/30/2009 
-- Description:    the data mempry of a CPU.
--
-------------------------------------------------------------------------------
--
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

library unisim;
use unisim.vcomponents.all;

entity data_mem is
    port (  I_CLK       : in  std_logic;

            I_ADR       : in  std_logic_vector(10 downto 0);
            I_DIN       : in  std_logic_vector(15 downto 0);
            I_WE        : in  std_logic_vector( 1 downto 0);

            Q_DOUT      : out std_logic_vector(15 downto 0));
end data_mem;

architecture Behavioral of data_mem is
 
constant zero_256 : bit_vector := X"00000000000000000000000000000000"
                                & X"00000000000000000000000000000000";
constant nine_256 : bit_vector := X"99999999999999999999999999999999"
                                & X"99999999999999999999999999999999";

signal L_ADR_0      : std_logic;
signal L_ADR_E      : std_logic_vector(11 downto 1);
signal L_ADR_O      : std_logic_vector(11 downto 1);
signal L_DIN_E      : std_logic_vector( 7 downto 0);
signal L_DIN_O      : std_logic_vector( 7 downto 0);
signal L_DOUT_E     : std_logic_vector( 7 downto 0);
signal L_DOUT_O     : std_logic_vector( 7 downto 0);
signal L_WE_E       : std_logic;
signal L_WE_O       : std_logic;
 
begin

    sr : RAMB16_S9_S9
	 generic map (
        INIT_00 => nine_256, INIT_01 => nine_256, INIT_02 => nine_256,
        INIT_03 => nine_256, INIT_04 => nine_256, INIT_05 => nine_256,
        INIT_06 => nine_256, INIT_07 => nine_256, INIT_08 => nine_256,
        INIT_09 => nine_256, INIT_0A => nine_256, INIT_0B => nine_256,
        INIT_0C => nine_256, INIT_0D => nine_256, INIT_0E => nine_256,
        INIT_0F => nine_256, 
		  INIT_10 => nine_256, INIT_11 => nine_256, INIT_12 => nine_256,
        INIT_13 => nine_256, INIT_14 => nine_256, INIT_15 => nine_256,
        INIT_16 => nine_256, INIT_17 => nine_256, INIT_18 => nine_256,
        INIT_19 => nine_256, INIT_1A => nine_256, INIT_1B => nine_256,
        INIT_1C => nine_256, INIT_1D => nine_256, INIT_1E => nine_256,
        INIT_1F => nine_256,
        INIT_20 => nine_256, INIT_21 => nine_256, INIT_22 => nine_256,
        INIT_23 => nine_256, INIT_24 => nine_256, INIT_25 => nine_256,
        INIT_26 => nine_256, INIT_27 => nine_256, INIT_28 => nine_256,
        INIT_29 => nine_256, INIT_2A => nine_256, INIT_2B => nine_256,
        INIT_2C => nine_256, INIT_2D => nine_256, INIT_2E => nine_256,
        INIT_2F => nine_256,
        INIT_30 => nine_256, INIT_31 => nine_256, INIT_32 => nine_256,
        INIT_33 => nine_256, INIT_34 => nine_256, INIT_35 => nine_256,
        INIT_36 => nine_256, INIT_37 => nine_256, INIT_38 => nine_256,
        INIT_39 => nine_256, INIT_3A => nine_256, INIT_3B => nine_256,
        INIT_3C => nine_256, INIT_3D => nine_256, INIT_3E => nine_256,
        INIT_3F => nine_256)

    port map (    ADDRA => L_ADR_E,    ADDRB => L_ADR_O,
	               CLKA  => I_CLK,      CLKB  => I_CLK,
						DIA   => L_DIN_E,    DIB   => L_DIN_O,
						ENA   => '1',        ENB   => '1',
						SSRA  => '0',        SSRB  => '0',
						WEA   => L_WE_E,     WEB   => L_WE_O,
						DOA   => L_DOUT_E,   DOB   => L_DOUT_O,
                  DOPA  => open,       DOPB  => open,
                  DIPA  => "0",        DIPB  => "0");


    -- remember ADR(0)
    --
    adr0: process(I_CLK)
    begin
        if (rising_edge(I_CLK)) then
            L_ADR_0 <= I_ADR(0);
        end if;
    end process;

    -- we use two memory blocks _E and _O (even and odd).
    -- This gives us a memory with ADR and ADR + 1 at th same time.
    -- The second port is currently unused, but may be used later,
    -- e.g. for DMA.
    --

    L_ADR_O <= "0" & I_ADR(10 downto 1);
    L_ADR_E <= "1" & (I_ADR(10 downto 1) + ("000000000" & I_ADR(0)));

    L_DIN_E <= I_DIN( 7 downto 0) when (I_ADR(0) = '0') else I_DIN(15 downto 8);
    L_DIN_O <= I_DIN( 7 downto 0) when (I_ADR(0) = '1') else I_DIN(15 downto 8);

    L_WE_E <= I_WE(1) or (I_WE(0) and not I_ADR(0));
    L_WE_O <= I_WE(1) or (I_WE(0) and     I_ADR(0));

    Q_DOUT( 7 downto 0) <= L_DOUT_E when (L_ADR_0 = '0') else L_DOUT_O;
    Q_DOUT(15 downto 8) <= L_DOUT_E when (L_ADR_0 = '1') else L_DOUT_O;
 
end Behavioral;

