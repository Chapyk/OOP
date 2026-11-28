#pragma once
#include "RectShape.h"
#include "LineShape.h"

class CubeShape : public Shape {
    RectShape front; // передній квадрат 
    RectShape back; // задній квадрат 
    LineShape c1, c2, c3, c4; // з'єднувальні лінії 
public:
    CubeShape(int x1, int y1, int x2, int y2, COLORREF col);
    void Draw(HDC hdc, bool preview = false) override;
};
