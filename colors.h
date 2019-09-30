#ifndef FATPUP_UI_COLORS_H
#define FATPUP_UI_COLORS_H

static constexpr uint8_t LIGHT_SQUARE_R = 255;
static constexpr uint8_t LIGHT_SQUARE_G = 219;
static constexpr uint8_t LIGHT_SQUARE_B = 153;

static constexpr uint8_t DARK_SQUARE_R = 145;
static constexpr uint8_t DARK_SQUARE_G = 94;
static constexpr uint8_t DARK_SQUARE_B = 55;

static constexpr uint8_t SELECTED_SQUARE_R = 255;
static constexpr uint8_t SELECTED_SQUARE_G = 81;
static constexpr uint8_t SELECTED_SQUARE_B = 0;

static constexpr uint8_t MOVE_PANEL_R = (LIGHT_SQUARE_R + DARK_SQUARE_R) / 2;
static constexpr uint8_t MOVE_PANEL_G = (LIGHT_SQUARE_G + DARK_SQUARE_G) / 2;
static constexpr uint8_t MOVE_PANEL_B = (LIGHT_SQUARE_B + DARK_SQUARE_B) / 2;

#endif // #define FATPUP_UI_COLORS_H
