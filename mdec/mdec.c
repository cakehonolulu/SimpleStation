#include <mdec/mdec.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
		DECODE_MACROBLOCK,
		SET_QUANT_TABLES,
		SET_SCALE_TABLE
} COMMAND;

COMMAND current_cmd;
bool set_color_qt = false;
int parameters = 0;
int qt_index = 0;
int st_index = 0;
int blk_index = 0;
int current_loading_blk = 0;
int output_index = 0;
int dma_out_index = 0;

uint8_t luminance_qt[64] = { 0x02, 0x10, 0x10, 0x13, 0x10, 0x13, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x1a, 0x18, 0x1a, 0x1b, 0x1b, 0x1b, 0x1a, 0x1a, 0x1a, 0x1a, 0x1b, 0x1b, 0x1b, 0x1d, 0x1d, 0x1d, 0x22, 0x22, 0x22, 0x1d, 0x1d, 0x1d, 0x1b, 0x1b, 0x1d, 0x1d, 0x20, 0x20, 0x22, 0x22, 0x25, 0x26, 0x25, 0x23, 0x23, 0x22, 0x23, 0x26, 0x26, 0x28, 0x28, 0x28, 0x30, 0x30, 0x2e, 0x2e, 0x38, 0x38, 0x3a, 0x45, 0x45, 0x53, };
uint8_t color_qt[64] = {0x02, 0x10, 0x10, 0x13, 0x10, 0x13, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x1a, 0x18, 0x1a, 0x1b, 0x1b, 0x1b, 0x1a, 0x1a, 0x1a, 0x1a, 0x1b, 0x1b, 0x1b, 0x1d, 0x1d, 0x1d, 0x22, 0x22, 0x22, 0x1d, 0x1d, 0x1d, 0x1b, 0x1b, 0x1d, 0x1d, 0x20, 0x20, 0x22, 0x22, 0x25, 0x26, 0x25, 0x23, 0x23, 0x22, 0x23, 0x26, 0x26, 0x28, 0x28, 0x28, 0x30, 0x30, 0x2e, 0x2e, 0x38, 0x38, 0x3a, 0x45, 0x45, 0x53 };
int16_t st[64] = { 23170, 23170, 23170, 23170, 23170, 23170, 23170, 23170, 32138, 27245, 18204, 6392, -6393, -18205, -27246, -32139, 30273, 12539, -12540, -30274, -30274, -12540, 12539, 30273, 27245, -6393, -32139, -18205, 18204, 32138, 6392, -27246, 23170, -23171, -23171, 23170, 23170, -23171, -23171, 23170, 18204, -32139, 6392, 27245, -27246, -6393, 32138, -18205, 12539, -30274, 30273, -12540, -12540, 30273, -30274, 12539, 6392, -18205, 27245, -32139, 32138, -27246, 18204, -6393 };

uint8_t* output;

int16_t cr[64] = { 0 };
int16_t cb[64] = { 0 };
int16_t y1_[64] = { 0 };
int16_t y2[64] = { 0 };
int16_t y3[64] = { 0 };
int16_t y4[64] = { 0 };
int16_t dst[64];

int zigzag[64] = {
		  0 ,1 ,5 ,6 ,14,15,27,28,
		  2 ,4 ,7 ,13,16,26,29,42,
		  3 ,8 ,12,17,25,30,41,43,
		  9 ,11,18,24,31,40,44,53,
		  10,19,23,32,39,45,52,54,
		  20,22,33,38,46,51,55,60,
		  21,34,37,47,50,56,59,61,
		  35,36,48,49,57,58,62,63
	};

static int32_t signed10bit(int32_t n) { return (n << 22) >> 22; }

static int16_t saturate(int16_t val, int16_t min, int16_t max) {
		if (val > max) {
			return max;
		}
		else if (val < min) {
			return min;
		}
		else {
			return val;
		}
	}

int zagzig[64];

void idct_core(int16_t* blk) {
	int16_t* src = blk;
	int16_t* dstptr = &dst[0];

	for (int pass = 0; pass < 2; pass++) {
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				int sum = 0;
				for (int z = 0; z < 8; z++) {
					sum += src[y + z * 8] * (st[x + z * 8] / 8);
				}
				dstptr[x + y * 8] = (sum + 0xfff) / 0x2000;
			}
		}
		int16_t* tmp;
		tmp = src;
		src = dstptr;
		dstptr = tmp;
	}
}

void yuv_to_rgb(int xx, int yy, int16_t* yblk) {
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			int16_t R = cr[((x + xx) / 2) + ((y + yy) / 2) * 8];
			int16_t B = cb[((x + xx) / 2) + ((y + yy) / 2) * 8];
			int16_t G = (-0.3437 * B) + (-0.7143 * R);
			R = (1.402 * R);
			B = (1.772 * B);
			int16_t Y = yblk[x + y * 8];
			R = saturate(Y + R, -128, 127);
			G = saturate(Y + G, -128, 127);
			B = saturate(Y + B, -128, 127);
			R ^= 0x80; G ^= 0x80; B ^= 0x80;
			output[((x + xx) + (y + yy) * 16) * 3 + 0 + output_index] = R;
			output[((x + xx) + (y + yy) * 16) * 3 + 1 + output_index] = G;
			output[((x + xx) + (y + yy) * 16) * 3 + 2 + output_index] = B;
		}
	}
}

void m_mdec_init(m_simplestation_state *m_simplestation)
{
    m_simplestation->m_mdec = malloc(sizeof(m_psx_mdec_t));
    m_simplestation->m_mdec->m_status = (1 << 31);
    output = malloc(sizeof(uint16_t) * 0x50000);
    for (int i = 0; i < 64; i++) zagzig[zigzag[i]] = i;
	for (int i = 0; i < 0x50000; i++) output[i] = 0;
}

void m_mdec_cmd(uint32_t cmd, m_simplestation_state *m_simplestation)
{

}
