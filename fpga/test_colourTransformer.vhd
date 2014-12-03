-- TestBench Template 

  LIBRARY ieee;
  USE ieee.std_logic_1164.ALL;
  USE ieee.numeric_std.ALL;

  ENTITY test_colourTransformer IS
  END test_colourTransformer;

  ARCHITECTURE behavior OF test_colourTransformer IS 

			 COMPONENT resultDistributor
			 PORT(
		clk             : in  std_logic;
		start           : in  std_logic;
		driverReadyVect : in  std_logic_vector(7  downto 0);
		driverStartVect : out std_logic_vector(7  downto 0);
		driverData      : out std_logic_vector(23 downto 0);

		outputMapAddr   : out std_logic_vector(11 downto 0);
		outputMapData   : in  std_logic_vector(15 downto 0);

		areaResultAddr  : out std_logic_vector(8 downto 0);
		areaResultR     : in  std_logic_vector(7 downto 0);
		areaResultG     : in  std_logic_vector(7 downto 0);
		arearesultB     : in  std_logic_vector(7 downto 0);

		colourCoefAddr  : out std_logic_vector(8 downto 0);
		colourCoefData  : in  std_logic_vector(63 downto 0);

		gammaTableRAddr : out std_logic_vector(10 downto 0);
		gammaTableRData : in  std_logic_vector(7 downto 0);
		gammaTableGAddr : out std_logic_vector(10 downto 0);
		gammaTableGData : in  std_logic_vector(7 downto 0);
		gammaTableBAddr : out std_logic_vector(10 downto 0);
		gammaTableBData : in  std_logic_vector(7 downto 0)
				);
			END COMPONENT;
			
			COMPONENT ws2811Driver
			PORT(
				clk : in  STD_LOGIC;
				idle : out STD_LOGIC;
				load : in  STD_LOGIC;
				datain : in  STD_LOGIC_VECTOR (23 downto 0);
				dataout : out  STD_LOGIC			
				);
			END COMPONENT;

		signal clk         : std_logic;
		signal start       : std_logic;
		signal driverReady : std_logic_vector(7  downto 0);
		signal driverStart : std_logic_vector(7  downto 0);
		signal driverData  : std_logic_vector(23 downto 0);
		signal driverOutput : std_logic_vector(7 downto 0);

		signal outputMapAddr : std_logic_vector(11 downto 0);
		signal outputMapData : std_logic_vector(15 downto 0);

		signal areaResultAddr : std_logic_vector(8 downto 0);
		signal areaResultR    : std_logic_vector(7 downto 0);
		signal areaResultG    : std_logic_vector(7 downto 0);
		signal arearesultB    : std_logic_vector(7 downto 0);

		signal colourCoefAddr : std_logic_vector(8 downto 0);
		signal colourCoefData : std_logic_vector(63 downto 0);

		signal gammaTableRAddr : std_logic_vector(10 downto 0);
		signal gammaTableRData : std_logic_vector(7 downto 0);
		signal gammaTableGAddr : std_logic_vector(10 downto 0);
		signal gammaTableGData : std_logic_vector(7 downto 0);
		signal gammaTableBAddr : std_logic_vector(10 downto 0);
		signal gammaTableBData : std_logic_vector(7 downto 0);
     
   constant clkperiod : time := 10 ns;

	type outputMapArray is array (0 to 4096) of std_logic_vector(15 downto 0);
	shared variable outputMap : outputMapArray := (others => "1000000000000000");
	
	type colourCoefArray is array (0 to 511) of std_logic_vector(63 downto 0);
	shared variable colourCoef : colourCoefArray := (
		x"0000100010001000",
		x"000000000000FF00",
		x"00000000FF000000",
		x"0000FF0000000000",
		others => (others => '0'));
	
	type gammaTableArray is array (0 to 255) of std_logic_vector(7 downto 0);
	constant gammaTableR : gammaTableArray := (others => (others => '0'));
	constant gammaTableG : gammaTableArray := (others => (others => '0'));
	constant gammaTableB : gammaTableArray := (others => (others => '0'));

  BEGIN

		rd: resultDistributor PORT MAP (
			clk => clk,
			start => start,
			driverReadyVect => driverReady,
			driverStartVect => driverStart,
			driverData      => driverData,
			outputMapAddr   => outputMapAddr,
			outputMapData   => outputMapData,

			areaResultAddr  => areaResultAddr,
			areaResultR     => areaResultR,
			areaResultG     => areaResultG,
			arearesultB     => areaResultB,

			colourCoefAddr  => colourCoefAddr,
			colourCoefData  => colourCoefData,

			gammaTableRAddr => gammaTableRAddr,
			gammaTableRData => gammaTableRData,
			gammaTableGAddr => gammaTableGAddr,
			gammaTableGData => gammaTableGData,
			gammaTableBAddr => gammaTableBAddr,
			gammaTableBData => gammaTableBData
			);

		 drv0: ws2811Driver PORT MAP(clk => clk, idle => driverReady(0), load => driverStart(0), datain => driverData,	dataout => driverOutput(0));
		 drv1: ws2811Driver PORT MAP(clk => clk, idle => driverReady(1), load => driverStart(1), datain => driverData,	dataout => driverOutput(1));
		 drv2: ws2811Driver PORT MAP(clk => clk, idle => driverReady(2), load => driverStart(2), datain => driverData,	dataout => driverOutput(2));
		 drv3: ws2811Driver PORT MAP(clk => clk, idle => driverReady(3), load => driverStart(3), datain => driverData,	dataout => driverOutput(3));
		 drv4: ws2811Driver PORT MAP(clk => clk, idle => driverReady(4), load => driverStart(4), datain => driverData,	dataout => driverOutput(4));
		 drv5: ws2811Driver PORT MAP(clk => clk, idle => driverReady(5), load => driverStart(5), datain => driverData,	dataout => driverOutput(5));
		 drv6: ws2811Driver PORT MAP(clk => clk, idle => driverReady(6), load => driverStart(6), datain => driverData,	dataout => driverOutput(6));
		 drv7: ws2811Driver PORT MAP(clk => clk, idle => driverReady(7), load => driverStart(7), datain => driverData,	dataout => driverOutput(7));

	process(clk)
	begin
		if(rising_edge(clk)) then
			areaResultR     <= areaResultAddr(7 downto 0);
			areaResultG     <= areaResultAddr(7 downto 0);
			areaResultB     <= areaResultAddr(7 downto 0);
			outputMapData   <= outputMap(to_integer(unsigned(outputMapAddr)));
			colourCoefData  <= colourCoef(to_integer(unsigned(colourCoefAddr)));
			--gammaTableRData <= gammaTableR(to_integer(unsigned(gammaTableRAddr)));
			--gammaTableGData <= gammaTableR(to_integer(unsigned(gammaTableGAddr)));
			--gammaTableBData <= gammaTableR(to_integer(unsigned(gammaTableBAddr)));
			gammaTableRData <= gammaTableRAddr(7 downto 0);
			gammaTableGData <= gammaTableGAddr(7 downto 0);
			gammaTableBData <= gammaTableBAddr(7 downto 0);
		end if;
	end process;


  --  Test Bench Statements
	tb : PROCESS
	BEGIN
		clk <= '0';
		start <= '0';
	
	colourCoef(0) := "0000000000" & ('0' & x"00" & "000000000") & ('0' & x"00" & "000000000") & ('1' & x"ff" & "000000000");
	colourCoef(1) := "0000000000" & ('0' & x"00" & "000000000") & ('1' & x"ff" & "000000000") & ('0' & x"00" & "000000000");
	colourCoef(2) := "0000000000" & ('1' & x"ff" & "000000000") & ('0' & x"00" & "000000000") & ('0' & x"00" & "000000000");
	colourCoef(3) := "0000000000" & ('0' & x"ff" & "000000000") & ('0' & x"ff" & "000000000") & ('0' & x"ff" & "000000000");
	
	for x in 0 to 4095 loop
		outputMap(x) := "10000000" & std_logic_vector(to_unsigned(x, 8));
	end loop;
	
	for x in (6*512 + 254) to (6*512 + 511) loop
		outputMap(x) := "0000000000000000";
	end loop;


        wait for 100 ns; -- wait until global set/reset completes

		clk <= '1';
		wait for clkperiod;
		clk <= '0';
		wait for clkperiod;
		
		start <= '1';
		clk <= '1';
		wait for clkperiod;
		clk <= '0';
		wait for clkperiod;
		
		for x in 0 to 1000000 loop
			clk <= '1';
			wait for clkperiod;
			clk <= '0';
			wait for clkperiod;
		end loop;


		wait; -- will wait forever
	END PROCESS tb;
	--  End Test Bench 

  END;
