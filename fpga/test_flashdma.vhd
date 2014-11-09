--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   21:08:12 11/08/2014
-- Design Name:   
-- Module Name:   /home/stephen/projects/hardware/hdmilight/fpga/test_flashdma.vhd
-- Project Name:  hdmilight
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: flashDMAController
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
 
ENTITY test_flashdma IS
END test_flashdma;
 
ARCHITECTURE behavior OF test_flashdma IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT flashDMAController
    PORT(
         clk : IN  std_logic;
         flashStartAddr : IN  std_logic_vector(23 downto 0);
         sramStartAddr : IN  std_logic_vector(15 downto 0);
         copySize : IN  std_logic_vector(15 downto 0);
         write : IN  std_logic;
         start : IN  std_logic;
         busy : OUT  std_logic;
         sramWe : OUT  std_logic;
         sramAddr : OUT  std_logic_vector(15 downto 0);
         sramDin : OUT  std_logic_vector(7 downto 0);
         sramDout : IN  std_logic_vector(7 downto 0);
         spiClk : OUT  std_logic;
         spiCs : OUT  std_logic;
         spiDo : OUT  std_logic;
         spiDi : IN  std_logic
        );
    END COMPONENT;
    

   --Inputs
   signal clk : std_logic := '0';
   signal flashStartAddr : std_logic_vector(23 downto 0) := (others => '0');
   signal sramStartAddr : std_logic_vector(15 downto 0) := (others => '0');
   signal copySize : std_logic_vector(15 downto 0) := (others => '0');
   signal write : std_logic := '0';
   signal start : std_logic := '0';
   signal sramDout : std_logic_vector(7 downto 0) := (others => '0');
   signal spiDi : std_logic := '0';

 	--Outputs
   signal busy : std_logic;
   signal sramWe : std_logic;
   signal sramAddr : std_logic_vector(15 downto 0);
   signal sramDin : std_logic_vector(7 downto 0);
   signal spiClk : std_logic;
   signal spiCs : std_logic;
   signal spiDo : std_logic;

   -- Clock period definitions
   constant clk_period : time := 10 ns;
   constant spiClk_period : time := 10 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: flashDMAController PORT MAP (
          clk => clk,
          flashStartAddr => flashStartAddr,
          sramStartAddr => sramStartAddr,
          copySize => copySize,
          write => write,
          start => start,
          busy => busy,
          sramWe => sramWe,
          sramAddr => sramAddr,
          sramDin => sramDin,
          sramDout => sramDout,
          spiClk => spiClk,
          spiCs => spiCs,
          spiDo => spiDo,
          spiDi => spiDi
        );

   -- Clock process definitions
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;
 
   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
      wait for 100 ns;	

      wait for clk_period*10;

      -- insert stimulus here 
      flashStartAddr <= x"F00F0F";
      write <= '0';
      copySize <= x"0100";
      start <= '1';

      wait for clk_period;

      start <= '0';

      wait;
   end process;

END;
