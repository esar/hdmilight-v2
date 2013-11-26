library ieee;
use ieee.std_logic_1164.all;

-----------------------------------------------------------------------------
-- UART Transmitter ---------------------------------------------------------
entity uart_tx is
        generic (
                fullbit  : integer );
        port (
                clk      : in  std_logic;
                reset    : in  std_logic;
                --
                din      : in  std_logic_vector(7 downto 0);
                wr       : in  std_logic;
                busy     : out std_logic;
                --
                txd      : out std_logic );
end uart_tx;


-----------------------------------------------------------------------------
-- Implemenattion -----------------------------------------------------------
architecture rtl of uart_tx is

constant halfbit : integer := fullbit / 2;

-- Signals
signal bitcount  : integer range 0 to 10;
signal count     : integer range 0 to fullbit;
signal shiftreg  : std_logic_vector(7 downto 0);

begin

proc: process(clk, reset)
begin
        if reset='1' then
                count    <= 0;
                bitcount <= 0;
                busy     <= '0';
                txd      <= '1';
        elsif clk'event and clk='1' then
                if count/=0 then
                        count <= count - 1;
                else
                        if bitcount=0 then
                                busy <= '0';
                                if wr='1' then          -- START BIT
                                        shiftreg <= din;
                                        busy     <= '1';
                                        txd      <= '0';                                       
                                        bitcount <= bitcount + 1;
                                        count    <= fullbit;
                                end if;
                        elsif bitcount=9 then      -- STOP BIT
                                txd         <= '1';
                                bitcount    <= 0;
                                count       <= fullbit;
                        else                       -- DATA BIT
                                shiftreg(6 downto 0) <= shiftreg(7 downto 1);
                                txd         <= shiftreg(0);                             
                                bitcount    <= bitcount + 1;                           
                                count       <= fullbit;                         
                        end if;
                end if;
        end if;
end process;

end rtl;