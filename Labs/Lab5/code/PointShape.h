#pragma once
#include "Shape.h"

class PointShape : public Shape {
public:
    PointShape(int x1, int y1, COLORREF col);
    void Draw(HDC hdc, bool preview = false) override;
};
