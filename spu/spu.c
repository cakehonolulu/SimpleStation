#include <spu/spu.h>
#include <ui/termcolour.h>


static const int32_t gaussTable[] = {
    -0x0001, -0x0001, -0x0001, -0x0001, -0x0001, -0x0001, -0x0001, -0x0001,
    -0x0001, -0x0001, -0x0001, -0x0001, -0x0001, -0x0001, -0x0001, -0x0001,
     0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0001,
     0x0001,  0x0001,  0x0001,  0x0002,  0x0002,  0x0002,  0x0003,  0x0003,
     0x0003,  0x0004,  0x0004,  0x0005,  0x0005,  0x0006,  0x0007,  0x0007,
     0x0008,  0x0009,  0x0009,  0x000A,  0x000B,  0x000C,  0x000D,  0x000E,
     0x000F,  0x0010,  0x0011,  0x0012,  0x0013,  0x0015,  0x0016,  0x0018,
     0x0019,  0x001B,  0x001C,  0x001E,  0x0020,  0x0021,  0x0023,  0x0025,
     0x0027,  0x0029,  0x002C,  0x002E,  0x0030,  0x0033,  0x0035,  0x0038,
     0x003A,  0x003D,  0x0040,  0x0043,  0x0046,  0x0049,  0x004D,  0x0050,
     0x0054,  0x0057,  0x005B,  0x005F,  0x0063,  0x0067,  0x006B,  0x006F,
     0x0074,  0x0078,  0x007D,  0x0082,  0x0087,  0x008C,  0x0091,  0x0096,
     0x009C,  0x00A1,  0x00A7,  0x00AD,  0x00B3,  0x00BA,  0x00C0,  0x00C7,
     0x00CD,  0x00D4,  0x00DB,  0x00E3,  0x00EA,  0x00F2,  0x00FA,  0x0101,
     0x010A,  0x0112,  0x011B,  0x0123,  0x012C,  0x0135,  0x013F,  0x0148,
     0x0152,  0x015C,  0x0166,  0x0171,  0x017B,  0x0186,  0x0191,  0x019C,
     0x01A8,  0x01B4,  0x01C0,  0x01CC,  0x01D9,  0x01E5,  0x01F2,  0x0200,
     0x020D,  0x021B,  0x0229,  0x0237,  0x0246,  0x0255,  0x0264,  0x0273,
     0x0283,  0x0293,  0x02A3,  0x02B4,  0x02C4,  0x02D6,  0x02E7,  0x02F9,
     0x030B,  0x031D,  0x0330,  0x0343,  0x0356,  0x036A,  0x037E,  0x0392,
     0x03A7,  0x03BC,  0x03D1,  0x03E7,  0x03FC,  0x0413,  0x042A,  0x0441,
     0x0458,  0x0470,  0x0488,  0x04A0,  0x04B9,  0x04D2,  0x04EC,  0x0506,
     0x0520,  0x053B,  0x0556,  0x0572,  0x058E,  0x05AA,  0x05C7,  0x05E4,
     0x0601,  0x061F,  0x063E,  0x065C,  0x067C,  0x069B,  0x06BB,  0x06DC,
     0x06FD,  0x071E,  0x0740,  0x0762,  0x0784,  0x07A7,  0x07CB,  0x07EF,
     0x0813,  0x0838,  0x085D,  0x0883,  0x08A9,  0x08D0,  0x08F7,  0x091E,
     0x0946,  0x096F,  0x0998,  0x09C1,  0x09EB,  0x0A16,  0x0A40,  0x0A6C,
     0x0A98,  0x0AC4,  0x0AF1,  0x0B1E,  0x0B4C,  0x0B7A,  0x0BA9,  0x0BD8,
     0x0C07,  0x0C38,  0x0C68,  0x0C99,  0x0CCB,  0x0CFD,  0x0D30,  0x0D63,
     0x0D97,  0x0DCB,  0x0E00,  0x0E35,  0x0E6B,  0x0EA1,  0x0ED7,  0x0F0F,
     0x0F46,  0x0F7F,  0x0FB7,  0x0FF1,  0x102A,  0x1065,  0x109F,  0x10DB,
     0x1116,  0x1153,  0x118F,  0x11CD,  0x120B,  0x1249,  0x1288,  0x12C7,
     0x1307,  0x1347,  0x1388,  0x13C9,  0x140B,  0x144D,  0x1490,  0x14D4,
     0x1517,  0x155C,  0x15A0,  0x15E6,  0x162C,  0x1672,  0x16B9,  0x1700,
     0x1747,  0x1790,  0x17D8,  0x1821,  0x186B,  0x18B5,  0x1900,  0x194B,
     0x1996,  0x19E2,  0x1A2E,  0x1A7B,  0x1AC8,  0x1B16,  0x1B64,  0x1BB3,
     0x1C02,  0x1C51,  0x1CA1,  0x1CF1,  0x1D42,  0x1D93,  0x1DE5,  0x1E37,
     0x1E89,  0x1EDC,  0x1F2F,  0x1F82,  0x1FD6,  0x202A,  0x207F,  0x20D4,
     0x2129,  0x217F,  0x21D5,  0x222C,  0x2282,  0x22DA,  0x2331,  0x2389,
     0x23E1,  0x2439,  0x2492,  0x24EB,  0x2545,  0x259E,  0x25F8,  0x2653,
     0x26AD,  0x2708,  0x2763,  0x27BE,  0x281A,  0x2876,  0x28D2,  0x292E,
     0x298B,  0x29E7,  0x2A44,  0x2AA1,  0x2AFF,  0x2B5C,  0x2BBA,  0x2C18,
     0x2C76,  0x2CD4,  0x2D33,  0x2D91,  0x2DF0,  0x2E4F,  0x2EAE,  0x2F0D,
     0x2F6C,  0x2FCC,  0x302B,  0x308B,  0x30EA,  0x314A,  0x31AA,  0x3209,
     0x3269,  0x32C9,  0x3329,  0x3389,  0x33E9,  0x3449,  0x34A9,  0x3509,
     0x3569,  0x35C9,  0x3629,  0x3689,  0x36E8,  0x3748,  0x37A8,  0x3807,
     0x3867,  0x38C6,  0x3926,  0x3985,  0x39E4,  0x3A43,  0x3AA2,  0x3B00,
     0x3B5F,  0x3BBD,  0x3C1B,  0x3C79,  0x3CD7,  0x3D35,  0x3D92,  0x3DEF,
     0x3E4C,  0x3EA9,  0x3F05,  0x3F62,  0x3FBD,  0x4019,  0x4074,  0x40D0,
     0x412A,  0x4185,  0x41DF,  0x4239,  0x4292,  0x42EB,  0x4344,  0x439C,
     0x43F4,  0x444C,  0x44A3,  0x44FA,  0x4550,  0x45A6,  0x45FC,  0x4651,
     0x46A6,  0x46FA,  0x474E,  0x47A1,  0x47F4,  0x4846,  0x4898,  0x48E9,
     0x493A,  0x498A,  0x49D9,  0x4A29,  0x4A77,  0x4AC5,  0x4B13,  0x4B5F,
     0x4BAC,  0x4BF7,  0x4C42,  0x4C8D,  0x4CD7,  0x4D20,  0x4D68,  0x4DB0,
     0x4DF7,  0x4E3E,  0x4E84,  0x4EC9,  0x4F0E,  0x4F52,  0x4F95,  0x4FD7,
     0x5019,  0x505A,  0x509A,  0x50DA,  0x5118,  0x5156,  0x5194,  0x51D0,
     0x520C,  0x5247,  0x5281,  0x52BA,  0x52F3,  0x532A,  0x5361,  0x5397,
     0x53CC,  0x5401,  0x5434,  0x5467,  0x5499,  0x54CA,  0x54FA,  0x5529,
     0x5558,  0x5585,  0x55B2,  0x55DE,  0x5609,  0x5632,  0x565B,  0x5684,
     0x56AB,  0x56D1,  0x56F6,  0x571B,  0x573E,  0x5761,  0x5782,  0x57A3,
     0x57C3,  0x57E2,  0x57FF,  0x581C,  0x5838,  0x5853,  0x586D,  0x5886,
     0x589E,  0x58B5,  0x58CB,  0x58E0,  0x58F4,  0x5907,  0x5919,  0x592A,
     0x593A,  0x5949,  0x5958,  0x5965,  0x5971,  0x597C,  0x5986,  0x598F,
     0x5997,  0x599E,  0x59A4,  0x59A9,  0x59AD,  0x59B0,  0x59B2,  0x59B3,
};

