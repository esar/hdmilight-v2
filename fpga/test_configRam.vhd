--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   21:18:15 10/30/2014
-- Design Name:   
-- Module Name:   /home/stephen/projects/hardware/hdmilight/fpga/test_configRam.vhd
-- Project Name:  hdmilight
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: lightConfigRam
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
USE ieee.numeric_std.ALL;
 
ENTITY test_configRam IS
END test_configRam;
 
ARCHITECTURE behavior OF test_configRam IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT lightConfigRam
    PORT(
         a_clk : IN  std_logic;
         a_wr : IN  std_logic;
         a_addr : IN  std_logic_vector(11 downto 0);
         a_din : IN  std_logic_vector(7 downto 0);
         a_dout : OUT  std_logic_vector(7 downto 0);
         b_clk : IN  std_logic;
         b_addr : IN  std_logic_vector(8 downto 0);
         b_dout : OUT  std_logic_vector(31 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal a_clk : std_logic := '0';
   signal a_wr : std_logic := '0';
   signal a_addr : std_logic_vector(11 downto 0) := (others => '0');
   signal a_din : std_logic_vector(7 downto 0) := (others => '0');
   signal b_clk : std_logic := '0';
   signal b_addr : std_logic_vector(8 downto 0) := (others => '0');

 	--Outputs
   signal a_dout : std_logic_vector(7 downto 0);
   signal b_dout : std_logic_vector(31 downto 0);

   -- Clock period definitions
   constant a_clk_period : time := 10 ns;
   constant b_clk_period : time := 10 ns;
 
 
	signal count : std_logic_vector(7 downto 0) := "00000000";
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: lightConfigRam PORT MAP (
          a_clk => a_clk,
          a_wr => a_wr,
          a_addr => a_addr,
          a_din => a_din,
          a_dout => a_dout,
          b_clk => b_clk,
          b_addr => b_addr,
          b_dout => b_dout
        );

   -- Clock process definitions
   a_clk_process :process
   begin
		a_clk <= '0';
		wait for a_clk_period/2;
		a_clk <= '1';
		wait for a_clk_period/2;
   end process;
 
   b_clk_process :process
   begin
		b_clk <= '0';
		wait for b_clk_period/2;
		b_clk <= '1';
		wait for b_clk_period/2;
   end process;

	process(a_clk)
	begin
		if(rising_edge(a_clk)) then
			count <= std_logic_vector(unsigned(count) + 1);
		end if;
	end process;
	
	a_din <= count;
	a_addr <= "0000" & count;
	a_wr <= '1' when unsigned(count) < 64 else '0';

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
      wait for 100 ns;	

      wait for a_clk_period*10;

      -- insert stimulus here 

      wait;
   end process;

END;
