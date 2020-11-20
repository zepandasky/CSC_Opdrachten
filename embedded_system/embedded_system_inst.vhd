	component embedded_system is
		port (
			clk_clk                   : in  std_logic                     := 'X'; -- clk
			reset_reset_n             : in  std_logic                     := 'X'; -- reset_n
			to_hex_readdata_readdata  : out std_logic_vector(31 downto 0);        -- readdata
			to_hex_readdata2_readdata : out std_logic_vector(31 downto 0)         -- readdata
		);
	end component embedded_system;

	u0 : component embedded_system
		port map (
			clk_clk                   => CONNECTED_TO_clk_clk,                   --              clk.clk
			reset_reset_n             => CONNECTED_TO_reset_reset_n,             --            reset.reset_n
			to_hex_readdata_readdata  => CONNECTED_TO_to_hex_readdata_readdata,  --  to_hex_readdata.readdata
			to_hex_readdata2_readdata => CONNECTED_TO_to_hex_readdata2_readdata  -- to_hex_readdata2.readdata
		);