int16_t ginterpolate(uint8_t idx, int16_t s0, int16_t s1, int16_t s2, int16_t s3) {
    int32_t out;

    out  = gaussTable[0x0FF - idx] * s0;
    out += gaussTable[0x1FF - idx] * s1;
    out += gaussTable[0x100 + idx] * s2;
    out += gaussTable[0x000 + idx] * s3;

    return out >> 16;
}

const int64_t SPU_RATE = 0x300;

const uint32_t SPU_BASE = 0x1F801C00;
const uint32_t RAM_SIZE = 0x80000;

/* --- SPU registers --- */

typedef enum {
    VOLL    = 0x1F801C00,
    VOLR    = 0x1F801C02,
    PITCH   = 0x1F801C04,
    ADDR    = 0x1F801C06,
    ADSR    = 0x1F801C08,
    ADSRVOL = 0x1F801C0C,
    LOOP    = 0x1F801C0E,
    MVOLL   = 0x1F801D80,
    MVOLR   = 0x1F801D82,
    VLOUT   = 0x1F801D84,
    VROUT   = 0x1F801D86,
    KON     = 0x1F801D88,
    KOFF    = 0x1F801D8C,
    PMON    = 0x1F801D90,
    NON     = 0x1F801D94,
    REVON   = 0x1F801D98,
    VON     = 0x1F801D9C,
    REVADDR = 0x1F801DA2,
    SPUADDR = 0x1F801DA6,
    SPUDATA = 0x1F801DA8,
    SPUCNT  = 0x1F801DAA,
    FIFOCNT = 0x1F801DAC,
    SPUSTAT = 0x1F801DAE,
    CDVOLL  = 0x1F801DB0,
    CDVOLR  = 0x1F801DB2,
    EVOLL   = 0x1F801DB4,
    EVOLR   = 0x1F801DB6,
    CVOLL   = 0x1F801DB8,
    CVOLR   = 0x1F801DBA,
} SPUReg;

