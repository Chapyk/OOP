#pragma once
#include "Shape.h"

class RectShape : public Shape {
public:
    RectShape(int x1, int y1, int x2, int y2, COLORREF col);
    void Draw(HDC hdc, bool preview = false) override;
};
