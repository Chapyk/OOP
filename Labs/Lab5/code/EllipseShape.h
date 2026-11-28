#pragma once
#include "Shape.h"

class EllipseShape : public Shape {
public:
    EllipseShape(int x1, int y1, int x2, int y2, COLORREF col);
    void Draw(HDC hdc, bool preview = false) override;
};
