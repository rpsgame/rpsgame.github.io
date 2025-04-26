module riscv_top(
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
	
	wire clk100m;
	pll pll(
		.inclk0(clk),
		.c0(clk100m));	

	assign clk_cpu = clk100m;
	assign clk_uart = clk100m;	
	assign rst_cpu = !r_reset_cnt[1];	
`endif
	
   wire 	timer_irq;

   wire [31:0]	o_ibus_adr;
   wire 	    	o_ibus_cyc;
   reg [31:0] 	i_ibus_rdt;
   reg   	   i_ibus_ack;

   wire [31:0]	o_dbus_adr;
   wire [31:0]	o_dbus_dat;
   wire [3:0] 	o_dbus_sel;
   wire 	    	o_dbus_we ;
   wire 	    	o_dbus_cyc;
   reg [31:0] 	i_dbus_rdt;
   reg   	   i_dbus_ack;

   serv_rf_top
     #(.RESET_PC(32'h0000_0000),
       .RESET_STRATEGY("NONE"),
       .WITH_CSR(0),
		 .RF_WIDTH(2),
       .COMPRESSED(0),
       .ALIGN(0))
   cpu(.clk      		(clk_cpu),
       .i_rst    		(rst_cpu),
       .i_timer_irq  (timer_irq),
 
       .o_ibus_adr   (o_ibus_adr),
       .o_ibus_cyc   (o_ibus_cyc),
       .i_ibus_rdt   (i_ibus_rdt),
       .i_ibus_ack   (i_ibus_ack),
 
       .o_dbus_adr   (o_dbus_adr),
       .o_dbus_dat   (o_dbus_dat),
       .o_dbus_sel   (o_dbus_sel),
       .o_dbus_we    (o_dbus_we ),
       .o_dbus_cyc   (o_dbus_cyc),
       .i_dbus_rdt   (i_dbus_rdt),
       .i_dbus_ack   (i_dbus_ack),
       
       .o_ext_rs1    (),
       .o_ext_rs2    (),
       .o_ext_funct3 (),
       .i_ext_rd     (32'b0),
       .i_ext_ready  (1'b0),
       .o_mdu_valid  ());

	wire ibus_req;
	wire dbus_req;
	wire dbus_rden;
	wire dbus_wren;
	wire [31:0] bus_addr;

	assign ibus_req = o_ibus_cyc && !i_ibus_ack;
	assign dbus_req = o_dbus_cyc && !i_dbus_ack;
	assign dbus_rden = dbus_req && !o_dbus_we;
	assign dbus_wren = dbus_req && o_dbus_we;
	assign bus_addr = ibus_req ? o_ibus_adr : o_dbus_adr;
	
	reg update_i;
	reg update_d;
	initial update_i = 1'b0;
	initial update_d = 1'b0;
	reg [31:0] r_rdata;
	
	reg [7:0] memory0 [0:4095];
	reg [7:0] memory1 [0:4095];
	reg [7:0] memory2 [0:4095];
	reg [7:0] memory3 [0:4095];
	initial begin
	$readmemh("firmware0.hex", memory0);
	$readmemh("firmware1.hex", memory1);
	$readmemh("firmware2.hex", memory2);
	$readmemh("firmware3.hex", memory3);
	end
	
	always @(posedge clk_cpu) begin
		r_rdata[ 7: 0] <= memory0[bus_addr[13:2]];
		r_rdata[15: 8] <= memory1[bus_addr[13:2]];
		r_rdata[23:16] <= memory2[bus_addr[13:2]];
		r_rdata[31:24] <= memory3[bus_addr[13:2]];
		if (dbus_wren && !o_dbus_adr[16]) begin
			if (o_dbus_sel[0]) memory0[o_dbus_adr[13:2]] <= o_dbus_dat[ 7: 0];
			if (o_dbus_sel[1]) memory1[o_dbus_adr[13:2]] <= o_dbus_dat[15: 8];
			if (o_dbus_sel[2]) memory2[o_dbus_adr[13:2]] <= o_dbus_dat[23:16];
			if (o_dbus_sel[3]) memory3[o_dbus_adr[13:2]] <= o_dbus_dat[31:24];
		end	
	end

	reg tx_stb;
	initial tx_stb = 1'b0;

	reg [7:0] tx_data;
	wire tx_busy;

	always @(posedge clk_cpu) begin
		i_ibus_ack <= 1'b0;
		i_dbus_ack <= 1'b0;
		
		if (update_i) begin
			update_i <= 1'b0;
			i_ibus_ack <= 1'b1;
			i_ibus_rdt <= r_rdata;
		end
		if (update_d) begin
			update_d <= 1'b0;
			i_dbus_ack <= 1'b1;
			i_dbus_rdt <= r_rdata;
		end
		
		if (dbus_wren) begin
			i_dbus_ack <= 1'b1;
		end

		if (ibus_req && !update_i) begin
			update_i <= 1'b1;
		end

		if (dbus_rden && !update_d && !o_dbus_adr[16]) begin
			update_d <= 1'b1;
		end

		if (dbus_wren && o_dbus_adr[16]) begin
			tx_data <= o_dbus_dat[7:0];
			tx_stb <= 1'b1;
		end

		if (tx_stb && (!tx_busy)) begin
			tx_stb <= 1'b0;
		end

		if (dbus_rden && o_dbus_adr[16]) begin
			i_dbus_ack <= 1'b1;
			i_dbus_rdt[7:0] <= tx_busy ? 8'h01 : 8'h00;
		end
	end

	txuartlite
		#(.CLOCKS_PER_BAUD(10'd868),
		  .TIMING_BITS(5'd10))
	transmitter(.i_clk(clk_uart),
					.i_wr(tx_stb),
					.i_data(tx_data),
					.o_uart_tx(uart_tx),
					.o_busy(tx_busy));

`ifdef VERILATOR
	always @(posedge clk) begin
		if (i_ibus_ack) begin
			//$display("i 0x%08x 0x%08x", o_ibus_adr, i_ibus_rdt);
		end
		if (i_dbus_ack) begin
			//$display("d 0x%08x 0x%08x", o_dbus_adr, o_dbus_dat);
		end
	end
`endif

endmodule
