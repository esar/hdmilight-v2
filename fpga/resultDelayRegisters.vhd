----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    11:40:08 10/19/2014 
-- Design Name: 
-- Module Name:    resultDelayRegisters - Behavioral 
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
use ieee.std_logic_unsigned.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity resultDelayRegisters is
    Port ( clk : in  STD_LOGIC;
           addr : in  STD_LOGIC_VECTOR (1 downto 0);
           din : in  STD_LOGIC_VECTOR (7 downto 0);
			  dout : out STD_LOGIC_VECTOR (7 downto 0);
           we : in  STD_LOGIC;
			  
           frameCount : out  STD_LOGIC_VECTOR (7 downto 0);
           tickCount : out  STD_LOGIC_VECTOR (23 downto 0));
end resultDelayRegisters;

architecture Behavioral of resultDelayRegisters is

constant ADDR_WIDTH : integer := 2;
constant DATA_WIDTH : integer := 8;
type mem_type is array ( (2**ADDR_WIDTH)-1 downto 0 ) of std_logic_vector(DATA_WIDTH-1 downto 0);
shared variable mem : mem_type := ("00000010", "00000000", "00000011", "11101000");

begin

process(clk)
begin
	if(rising_edge(clk)) then
		if(we = '1') then
			mem(conv_integer(addr)) := din;
		end if;
		dout <= mem(conv_integer(addr));
	end if;
end process;

frameCount <= mem(0);
tickCount <= mem(1) & mem(2) & mem(3);

end Behavioral;

