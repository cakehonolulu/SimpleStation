#include <cpu/gte.h>
#include <cpu/cpu.h>

#include <math.h>

uint8_t gte_init(m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gte = calloc(1, sizeof(m_psx_gte_t));
}

uint8_t gte_exit(m_simplestation_state *m_simplestation)
{
    free(m_simplestation->m_gte);
}

static uint32_t countLeadingZeros16(uint16_t value) {
		int total_bits = sizeof(value) * 8;
    int res = 0;
    while ( !(value & (1 << (total_bits - 1))) )
    {
        value = (value << 1);
        res++;
    }
 
    return res;
	}

static uint32_t gte_divide(uint16_t numerator, uint16_t denominator) {
		if (numerator >= denominator * 2) {  // Division overflow
			//FLAG |= (1 << 31) | (1 << 17);
			return 0x1ffff;
		}

		static uint8_t table[] = {
			0xff, 0xfd, 0xfb, 0xf9, 0xf7, 0xf5, 0xf3, 0xf1, 0xef, 0xee, 0xec, 0xea, 0xe8, 0xe6, 0xe4, 0xe3, 0xe1, 0xdf,
			0xdd, 0xdc, 0xda, 0xd8, 0xd6, 0xd5, 0xd3, 0xd1, 0xd0, 0xce, 0xcd, 0xcb, 0xc9, 0xc8, 0xc6, 0xc5, 0xc3, 0xc1,
			0xc0, 0xbe, 0xbd, 0xbb, 0xba, 0xb8, 0xb7, 0xb5, 0xb4, 0xb2, 0xb1, 0xb0, 0xae, 0xad, 0xab, 0xaa, 0xa9, 0xa7,
			0xa6, 0xa4, 0xa3, 0xa2, 0xa0, 0x9f, 0x9e, 0x9c, 0x9b, 0x9a, 0x99, 0x97, 0x96, 0x95, 0x94, 0x92, 0x91, 0x90,
			0x8f, 0x8d, 0x8c, 0x8b, 0x8a, 0x89, 0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81, 0x7f, 0x7e, 0x7d, 0x7c, 0x7b,
			0x7a, 0x79, 0x78, 0x77, 0x75, 0x74, 0x73, 0x72, 0x71, 0x70, 0x6f, 0x6e, 0x6d, 0x6c, 0x6b, 0x6a, 0x69, 0x68,
			0x67, 0x66, 0x65, 0x64, 0x63, 0x62, 0x61, 0x60, 0x5f, 0x5e, 0x5d, 0x5d, 0x5c, 0x5b, 0x5a, 0x59, 0x58, 0x57,
			0x56, 0x55, 0x54, 0x53, 0x53, 0x52, 0x51, 0x50, 0x4f, 0x4e, 0x4d, 0x4d, 0x4c, 0x4b, 0x4a, 0x49, 0x48, 0x48,
			0x47, 0x46, 0x45, 0x44, 0x43, 0x43, 0x42, 0x41, 0x40, 0x3f, 0x3f, 0x3e, 0x3d, 0x3c, 0x3c, 0x3b, 0x3a, 0x39,
			0x39, 0x38, 0x37, 0x36, 0x36, 0x35, 0x34, 0x33, 0x33, 0x32, 0x31, 0x31, 0x30, 0x2f, 0x2e, 0x2e, 0x2d, 0x2c,
			0x2c, 0x2b, 0x2a, 0x2a, 0x29, 0x28, 0x28, 0x27, 0x26, 0x26, 0x25, 0x24, 0x24, 0x23, 0x22, 0x22, 0x21, 0x20,
			0x20, 0x1f, 0x1e, 0x1e, 0x1d, 0x1d, 0x1c, 0x1b, 0x1b, 0x1a, 0x19, 0x19, 0x18, 0x18, 0x17, 0x16, 0x16, 0x15,
			0x15, 0x14, 0x14, 0x13, 0x12, 0x12, 0x11, 0x11, 0x10, 0x0f, 0x0f, 0x0e, 0x0e, 0x0d, 0x0d, 0x0c, 0x0c, 0x0b,
			0x0a, 0x0a, 0x09, 0x09, 0x08, 0x08, 0x07, 0x07, 0x06, 0x06, 0x05, 0x05, 0x04, 0x04, 0x03, 0x03, 0x02, 0x02,
			0x01, 0x01, 0x00, 0x00, 0x00 };

		int shift = countLeadingZeros16(denominator);

		int r1 = (denominator << shift) & 0x7fff;
		int r2 = table[((r1 + 0x40) >> 7)] + 0x101;
		int r3 = ((0x80 - (r2 * (r1 + 0x8000))) >> 8) & 0x1ffff;
		uint32_t reciprocal = ((r2 * r3) + 0x80) >> 8;

		const uint32_t res = ((((uint64_t)reciprocal * (numerator << shift)) + 0x8000) >> 16);

		// Some divisions like 0xF015/0x780B result in 0x20000, but are saturated to 0x1ffff without setting FLAG
		return (uint32_t) MIN(res, 0x1ffff);
	}

