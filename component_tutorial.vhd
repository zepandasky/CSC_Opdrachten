LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY component_tutorial IS
	PORT ( CLOCK_50 : IN STD_LOGIC;
			 KEY : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
			 HEX0 : OUT STD_LOGIC_VECTOR(0 TO 6);
			 HEX1 : OUT STD_LOGIC_VECTOR(0 TO 6);
			 HEX2 : OUT STD_LOGIC_VECTOR(0 TO 6);
			 HEX3 : OUT STD_LOGIC_VECTOR(0 TO 6);
			 HEX4 : OUT STD_LOGIC_VECTOR(0 TO 6);
			 HEX5 : OUT STD_LOGIC_VECTOR(0 TO 6) );
END component_tutorial;

ARCHITECTURE Structure OF component_tutorial IS
	SIGNAL to_HEX : STD_LOGIC_VECTOR(31 DOWNTO 0);
	SIGNAL to_HEX2: STD_LOGIC_VECTOR(31 DOWNTO 0);
	
	 component embedded_system is
        port (
            clk_clk                   : in  std_logic                     := 'X'; -- clk
            reset_reset_n             : in  std_logic                     := 'X'; -- reset_n
            to_hex_readdata_readdata  : out std_logic_vector(31 downto 0);        -- readdata
            to_hex_readdata2_readdata : out std_logic_vector(31 downto 0)         -- readdata
        );
    end component embedded_system;

	
	COMPONENT hex7seg IS
		PORT ( hex : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
				 display : OUT STD_LOGIC_VECTOR(0 TO 6) );
	END COMPONENT hex7seg;
BEGIN
	U0: embedded_system PORT MAP (clk_clk => CLOCK_50, reset_reset_n => KEY(0), to_hex_readdata_readdata => to_HEX, 
	to_hex_readdata2_readdata => to_HEX2);
	
	h0: hex7seg PORT MAP (to_HEX(3 DOWNTO 0)	, HEX0);
	h1: hex7seg PORT MAP (to_HEX(7 DOWNTO 4)	, HEX1);
	h2: hex7seg PORT MAP (to_HEX(11 DOWNTO 8) , HEX2);
	h3: hex7seg PORT MAP (to_HEX(15 DOWNTO 12), HEX3);
	H4: hex7seg PORT MAP (to_HEX2(3 DOWNTO 0) , HEX4);
	H5: hex7seg PORT MAP (to_HEX2(7 DOWNTO 4) , HEX5);
END Structure;