/* SPU control */
typedef struct {
    bool cden;   // CD audio enable
    bool exten;  // External audio enable
    bool cdrev;  // CD reverb enable
    bool extrev; // External reverb enable
    uint8_t   trxmod; // Transfer mode
    bool irqen;  // IRQ enable
    bool reven;  // Reverb master enable
    uint8_t   nstep;  // Noise step
    uint8_t   nshift; // Noise shift
    bool unmute; // (un)mute SPU
    bool spuen;  // SPU enable
} SPUCNT_;

/* SPU status */
typedef struct {
    uint8_t   spumod; // SPU mode
    bool irq9;   // IRQ flag
    bool dmard;  // DMA read (0 = write)
    bool wrreq;  // DMA write request
    bool rdreq;  // DMA read request
    bool busy;   // SPU busy
    bool cbuf;   // Current capture buffer
} SPUSTAT_;

typedef struct {
    bool on; // Set by KON

    int16_t voll, volr;

    uint16_t pitch;
    uint32_t pitchCounter;

    uint32_t addr, loopaddr, caddr;

    uint8_t   adpcmBlock[16];
    bool hasBlock;

    int16_t s[4]; // Most recent samples
} Voice;

uint8_t *ram;

int16_t sound[2 * 2048];
int soundIdx = 0;

SPUCNT_  spucnt;
SPUSTAT_ spustat;

uint32_t kon, koff;

uint32_t spuaddr, caddr; // SPU address, current address

Voice voices[24];

uint64_t idStep;

/* Returns true if address is in range [base;size] */
bool inRange(uint64_t addr, uint64_t base, uint64_t size) {
    return (addr >= base) && (addr < (base + size));
}

