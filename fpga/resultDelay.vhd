----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    17:04:21 10/18/2014 
-- Design Name: 
-- Module Name:    resultDelay - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity resultDelay is
    Port ( clk : in  STD_LOGIC;
	        cfg_we : in STD_LOGIC;
			  cfg_addr : in STD_LOGIC_VECTOR (1 downto 0);
			  cfg_din : in STD_LOGIC_VECTOR (7 downto 0);
			  cfg_dout : out STD_LOGIC_VECTOR (7 downto 0);
           in_vblank : in  STD_LOGIC;
           in_addr : out  STD_LOGIC_VECTOR (8 downto 0);
           in_data : in  STD_LOGIC_VECTOR (31 downto 0);
           out_vblank : out  STD_LOGIC;
           out_addr : in  STD_LOGIC_VECTOR (7 downto 0);
           out_data : out  STD_LOGIC_VECTOR (31 downto 0));
end resultDelay;

architecture Behavioral of resultDelay is

signal delay_frames : std_logic_vector(7 downto 0);
signal delay_ticks : std_logic_vector(23 downto 0);

signal lastvblank : std_logic;
signal start : std_logic;
signal enable : std_logic;
signal count : std_logic_vector(8 downto 0);
signal tickcount : std_logic_vector(23 downto 0);
signal count_ram_in : std_logic_vector(2 downto 0);
signal count_ram_out : std_logic_vector(2 downto 0);

signal ram_wr_in : std_logic;
signal ram_addr_in : std_logic_vector(10 downto 0);
signal ram_data_in : std_logic_vector(31 downto 0);
signal ram_addr_out : std_logic_vector(10 downto 0);
signal ram_data_out : std_logic_vector(31 downto 0);

begin
delayRam : entity work.blockram
  GENERIC MAP(
    ADDR => 11,
	 DATA => 32
  )
  PORT MAP (
    a_clk => clk,
    a_en => '1',
    a_wr => ram_wr_in,
	 a_rst => '0',
    a_addr => ram_addr_in,
    a_din => ram_data_in,
    a_dout => open,
    b_clk => clk,
	 b_en => '1',
    b_wr => '0',
	 b_rst => '0',
    b_addr => ram_addr_out,
    b_din => (others=> '0'),
    b_dout => ram_data_out
  );

configRegisters : entity work.resultDelayRegisters
	PORT MAP (
		clk => clk,
		we => cfg_we,
		addr => cfg_addr,
		din => cfg_din,
		dout => cfg_dout,
		frameCount => delay_frames,
		tickCount => delay_ticks
	);

-- generate start pulse when incoming vblank goes high
process(clk)
begin
	if(rising_edge(clk)) then
		if(in_vblank = '1' and lastvblank = '0') then
			start <= '1';
		else
			start <= '0';
		end if;

		lastvblank <= in_vblank;
	end if;
end process;

-- increment write address once per frame (when we get the start pulse)
process(clk)
begin
	if(rising_edge(clk)) then
		if(start = '1') then
			count_ram_in <= std_logic_vector(unsigned(count_ram_in) + 1);
		else
			count_ram_in <= count_ram_in;
		end if;
	end if;
end process;

-- set the read address to the write address minus the required delay (in whole frames)
process(clk)
begin
	if(rising_edge(clk)) then
		count_ram_out <= std_logic_vector(unsigned(count_ram_in) - unsigned(delay_frames(2 downto 0)));
	end if;
end process;

-- counter for copying the 256 values from the current set of results to the delay ram
process(clk)
begin
	if(rising_edge(clk)) then
		if(start = '1') then
			count <= (others => '0');
		elsif(enable = '1') then
			count <= std_logic_vector(unsigned(count) + 1);
		else
			count <= count;
		end if;
	end if;
end process;

-- counter for tick delay, start counting down toward zero when copying of current results finishes
process(clk)
begin
	if(rising_edge(clk)) then
		if(enable = '1') then
			tickcount <= delay_ticks;
		elsif(unsigned(tickcount) /= 0) then
			tickcount <= std_logic_vector(unsigned(tickcount) - 1);
		end if;
	end if;
end process;

enable <= not count(8);

out_vblank <= '1' when unsigned(tickcount) = 0 else '0';

in_addr <= "0" & count(7 downto 0);
ram_addr_in <= count_ram_in & count(7 downto 0);
ram_data_in <= in_data;
ram_wr_in <= enable;

ram_addr_out <= count_ram_out & out_addr;
out_data <= ram_data_out;

end Behavioral;

