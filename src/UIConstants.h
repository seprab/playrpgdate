//
// Created by Sergio Prada on 10/12/25.
//

#ifndef CARDOBLAST_UICONSTANTS_H
#define CARDOBLAST_UICONSTANTS_H

#include "pd_api.h"
#include "pdcpp/graphics/Rectangle.h"
#include "pdcpp/graphics/Point.h"

namespace UIConstants {
    // Screen dimensions
    constexpr int SCREEN_WIDTH = 400;
    constexpr int SCREEN_HEIGHT = 240;
    constexpr int SCREEN_CENTER_X = SCREEN_WIDTH / 2;
    constexpr int SCREEN_CENTER_Y = SCREEN_HEIGHT / 2;

    // Loading Screen
    namespace Loading {
        constexpr int BANNER_Y = 170;
        constexpr int BANNER_HEIGHT = 100;
        constexpr int PROGRESS_BAR_X = 50;
        constexpr int PROGRESS_BAR_Y = 215;
        constexpr int PROGRESS_BAR_WIDTH = 300;
        constexpr int PROGRESS_BAR_HEIGHT = 10;
        constexpr int PROGRESS_BAR_PADDING = 1;
        constexpr int TEXT_Y = 185;
        constexpr int TEXT_X_CENTER = 200;
        constexpr int TEXT_X_LEFT = 100;
        constexpr int ANIMATION_FRAME_DELAY = 10;
    }

    // Main Menu
    namespace MainMenu {
        constexpr int PANEL_X = 80;
        constexpr int PANEL_Y = 30;
        constexpr int PANEL_WIDTH = 240;
        constexpr int PANEL_HEIGHT = 160;
        constexpr int TITLE_Y = 50;
        constexpr int TITLE_X = 150;
        constexpr int MENU_START_Y = 100;
        constexpr int MENU_ITEM_SPACING = 30;
        constexpr int MENU_ITEM_HEIGHT = 20;
        constexpr int MENU_ITEM_X = 90;
        constexpr int MENU_ITEM_WIDTH = 140;
        constexpr int MENU_TEXT_X = 100;
        constexpr int MENU_TEXT_Y_OFFSET = 2;
    }

    // Game Screen (HUD)
    namespace GameHUD {
        constexpr int OVERLAY_OFFSET_X = -200;
        constexpr int OVERLAY_OFFSET_Y = -120;
        constexpr int COORDS_X_OFFSET = 125;
        constexpr int COORDS_Y_OFFSET_X = 170;
        constexpr int COORDS_Y_OFFSET_Y = -115;
        constexpr int MAGIC_COOLDOWN_Y_OFFSET = -106;
        constexpr int MAGIC_ICON_CENTER_X_OFFSET = -17;
        constexpr int MAGIC_ICON_CENTER_Y_OFFSET = 85;
        constexpr int MAGIC_ICON_LEFT_X_OFFSET = -57;
        constexpr int MAGIC_ICON_LEFT_Y_OFFSET = 87;
        constexpr int MAGIC_ICON_RIGHT_X_OFFSET = 23;
        constexpr int MAGIC_ICON_RIGHT_Y_OFFSET = 87;
        constexpr int PLAYER_FACE_X_OFFSET = -205;
        constexpr int PLAYER_FACE_Y_OFFSET = -130;
        constexpr int COOLDOWN_RADIUS = 13;
    }

    // Game Over Screen
    namespace GameOver {
        constexpr int PANELA_OFFSET_X = -200;
        constexpr int PANELA_OFFSET_Y = -120;
        constexpr int PANELA_WIDTH = PANELA_OFFSET_X * -2;
        constexpr int PANELA_HEIGHT = PANELA_OFFSET_Y * -2;

        constexpr int PANELB_OFFSET_X = -200;
        constexpr int PANELB_OFFSET_Y = -50;
        constexpr int PANELB_WIDTH = PANELB_OFFSET_X * -2;
        constexpr int PANELB_HEIGHT = PANELB_OFFSET_Y * -2;

        constexpr int LINE_SPACING = 16;  // Using PLAYER_SIZE from Globals
    }

    // UI Theme
    namespace Theme {
        constexpr LCDColor BACKGROUND_COLOR = kColorBlack;
        constexpr LCDColor TEXT_COLOR = kColorWhite;
        constexpr LCDColor PANEL_COLOR = kColorBlack;
        constexpr LCDColor SELECTED_COLOR = kColorWhite;
        constexpr LCDColor SELECTED_TEXT_COLOR = kColorBlack;
        constexpr LCDColor PROGRESS_BAR_COLOR = kColorWhite;
        constexpr LCDColor PROGRESS_BAR_BACKGROUND = kColorBlack;
    }

    // General spacing and padding
    namespace Spacing {
        constexpr int SMALL = 2;
        constexpr int MEDIUM = 5;
        constexpr int LARGE = 10;
    }

    // Input settings
    namespace Input {
        constexpr float COOLDOWN = 0.5f;
    }
}

// Helper struct for rectangle operations
struct UIRect {
    int x, y, width, height;

    UIRect(int x_, int y_, int w_, int h_) : x(x_), y(y_), width(w_), height(h_) {}

    int centerX() const { return x + width / 2; }
    int centerY() const { return y + height / 2; }
    int right() const { return x + width; }
    int bottom() const { return y + height; }

    // Create a rect centered at a point
    static UIRect centered(int centerX, int centerY, int width, int height) {
        return UIRect(centerX - width / 2, centerY - height / 2, width, height);
    }
};

#endif //CARDOBLAST_UICONSTANTS_H