/* Steps the SPU, calculates current sample */
void step() {
    sound[2 * soundIdx + 0] = 0;
    sound[2 * soundIdx + 1] = 0;

    if (spucnt.spuen && spucnt.unmute) {
        for (int i = 0; i < 24; i++) {
            Voice *v = &voices[i];

            if (!v->on || !v->pitch) continue;
            
            if (!v->hasBlock) {
                /* Load new ADPCM block */

                memcpy(v->adpcmBlock, &ram[v->caddr], 16);

                v->hasBlock = true;
            }

            /* Increment pitch counter */

            uint32_t step = v->pitch;

            /* TODO: handle PMON */

            if (step > 0x3FFF) step = 0x4000;

            v->pitchCounter += step;

            /* Fetch new ADPCM sample */

            const auto adpcmIdx = v->pitchCounter >> 12;

            if (adpcmIdx < 28) {
                /* Move old samples */

                for (int i = 0; i < 3; i++) {
                    v->s[i] = v->s[i + 1];
                }

                v->s[3] = (int16_t)(int8_t)((v->adpcmBlock[2 + (adpcmIdx >> 1)] >> (4 * (adpcmIdx & 1))) << 4);

                const auto s = ginterpolate(v->pitchCounter >> 3, v->s[0], v->s[1], v->s[2], v->s[3]);

                sound[2 * soundIdx + 0] += s * (v->voll >> 7);
                sound[2 * soundIdx + 1] += s * (v->volr >> 7);
            } else {
                const auto flags = v->adpcmBlock[1];

                if (flags & (1 << 2)) {
                    v->loopaddr = v->caddr;
                }

                switch (flags & 3) {
                    case 0: case 2:
                        v->caddr += 16;
                        break;
                    case 3: case 1:
                        v->caddr = v->loopaddr;

                        v->loopaddr = v->caddr;

                        v->on = false;
                        break;
                }

                v->pitchCounter = 0;

                v->hasBlock = false;
            }
        }
    }

    soundIdx++;
}

void doKOFF() {
    for (int i = 0; i < 24; i++) {
        Voice *v = &voices[i];

        if (koff & (1 << i)) {
            /* TODO: ADSR */
            v->on = false;
        }
    }
}

/* Handle Key On event */
void doKON() {
    for (int i = 0; i < 24; i++) {
        Voice * v = &voices[i];

        if (kon & (1 << i)) {
            /* TODO: set initial volume etc */

            v->caddr = 8 * v->addr;

            v->on = true;
        }
    }
}

void spu_init() {
    const int16_t out = 0;

    memset(&voices, 0, 24 * sizeof(Voice));

    ram = malloc(sizeof(uint16_t) * RAM_SIZE);
}

/* Write audio to file */
void save() {

    soundIdx = 0;
}

uint16_t readRAM(uint32_t addr) {
    assert(addr < RAM_SIZE);

    uint16_t data;

    memcpy(&data, &ram[addr], 2);

    return data;
}

/* Writes a halfword to SPU RAM */
void writeRAM(uint16_t data) {
    //assert(caddr < RAM_SIZE);

    //printf("[SPU       ] [0x%05X] = 0x%04X\n", caddr, data);

    memcpy(&ram[caddr], &data, 2);

    ram[caddr] = data;

    caddr += 2;
}

