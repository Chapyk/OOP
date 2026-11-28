#include "RectShape.h"

RectShape::RectShape(int x1, int y1, int x2, int y2, COLORREF col)
    : Shape(x1, y1, x2, y2, col) {}

void RectShape::Draw(HDC hdc, bool preview) {
    HPEN hPen = CreatePen(preview ? PS_DASH : PS_SOLID, 1,
        preview ? RGB(0, 0, 0) : RGB(0, 0, 255));
    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    int left = min(startX, endX); 
    int top = min(startY, endY); 
    int right = max(startX, endX); 
    int bottom = max(startY, endY); 
    Rectangle(hdc, left, top, right, bottom);
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
}
