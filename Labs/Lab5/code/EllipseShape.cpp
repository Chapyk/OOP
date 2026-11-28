#include "EllipseShape.h"

EllipseShape::EllipseShape(int x1, int y1, int x2, int y2, COLORREF col)
    : Shape(x1, y1, x2, y2, col) {}

void EllipseShape::Draw(HDC hdc, bool preview) {
    HPEN hPen = CreatePen(preview ? PS_DASH : PS_SOLID, 1, preview ? RGB(0, 0, 0) : RGB(0, 0, 255));
    HBRUSH hBrush = CreateSolidBrush(RGB(200, 220, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, preview ? GetStockObject(NULL_BRUSH) : hBrush);
    Ellipse(hdc, min(startX, endX), min(startY, endY), max(startX, endX), max(startY, endY));
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}
