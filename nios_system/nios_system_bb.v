
module nios_system (
	clk_clk,
	hex0_3_export,
	hex4_5_export,
	leds_export,
	reset_reset_n,
	switches_export);	

	input		clk_clk;
	output	[27:0]	hex0_3_export;
	output	[13:0]	hex4_5_export;
	output	[9:0]	leds_export;
	input		reset_reset_n;
	input	[9:0]	switches_export;
endmodule
