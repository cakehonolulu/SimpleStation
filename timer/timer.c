#include <timer/timer.h>

typedef union {
    struct {
        uint32_t value : 16;
        uint32_t garbage : 16;
    };

    uint32_t _value;

} TimerCounterValue;

typedef union {
    struct {
        uint32_t syncEnable : 1;
        uint32_t _syncMode : 2;
        uint32_t _resetCounter : 1;
        uint32_t IRQWhenTarget : 1;
        uint32_t IRQWhenOverflow : 1;
        uint32_t _IRQOnceOrRepeatMode : 1;
        uint32_t _IRQPulseOrToggle : 1;
        uint32_t _clockSource : 2;
        uint32_t IRQ : 1;
        uint32_t rearchedTarget : 1;
        uint32_t rearchedOverflow : 1;
        uint32_t unknown : 3;
        uint32_t garbage : 16;
    };

    uint32_t _value;

} TimerCounterMode;

typedef union {
    struct {
        uint32_t target : 16;
        uint32_t garbage : 16;
    };

    uint32_t _value;

} TimerCounterTarget;

bool oneShotTimerFired = false;
uint32_t counter = 0;

/* TIMER0 */
TimerCounterValue tmr0_counterValue = { 0 };

TimerCounterMode tmr0_counterMode = { 0 };

TimerCounterTarget tmr0_counterTarget = { 0 };

uint32_t tmr0_counterValueRegister(m_simplestation_state *m_simplestation) {
    return tmr0_counterValue._value;
}

uint32_t tmr0_counterModeRegister(m_simplestation_state *m_simplestation) {
    return tmr0_counterMode._value;
}

uint32_t tmr0_counterTargetRegister(m_simplestation_state *m_simplestation) {
    return tmr0_counterTarget._value;
}

void tmr0_setCounterValueRegister(uint32_t value, m_simplestation_state *m_simplestation) {
    tmr0_counterValue._value = value;
}

void tmr0_setCounterModeRegister(uint32_t value, m_simplestation_state *m_simplestation) {
    tmr0_counterMode._value = value;
    tmr0_counterValue._value = 0;
}

void tmr0_setCounterTargetRegister(uint32_t value, m_simplestation_state *m_simplestation) {
    tmr0_counterTarget._value = value;
}

/* TIMER1 */
TimerCounterValue tmr1_counterValue = { 0 };

TimerCounterMode tmr1_counterMode = { 0 };

TimerCounterTarget tmr1_counterTarget = { 0 };

uint32_t tmr1_counterValueRegister(m_simplestation_state *m_simplestation) {
    return tmr1_counterValue._value;
}

uint32_t tmr1_counterModeRegister(m_simplestation_state *m_simplestation) {
    return tmr1_counterMode._value;
}

uint32_t tmr1_counterTargetRegister(m_simplestation_state *m_simplestation) {
    return tmr1_counterTarget._value;
}

void tmr1_setCounterValueRegister(uint32_t value, m_simplestation_state *m_simplestation) {
    tmr1_counterValue._value = value;
}

void tmr1_setCounterModeRegister(uint32_t value, m_simplestation_state *m_simplestation) {
    tmr1_counterMode._value = value;
    tmr1_counterValue._value = 0;
}

void tmr1_setCounterTargetRegister(uint32_t value, m_simplestation_state *m_simplestation) {
    tmr1_counterTarget._value = value;
}

/* TIMER2 */
TimerCounterValue tmr2_counterValue = { 0 };

TimerCounterMode tmr2_counterMode = { 0 };

TimerCounterTarget tmr2_counterTarget = { 0 };

uint32_t tmr2_counterValueRegister(m_simplestation_state *m_simplestation) {
    return tmr2_counterValue._value;
}

uint32_t tmr2_counterModeRegister(m_simplestation_state *m_simplestation) {
    return tmr2_counterMode._value;
}

uint32_t tmr2_counterTargetRegister(m_simplestation_state *m_simplestation) {
    return tmr2_counterTarget._value;
}

void tmr2_setCounterValueRegister(uint32_t value, m_simplestation_state *m_simplestation) {
    tmr2_counterValue._value = value;
}

