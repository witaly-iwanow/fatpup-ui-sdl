#ifndef FATPUP_UI_COLORS_H
#define FATPUP_UI_COLORS_H

// RGBA
static constexpr uint8_t LIGHT_SQUARE[] = { 255, 219, 153, 255 };

static constexpr uint8_t DARK_SQUARE[] = { 145, 94, 55, 255 };

static constexpr uint8_t SELECTED_SQUARE[] = { 255, 81, 0, 255 };
static constexpr uint8_t LAST_MOVE_SQUARE[] = { SELECTED_SQUARE[0], SELECTED_SQUARE[1], SELECTED_SQUARE[2], 130 };

static constexpr uint8_t MOVE_PANEL[] =
{
    (LIGHT_SQUARE[0] + DARK_SQUARE[0]) / 2,
    (LIGHT_SQUARE[1] + DARK_SQUARE[1]) / 2,
    (LIGHT_SQUARE[2] + DARK_SQUARE[2]) / 2,
    255
};

#endif // #define FATPUP_UI_COLORS_H
