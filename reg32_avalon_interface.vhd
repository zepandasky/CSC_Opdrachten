LIBRARY ieee;
USE ieee.std_logic_1164.all;
ENTITY reg32_avalon_interface IS
	PORT ( 	clock, resetn : IN STD_LOGIC;
				read, write, chipselect : IN STD_LOGIC;
				read2, write2, chipselect2		 : IN STD_LOGIC;
				writedata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
				writedata2 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
				byteenable : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
				byteenable2 : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
				readdata : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
				readdata2 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
				Q_export : OUT STD_LOGIC_VECTOR(31 DOWNTO 0); 	--1 reg
				Q_export2 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0)
			);
END reg32_avalon_interface;

ARCHITECTURE Structure OF reg32_avalon_interface IS
	SIGNAL local_byteenable : STD_LOGIC_VECTOR(3 DOWNTO 0);
	SIGNAL local_byteenable2 : STD_LOGIC_VECTOR(3 DOWNTO 0);
	SIGNAL to_reg, from_reg : STD_LOGIC_VECTOR(31 DOWNTO 0);
	SIGNAL to_reg2, from_reg2 : STD_LOGIC_VECTOR(31 DOWNTO 0);

		COMPONENT reg32
			PORT ( clock, resetn : IN STD_LOGIC;
					 D : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
				    byteenable : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
					 Q : OUT STD_LOGIC_VECTOR(31 DOWNTO 0) 
			);
		END COMPONENT;
BEGIN
	to_reg  <= writedata;
	to_reg2 <= writedata2;
	
	WITH (chipselect AND write) SELECT
		local_byteenable <= byteenable WHEN '1', "0000" WHEN OTHERS;
	WITH (chipselect2 AND write2) SELECT
		local_byteenable2 <= byteenable2 WHEN '1', "0000" WHEN OTHERS;	
		
		
	reg_instance:  reg32 PORT MAP (clock, resetn, to_reg, local_byteenable,from_reg);
	reg_instance2: reg32 PORT MAP (clock, resetn, to_reg2,local_byteenable2, from_reg2);

	
	
	readdata <= from_reg; 
	Q_export <= from_reg;
	readdata2 <= from_reg2; 
	Q_export2 <= from_reg2;
END Structure;
