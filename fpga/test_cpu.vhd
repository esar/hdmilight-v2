--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   18:17:47 11/22/2014
-- Design Name:   
-- Module Name:   /home/stephen/projects/hardware/hdmilight/fpga/test_cpu.vhd
-- Project Name:  hdmilight
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: cpu_core
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
 
ENTITY test_cpu IS
END test_cpu;
 
ARCHITECTURE behavior OF test_cpu IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    component data_mem
        port (  I_CLK       : in  std_logic;

                I_ADR       : in  std_logic_vector(10 downto 0);
                I_DIN       : in  std_logic_vector(15 downto 0);
                I_WE        : in  std_logic_vector( 1 downto 0);

                Q_DOUT      : out std_logic_vector(15 downto 0));
    end component;

    COMPONENT cpu_core
    PORT(
         I_CLK : IN  std_logic;
         I_CE : IN  std_logic;
         I_CLR : IN  std_logic;
         I_INTVEC : IN  std_logic_vector(5 downto 0);
         I_DIN : IN  std_logic_vector(15 downto 0);
         Q_OPC : OUT  std_logic_vector(15 downto 0);
         Q_PC : OUT  std_logic_vector(15 downto 0);
         Q_DOUT : OUT  std_logic_vector(15 downto 0);
         Q_ADR : OUT  std_logic_vector(15 downto 0);
         Q_RD_IO : OUT  std_logic;
         Q_WE_IO : OUT  std_logic;
         Q_WE_SRAM : OUT  std_logic_vector(1 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal I_CLK : std_logic := '0';
   signal I_CE : std_logic := '0';
   signal I_CLR : std_logic := '0';
   signal I_INTVEC : std_logic_vector(5 downto 0) := (others => '0');
   signal I_DIN : std_logic_vector(15 downto 0) := (others => '0');

 	--Outputs
   signal Q_OPC : std_logic_vector(15 downto 0);
   signal Q_PC : std_logic_vector(15 downto 0);
   signal Q_DOUT : std_logic_vector(15 downto 0);
   signal Q_ADR : std_logic_vector(15 downto 0);
   signal Q_RD_IO : std_logic;
   signal Q_WE_IO : std_logic;
   signal Q_WE_SRAM : std_logic_vector(1 downto 0);

   -- Clock period definitions
   constant I_CLK_period : time := 10 ns;
 
   signal RST: std_logic:= '1';
   signal RST_COUNT: std_logic_vector(1 downto 0):="00";
   signal DMA_IN_PROGRESS : std_logic := '0';
BEGIN
 
    SRAM : data_mem port map(
    	I_CLK   => I_CLK,
    	I_ADR   => Q_ADR(10 downto 0),
    	I_DIN   => Q_DOUT,
    	I_WE    => Q_WE_SRAM,
    	Q_DOUT  => I_DIN
    );
	-- Instantiate the Unit Under Test (UUT)
   uut: cpu_core PORT MAP (
          I_CLK => I_CLK,
          I_CE => I_CE,
          I_CLR => I_CLR,
          I_INTVEC => I_INTVEC,
          I_DIN => I_DIN,
          Q_OPC => Q_OPC,
          Q_PC => Q_PC,
          Q_DOUT => Q_DOUT,
          Q_ADR => Q_ADR,
          Q_RD_IO => Q_RD_IO,
          Q_WE_IO => Q_WE_IO,
          Q_WE_SRAM => Q_WE_SRAM
        );

   -- Clock process definitions
   I_CLK_process :process
   begin
		I_CLK <= '0';
		wait for I_CLK_period/2;
		I_CLK <= '1';
		wait for I_CLK_period/2;
   end process;
 
-- Reset Generator
process (I_CLK)
begin
	if (rising_edge(I_CLK)) then
		if (RST_COUNT = "11") then
			RST <= '0';
		else
			RST_COUNT <= std_logic_vector(unsigned(RST_COUNT) + 1);
		end if;
	end if;
end process;

-- IO memory space handler
process (I_CLK)

begin
	if (rising_edge(I_CLK)) then
		-- IO Write Cycle
		if (Q_WE_IO = '1') then
			case Q_ADR is
				when X"0053"  =>
					DMA_IN_PROGRESS <= '1';
				when others => 
			end case;												
		end if;	
	
		if(DMA_IN_PROGRESS = '1') then
			DMA_IN_PROGRESS <= '0';
		end if;
	end if;
end process;

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
      wait for 100 ns;	

      wait for I_CLK_period*10;

      -- insert stimulus here 

      wait;
   end process;

I_CLR <= RST;
I_CE  <= not DMA_IN_PROGRESS;

END;
