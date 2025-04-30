module r8051_top(
	input wire clk,
	input wire rst,	
	output wire uart_tx);
	
	wire clk_cpu;
	wire rst_cpu;
	wire clk_uart;
	
`ifdef VERILATOR	
	assign clk_cpu = clk;
	assign rst_cpu = rst;
	assign clk_uart = clk;
`else
	reg [1:0] r_reset_cnt;
	initial r_reset_cnt = 2'b00;
	always @(posedge clk_cpu) begin
		if (r_reset_cnt[1] != 1'b1)
			r_reset_cnt <= r_reset_cnt + 1'b1;
	end
	
	wire clk40m;
	pll pll(
		.inclk0(clk),
		.c0(clk40m));	

	assign clk_cpu = clk40m;
	assign clk_uart = clk40m;
	assign rst_cpu = !r_reset_cnt[1];
`endif	
	
	wire       rom_en;
	wire[15:0] rom_addr;
	wire [7:0]  rom_byte;
	wire        rom_vld;
		
	wire        ram_rd_en_data;
	wire        ram_rd_en_sfr;
	wire        ram_rd_en_idata;
	wire        ram_rd_en_xdata;
	wire [15:0] ram_rd_addr;
	wire [7:0]  ram_rd_byte;
	wire        ram_rd_vld;
		
	wire        ram_wr_en_data;
	wire        ram_wr_en_sfr;
	wire        ram_wr_en_idata;
	wire        ram_wr_en_xdata;
	wire [15:0] ram_wr_addr;
	wire [7:0]  ram_wr_byte;
	
	reg [7:0] rom [0:16383];
	initial begin
	$readmemh("firmware.hex", rom);
	end
	reg [7:0] rom_rd_byte;
	always @(posedge clk_cpu) begin
		if (rom_en) begin
			rom_rd_byte <= rom[rom_addr];
		end
	end
	assign rom_byte = rom_rd_byte;
	
	reg r_rom_vld;
	always @(posedge clk_cpu) begin
		r_rom_vld <= rom_en;
	end	
	assign rom_vld = r_rom_vld;
	
	reg rd_en_data;
	reg rd_en_idata;
	reg rd_en_xdata;
	always @(posedge clk_cpu) begin
		rd_en_data <= ram_rd_en_data;
		rd_en_idata <= ram_rd_en_idata;
		rd_en_xdata <= ram_rd_en_xdata;
	end
	
	reg [7:0] data [0:127];
	reg [7:0] data_rd_byte;
	always @(posedge clk_cpu) begin
		//if (ram_rd_en_data) begin
			data_rd_byte <= data[ram_rd_addr[6:0]];
		//end
		if (ram_wr_en_data) begin
			data[ram_wr_addr[6:0]] = ram_wr_byte;
		end		
	end
	
	reg [7:0] idata [0:127];
	reg [7:0] idata_rd_byte;
	always @(posedge clk_cpu) begin
		//if (ram_rd_en_idata) begin
			idata_rd_byte <= idata[ram_rd_addr[6:0]];
		//end
		if (ram_wr_en_idata) begin
			idata[ram_wr_addr[6:0]] = ram_wr_byte;
		end		
	end

	reg [7:0] xdata [0:8191];
	reg [7:0] xdata_rd_byte;
	always @(posedge clk_cpu) begin
		//if (ram_rd_en_xdata) begin
			xdata_rd_byte <= xdata[ram_rd_addr];
		//end
		if (ram_wr_en_xdata && !ram_wr_addr[15]) begin
			xdata[ram_wr_addr] = ram_wr_byte;
		end		
	end
	
	reg [7:0] sfr;
	
	assign ram_rd_byte = rd_en_data ? data_rd_byte :
								(rd_en_idata ? idata_rd_byte :
								(rd_en_xdata ? xdata_rd_byte : sfr));
	assign ram_rd_vld = 1'b1;
	
	r8051 r8051(
	.clk(clk_cpu),
	.rst(rst_cpu),
	.cpu_en(1),
	.cpu_restart(0),
	
	.rom_en(rom_en),
	.rom_addr(rom_addr),
	.rom_byte(rom_byte),
	.rom_vld(rom_vld),
	
	.ram_rd_en_data(ram_rd_en_data),
	.ram_rd_en_sfr(ram_rd_en_sfr),
	.ram_rd_en_idata(ram_rd_en_idata),
	.ram_rd_en_xdata(ram_rd_en_xdata),
	.ram_rd_addr(ram_rd_addr),
	.ram_rd_byte(ram_rd_byte),
	.ram_rd_vld(ram_rd_vld),

	.ram_wr_en_data(ram_wr_en_data),
	.ram_wr_en_sfr(ram_wr_en_sfr),
	.ram_wr_en_idata(ram_wr_en_idata),
	.ram_wr_en_xdata(ram_wr_en_xdata),
	.ram_wr_addr(ram_wr_addr),
	.ram_wr_byte(ram_wr_byte));
	
	reg tx_stb;
	initial tx_stb = 1'b0;

	reg [7:0] tx_data;
	wire tx_busy;
	
	always @(posedge clk_cpu) begin
		if (tx_stb && (!tx_busy)) begin
			tx_stb <= 1'b0;
		end
		if (ram_wr_en_sfr && ram_wr_addr[0]) begin
			tx_data <= ram_wr_byte;
			tx_stb <= 1'b1;
		end
		if (ram_rd_en_sfr && ram_rd_addr[0]) begin
			sfr <= tx_busy ? 8'h01 : 8'h00;
		end
	end
	 
	txuartlite
		#(.CLOCKS_PER_BAUD(10'd347),
		  .TIMING_BITS(5'd10))
	transmitter(.i_clk(clk_uart),
					.i_wr(tx_stb),
					.i_data(tx_data),
					.o_uart_tx(uart_tx),
					.o_busy(tx_busy));
			
`ifdef VERILATOR
	always @(posedge clk) begin
		if (ram_wr_en_idata) begin
   		//$display("idata wr 0x%04x 0x%02x", ram_wr_addr, ram_wr_byte);
		end
		if (ram_rd_en_idata) begin
			//$display("idata rd 0x%04x 0x%02x", ram_rd_addr, idata[ram_rd_addr[6:0]]);
		end
	end
	always @(posedge clk) begin
		if (ram_wr_en_data) begin
   		//$display("data wr 0x%04x 0x%02x", ram_wr_addr, ram_wr_byte);
		end
		if (ram_rd_en_data) begin
			//$display("data rd 0x%04x 0x%02x", ram_rd_addr, data[ram_rd_addr[6:0]]);
		end
	end
	always @(posedge clk) begin
		if (ram_wr_en_sfr) begin
   		//$display("sfr wr 0x%04x 0x%02x", ram_wr_addr, ram_wr_byte);
		end
		if (ram_rd_en_sfr) begin
			//$display("sfr rd 0x%04x 0x%02x 0x%02x", ram_rd_addr, ram_rd_byte, tx_busy);
		end
	end
	always @(posedge clk) begin
		if (rom_en) begin
   		//$display("rom 0x%04x 0x%02x", rom_addr, rom[rom_addr[9:0]]);
		end
	end	
`endif			
endmodule	
