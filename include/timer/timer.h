#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <simplestation.h>

typedef enum {
    PauseDuringHblank = 0,
    ResetToZeroAtHblank = 1,
    ResetToZeroAtHblankAndPauseOutsideHblank = 2,
    PauseUntilHblankThenSwitchToFreeRun = 3
} Timer0SyncMode;

typedef enum {
    PauseDuringVblank = 0,
    ResetToZeroAtVblank = 1,
    ResetToZeroAtHblankAndPauseOutsideVblank = 2,
    PauseUntilVblankThenSwitchToFreeRun = 3
} Timer1SyncMode;

typedef enum {
    StopCounterAtCurrentValue,
    FreeRun
} Timer2SyncMode;

typedef enum {
    T0SystemClock,
    DotClock
} Timer0ClockSource;

typedef enum {
    T1SystemClock,
    Hblank
} Timer1ClockSource;

typedef enum {
    T2SystemClock,
    SystemClockByEight
} Timer2ClockSource;

typedef enum {
    AfterOverflow = 0,
    AfterTarget = 1
} TimerResetCounter;

typedef enum {
    ShortPulse = 0,
    Toggle = 1
} TimerPulseOrToggleMode;

typedef enum {
    OneShot = 0,
    Repeatedly = 1
} TimerOnceOrRepeatMode;

const uint32_t VideoSystemClocksPerDot = 6;
const uint32_t VideoSystemClocksPerScanline = 3413;
