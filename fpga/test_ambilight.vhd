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

LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY test_ambilight IS
END test_ambilight;
 
ARCHITECTURE behavior OF test_ambilight IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT ambilight
    PORT(
         vidclk : IN  std_logic;
         viddata_r : IN  std_logic_vector(7 downto 0);
         viddata_g : IN  std_logic_vector(7 downto 0);
         viddata_b : IN  std_logic_vector(7 downto 0);
         hblank : IN  std_logic;
         vblank : IN  std_logic;
         cfgclk : IN  std_logic;
         cfgwe : IN  std_logic;
         cfglight : IN  std_logic_vector(7 downto 0);
         cfgcomponent : IN  std_logic_vector(3 downto 0);
         cfgdatain : IN  std_logic_vector(7 downto 0);
         cfgdataout : OUT  std_logic_vector(7 downto 0);
         output : OUT  std_logic
        );
    END COMPONENT;
    

   --Inputs
   signal vidclk : std_logic := '0';
   signal viddata_r : std_logic_vector(7 downto 0) := (others => '0');
   signal viddata_g : std_logic_vector(7 downto 0) := (others => '0');
   signal viddata_b : std_logic_vector(7 downto 0) := (others => '0');
   signal hblank : std_logic := '0';
   signal vblank : std_logic := '0';
   signal cfgclk : std_logic := '0';
   signal cfgwe : std_logic := '0';
   signal cfglight : std_logic_vector(7 downto 0) := (others => '0');
   signal cfgcomponent : std_logic_vector(3 downto 0) := (others => '0');
   signal cfgdatain : std_logic_vector(7 downto 0) := (others => '0');

 	--Outputs
   signal cfgdataout : std_logic_vector(7 downto 0);
   signal output : std_logic;

   -- Clock period definitions
   constant vidclk_period : time := 10 ns;
   constant cfgclk_period : time := 10 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: ambilight PORT MAP (
          vidclk => vidclk,
          viddata_r => viddata_r,
          viddata_g => viddata_g,
          viddata_b => viddata_b,
          hblank => hblank,
          vblank => vblank,
          cfgclk => cfgclk,
          cfgwe => cfgwe,
          cfglight => cfglight,
          cfgcomponent => cfgcomponent,
          cfgdatain => cfgdatain,
          cfgdataout => cfgdataout,
          output => output
        );

   -- Clock process definitions
   vidclk_process :process
   begin
		vidclk <= '0';
		wait for vidclk_period/2;
		vidclk <= '1';
		wait for vidclk_period/2;
   end process;
 
   cfgclk_process :process
   begin
		cfgclk <= '0';
		wait for cfgclk_period/2;
		cfgclk <= '1';
		wait for cfgclk_period/2;
   end process;
 

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
      wait for 100 ns;	

      wait for vidclk_period*10;

		cfglight <= (others => '0');
		cfgcomponent <= x"0";
		wait for cfgclk_period*2;
		cfgdatain <= x"00";
		cfgwe <= '1';
		wait for cfgclk_period*2;
		
		cfgcomponent <= x"1";
		wait for cfgclk_period*2;
		cfgdatain <= x"07";
		cfgwe <= '1';
		wait for cfgclk_period*2;
		
		cfgcomponent <= x"2";
		wait for cfgclk_period*2;
		cfgdatain <= x"00";
		cfgwe <= '1';
		wait for cfgclk_period*2;
		
		cfgcomponent <= x"3";
		wait for cfgclk_period*2;
		cfgdatain <= x"07";
		cfgwe <= '1';
		wait for cfgclk_period*2;
		
		cfgcomponent <= x"4";
		wait for cfgclk_period*2;
		cfgdatain <= x"06";
		cfgwe <= '1';
		wait for cfgclk_period*2;
		
		cfgwe <= '0';
		wait for cfgclk_period;
		
		for field in 0 to 10 loop
			-- vblank = hBlank = 1
			hblank <= '1';
			vblank <= '1';
			
			for y in 0 to 20 loop
				for x in 0 to 820 loop
					viddata_r <= x"00";
					viddata_g <= x"00";
					viddata_b <= x"00";
					wait for vidclk_period;
				end loop;
			end loop;

			
			for y in 0 to 288 loop
				-- vBlank = hBlank = 0
				hblank <= '0';
				vblank <= '0';
			
				-- line of video, 720 pixels in total
				for x in 0 to 720 loop
					viddata_r <= x"aa";
					viddata_g <= x"00";
					viddata_b <= x"00";
					wait for vidclk_period;
				end loop;
				
				-- hBlank = 1
				hblank <= '1';
				
				-- blank data
				for x in 0 to 100 loop
					viddata_r <= x"00";
					viddata_g <= x"00";
					viddata_b <= x"00";
					wait for vidclk_period;
				end loop;
			end loop;

		end loop;

      wait;
   end process;

END;
