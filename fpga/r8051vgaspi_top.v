module r8051vga_top(
	input wire clk,
	input wire rst,	
	output vgaH,
	output vgaV,
	output reg [4:0] vgaR,
	output reg [5:0] vgaG,
	output reg [4:0] vgaB,	
	output wire uart_tx,
	input wire uart_rx,
	output reg flash_clk,
	output reg flash_cs,
	output reg flash_di,
	input flash_do);
	
	wire clk_cpu;
	wire rst_cpu;
	wire clk_uart;
	wire clk25m;
	wire clk50m;
	
`ifdef VERILATOR	
	assign clk_cpu = clk;
	assign rst_cpu = rst;
	assign clk_uart = clk;
	assign clk25m = clk;
	assign clk50m = clk;
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
		.c0(clk40m),
		.c1(clk25m),
		.c2(clk50m));	

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
	
	reg [15:0] vgacolor;
	initial vgacolor = 16'h0ffff;	
	
	reg LH;
	initial LH = 1'b1;
	
	initial flash_clk = 1'b1;
	initial flash_cs = 1'b1;
	initial flash_di = 1'b1;

	wire [7:0] rxuart_data;
	wire rxuart_wr;	
	reg rxuart_ready;
		
	always @(posedge clk_cpu) begin
		if (tx_stb && (!tx_busy)) begin
			tx_stb <= 1'b0;
		end
		if (ram_wr_en_sfr && ram_wr_addr[0]) begin
			tx_data <= ram_wr_byte;
			tx_stb <= 1'b1;
		end
		if (rxuart_wr) begin
			rxuart_ready <= 1'b1;
		end		
		if (ram_rd_en_sfr && ram_rd_addr[0]) begin
			sfr <= rxuart_data;
			rxuart_ready <= 1'b0;
		end		
		if (ram_rd_en_sfr && ram_rd_addr[1]) begin
			sfr <= {6'h00, rxuart_ready, tx_busy};
		end
		if (ram_wr_en_sfr && ram_wr_addr[2]) begin
			LH <= ~LH;
			if (LH) begin
				vgacolor[7:0] <= ram_wr_byte;
			end else begin
				vgacolor[15:8] <= ram_wr_byte;
			end
		end
		if (ram_rd_en_sfr && ram_rd_addr[2]) begin
			sfr <= {6'h00, vgaH, vgaV};
		end		
		if (ram_wr_en_sfr && ram_wr_addr[4]) begin
			flash_clk <= ram_wr_byte[0];
			flash_cs <= ram_wr_byte[1];
			flash_di <= ram_wr_byte[2];
			//$display("sfr wr 0x%04x: 0x%02x 0x%02x 0x%02x", ram_wr_addr, ram_wr_byte[0], ram_wr_byte[1], ram_wr_byte[2]);
		end
		if (ram_rd_en_sfr && ram_rd_addr[4]) begin
			sfr <= 8'h00 | flash_do;
		end
	end

	reg [7:0] vgabuffer [0:19][0:119];
	always @(posedge clk_cpu) begin
		if (ram_wr_en_xdata && ram_wr_addr[15]) begin
			vgabuffer[ram_wr_addr[4:0]][ram_wr_addr[14:8]] = ram_wr_byte;
		end
	end
	
	reg [7:0] pix;
	always @(negedge clk50m) begin
		pix <= vgabuffer[sx[9:5]][sy[9:2]];
	end
	
   wire [9:0] sx, sy;
   wire de;
   simple_480p display_inst (
       .clk_pix(clk25m),
       .rst_pix(0),
       .sx(sx),
       .sy(sy),
       .hsync(vgaH),
       .vsync(vgaV),
       .de(de)
   );
	
	wire paint;
	assign paint = pix[sx[4:2]] & de;

   always @(posedge clk25m) begin
       vgaR <= paint ? vgacolor[15:11] : 5'h0;
       vgaG <= paint ? vgacolor[10:5] : 6'h0;
       vgaB <= paint ? vgacolor[4:0] : 5'h0;
   end
	
	txuartlite
		#(.CLOCKS_PER_BAUD(10'd347),
		  .TIMING_BITS(5'd10))
	transmitter(.i_clk(clk_uart),
					.i_wr(tx_stb),
					.i_data(tx_data),
					.o_uart_tx(uart_tx),
					.o_busy(tx_busy));

	rxuartlite
		#(.CLOCKS_PER_BAUD(10'd347),
		  .TIMER_BITS(5'd10))
		receiver(
			.i_clk(clk_uart),
			.i_uart_rx(uart_rx),
			.o_wr(rxuart_wr),
			.o_data(rxuart_data));	
	
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
