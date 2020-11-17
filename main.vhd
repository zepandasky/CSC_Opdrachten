LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_unsigned.ALL;


ENTITY lights IS
	PORT (
		CLOCK_50 : IN STD_LOGIC;
		KEY 		: IN STD_LOGIC_VECTOR  (0 DOWNTO 0);
		SW 		: IN STD_LOGIC_VECTOR  (9 DOWNTO 0);
		LEDR 		: OUT STD_LOGIC_VECTOR (9 DOWNTO 0);
		HEX		: OUT STD_LOGIC_VECTOR (41 DOWNTO 0) --6 HEX DISPLAYS
	);
END lights;


ARCHITECTURE lights_rtl OF lights IS
    component nios_system is
        port (
            clk_clk         : in  std_logic                     := 'X';             -- clk
            hex0_3_export   : out std_logic_vector(27 downto 0);                    -- export
            hex4_5_export   : out std_logic_vector(13 downto 0);                    -- export
            leds_export     : out std_logic_vector(9 downto 0);                     -- export
            reset_reset_n   : in  std_logic                     := 'X';             -- reset_n
            switches_export : in  std_logic_vector(9 downto 0)  := (others => 'X')  -- export
        );
END COMPONENT;



BEGIN
	NiosII : nios_system
	PORT MAP(
		clk_clk => CLOCK_50,
		reset_reset_n => KEY(0),
		switches_export => SW(9 DOWNTO 0),
		leds_export => LEDR(9 DOWNTO 0),
		hex0_3_export => HEX(27 DOWNTO 0),	-- 28 BITS FOR HEX 0-3 LOWEST 8 BITS IS FOR HEX0 THE NEXT FOR HEX 1 ETC.
		hex4_5_export => HEX(41 DOWNTO 28)	-- 14 BITS FOR HEX 4-5
	);
END lights_rtl;