uint16_t m_spu_read(uint32_t addr, m_simplestation_state *m_simplestation) {
    uint16_t data;

    if (addr < (uint32_t) (MVOLL)) { // SPU voices
        const auto vID = (addr >> 4) & 0x1F;

        Voice *v = &voices[vID];

        switch (addr & ~(0x1F << 4)) {
            case (uint32_t) (VOLL):
                //printf("[SPU       ] 16-bit read @ V%u_VOLL\n", vID);
                return v->voll;
            case (uint32_t) (VOLR):
                //printf("[SPU       ] 16-bit read @ V%u_VOLR\n", vID);
                return v->volr;
            case (uint32_t) (PITCH):
                //printf("[SPU       ] 16-bit read @ V%u_PITCH\n", vID);
                return v->pitch;
            case (uint32_t) (ADDR):
                //printf("[SPU       ] 16-bit read @ V%u_ADDR\n", vID);
                return v->addr;
            case (uint32_t) (ADSR):
                //printf("[SPU       ] 16-bit read @ V%u_ADSR_HI\n", vID);
                break;
            case (uint32_t) (ADSR) + 2:
                //printf("[SPU       ] 16-bit read @ V%u_ADSR_HI\n", vID);
                break;
            case (uint32_t) (ADSRVOL):
                //printf("[SPU       ] 16-bit read @ V%u_ADSRVOL\n", vID);
                break;
            default:
                //printf("[SPU       ] Unhandled 16-bit voice %u read @ 0x%08X\n", vID, addr);

                //exit(0);
break;
        }

        return 0;
    } else if (inRange(addr, SPU_BASE + 0x188, 0x18)) { // Voice control
        switch (addr) {
            case (uint32_t) (KON):
                //printf("[SPU       ] 16-bit read @ KON_LO\n");
                return kon;
            case (uint32_t) (KON) + 2:
                //printf("[SPU       ] 16-bit read @ KON_HI\n");
                return kon >> 16;
            case (uint32_t) (KOFF):
                //printf("[SPU       ] 16-bit read @ KOFF_LO\n");
                return koff;
            case (uint32_t) (KOFF) + 2:
                //printf("[SPU       ] 16-bit read @ KOFF_HI\n");
                return koff >> 16;
            case (uint32_t) (NON):
                //printf("[SPU       ] 16-bit read @ NON_LO\n");
                return 0;
            case (uint32_t) (NON) + 2:
                //printf("[SPU       ] 16-bit read @ NON_HI\n");
                return 0;
            case (uint32_t) (REVON):
                //printf("[SPU       ] 16-bit read @ REVON_LO\n");
                return 0;
            case (uint32_t) (REVON) + 2:
                //printf("[SPU       ] 16-bit read @ REVON_HI\n");
                return 0;
            default:
                //printf("[SPU       ] Unhandled 16-bit voice control read @ 0x%08X\n", addr);

                //exit(0);
break;
        }
    } else if (inRange(addr, SPU_BASE + 0x1A2, 0x1E)) { // SPU control
        switch (addr) {
            case (uint32_t) (SPUADDR):
                //printf("[SPU       ] 16-bit read @ SPUADDR\n");
                return spuaddr;

            case (uint32_t) (SPUCNT):
                //printf("[SPU       ] 16-bit read @ SPUCNT\n");

                data  = spucnt.cden   << 0;
                data |= spucnt.exten  << 1;
                data |= spucnt.cdrev  << 2;
                data |= spucnt.extrev << 3;
                data |= spucnt.trxmod << 4;
                data |= spucnt.irqen  << 6;
                data |= spucnt.reven  << 7;
                data |= spucnt.nstep  << 8;
                data |= spucnt.nshift << 10;
                data |= spucnt.unmute << 14;
                data |= spucnt.spuen  << 15;
                break;
            case (uint32_t) (FIFOCNT):
                //printf("[SPU       ] 16-bit read @ FIFOCNT\n");
                return 4;
            case (uint32_t) (SPUSTAT):
                //printf("[SPU       ] 16-bit read @ SPUSTAT\n");

                data  = spustat.spumod;
                data |= spustat.irq9  << 6;
                data |= spustat.dmard << 7;
                data |= spustat.wrreq << 8;
                data |= spustat.rdreq << 9;
                data |= spustat.busy  << 10;
                data |= spustat.cbuf  << 11;
                break;
            case (uint32_t) (CVOLL):
                //printf("[SPU       ] 16-bit read @ CVOLL\n");
                return 0;
            case (uint32_t) (CVOLR):
                //printf("[SPU       ] 16-bit read @ CVOLR\n");
                return 0;
            default:
                //printf("[SPU       ] Unhandled control 16-bit read @ 0x%08X\n", addr);

                //exit(0);
break;
        }
    } else {
        //printf("[SPU       ] Unhandled 16-bit read @ 0x%08X\n", addr);

        //exit(0);

    }

    return data;
}

