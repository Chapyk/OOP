#pragma once
#include "LineShape.h"
#include "EllipseShape.h"

// Нова фігура: Лінія з кружечками (через композицію, без множинного успадкування)
class LineWithCirclesShape : public Shape {
    LineShape line;
    EllipseShape startCircle;
    EllipseShape endCircle;
public:
    LineWithCirclesShape(int x1, int y1, int x2, int y2, COLORREF col);
    void Draw(HDC hdc, bool preview = false) override;
};
