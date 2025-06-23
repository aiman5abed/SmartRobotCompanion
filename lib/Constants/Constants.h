#ifndef CONSTANTS_H
#define CONSTANTS_H

// ── Pins ────────────────────────────────────────────────────────────────
#define TFT_CS        15
#define TFT_DC         5
#define TFT_RST        4
#define JOY_VRX       34
#define JOY_VRY       35
#define JOY_SW        32
#define BUZZER_PIN    27

// ── Grid / Battery / Timing ─────────────────────────────────────────────
#define GRID_SIZE      20
#define MAX_DIRT        7

#define MOVE_DELAY    150    // ms per forward
#define ROTATE_DELAY  300    // ms per 90° turn
#define CLEAN_DELAY   300    // ms per clean

#define BAT_DRAIN_MOVE          0.5f
#define BAT_DRAIN_ROTATE        0.25f
#define BAT_DRAIN_CLEAN_THRESH  5
#define BAT_LOW_THRESHOLD      25.0f

#define BAT_DRAIN_BG_INTERVAL 10000u
#define BAT_DRAIN_BG_AMOUNT   0.1f

#define DIRT_ACCUM_INTERVAL 40000u

#define CELL_SIZE      12
#define HEADER_HEIGHT  50

#endif // CONSTANTS_H
