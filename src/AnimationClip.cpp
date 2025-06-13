//
// Created by Sergio Prada on 8/12/24.
//

#include "AnimationClip.h"
#include "Log.h"

void AnimationClip::LoadBitmaps()
{
    if (loaded)
    {
        Log::Info("Attempting to load an already loaded bitmap");
        return;
    }
    for (auto& path : paths)
    {
        const char** error{};
        LCDBitmap* bitmap = pdcpp::GlobalPlaydateAPI::get()->graphics->loadBitmap(path.c_str(), error);
        if (bitmap == nullptr && *error != nullptr)
        {
            Log::Error("Failed to load bitmap: %s", *error);
            return;
        }
        bitmaps.emplace_back(bitmap);
    }
    frameCount = static_cast<int>(bitmaps.size());
    loaded = true;
}
void AnimationClip::Draw(int x, int y)
{
    if (!loaded)
    {
        Log::Error("Attempting to draw an unloaded bitmap");
        return;
    }

    if (frameDelayCounter >= frameDelay)
    {
        currentframe++;
        if (currentframe >= frameCount)
        {
            currentframe = 0;
        }
        frameDelayCounter = 0;
    }
    else
    {
        frameDelayCounter++;
    }
    pdcpp::GlobalPlaydateAPI::get()->graphics->drawBitmap(bitmaps[currentframe], x, y, flip ? kBitmapFlippedX : kBitmapUnflipped);
    previousPosition = std::pair<int, int>(x, y);
}