void tmr2_setCounterModeRegister(uint32_t value, m_simplestation_state *m_simplestation) {
    tmr1_counterMode._value = value;
    tmr1_counterValue._value = 0;
}

void tmr2_setCounterTargetRegister(uint32_t value, m_simplestation_state *m_simplestation) {
    tmr2_counterTarget._value = value;
}

void timers_init() {

}

uint16_t timers_read(uint32_t addr, m_simplestation_state *m_simplestation) {
    uint16_t data;

    switch ((addr & 0x000000F0) >> 4) {

        case 0:
            switch (addr & 0xF)
            {
                case 0:
                    data = tmr0_counterValueRegister(m_simplestation);
                    break;

                case 4:
                    data = tmr0_counterModeRegister(m_simplestation);
                    break;

                case 8:
                    data = tmr0_counterTargetRegister(m_simplestation);
                    break;

                default:
                    printf(BOLD RED "[TIMER0] Unhandled read at 0x%X" NORMAL "\n", addr);
                    m_simplestation_exit(m_simplestation, 1);
                    break;
            }
            break;
        
        case 1:
            switch (addr & 0xF)
            {
                case 0:
                    data = tmr1_counterValueRegister(m_simplestation);
                    break;

                case 4:
                    data = tmr1_counterModeRegister(m_simplestation);
                    break;

                case 8:
                    data = tmr1_counterTargetRegister(m_simplestation);
                    break;

                default:
                    printf(BOLD RED "[TIMER1] Unhandled read at 0x%X" NORMAL "\n", addr);
                    m_simplestation_exit(m_simplestation, 1);
                    break;
            }
            break;

        case 2:
            switch (addr & 0xF)
            {
                case 0:
                    data = tmr2_counterValueRegister(m_simplestation);
                    break;

                case 4:
                    data = tmr2_counterModeRegister(m_simplestation);
                    break;

                case 8:
                    data = tmr2_counterTargetRegister(m_simplestation);
                    break;

                default:
                    printf(BOLD RED "[TIMER2] Unhandled read at 0x%X" NORMAL "\n", addr);
                    m_simplestation_exit(m_simplestation, 1);
                    break;
            }
            break;
        
        default:
            printf(BOLD RED "[TIMER ] Unhandled read where timer source is invalid: 0x%X" NORMAL "\n", addr);
            m_simplestation_exit(m_simplestation, 1);
            break;
    }

    return data;
}

void timers_write(uint32_t addr, uint16_t data, m_simplestation_state *m_simplestation) {
    switch ((addr & 0x000000F0) >> 4) {

        case 0:
            switch (addr & 0x0000000F)
            {
                case 0:
                    tmr0_setCounterValueRegister(data, m_simplestation);
                    break;

                case 4:
                    tmr0_setCounterModeRegister(data, m_simplestation);
                    break;

                case 8:
                    tmr0_setCounterTargetRegister(data, m_simplestation);
                    break;

                default:
                    printf(BOLD RED "[TIMER0] Unhandled write at 0x%X" NORMAL "\n", addr);
                    m_simplestation_exit(m_simplestation, 1);
                    break;
            }
            break;

        case 1:
            switch (addr & 0x0000000F)
            {
                case 0:
                    tmr1_setCounterValueRegister(data, m_simplestation);
                    break;

                case 4:
                    tmr1_setCounterModeRegister(data, m_simplestation);
                    break;

                case 8:
                    tmr1_setCounterTargetRegister(data, m_simplestation);
                    break;

                default:
                    printf(BOLD RED "[TIMER1] Unhandled write at 0x%X" NORMAL "\n", addr);
                    m_simplestation_exit(m_simplestation, 1);
                    break;
            }
            break;

        case 2:
            switch (addr & 0x0000000F)
            {
                case 0:
                    tmr2_setCounterValueRegister(data, m_simplestation);
                    break;

                case 4:
                    tmr2_setCounterModeRegister(data, m_simplestation);
                    break;

                case 8:
                    tmr2_setCounterTargetRegister(data, m_simplestation);
                    break;

                default:
                    printf(BOLD RED "[TIMER2] Unhandled write at 0x%X" NORMAL "\n", addr);
                    m_simplestation_exit(m_simplestation, 1);
                    break;
            }
            break;

        default:
            printf(BOLD RED "[TIMER ] Unhandled write where timer source is invalid: 0x%X, tmr: %X" NORMAL "\n", addr, (addr & 0x000000F0) >> 4);
            m_simplestation_exit(m_simplestation, 1);
            break;
    }
}

