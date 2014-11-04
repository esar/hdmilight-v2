--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   17:45:47 10/25/2014
-- Design Name:   
-- Module Name:   /home/stephen/projects/hardware/hdmilight/fpga/test_resultDelay.vhd
-- Project Name:  hdmilight
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: resultDelay
-- 
-- Dependencies:
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--
-- Notes: 
-- This testbench has been automatically generated using types std_logic and
-- std_logic_vector for the ports of the unit under test.  Xilinx recommends
-- that these types always be used for the top-level I/O of a design in order
-- to guarantee that the testbench will bind correctly to the post-implementation 
-- simulation model.
--------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY test_resultDelay IS
END test_resultDelay;
 
ARCHITECTURE behavior OF test_resultDelay IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT resultDelay
    PORT(
         clk : IN  std_logic;
         in_vblank : IN  std_logic;
         in_addr : OUT  std_logic_vector(8 downto 0);
         in_data : IN  std_logic_vector(31 downto 0);
         out_vblank : OUT  std_logic;
         out_addr : IN  std_logic_vector(8 downto 0);
         out_data : OUT  std_logic_vector(31 downto 0);
         delayFrames : IN  std_logic_vector(7 downto 0);
         delayTicks : IN  std_logic_vector(23 downto 0);
			temporalSmoothingRatio : IN std_logic_vector(8 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal clk : std_logic := '0';
   signal in_vblank : std_logic := '0';
   signal in_data : std_logic_vector(31 downto 0) := (others => '0');
   signal out_addr : std_logic_vector(8 downto 0) := (others => '0');
   signal delayFrames : std_logic_vector(7 downto 0) := x"01";
   signal delayTicks : std_logic_vector(23 downto 0) := x"000020";
	signal temporalSmoothingRatio : std_logic_vector(8 downto 0) := "100000000";

 	--Outputs
   signal in_addr : std_logic_vector(8 downto 0);
   signal out_vblank : std_logic;
   signal out_data : std_logic_vector(31 downto 0);

   -- Clock period definitions
   constant clk_period : time := 10 ns;
 
	signal start : std_logic;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: resultDelay PORT MAP (
          clk => clk,
          in_vblank => in_vblank,
          in_addr => in_addr,
          in_data => in_data,
          out_vblank => out_vblank,
          out_addr => out_addr,
          out_data => out_data,
          delayFrames => delayFrames,
          delayTicks => delayTicks,
			 temporalsmoothingRatio => temporalSmoothingRatio
        );

   -- Clock process definitions
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;
 
	process(clk)
	begin
		if(rising_edge(clk)) then
			in_data <= in_addr(7 downto 0) & in_addr(7 downto 0) & in_addr(7 downto 0) & in_addr(7 downto 0);
		end if;
	end process;

	process(clk)
	begin
		if(rising_edge(clk)) then
			in_vblank <= out_vblank or start;
		end if;
	end process;

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
      wait for 100 ns;	

		start <= '0';

      wait for clk_period*10;

      -- insert stimulus here 
		start <= '1';
		wait for clk_period*2;
		start <= '0';

      wait;
   end process;

END;
