	component week2 is
		port (
			clk_clk                  : in  std_logic                     := 'X'; -- clk
			reset_reset_n            : in  std_logic                     := 'X'; -- reset_n
			to_hex_readdata_readdata : out std_logic_vector(31 downto 0)         -- readdata
		);
	end component week2;

	u0 : component week2
		port map (
			clk_clk                  => CONNECTED_TO_clk_clk,                  --             clk.clk
			reset_reset_n            => CONNECTED_TO_reset_reset_n,            --           reset.reset_n
			to_hex_readdata_readdata => CONNECTED_TO_to_hex_readdata_readdata  -- to_hex_readdata.readdata
		);