void m_spu_write(uint32_t addr, uint16_t data, m_simplestation_state *m_simplestation) {
    if (addr < (uint32_t) (MVOLL)) { // SPU voices
        const auto vID = (addr >> 4) & 0x1F;

        Voice *v = &voices[vID];

        switch (addr & ~(0x1F << 4)) {
            case (uint32_t) (VOLL):
                //printf("[SPU       ] 16-bit write @ V%u_VOLL = 0x%04X\n", vID, data);
                v->voll = data;
                break;
            case (uint32_t) (VOLR):
                //printf("[SPU       ] 16-bit write @ V%u_VOLR = 0x%04X\n", vID, data);
                v->volr = data;
                break;
            case (uint32_t) (PITCH):
                //printf("[SPU       ] 16-bit write @ V%u_PITCH = 0x%04X\n", vID, data);

                v->pitch = data;
                break;
            case (uint32_t) (ADDR):
                //printf("[SPU       ] 16-bit write @ V%u_ADDR = 0x%04X\n", vID, data);

                v->addr = data;
                break;
            case (uint32_t) (ADSR):
                //printf("[SPU       ] 16-bit write @ V%u_ADSR_LO = 0x%04X\n", vID, data);
                break;
            case (uint32_t) (ADSR) + 2:
                //printf("[SPU       ] 16-bit write @ V%u_ADSR_HI = 0x%04X\n", vID, data);
                break;
            case (uint32_t) (ADSRVOL):
                //printf("[SPU       ] 16-bit write @ V%u_ADSRVOL = 0x%04X\n", vID, data);
                break;
            case (uint32_t) (LOOP):
                //printf("[SPU       ] 16-bit write @ V%u_LOOP = 0x%04X\n", vID, data);

                v->loopaddr = 8 * data;
                break;
            default:
                //printf("[SPU       ] Unhandled 16-bit voice %u write @ 0x%08X = 0x%04X\n", vID, addr, data);

                //exit(0);
break;
        }
    } else if (inRange(addr, SPU_BASE + 0x180, 8)) { // SPU volume control
        switch (addr) {
            case (uint32_t) (MVOLL):
                //printf("[SPU       ] 16-bit write @ MVOLL = 0x%04X\n", data);
                break;
            case (uint32_t) (MVOLR):
                //printf("[SPU       ] 16-bit write @ MVOLR = 0x%04X\n", data);
                break;
            case (uint32_t) (VLOUT):
                //printf("[SPU       ] 16-bit write @ VLOUT = 0x%04X\n", data);
                break;
            case (uint32_t) (VROUT):
                //printf("[SPU       ] 16-bit write @ VROUT = 0x%04X\n", data);
                break;
            default:
                //printf("[SPU       ] Unhandled 16-bit control write @ 0x%08X = 0x%04X\n", addr, data);

                //exit(0);
break;
        }
    } else if (inRange(addr, SPU_BASE + 0x188, 0x18)) { // Voice control
        switch (addr) {
            case (uint32_t) (KON):
                //printf("[SPU       ] 16-bit write @ KON_LO = 0x%04X\n", data);

                kon = (kon & 0xFFFF0000) | data;
                break;
            case (uint32_t) (KON) + 2:
                //printf("[SPU       ] 16-bit write @ KON_HI = 0x%04X\n", data);

                kon = (kon & 0xFFFF) | (data << 16);

                doKON();
                break;
            case (uint32_t) (KOFF):
                //printf("[SPU       ] 16-bit write @ KOFF_LO = 0x%04X\n", data);

                koff = (koff & 0xFFFF0000) | data;
                break;
            case (uint32_t) (KOFF) + 2:
                //printf("[SPU       ] 16-bit write @ KOFF_HI = 0x%04X\n", data);

                koff = (koff & 0xFFFF) | (data << 16);

                doKOFF();
                break;
            case (uint32_t) (PMON):
                //printf("[SPU       ] 16-bit write @ PMON_LO = 0x%04X\n", data);
                break;
            case (uint32_t) (PMON) + 2:
                //printf("[SPU       ] 16-bit write @ PMON_HI = 0x%04X\n", data);
                break;
            case (uint32_t) (NON):
                //printf("[SPU       ] 16-bit write @ NON_LO = 0x%04X\n", data);
                break;
            case (uint32_t) (NON) + 2:
                //printf("[SPU       ] 16-bit write @ NON_HI = 0x%04X\n", data);
                break;
            case (uint32_t) (REVON):
                //printf("[SPU       ] 16-bit write @ REVON_LO = 0x%04X\n", data);
                break;
            case (uint32_t) (REVON) + 2:
                //printf("[SPU       ] 16-bit write @ REVON_HI = 0x%04X\n", data);
                break;
            case (uint32_t) (VON):
                //printf("[SPU       ] 16-bit write @ VON_LO = 0x%04X\n", data);
                break;
            case (uint32_t) (VON) + 2:
                //printf("[SPU       ] 16-bit write @ VON_HI = 0x%04X\n", data);
                break;
            default:
                //printf("[SPU       ] Unhandled 16-bit voice control write @ 0x%08X = 0x%04X\n", addr, data);

                //exit(0);
break;
        }
    } else if (inRange(addr, SPU_BASE + 0x1A2, 0x1E)) { // SPU control
        switch (addr) {
            case (uint32_t) (REVADDR):
                //printf("[SPU       ] 16-bit write @ REVADDR = 0x%04X\n", data);
                break;
            case (uint32_t) (SPUADDR):
                //printf("[SPU       ] 16-bit write @ SPUADDR = 0x%04X\n", data);

                spuaddr = data;
                
                caddr = 8 * spuaddr;
                break;
            case (uint32_t) (SPUDATA):
                //printf("[SPU       ] 16-bit write @ SPUDATA = 0x%04X\n", data);

                writeRAM(data);
                break;
            case (uint32_t) (SPUCNT):
                //printf("[SPU       ] 16-bit write @ SPUCNT = 0x%04X\n", data);

                spucnt.cden   = data & (1 << 0);
                spucnt.exten  = data & (1 << 1);
                spucnt.cdrev  = data & (1 << 2);
                spucnt.extrev = data & (1 << 3);
                spucnt.trxmod = (data >> 4) & 3;
                spucnt.irqen  = data & (1 << 6);
                spucnt.reven  = data & (1 << 7);
                spucnt.nstep  = (data >>  8) & 3;
                spucnt.nshift = (data >> 10) & 0xF;
                spucnt.unmute = data & (1 << 14);
                spucnt.spuen  = data & (1 << 15);

                spustat.spumod = data & 0x3F;
                spustat.dmard  = data & (1 << 5);

                if (!spucnt.irqen) spustat.irq9 = false;
                break;
            case (uint32_t) (FIFOCNT):
                //printf("[SPU       ] 16-bit write @ FIFOCNT = 0x%04X\n", data);

                assert(data == 0x0004); // ??
                break;
            case (uint32_t) (CDVOLL):
                //printf("[SPU       ] 16-bit write @ CDVOLL = 0x%04X\n", data);
                break;
            case (uint32_t) (CDVOLR):
                //printf("[SPU       ] 16-bit write @ CDVOLR = 0x%04X\n", data);
                break;
            case (uint32_t) (EVOLL):
                //printf("[SPU       ] 16-bit write @ EVOLL = 0x%04X\n", data);
                break;
            case (uint32_t) (EVOLR):
                //printf("[SPU       ] 16-bit write @ EVOLR = 0x%04X\n", data);
                break;
            default:
                //printf("[SPU       ] Unhandled 16-bit control write @ 0x%08X = 0x%04X\n", addr, data);

                //exit(0);
break;
        }
    } else if (inRange(addr, SPU_BASE + 0x1C0, 0x40)) {
        //printf("[SPU       ] Unhandled 16-bit reverb write @ 0x%08X = 0x%04X\n", addr, data);
    } else {
        //printf("[SPU       ] Unhandled 16-bit write @ 0x%08X = 0x%04X\n", addr, data);

        //exit(0);

    }
}
