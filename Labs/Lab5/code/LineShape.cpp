#include "LineShape.h"

LineShape::LineShape(int x1, int y1, int x2, int y2, COLORREF col)
    : Shape(x1, y1, x2, y2, col) {}

void LineShape::Draw(HDC hdc, bool preview) {
    HPEN hPen = CreatePen(preview ? PS_DASH : PS_SOLID, 1, preview ? RGB(0, 0, 0) : color);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, startX, startY, NULL);
    LineTo(hdc, endX, endY);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}
