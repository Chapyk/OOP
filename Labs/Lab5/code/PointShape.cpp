#include "PointShape.h"

PointShape::PointShape(int x1, int y1, COLORREF col)
    : Shape(x1, y1, x1, y1, col) {}

void PointShape::Draw(HDC hdc, bool preview) {
    HPEN hPen = CreatePen(preview ? PS_DASH : PS_SOLID, 1, color);
    HBRUSH hBrush = CreateSolidBrush(color);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, preview ? GetStockObject(NULL_BRUSH) : hBrush);
    Ellipse(hdc, startX - 2, startY - 2, startX + 2, startY + 2);
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}
