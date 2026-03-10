//
// Created by Sergio Prada on 8/12/24.
//

#include "AnimationClip.h"
#include "Log.h"

void AnimationClip::AddImagePath(const std::string& path)
{
    paths.push_back(path);
}

void AnimationClip::LoadImages()
{
    if (loaded)
    {
        Log::Info("Attempting to load an already loaded image");
        return;
    }

    for (const auto& path : paths)
    {
        try
        {
            auto image = std::make_unique<pdcpp::Image>(path);
            images.push_back(std::move(image));
        }
        catch (const std::exception& e)
        {
            Log::Error("Failed to load image: %s", path.c_str());
            return;
        }
    }

    frameCount = static_cast<int>(images.size());
    loaded = true;
}

void AnimationClip::Draw(int x, int y)
{
    if (!loaded || images.empty())
    {
        Log::Error("Attempting to draw an unloaded or empty animation");
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

    pdcpp::Point<int> position(x, y);
    LCDBitmapFlip flipMode = flip ? kBitmapFlippedX : kBitmapUnflipped;
    images[currentframe]->draw(position, flipMode);
}
