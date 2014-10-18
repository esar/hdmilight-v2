
-- A parameterized, inferable, true dual-port, dual-clock block RAM in VHDL.
-- Originally from http://danstrother.com/2010/09/11/inferring-rams-in-fpgas/


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
 
entity blockram is
generic (
    DATA    : integer := 72;
    ADDR    : integer := 10
);
port (
    -- Port A
    a_clk   : in  std_logic;
	 a_en    : in  std_logic;
    a_wr    : in  std_logic;
	 a_rst   : in  std_logic;
    a_addr  : in  std_logic_vector(ADDR-1 downto 0);
    a_din   : in  std_logic_vector(DATA-1 downto 0);
    a_dout  : out std_logic_vector(DATA-1 downto 0);
     
    -- Port B
    b_clk   : in  std_logic;
	 b_en    : in  std_logic;
    b_wr    : in  std_logic;
	 b_rst   : in  std_logic;
    b_addr  : in  std_logic_vector(ADDR-1 downto 0);
    b_din   : in  std_logic_vector(DATA-1 downto 0);
    b_dout  : out std_logic_vector(DATA-1 downto 0)
);
end blockram;
 
architecture Behavioral of blockram is
    -- Shared memory
    type mem_type is array ( (2**ADDR)-1 downto 0 ) of std_logic_vector(DATA-1 downto 0);
    shared variable mem : mem_type;
begin
 
-- Port A
process(a_en, a_clk)
begin
    if(a_en='1' and a_clk'event and a_clk='1') then
        if(a_wr='1') then
            mem(conv_integer(a_addr)) := a_din;
        end if;
		  if(a_rst='1') then
		      a_dout <= (others => '0');
		  else
            a_dout <= mem(conv_integer(a_addr));
		  end if;
    end if;
end process;
 
-- Port B
process(b_en, b_clk)
begin
    if(b_en='1' and b_clk'event and b_clk='1') then
        if(b_wr='1') then
            mem(conv_integer(b_addr)) := b_din;
        end if;
		  if(b_rst='1') then
		      b_dout <= (others => '0');
		  else
            b_dout <= mem(conv_integer(b_addr));
		  end if;
    end if;
end process;
 
end Behavioral;