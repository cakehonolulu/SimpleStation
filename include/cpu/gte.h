#pragma once

#include <simplestation.h>

/* GTE Opcodes */
#define MOVE    0x00
#define MFC2 0
#define CFC2 2
#define MTC2 4
#define CTC2 6

#include <cpu/cpu.h>


#define VX0 (m_simplestation->m_gte->cop2d.r[0].sw.l)
#define VY0 (m_simplestation->m_gte->cop2d.r[0].sw.h)
#define VZ0 (m_simplestation->m_gte->cop2d.r[1].sw.l)
#define VX1 (m_simplestation->m_gte->cop2d.r[2].w.l)
#define VY1 (m_simplestation->m_gte->cop2d.r[2].w.h)
#define VZ1 (m_simplestation->m_gte->cop2d.r[3].w.l)
#define VX2 (m_simplestation->m_gte->cop2d.r[4].w.l)
#define VY2 (m_simplestation->m_gte->cop2d.r[4].w.h)
#define VZ2 (m_simplestation->m_gte->cop2d.r[5].w.l)

#define RGBC m_simplestation->m_gte->cop2d.raw[6]
#define R (m_simplestation->m_gte->cop2d.r[6].b.l)
#define G (m_simplestation->m_gte->cop2d.r[6].b.h)
#define B (m_simplestation->m_gte->cop2d.r[6].b.h2)
#define CD2 (m_simplestation->m_gte->cop2d.r[6].b.h3)

#define OTZ (m_simplestation->m_gte->cop2d.r[7].w.l)

#define IR0 (m_simplestation->m_gte->cop2d.r[8].sw.l)
#define IR1 (m_simplestation->m_gte->cop2d.r[9].sw.l)
#define IR2 (m_simplestation->m_gte->cop2d.r[10].sw.l)
#define IR3 (m_simplestation->m_gte->cop2d.r[11].sw.l)

#define SXY0 (m_simplestation->m_gte->cop2d.r[12].d)
#define SX0 (m_simplestation->m_gte->cop2d.r[12].sw.l)
#define SY0 (m_simplestation->m_gte->cop2d.r[12].sw.h)
#define SXY1 (m_simplestation->m_gte->cop2d.r[13].d)
#define SX1 (m_simplestation->m_gte->cop2d.r[13].sw.l)
#define SY1 (m_simplestation->m_gte->cop2d.r[13].sw.h)
#define SXY2 (m_simplestation->m_gte->cop2d.r[14].d)
#define SX2 (m_simplestation->m_gte->cop2d.r[14].sw.l)
#define SY2 (m_simplestation->m_gte->cop2d.r[14].sw.h)
#define SXYP (m_simplestation->m_gte->cop2d.r[15].d)
#define SXP (m_simplestation->m_gte->cop2d.r[15].sw.l)
#define SYP (m_simplestation->m_gte->cop2d.r[15].sw.h)

#define SZ0 (m_simplestation->m_gte->cop2d.r[16].w.l)
#define SZ1 (m_simplestation->m_gte->cop2d.r[17].w.l)
#define SZ2 (m_simplestation->m_gte->cop2d.r[18].w.l)
#define SZ3 (m_simplestation->m_gte->cop2d.r[19].w.l)

#define RGB0 (m_simplestation->m_gte->cop2d.r[20].d)
#define R0 (m_simplestation->m_gte->cop2d.r[20].b.l)
#define G0 (m_simplestation->m_gte->cop2d.r[20].b.h)
#define B0 (m_simplestation->m_gte->cop2d.r[20].b.h2)
#define RGB1 (m_simplestation->m_gte->cop2d.r[21].d)
#define RGB2 (m_simplestation->m_gte->cop2d.r[22].d)

#define MAC0 (m_simplestation->m_gte->cop2d.r[24].sd)
#define MAC1 (m_simplestation->m_gte->cop2d.r[25].sd)
#define MAC2 (m_simplestation->m_gte->cop2d.r[26].sd)
#define MAC3 (m_simplestation->m_gte->cop2d.r[27].sd)


