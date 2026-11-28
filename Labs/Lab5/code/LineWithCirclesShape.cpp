#include "LineWithCirclesShape.h"

LineWithCirclesShape::LineWithCirclesShape(int x1, int y1, int x2, int y2, COLORREF col)
    : Shape(x1, y1, x2, y2, col),
    line(x1, y1, x2, y2, col),
    startCircle(x1 - 4, y1 - 4, x1 + 4, y1 + 4, col),
    endCircle(x2 - 4, y2 - 4, x2 + 4, y2 + 4, col) {}

void LineWithCirclesShape::Draw(HDC hdc, bool preview) {
    line.Draw(hdc, preview);
    startCircle.Draw(hdc, preview);
    endCircle.Draw(hdc, preview);
}