void tmr0_checkInterruptRequest(m_simplestation_state *m_simplestation) {
    if (((TimerPulseOrToggleMode) tmr0_counterMode._IRQPulseOrToggle) == Toggle) {
        tmr0_counterMode.IRQ = !tmr0_counterMode.IRQ;
    } else {
        tmr0_counterMode.IRQ = false;
    }

    if (((TimerOnceOrRepeatMode) tmr0_counterMode._IRQOnceOrRepeatMode) == OneShot && oneShotTimerFired) {
        return;
    }

    if (!tmr0_counterMode.IRQ) {
        m_interrupts_request(TMR0, m_simplestation);
        oneShotTimerFired = true;
    }
    tmr0_counterMode.IRQ = true;
}

void tmr1_checkInterruptRequest(m_simplestation_state *m_simplestation) {
    if (((TimerPulseOrToggleMode) tmr1_counterMode._IRQPulseOrToggle) == Toggle) {
        tmr1_counterMode.IRQ = !tmr0_counterMode.IRQ;
    } else {
        tmr1_counterMode.IRQ = false;
    }

    if (((TimerOnceOrRepeatMode) tmr1_counterMode._IRQOnceOrRepeatMode) == OneShot && oneShotTimerFired) {
        return;
    }

    if (!tmr1_counterMode.IRQ) {
        m_interrupts_request(TMR1, m_simplestation);
        oneShotTimerFired = true;
    }
    tmr1_counterMode.IRQ = true;
}

void tmr2_checkInterruptRequest(m_simplestation_state *m_simplestation) {
    if (((TimerPulseOrToggleMode) tmr2_counterMode._IRQPulseOrToggle) == Toggle) {
        tmr2_counterMode.IRQ = !tmr2_counterMode.IRQ;
    } else {
        tmr2_counterMode.IRQ = false;
    }

    if (((TimerOnceOrRepeatMode) tmr2_counterMode._IRQOnceOrRepeatMode) == OneShot && oneShotTimerFired) {
        return;
    }

    if (!tmr2_counterMode.IRQ) {
        m_interrupts_request(TMR2, m_simplestation);
        oneShotTimerFired = true;
    }
    tmr2_counterMode.IRQ = true;
}

void tmr0_checkTargetsAndOverflows(m_simplestation_state *m_simplestation) {
    bool checkIRQ = false;

    if (tmr0_counterValue.value >= tmr0_counterTarget.target) {
        tmr0_counterMode.rearchedTarget = true;
        TimerResetCounter timerResetCounter = (TimerResetCounter) tmr0_counterMode._resetCounter;
        if (timerResetCounter == AfterTarget) {
            tmr0_counterValue._value = 0;
        }
        if (tmr0_counterMode.IRQWhenTarget) {
            checkIRQ = true;
        }
    }

    if (tmr0_counterValue.value >= 0xffff) {
        tmr0_counterMode.rearchedOverflow = true;
        TimerResetCounter timerResetCounter = (TimerResetCounter) tmr0_counterMode._resetCounter;
        if (timerResetCounter == AfterOverflow) {
            tmr0_counterValue._value = 0;
        }
        if (tmr0_counterMode.IRQWhenOverflow) {
            checkIRQ = true;
        }
    }

    if (!checkIRQ) {
        return;
    }

    tmr0_checkInterruptRequest(m_simplestation);
}

