//
// Created by Sergio Prada on 8/12/24.
//
#include <filesystem>
#include "pdcpp/core/GlobalPlaydateAPI.h"

#ifndef CARDOBLAST_ANIMATIONCLIP_H
#define CARDOBLAST_ANIMATIONCLIP_H


class AnimationClip{
public:
    AnimationClip()=default;
    ~AnimationClip()=default;

    void AddImagePath(const char* path){paths.emplace_back(path);}
    void LoadBitmaps();
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
    std::pair<int, int> previousPosition;

    std::vector<LCDBitmap*> bitmaps;
    std::vector<std::filesystem::path> paths;
};


#endif //CARDOBLAST_ANIMATIONCLIP_H
