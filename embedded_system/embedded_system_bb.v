
module embedded_system (
	clk_clk,
	reset_reset_n,
	to_hex_readdata_readdata,
	to_hex_readdata2_readdata);	

	input		clk_clk;
	input		reset_reset_n;
	output	[31:0]	to_hex_readdata_readdata;
	output	[31:0]	to_hex_readdata2_readdata;
endmodule