void tmr1_checkTargetsAndOverflows(m_simplestation_state *m_simplestation) {
    bool checkIRQ = false;

    if (tmr1_counterValue.value >= tmr1_counterTarget.target) {
        tmr1_counterMode.rearchedTarget = true;
        TimerResetCounter timerResetCounter = (TimerResetCounter) tmr1_counterMode._resetCounter;
        if (timerResetCounter == AfterTarget) {
            tmr1_counterValue._value = 0;
        }
        if (tmr1_counterMode.IRQWhenTarget) {
            checkIRQ = true;
        }
    }

    if (tmr1_counterValue.value >= 0xffff) {
        tmr1_counterMode.rearchedOverflow = true;
        TimerResetCounter timerResetCounter = (TimerResetCounter) tmr1_counterMode._resetCounter;
        if (timerResetCounter == AfterOverflow) {
            tmr1_counterValue._value = 0;
        }
        if (tmr1_counterMode.IRQWhenOverflow) {
            checkIRQ = true;
        }
    }

    if (!checkIRQ) {
        return;
    }

    tmr1_checkInterruptRequest(m_simplestation);
}

void tmr2_checkTargetsAndOverflows(m_simplestation_state *m_simplestation) {
    bool checkIRQ = false;

    if (tmr2_counterValue.value >= tmr2_counterTarget.target) {
        tmr2_counterMode.rearchedTarget = true;
        TimerResetCounter timerResetCounter = (TimerResetCounter) tmr2_counterMode._resetCounter;
        if (timerResetCounter == AfterTarget) {
            tmr2_counterValue._value = 0;
        }
        if (tmr2_counterMode.IRQWhenTarget) {
            checkIRQ = true;
        }
    }

    if (tmr2_counterValue.value >= 0xffff) {
        tmr2_counterMode.rearchedOverflow = true;
        TimerResetCounter timerResetCounter = (TimerResetCounter) tmr2_counterMode._resetCounter;
        if (timerResetCounter == AfterOverflow) {
            tmr2_counterValue._value = 0;
        }
        if (tmr2_counterMode.IRQWhenOverflow) {
            checkIRQ = true;
        }
    }

    if (!checkIRQ) {
        return;
    }

    tmr2_checkInterruptRequest(m_simplestation);
}

Timer0ClockSource timer0ClockSource(m_simplestation_state *m_simplestation) {
    if (tmr0_counterMode._clockSource == 0 || tmr0_counterMode._clockSource == 3) {
        return T0SystemClock;
    } else {
        return DotClock;
    }
}

Timer1ClockSource timer1ClockSource(m_simplestation_state *m_simplestation) {
    if (tmr1_counterMode._clockSource == 0 || tmr1_counterMode._clockSource == 3) {
        return T1SystemClock;
    } else {
        return Hblank;
    }
}

Timer2ClockSource timer2ClockSource(m_simplestation_state *m_simplestation) {
    if (tmr2_counterMode._clockSource == 0 || tmr2_counterMode._clockSource == 3) {
        return T2SystemClock;
    } else {
        return SystemClockByEight;
    }
}

void tmr0_step(uint32_t cycles, m_simplestation_state *m_simplestation) {
    Timer0ClockSource clockSource = timer0ClockSource(m_simplestation);
    if (clockSource == DotClock) {
        uint32_t videoCycles = cycles*11/7;
        counter += videoCycles;
        tmr0_counterValue.value += counter / VideoSystemClocksPerDot;
        counter %= VideoSystemClocksPerDot;
    } else {
        counter += cycles;
        tmr0_counterValue.value += cycles;
    }

    tmr0_checkTargetsAndOverflows(m_simplestation);
}

void tmr1_step(uint32_t cycles, m_simplestation_state *m_simplestation) {
    Timer1ClockSource clockSource = timer1ClockSource(m_simplestation);
    if (clockSource == Hblank) {
        uint32_t videoCycles = cycles*11/7;
        counter += videoCycles;
        tmr1_counterValue.value += counter / VideoSystemClocksPerScanline;
        counter %= VideoSystemClocksPerScanline;
    } else {
        counter += cycles;
        tmr1_counterValue.value += cycles;
    }

    tmr1_checkTargetsAndOverflows(m_simplestation);
}

void tmr2_step(uint32_t cycles, m_simplestation_state *m_simplestation) {
    Timer2ClockSource clockSource = timer2ClockSource(m_simplestation);
    if (clockSource == SystemClockByEight) {
        counter += cycles;
        tmr2_counterValue.value += counter / 8;
        counter %= 8;
    } else {
        counter += cycles;
        tmr2_counterValue.value += cycles;
    }

    tmr2_checkTargetsAndOverflows(m_simplestation);
}