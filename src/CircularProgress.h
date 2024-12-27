//
// Created by Sergio Prada on 27/12/24.
//

#ifndef CARDOBLAST_CIRCULARPROGRESS_H
#define CARDOBLAST_CIRCULARPROGRESS_H


class CircularProgress {
private:
    int centerX, centerY;
    int radius;
    float progress;  // 0.0 to 1.0

public:
    CircularProgress(int x, int y, int r);
    void UpdatePosition(int x, int y){centerX = x; centerY = y;}
    void Draw(float percentage);
};


#endif //CARDOBLAST_CIRCULARPROGRESS_H
