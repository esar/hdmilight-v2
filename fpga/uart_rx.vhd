library ieee;
use ieee.std_logic_1164.all;

-----------------------------------------------------------------------------
-- UART Receiver ------------------------------------------------------------
entity uart_rx is
        generic (
                fullbit  : integer );
        port (
                clk      : in  std_logic;
                reset    : in  std_logic;
                --
                dout     : out std_logic_vector(7 downto 0);
                avail    : out std_logic;
                error    : out std_logic;
                clear    : in  std_logic;
                --
                rxd      : in  std_logic );
end uart_rx;

-----------------------------------------------------------------------------
-- Implemenattion -----------------------------------------------------------
architecture rtl of uart_rx is

constant halfbit : integer := fullbit / 2;

-- Signals
signal bitcount  : integer range 0 to 10;
signal count     : integer range 0 to FULLBIT;
signal shiftreg  : std_logic_vector(7 downto 0);
signal rxd2      : std_logic;

begin

proc: process(clk, reset) is
begin
        if reset='1' then
                bitcount <= 0;
                count    <= 0;
                error    <= '0';
                avail    <= '0';
        elsif clk'event and clk='1' then
                if clear='1' then
                        error <= '0';
                        avail <= '0';
                end if;
       
                if count/=0 then
                        count <= count - 1;
                else
                        if bitcount=0 then     -- wait for startbit
                                if rxd2='0' then     -- FOUND
                                        count    <= HALFBIT;
                                        bitcount <= bitcount + 1;                                               
                                end if;
                        elsif bitcount=1 then  -- sample mid of startbit
                                if rxd2='0' then     -- OK
                                        count    <= FULLBIT;
                                        bitcount <= bitcount + 1;
                                        shiftreg <= "00000000";
                                else                -- ERROR
                                        error    <= '1';
                                        bitcount <= 0;
                                end if;
                        elsif bitcount=10 then -- stopbit
                                if rxd2='1' then     -- OK
                                        count    <= 0;
                                        bitcount <= 0;
                                        dout     <= shiftreg;
                                        avail    <= '1';
                                else                -- ERROR
                                        count    <= FULLBIT;
                                        bitcount <= 0;
                                        error    <= '1';
                                end if;
                        else
                                shiftreg(6 downto 0) <= shiftreg(7 downto 1);
                                shiftreg(7) <= rxd2;
                                count    <= FULLBIT;
                                bitcount <= bitcount + 1;
                        end if;
                end if;
        end if;
end process;

-----------------------------------------------------------------------------
-- Sync incoming RXD (anti metastable) --------------------------------------
syncproc: process(reset, clk) is
begin
        if reset='1' then
                rxd2 <= '1';
        elsif clk'event and clk='1' then
                rxd2 <= rxd;
        end if;
end process;
                                       
end rtl;