#define RT11 (m_simplestation->m_gte->cop2c.r[0].sw.l)
#define RT12 (m_simplestation->m_gte->cop2c.r[0].sw.h)
#define RT13 (m_simplestation->m_gte->cop2c.r[1].sw.l)
#define RT21 (m_simplestation->m_gte->cop2c.r[1].sw.h)
#define RT22 (m_simplestation->m_gte->cop2c.r[2].sw.l)
#define RT23 (m_simplestation->m_gte->cop2c.r[2].sw.h)
#define RT31 (m_simplestation->m_gte->cop2c.r[3].sw.l)
#define RT32 (m_simplestation->m_gte->cop2c.r[3].sw.h)
#define RT33 (m_simplestation->m_gte->cop2c.r[4].sw.l)

#define TRX (m_simplestation->m_gte->cop2c.r[5].sd)
#define TRY (m_simplestation->m_gte->cop2c.r[6].sd)
#define TRZ (m_simplestation->m_gte->cop2c.r[7].sd)

#define L11 (m_simplestation->m_gte->cop2c.r[8].sw.l)
#define L12 (m_simplestation->m_gte->cop2c.r[8].sw.h)
#define L13 (m_simplestation->m_gte->cop2c.r[9].sw.l)
#define L21 (m_simplestation->m_gte->cop2c.r[9].sw.h)
#define L22 (m_simplestation->m_gte->cop2c.r[10].sw.l)
#define L23 (m_simplestation->m_gte->cop2c.r[10].sw.h)
#define L31 (m_simplestation->m_gte->cop2c.r[11].sw.l)
#define L32 (m_simplestation->m_gte->cop2c.r[11].sw.h)
#define L33 (m_simplestation->m_gte->cop2c.r[12].sw.l)

#define RBK (m_simplestation->m_gte->cop2c.r[13].sd)
#define GBK (m_simplestation->m_gte->cop2c.r[14].sd)
#define BBK (m_simplestation->m_gte->cop2c.r[15].sd)

#define LR1 (m_simplestation->m_gte->cop2c.r[16].sw.l)
#define LR2 (m_simplestation->m_gte->cop2c.r[16].sw.h)
#define LR3 (m_simplestation->m_gte->cop2c.r[17].sw.l)
#define LG1 (m_simplestation->m_gte->cop2c.r[17].sw.h)
#define LG2 (m_simplestation->m_gte->cop2c.r[18].sw.l)
#define LG3 (m_simplestation->m_gte->cop2c.r[18].sw.h)
#define LB1 (m_simplestation->m_gte->cop2c.r[19].sw.l)
#define LB2 (m_simplestation->m_gte->cop2c.r[19].sw.h)
#define LB3 (m_simplestation->m_gte->cop2c.r[20].sw.l)

#define RFC (m_simplestation->m_gte->cop2c.r[21].sd)
#define GFC (m_simplestation->m_gte->cop2c.r[22].sd)
#define BFC (m_simplestation->m_gte->cop2c.r[23].sd)

#define OFX (m_simplestation->m_gte->cop2c.r[24].sd)
#define OFY (m_simplestation->m_gte->cop2c.r[25].sd)

#define H (m_simplestation->m_gte->cop2c.r[26].sw.l)

#define DQA (m_simplestation->m_gte->cop2c.r[27].sw.l)
#define DQB (m_simplestation->m_gte->cop2c.r[28].sd)

#define ZSF3 (m_simplestation->m_gte->cop2c.r[29].sw.l)
#define ZSF4 (m_simplestation->m_gte->cop2c.r[30].sw.l)


#define sf(instr) ((instr >> 19) & 1)
#define lm(instr) ((instr >> 10) & 1)
#define m(instr) ((instr >> 17) & 3)
#define v(instr) ((instr >> 15) & 3)
#define cv(instr) ((instr >> 13) & 3)

#define MIN(x,y) ((x<y)?x:y)

/* Function Definitions */
uint8_t gte_init(m_simplestation_state *m_simplestation);
uint8_t gte_exit(m_simplestation_state *m_simplestation);
void gte_execute(uint32_t opcode, m_simplestation_state *m_simplestation);
