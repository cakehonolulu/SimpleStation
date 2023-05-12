#include <mdec/mdec.h>
#include <stdbool.h>
#include <stdint.h>

const int LUM_TABLE = 0;
const int COL_TABLE = 64;

uint32_t readData(m_simplestation_state *m_simplestation) {
    //printf("[MDEC      ] 32-bit read @ MDEC1\n");

    return 0;
}

uint32_t readStat(m_simplestation_state *m_simplestation) {
    //printf("[MDEC      ] 32-bit read @ MDEC0\n");

    uint32_t data;

    data  = m_simplestation->m_mdec->stat.rem;
    data |= m_simplestation->m_mdec->stat.blk   << 16;
    data |= m_simplestation->m_mdec->stat.b15   << 23;
    data |= m_simplestation->m_mdec->stat.sign  << 24;
    data |= m_simplestation->m_mdec->stat.dep   << 25;
    data |= m_simplestation->m_mdec->stat.oreq  << 27;
    data |= m_simplestation->m_mdec->stat.ireq  << 28;
    data |= m_simplestation->m_mdec->stat.busy  << 29;
    data |= m_simplestation->m_mdec->stat.full  << 30;
    data |= m_simplestation->m_mdec->stat.empty << 31;

    return data;
}

void writeCmd(uint32_t data, m_simplestation_state *m_simplestation) {
    printf("[MDEC      ] 32-bit write @ MDEC0 = 0x%08X\n", data);

    switch (m_simplestation->m_mdec->state) {
        case Idle:
            {
                const auto cmd = data >> 29;

                /* Always copied */
                m_simplestation->m_mdec->stat.b15  = data & (1 << 25);
                m_simplestation->m_mdec->stat.sign = data & (1 << 26);
                m_simplestation->m_mdec->stat.dep  = (data >> 27) & 3;

                switch (cmd) {
                    case NOP:
                        printf("[MDEC      ] NOP\n");

                        m_simplestation->m_mdec->stat.rem = data;
                        return;
                    case DecodeMacroblock: // TODO: handle this
                        printf("[MDEC      ] Decode Macroblock\n");

                        m_simplestation->m_mdec->cmdLen = data & 0xFFFF;

                        m_simplestation->m_mdec->state = ReceiveMacroblock;
                        break;
                    case SetQuantTables:
                        printf("[MDEC      ] Set Quant Tables\n");

                        m_simplestation->m_mdec->quantIdx = 0;

                        m_simplestation->m_mdec->cmdLen = (data & 1) ? 32 : 16; // cmd[0] == 1 sets lum and col

                        m_simplestation->m_mdec->state = ReceiveQuantTables;
                        break;
                    case SetScaleTable:
                        printf("[MDEC      ] Set Scale Table\n");

                        m_simplestation->m_mdec->scaleIdx = 0;

                        m_simplestation->m_mdec->cmdLen = 32;

                        m_simplestation->m_mdec->state = ReceiveScaleTable;
                        break;
                    default:
                        printf("[MDEC      ] Unhandled command %u\n", cmd);

                        exit(0);
                }

                m_simplestation->m_mdec->stat.busy = true;

                //dmac::setDRQ(Channel::MDECIN, true);
            }
            break;
        case ReceiveMacroblock:
            if (!--m_simplestation->m_mdec->cmdLen) {
                m_simplestation->m_mdec->stat.rem  = 0xFFFF;
                m_simplestation->m_mdec->stat.busy = false;

                /* Clear MDEC_IN request, set MDEC_OUT request */

                m_simplestation->m_mdec->stat.full = true;
                m_simplestation->m_mdec->stat.ireq = false;

                m_simplestation->m_mdec->stat.empty = false;
                m_simplestation->m_mdec->stat.oreq  = true;

                //dmac::setDRQ(Channel::MDECOUT, true);

                m_simplestation->m_mdec->state = Idle;
            }
            break;
        case ReceiveQuantTables:
            assert(m_simplestation->m_mdec->quantIdx < 128);

            //printf("%d, %d\n", quantIdx, cmdLen);

            memcpy(&m_simplestation->m_mdec->quantTable[m_simplestation->m_mdec->quantIdx], &data, 4);

            m_simplestation->m_mdec->quantIdx += 4;

            if (!--m_simplestation->m_mdec->cmdLen) {
                //printf("Done\n");

                m_simplestation->m_mdec->stat.rem  = 0xFFFF;
                m_simplestation->m_mdec->stat.busy = false;

                m_simplestation->m_mdec->state = Idle;
            }
            break;
        case ReceiveScaleTable:
            assert(m_simplestation->m_mdec->scaleIdx < 64);

            memcpy(&m_simplestation->m_mdec->scaleTable[m_simplestation->m_mdec->scaleIdx], &data, 4);

            m_simplestation->m_mdec->scaleIdx += 2;

            if (!--m_simplestation->m_mdec->cmdLen) {
                m_simplestation->m_mdec->stat.rem  = 0xFFFF;
                m_simplestation->m_mdec->stat.busy = false;

                m_simplestation->m_mdec->state = Idle;
            }
            break;
    }
}

void writeCtrl(uint32_t data, m_simplestation_state *m_simplestation) {
    printf("[MDEC      ] 32-bit write @ MDEC1 = 0x%08X\n", data);

    if (data & (1 << 31)) {
        printf("[MDEC      ] MDEC reset\n");

        m_simplestation->m_mdec->stat.rem  = 0;
        m_simplestation->m_mdec->stat.blk  = 0;
        m_simplestation->m_mdec->stat.b15  = false;
        m_simplestation->m_mdec->stat.sign = false;
        m_simplestation->m_mdec->stat.dep  = 0;
        m_simplestation->m_mdec->stat.oreq = false;
        m_simplestation->m_mdec->stat.ireq = true;
        m_simplestation->m_mdec->stat.busy = false;

        m_simplestation->m_mdec->state = Idle;
    }
}

void m_mdec_init(m_simplestation_state *m_simplestation)
{
    m_simplestation->m_mdec = malloc(sizeof(m_psx_mdec_t));
	
	m_simplestation->m_mdec->quantIdx = 0;

	m_simplestation->m_mdec->scaleIdx = 0;

	m_simplestation->m_mdec->state = Idle;

}

