#include "CircularProgress.h"
#include "pdcpp/core/GlobalPlaydateAPI.h"

// In your cpp file
CircularProgress::CircularProgress(int x, int y, int r) {
    centerX = x;
    centerY = y;
    radius = r;
    progress = 0.0f;
}

void CircularProgress::Draw(float percentage) {
    // Clamp progress between 0 and 1
    progress = std::min(1.0f, std::max(0.0f, percentage));

    // Calculate end angle (Playdate uses 0-360 degrees)
    float endAngle = 360.0f * progress;

    // Draw background circle
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawEllipse(
            centerX - radius,
            centerY - radius,
            radius * 2,
            radius * 2,
            1, // line width
            0, // start angle
            360, // end angle
            kColorBlack
    );

    // Draw progress arc
    pdcpp::GlobalPlaydateAPI::get()->graphics->fillEllipse(
            centerX - radius,
            centerY - radius,
            radius * 2,
            radius * 2,
            0, // start angle
            endAngle,
            kColorWhite
    );
}
