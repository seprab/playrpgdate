//
// Created by Sergio Prada on 8/12/24.
//
#include <filesystem>
#include <vector>
#include <memory>
#include "pdcpp/core/GlobalPlaydateAPI.h"
#include "pdcpp/graphics/Image.h"
#include "pdcpp/graphics/Point.h"

#ifndef CARDOBLAST_ANIMATIONCLIP_H
#define CARDOBLAST_ANIMATIONCLIP_H


class AnimationClip{
public:
    AnimationClip()=default;
    ~AnimationClip()=default;

    void AddImagePath(const std::string& path);
    void LoadImages();
    void Draw(pdcpp::Point<int> location);
    void Draw(int x, int y);
    void SetDelay(int value){frameDelay = value; }
    void SetFlip(bool value){flip = value; }

private:
    bool flip{false};
    bool loaded{false};
    int currentframe{0};
    int frameCount{0};
    int frameDelay{0};
    int frameDelayCounter{0};

    std::vector<std::unique_ptr<pdcpp::Image>> images;
    std::vector<std::string> paths;
};


#endif //CARDOBLAST_ANIMATIONCLIP_H