static int32_t saturate(int32_t val, int32_t min, int32_t max) {
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

void pushZ(uint16_t value, m_simplestation_state *m_simplestation) {
	SZ0 = SZ1;
	SZ1 = SZ2;
	SZ2 = SZ3;
	SZ3 = value;
}

void pushColour(m_simplestation_state *m_simplestation) {
	RGB0 = RGB1;
	RGB1 = RGB2;

	const uint32_t col = (saturate(((int32_t)MAC1 >> 4), 0, 0xff) << 0) | (saturate(((int32_t)MAC2 >> 4), 0, 0xff) << 8) | (saturate(((int32_t)MAC3 >> 4), 0, 0xff) << 16) | (CD2 << 24);
	//RGB2 = 0x00345678 | (CD2 << 24);
	RGB2 = col;
}

void setIRFromMAC(bool lm, m_simplestation_state *m_simplestation) {
	IR1 = (int16_t)saturate(MAC1, -0x8000 * (lm ? 0 : 1), 0x7fff);
	IR2 = (int16_t)saturate(MAC2, -0x8000 * (lm ? 0 : 1), 0x7fff);
	IR3 = (int16_t)saturate(MAC3, -0x8000 * (lm ? 0 : 1), 0x7fff);
}

uint32_t readCop2d(uint32_t reg, m_simplestation_state *m_simplestation) {
	switch (reg) {
	case 1:
	case 3:
	case 5:
	case 8:
	case 9:
	case 10:
	case 11:
		return (uint32_t)(int16_t)cop2d[reg];
	
	case 7:
	case 16:
	case 17:
	case 18:
	case 19:
		return cop2d[reg] & 0xffff;

	case 15: // SXYP returns SXY2
		return cop2d[14];
	
	default:
		return cop2d[reg];
	}
}
void writeCop2d(uint32_t reg, uint32_t value, m_simplestation_state *m_simplestation) {
	switch (reg) {
	case 0: 
	case 2:
	case 4:
	case 6: {
		cop2d[reg] = value;
		break;
	}
	case 1:
	case 3:
	case 5: {
		cop2d[reg] = (uint32_t)(int16_t)(value);
		break;
	}
	default:
		cop2d[reg] = value;
		break;
	}
}

void writeCop2c(uint32_t reg, uint32_t value, m_simplestation_state *m_simplestation) {
	switch (reg) {
	case 4:
	case 12:
	case 20:
	case 26: 
	case 27:
	case 29:
	case 30:
		cop2c[reg] = (uint32_t)(int16_t)value;
		break;
	default:
		cop2c[reg] = value;
		break;
	}
}

void moveMFC2(uint32_t opcode, m_simplestation_state *m_simplestation) {
	REGS[(opcode >> 16) & 0x1f] = readCop2d((opcode >> 11) & 0x1f, m_simplestation);
}
void moveMTC2(uint32_t opcode, m_simplestation_state *m_simplestation) {
	writeCop2d((opcode >> 11) & 0x1f, REGS[(opcode >> 16) & 0x1f], m_simplestation);
}
void moveCFC2(uint32_t opcode, m_simplestation_state *m_simplestation) {
	REGS[(opcode >> 16) & 0x1f] = cop2c[(opcode >> 11) & 0x1f];
}
void moveCTC2(uint32_t opcode, m_simplestation_state *m_simplestation) {
	writeCop2c((opcode >> 11) & 0x1f, REGS[(opcode >> 16) & 0x1f], m_simplestation);
}

void commandRTPT(uint32_t opcode, m_simplestation_state *m_simplestation)
{	const int shift = sf(opcode) * 12;
	const int lm = 0;
	MAC1 = (int64_t) (((int64_t)(int32_t)(TRX) * 0x1000) + ((int16_t)RT11 * (int16_t)VX0) + ((int16_t)RT12 * (int16_t)VY0) + ((int16_t)RT13 * (int16_t)VZ0)) >> shift;
	MAC2 = (int64_t) (((int64_t)(int32_t)(TRY) * 0x1000) + ((int16_t)RT21 * (int16_t)VX0) + ((int16_t)RT22 * (int16_t)VY0) + ((int16_t)RT23 * (int16_t)VZ0)) >> shift;
	MAC3 = (int64_t) (((int64_t)(int32_t)(TRZ) * 0x1000) + ((int16_t)RT31 * (int16_t)VX0) + ((int16_t)RT32 * (int16_t)VY0) + ((int16_t)RT33 * (int16_t)VZ0)) >> shift;
	setIRFromMAC(lm, m_simplestation);
	int32_t newZ = (int32_t) (MAC3) >> ((1 - sf(opcode)) * 12);
	pushZ(newZ, m_simplestation);

	SXY0 = SXY1;
	SXY1 = SXY2;
	//uint32_t _proj_factor = (((((uint32_t)(H) * 0x20000) / (uint32_t)(SZ3)) + 1) / 2);
	int32_t _proj_factor =  (((((uint32_t)(H) * 0x20000) / (uint32_t)(SZ3)) + 1) / 2);
	//_proj_factor = gte_divide(H, SZ3);
	int64_t proj_factor = (int64_t)(_proj_factor);
	int64_t _x = (int64_t)(int16_t)(IR1);
	int64_t _y = (int64_t)(int16_t)(IR2);
	int64_t x = ((_x * proj_factor) + (int64_t)(int32_t)(OFX));
	int64_t y = ((_y * proj_factor) + (int64_t)(int32_t)(OFY));
	x = saturate((x >> 16), -0x400, 0x3ff);
	y = saturate((y >> 16), -0x400, 0x3ff);
	SETSX2(x);
	SETSY2(y);
	SXY2 = (y << 16) | x;
	//MAC0 = ((int64_t)(((((uint16_t)(H) * 0x20000) / (uint16_t)(SZ3)) + 1) / 2) * (int64_t)(int16_t)(IR1)) + OFX; SETSX2(((int32_t)(MAC0)) / 0x10000);
	//MAC0 = ((int64_t)(((((uint16_t)(H) * 0x20000) / (uint16_t)(SZ3)) + 1) / 2) * (int64_t)(int16_t)(IR2)) + OFY; SETSY2(((int32_t)(MAC0)) / 0x10000);
	//MAC0 = ((((((uint16_t)(H) * 0x20000) / (uint16_t)(SZ3)) + 1) / 2) * DQA) + DQB; IR0 = 
	/*int64_t depth = ((int64_t)DQB + ((int64_t)DQA * proj_factor));
	MAC0 = (int32_t)(depth);
	depth >>= 12;
	IR0 = (int16_t)(depth);*/


	MAC1 = (int64_t) (((int64_t)(int32_t)(TRX) * 0x1000) + ((int16_t)RT11 * (int16_t)VX1) + ((int16_t)RT12 * (int16_t)VY1) + ((int16_t)RT13 * (int16_t)VZ1)) >> shift;
	MAC2 = (int64_t) (((int64_t)(int32_t)(TRY) * 0x1000) + ((int16_t)RT21 * (int16_t)VX1) + ((int16_t)RT22 * (int16_t)VY1) + ((int16_t)RT23 * (int16_t)VZ1)) >> shift;
	MAC3 = (int64_t) (((int64_t)(int32_t)(TRZ) * 0x1000) + ((int16_t)RT31 * (int16_t)VX1) + ((int16_t)RT32 * (int16_t)VY1) + ((int16_t)RT33 * (int16_t)VZ1)) >> shift;
	setIRFromMAC(lm, m_simplestation);
	newZ = (int32_t) (MAC3) >> ((1 - sf(opcode)) * 12);
	pushZ(newZ, m_simplestation);

	SXY0 = SXY1;
	SXY1 = SXY2;
	//_proj_factor = (((((uint32_t)(H) * 0x20000) / (uint32_t)(SZ3)) + 1) / 2);
	_proj_factor = (((((uint32_t)(H) * 0x20000) / (uint32_t)(SZ3)) + 1) / 2);
	//_proj_factor = gte_divide(H, SZ3);
	proj_factor = (int64_t)(_proj_factor);
	_x = (int64_t)(int16_t)(IR1);
	_y = (int64_t)(int16_t)(IR2);
	x = ((_x * proj_factor) + (int64_t)(int32_t)(OFX));
	y = ((_y * proj_factor) + (int64_t)(int32_t)(OFY));
	x = saturate((x >> 16), -0x400, 0x3ff);
	y = saturate((y >> 16), -0x400, 0x3ff);
	SETSX2(x);
	SETSY2(y);
	SXY2 = (y << 16) | x;
	//MAC0 = ((int64_t)(((((uint16_t)(H) * 0x20000) / (uint16_t)(SZ3)) + 1) / 2) * (int64_t)(int16_t)(IR1)) + OFX; SETSX2(((int32_t)(MAC0)) / 0x10000);
	//MAC0 = ((int64_t)(((((uint16_t)(H) * 0x20000) / (uint16_t)(SZ3)) + 1) / 2) * (int64_t)(int16_t)(IR2)) + OFY; SETSY2(((int32_t)(MAC0)) / 0x10000);
	//MAC0 = ((((((uint16_t)(H) * 0x20000) / (uint16_t)(SZ3)) + 1) / 2) * DQA) + DQB; IR0 = 
	/*depth = ((int64_t)DQB + ((int64_t)DQA * proj_factor));
	MAC0 = (int32_t)(depth);
	depth >>= 12;
	IR0 = (int16_t)(depth);*/

	MAC1 = (int64_t) (((int64_t)(int32_t)(TRX) * 0x1000) + ((int16_t)RT11 * (int16_t)VX2) + ((int16_t)RT12 * (int16_t)VY2) + ((int16_t)RT13 * (int16_t)VZ2)) >> shift;
	MAC2 = (int64_t) (((int64_t)(int32_t)(TRY) * 0x1000) + ((int16_t)RT21 * (int16_t)VX2) + ((int16_t)RT22 * (int16_t)VY2) + ((int16_t)RT23 * (int16_t)VZ2)) >> shift;
	MAC3 = (int64_t) (((int64_t)(int32_t)(TRZ) * 0x1000) + ((int16_t)RT31 * (int16_t)VX2) + ((int16_t)RT32 * (int16_t)VY2) + ((int16_t)RT33 * (int16_t)VZ2)) >> shift;
	setIRFromMAC(lm, m_simplestation);
	newZ = (int32_t) (MAC3) >> ((1 - sf(opcode)) * 12);
	pushZ(newZ, m_simplestation);

	SXY0 = SXY1;
	SXY1 = SXY2;
	//_proj_factor = (((((uint32_t)(H) * 0x20000) / (uint32_t)(SZ3)) + 1) / 2);
	_proj_factor = (((((uint32_t)(H) * 0x20000) / (uint32_t)(SZ3)) + 1) / 2);
	//_proj_factor = gte_divide(H, SZ3);
	proj_factor = (int64_t)(_proj_factor);
	_x = (int64_t)(int16_t)(IR1);
	_y = (int64_t)(int16_t)(IR2);
	x = ((_x * proj_factor) + (int64_t)(int32_t)(OFX));
	y = ((_y * proj_factor) + (int64_t)(int32_t)(OFY));
	x = saturate((x >> 16), -0x400, 0x3ff);
	y = saturate((y >> 16), -0x400, 0x3ff);
	SETSX2(x);
	SETSY2(y);
	SXY2 = (y << 16) | x;
	//MAC0 = ((int64_t)(((((uint16_t)(H) * 0x20000) / (uint16_t)(SZ3)) + 1) / 2) * (int64_t)(int16_t)(IR1)) + OFX; SETSX2(((int32_t)(MAC0)) / 0x10000);
	//MAC0 = ((int64_t)(((((uint16_t)(H) * 0x20000) / (uint16_t)(SZ3)) + 1) / 2) * (int64_t)(int16_t)(IR2)) + OFY; SETSY2(((int32_t)(MAC0)) / 0x10000);
	//MAC0 = ((((((uint16_t)(H) * 0x20000) / (uint16_t)(SZ3)) + 1) / 2) * DQA) + DQB; IR0 = 
	int64_t depth = ((int64_t)DQB + ((int64_t)DQA * proj_factor));
	MAC0 = (int32_t)(depth);
	depth >>= 12;
	IR0 = saturate((int16_t)(depth), 0, 0x1000);
}

void commandNCLIP(uint32_t opcode, m_simplestation_state *m_simplestation) {
	MAC0 = (int64_t)((int32_t)(SX0) * (int32_t)(SY1)) + ((int32_t)(SX1) * (int32_t)(SY2)) + ((int32_t)(SX2) * (int32_t)(SY0)) - ((int32_t)(SX0) * (int32_t)(SY2)) - ((int32_t)(SX1) * (int32_t)(SY0)) - ((int32_t)(SX2) * (int32_t)(SY1));
}

void commandNCDS(uint32_t opcode, m_simplestation_state *m_simplestation) {
	const int shift = sf(opcode) * 12;
	const int lm = lm(opcode);
	MAC1 = (int32_t) ((int64_t)((int16_t)L11 * (int16_t)VX0) + (int64_t)((int16_t)L12 * (int16_t)VY0) + (int64_t)((int16_t)L13 * (int16_t)VZ0)) >> shift;
	MAC2 = (int32_t) ((int64_t)((int16_t)L21 * (int16_t)VX0) + (int64_t)((int16_t)L22 * (int16_t)VY0) + (int64_t)((int16_t)L23 * (int16_t)VZ0)) >> shift;
	MAC3 = (int32_t) ((int64_t)((int16_t)L31 * (int16_t)VX0) + (int64_t)((int16_t)L32 * (int16_t)VY0) + (int64_t)((int16_t)L33 * (int16_t)VZ0)) >> shift;
	//setIRFromMAC();
	IR1 = (int16_t)saturate(MAC1, -0x8000 * (lm ? 0 : 1), 0x7fff);
	IR2 = (int16_t)saturate(MAC2, -0x8000 * (lm ? 0 : 1), 0x7fff);
	IR3 = (int16_t)saturate(MAC3, -0x8000 * (lm ? 0 : 1), 0x7fff);
	MAC1 = (int32_t) (((int64_t)RBK * 0x1000) + ((int64_t)((int16_t)LR1 * (int16_t)IR1) + (int64_t)((int16_t)LR2 * (int16_t)IR2) + (int64_t)((int16_t)LR3 * (int16_t)IR3))) >> shift;
	MAC2 = (int32_t) (((int64_t)GBK * 0x1000) + ((int64_t)((int16_t)LG1 * (int16_t)IR1) + (int64_t)((int16_t)LG2 * (int16_t)IR2) + (int64_t)((int16_t)LG3 * (int16_t)IR3))) >> shift;
	MAC3 = (int32_t) (((int64_t)BBK * 0x1000) + ((int64_t)((int16_t)LB1 * (int16_t)IR1) + (int64_t)((int16_t)LB2 * (int16_t)IR2) + (int64_t)((int16_t)LB3 * (int16_t)IR3))) >> shift;
	//setIRFromMAC();
	IR1 = (int16_t)saturate(MAC1, -0x8000 * (lm ? 0 : 1), 0x7fff);
	IR2 = (int16_t)saturate(MAC2, -0x8000 * (lm ? 0 : 1), 0x7fff);
	IR3 = (int16_t)saturate(MAC3, -0x8000 * (lm ? 0 : 1), 0x7fff);
	MAC1 = ((int64_t)R * (int16_t)IR1) << 4;
	MAC2 = ((int64_t)G * (int16_t)IR2) << 4;
	MAC3 = ((int64_t)B * (int16_t)IR3) << 4;

	// Interpolate colour
	uint32_t _MAC1 = MAC1;
	uint32_t _MAC2 = MAC2;
	uint32_t _MAC3 = MAC3;
	MAC1 = (int32_t)((((int64_t)RFC << 12) - (int32_t)MAC1) >> shift);
	MAC2 = (int32_t)((((int64_t)GFC << 12) - (int32_t)MAC2) >> shift);
	MAC3 = (int32_t)((((int64_t)BFC << 12) - (int32_t)MAC3) >> shift);
	IR1 = (int16_t)saturate(MAC1, -0x8000, 0x7fff);	
	IR2 = (int16_t)saturate(MAC2, -0x8000, 0x7fff);	
	IR3 = (int16_t)saturate(MAC3, -0x8000, 0x7fff);
	MAC1 = (int32_t)(((int64_t)IR1 * IR0) + (int32_t)_MAC1) >> shift;
	MAC2 = (int32_t)(((int64_t)IR2 * IR0) + (int32_t)_MAC2) >> shift;
	MAC3 = (int32_t)(((int64_t)IR3 * IR0) + (int32_t)_MAC3) >> shift;
	IR1 = (int16_t)saturate(MAC1, -0x8000 * (lm ? 0 : 1), 0x7fff);
	IR2 = (int16_t)saturate(MAC2, -0x8000 * (lm ? 0 : 1), 0x7fff);
	IR3 = (int16_t)saturate(MAC3, -0x8000 * (lm ? 0 : 1), 0x7fff);
	pushColour(m_simplestation);
}

void commandAVSZ3(uint32_t opcode, m_simplestation_state *m_simplestation) {
	MAC0 = (int64_t)ZSF3 * ((uint16_t)SZ1 + (uint16_t)SZ2 + (uint16_t)SZ3);
	//MAC0 = 0x10000;
	MAC0 = ((int64_t)(ZSF3 * SZ1) + (ZSF3 * SZ2) + (ZSF3 * SZ3));
	OTZ = saturate((int32_t)MAC0 / 0x1000, 0, 0xffff);
}

void gte_execute(uint32_t opcode, m_simplestation_state *m_simplestation)
{
	switch (opcode & 0x3F)
	{
		case MOVE:
			switch ((opcode >> 21) & 0x1f) {
				case MFC2: moveMFC2(opcode, m_simplestation); break;
				case CFC2: moveCFC2(opcode, m_simplestation); break;
				case MTC2: moveMTC2(opcode, m_simplestation); break;
				case CTC2: moveCTC2(opcode, m_simplestation); break;
				default:
					printf("Unimplemented GTE MOVE instruction: 0x%x\n", (opcode >> 21) & 0x1f);
					exit(1);
			}
			break;
		
		case 0x06:
			cop2c[31] = 0;
			commandNCLIP(opcode, m_simplestation);
			break;

		case 0x13:
			cop2c[31] = 0;
			commandNCDS(opcode, m_simplestation);
			break;
		
		case 0x2D:
			cop2c[31] = 0;
			commandAVSZ3(opcode, m_simplestation);
			break;

		case 0x30:
			cop2c[31] = 0;
			commandRTPT(opcode, m_simplestation);
			break;

		default:
			printf(RED "[CPU] gte: Unimplemented 'GTE' Opcode: 0x%08X\n" NORMAL, opcode & 0x3F);
			m_simplestation_exit(m_simplestation, 1);
			break;
	}
}
