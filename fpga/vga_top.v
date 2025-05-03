module vga_top(
	input wire clk,
	input wire rst,
	output vgaH,
	output vgaV,
	output reg [4:0] vgaR,
	output reg [5:0] vgaG,
	output reg [4:0] vgaB);
	
	wire clk25m;
	wire clk100m;
	
`ifdef VERILATOR	
	assign clk25m = clk;
	assign clk100m = clk;
`else
	pll pll(
		.inclk0(clk),
		.c0(clk25m),
		.c1(clk100m));
`endif

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

	reg [15:0] doge_palette_256[0:255];
	initial begin
	$readmemh("doge_palette_256.hex", doge_palette_256);
	end

	reg [7:0] doge_160120_256[0:(160*120-1)];
	initial begin
	$readmemh("doge_160120_256.hex", doge_160120_256);
	end
	
	reg [7:0] idx;
	always @(posedge clk100m) begin
		idx <= doge_160120_256[sy[9:2] * 160 + sx[9:2]];
	end
	
	reg [15:0] color;
	always @(posedge clk100m) begin
		color <= doge_palette_256[idx];
	end

	always @(posedge clk25m) begin
		if (de) begin
			vgaR <= color[15:11];
			vgaG <= color[10:5];
			vgaB <= color[4:0];
		end else begin
			vgaR <= 5'h0;
			vgaG <= 6'h1;
			vgaB <= 5'h3;
		end
	end			
endmodule	