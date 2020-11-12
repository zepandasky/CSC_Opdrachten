	component nios_system is
		port (
			clk_clk         : in  std_logic                     := 'X';             -- clk
			hex0_3_export   : out std_logic_vector(27 downto 0);                    -- export
			hex4_5_export   : out std_logic_vector(13 downto 0);                    -- export
			leds_export     : out std_logic_vector(9 downto 0);                     -- export
			reset_reset_n   : in  std_logic                     := 'X';             -- reset_n
			switches_export : in  std_logic_vector(9 downto 0)  := (others => 'X')  -- export
		);
	end component nios_system;

	u0 : component nios_system
		port map (
			clk_clk         => CONNECTED_TO_clk_clk,         --      clk.clk
			hex0_3_export   => CONNECTED_TO_hex0_3_export,   --   hex0_3.export
			hex4_5_export   => CONNECTED_TO_hex4_5_export,   --   hex4_5.export
			leds_export     => CONNECTED_TO_leds_export,     --     leds.export
			reset_reset_n   => CONNECTED_TO_reset_reset_n,   --    reset.reset_n
			switches_export => CONNECTED_TO_switches_export  -- switches.export
		);

