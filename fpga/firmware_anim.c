#include <stdio.h>
#include <stdint.h>

#define W 160
#define H 120

__sfr __at(0xc1) uart;
__sfr __at(0xc2) uart_flag;
__sfr __at(0xc4) vgacolorL;
__sfr __at(0xc4) vgacolorH;
__sfr __at(0xc4) vgaHV;
__sfr __at(0x90) spi;

static __data uint8_t spi_data;
#define SPI_CLK_0 spi_data &= 0xfe; spi = spi_data;
#define SPI_CLK_1 spi_data |= 0x01; spi = spi_data;
#define SPI_CS_0 spi_data &= 0xfd; spi = spi_data;
#define SPI_CS_1 spi_data |= 0x02; spi = spi_data;
#define SPI_DI_0 spi_data &= 0xfb; spi = spi_data;
#define SPI_DI_1 spi_data |= 0x04; spi = spi_data;
#define SPI_DO (spi & 0x01)

static void SPI_Write(uint8_t in)
{
	for (uint8_t i = 0; i < 8; ++i)
	{
		if (in & 0x80)
		{
			SPI_DI_1;
		}
		else
		{
			SPI_DI_0;
		}
		SPI_CLK_0;
		SPI_CLK_1;
		in <<= 1;
	}
}
static uint8_t SPI_Read(void)
{
	uint8_t out = 0;
	for (uint8_t i = 0; i < 8; ++i)
	{
		SPI_CLK_0;
		SPI_CLK_1;
		out <<= 1;
		out |= SPI_DO;
	}
	return out;
}

static uint8_t SPIFLASH_Busy(void)
{
	SPI_CS_0;
	SPI_Write(0x05);
	uint8_t r0 = SPI_Read();
	SPI_CS_1;
	return r0 & 0x01;
}
static uint32_t SPIFLASH_Status(void)
{
	SPI_CS_0;
	SPI_Write(0x05);
	uint32_t r0 = SPI_Read();
	SPI_CS_1;

	SPI_CS_0;
	SPI_Write(0x35);
	uint32_t r1 = SPI_Read();
	SPI_CS_1;

	SPI_CS_0;
	SPI_Write(0x15);
	uint32_t r2 = SPI_Read();
	SPI_CS_1;

	return (r2 << 16) | (r1 << 8) | (r0);
}
static void SPIFLASH_WriteEnable(void)
{
	SPI_CS_0;
	SPI_Write(0x06);
	SPI_CS_1;
}
static void SPIFLASH_Erase(uint32_t addr)
{
	SPIFLASH_WriteEnable();
	SPI_CS_0;
	SPI_Write(0x20);
	SPI_Write((addr >> 16) & 0xff);
	SPI_Write((addr >> 8) & 0xff);
	SPI_Write(addr & 0xff);
	SPI_CS_1;
	while (SPIFLASH_Busy());
}

int putchar(int c)
{
	while (uart_flag & 0x01);
	uart = c;
	return c;
}

static uint8_t __xdata buffer[20][120];
static inline void drawpixel(int16_t x, int16_t y)
{
	if (x < 0 || x > W - 1 || y < 0 || y > H - 1)
		return;
	buffer[x >> 3][y] |= (uint8_t)1 << (x & 7);
}
static inline void clear(void)
{
	for (uint8_t x = 0; x < 20; ++x)
	{
		for (uint8_t y = 0; y < 120; ++y)
		{
			buffer[x][y] = 0;
		}
	}
}
static inline void display(void)
{
	for (uint8_t x = 0; x < 20; ++x)
	{
		for (uint8_t y = 0; y < 120; ++y)
		{
			(*(uint8_t volatile __xdata*)((uint16_t)0x8000 + (uint16_t)x + (uint16_t)y * 256)) = buffer[x][y];
		}
	}
}
static uint8_t charbuffer[120];
static uint8_t charcount;
void main(void)
{
	printf_small("Hello, world!\n");
	spi_data = 0xff;
	SPI_CS_0;
	SPI_Write(0x9F);
	uint8_t RDID[3];
	RDID[0] = SPI_Read();
	RDID[1] = SPI_Read();
	RDID[2] = SPI_Read();
	SPI_CS_1;
	printf("RDID:%02x%02x%02x\r\n", RDID[0], RDID[1], RDID[2]);

#define VGACOLOR 0X9b43
	//vgacolorL = VGACOLOR & 0xff;
	//vgacolorH = (VGACOLOR >> 8) & 0xff;

	uint16_t vgacolor;
	vgacolor = 0;

	uint8_t r, g, b;
	r = (VGACOLOR >> 11) & 0x1f;
	g = (VGACOLOR >> 5) & 0x3f;
	b = (VGACOLOR >> 0) & 0x1f;

	uint8_t rf, gf, bf;
	rf = 0;	gf = 0;	bf = 0;

	uint8_t vblank;
	vblank = 0;
	uint8_t vblank_count;
	vblank_count = 0;

	while (1)
	{
		SPI_CS_0;
		SPI_Write(0x03);
		SPI_Write((0x200000 >> 16) & 0xff);
		SPI_Write((0x200000 >> 8) & 0xff);
		SPI_Write(0x200000 & 0xff);
		for (uint8_t i = 0; i < 21; ++i)
		{
			for (uint8_t x = 0; x < 20; ++x)
			{
				for (uint8_t y = 0; y < 120; ++y)
				{
					(*(uint8_t volatile __xdata*)((uint16_t)0x8000 + (uint16_t)x + (uint16_t)y * 256)) = SPI_Read();
				}
			}

			if (r == 0) rf = 0;
			if (g == 0) gf = 0;
			if (b == 0) bf = 0;

			if (r == 0x1f) rf = 1;
			if (g == 0x3f) gf = 1;
			if (b == 0x1f) bf = 1;

			if (rf) r--; else r++;
			if (gf) g--; else g++;
			if (bf) b--; else b++;

			vgacolor = ((uint16_t)r << 11) | ((uint16_t)g << 5) | (uint16_t)b;
			vgacolorL = vgacolor & 0xff;
			vgacolorH = (vgacolor >> 8) & 0xff;
			while (1)
			{
				const uint8_t HV = vgaHV;
				const uint8_t vsync = !(HV & 0x01);
				const uint8_t hsync = !(HV & 0x02);
				if (vblank == 0 && vsync)
				{
					vblank = 1;
					if (vblank_count++ == 5)
					{
						vblank_count = 0;
						break;
					}
				}
				if (vblank == 1 && !vsync)
				{
					vblank = 0;
				}
			}
		}
		SPI_CS_1;
	}
}
