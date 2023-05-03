#include <timer/timer.h>

typedef enum {
    COUNT = 0x1F801100,
    MODE  = 0x1F801104,
    COMP  = 0x1F801108,
} TimerReg;

/* Timer mode register */
typedef struct {
    bool gate; // GATE enable
    uint8_t   gats; // GATe Select
    bool zret; // Zero RETurn
    bool cmpe; // CoMPare Enable
    bool ovfe; // OVerFlow Enable
    bool rept; // REPeaT interrupt
    bool levl; // LEVL
    uint8_t   clks; // CLocK Select
    bool intf; // INTerrupt Flag
    bool equf; // EQUal Flag
    bool ovff; // OVerFlow Flag
} Mode;

/* Timer */
typedef struct {
    Mode mode; // T_MODE

    uint32_t count; // T_COUNT
    uint16_t comp;  // T_COMP

    // Prescaler
    uint16_t subcount;
    uint16_t prescaler;

    bool isPaused;
} Timer;

Timer timers[3];

/* Returns timer ID from address */
int getTimer(uint32_t addr) { 
    switch ((addr >> 4) & 0xFF) {
        case 0x10: return 0;
        case 0x11: return 1;
        case 0x12: return 2;
        default:
            printf("[Timer     ] Invalid timer\n");

            exit(0);
            break;
    }
}

void sendInterrupt(int tmID, m_simplestation_state *m_simplestation) {

    if (timers[tmID].mode.intf) m_interrupts_request((m_int_types) (tmID + 4), m_simplestation);

    if (timers[tmID].mode.rept && timers[tmID].mode.levl) {
        timers[tmID].mode.intf = !timers[tmID].mode.intf; // Toggle interrupt flag
    } else {
        timers[tmID].mode.intf = false;
    }
}

void timers_init() {
    memset(&timers, 0, 3 * sizeof(Timer));

    for (int i = 0; i < sizeof(timers); i++)
    {
        timers[i].prescaler = 1;
    }
}

uint16_t timers_read(uint32_t addr, m_simplestation_state *m_simplestation) {
    uint16_t data;

    // Get channel ID
    const auto chn = getTimer(addr);

    switch ((addr & ~0xFF0) | (1 << 8)) {
        case COUNT:
            //printf("[Timer     ] 16-bit read @ T%d_COUNT\n", chn);
            return timers[chn].count;
        case MODE:
            {
                //printf("[Timer     ] 16-bit read @ T%d_MODE\n", chn);

                data  = timers[chn].mode.gate;
                data |= timers[chn].mode.gats << 1;
                data |= timers[chn].mode.zret << 3;
                data |= timers[chn].mode.cmpe << 4;
                data |= timers[chn].mode.ovfe << 5;
                data |= timers[chn].mode.rept << 6;
                data |= timers[chn].mode.levl << 7;
                data |= timers[chn].mode.clks << 8;
                data |= timers[chn].mode.intf << 10;
                data |= timers[chn].mode.equf << 11;
                data |= timers[chn].mode.ovff << 12;

                /* Clear interrupt flags */

                timers[chn].mode.equf = false;
                timers[chn].mode.ovff = false;
            }
            break;
        case COMP:
            //printf("[Timer     ] 16-bit read @ T%d_COMP\n", chn);
            return timers[chn].comp;
        default:
            printf("[Timer     ] Unhandled 16-bit read @ 0x%08X\n", addr);

            exit(0);
            break;
    }

    return data;
}

void timers_write(uint32_t addr, uint16_t data, m_simplestation_state *m_simplestation) {
    // Get channel ID
    const auto chn = getTimer(addr);

    switch ((addr & ~0xFF0) | (1 << 8)) {
        case COUNT:
            //printf("[Timer     ] 16-bit write @ T%d_COUNT = 0x%04X\n", chn, data);

            timers[chn].count = data;
            break;
        case MODE:
            {
                //printf("[Timer     ] 16-bit write @ T%d_MODE = 0x%04X\n", chn, data);

                timers[chn].mode.gate = data & 1;
                timers[chn].mode.gats = (data >> 1) & 3;
                timers[chn].mode.zret = data & (1 << 3);
                timers[chn].mode.cmpe = data & (1 << 4);
                timers[chn].mode.ovfe = data & (1 << 5);
                timers[chn].mode.rept = data & (1 << 6);
                timers[chn].mode.levl = data & (1 << 7);
                timers[chn].mode.clks = (data >> 8) & 3;

                timers[chn].mode.intf = true; // Always reset to 1

                timers[chn].isPaused = false;

                if (timers[chn].mode.gate) {
                    switch (chn) {
                        case 0: // HBLANK gate
                            printf("[Timer     ] Unhandled timer 0 gate\n");

                            exit(0);
                        case 1: // VBLANK gate
                            switch (timers[chn].mode.gats) {
                                case 0: break; // Pause during VBLANK
                                case 1: break; // Reset counter at VBLANK start
                                case 2: // Reset counter at VBLANK start, pause outside of VBLANK
                                    timers[chn].isPaused = true;
                                    break;
                                case 3: // Pause ONCE until VBLANK start
                                    timers[chn].isPaused = true;
                                    break;
                            }
                            break;
                        case 2:
                            switch (timers[chn].mode.gats) {
                                case 0: case 3: // Pause forever
                                    timers[chn].isPaused = true;
                                    break;
                                case 1: case 2: // Nothing
                                    break;
                            }
                            break;
                    }
                }

                if (timers[chn].mode.clks) {
                    switch (chn) {
                        case 1: break;
                        case 2: timers[chn].prescaler = 1 + 7 * (uint16_t)(timers[chn].mode.clks > 1); break;
                        default:
                            printf("[Timer     ] Unhandled clock source\n");

                            exit(0);
                    }
                }

                timers[chn].subcount = 0;
                timers[chn].count = 0;    // Always cleared
            }
            break;
        case COMP:
            //printf("[Timer     ] 16-bit write @ T%d_COMP = 0x%04X\n", chn, data);

            timers[chn].comp = data;

            if (!timers[chn].mode.levl) timers[chn].mode.intf = true; // Set INTF if in toggle mode
            break;
        default:
            printf("[Timer     ] Unhandled 16-bit write @ 0x%08X = 0x%04X\n", addr, data);

            exit(0);
    }
}

/* Steps timers */
void timer_step(int64_t c, m_simplestation_state *m_simplestation) {
    for (int i = 0; i < 3; i++) {

        /* Timers 0 and 1 have a different clock source if CLKS is odd */
        if ((timers[i].mode.clks & 1) && ((i == 0) || (i == 1))) continue;

        if (timers[i].isPaused) continue;

        timers[i].subcount += c;

        while (timers[i].subcount > timers[i].prescaler) {
               
            timers[i].count++;

            if (timers[i].count & (1 << 16)) {
                if (timers[i].mode.ovfe && !timers[i].mode.ovff) {
                    // Checking OVFF is necessary because timer IRQs are edge-triggered
                    timers[i].mode.ovff = true;

                    sendInterrupt(i, m_simplestation);
                }
            }

            timers[i].count &= 0xFFFF;

            if (timers[i].count == timers[i].comp) {
                if (timers[i].mode.cmpe && !timers[i].mode.equf) {
                    // Checking EQUF is necessary because timer IRQs are edge-triggered
                    timers[i].mode.equf = true;

                    sendInterrupt(i, m_simplestation);
                }

                if (timers[i].mode.zret) timers[i].count = 0;
            }

            timers[i].subcount -= timers[i].prescaler;
        }
    }
}