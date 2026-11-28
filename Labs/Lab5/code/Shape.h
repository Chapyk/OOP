#pragma once
#include <windows.h>

class Shape {
protected:
    int startX, startY;
    int endX, endY;
    COLORREF color;
public:
    Shape(int x1, int y1, int x2, int y2, COLORREF col);
    virtual ~Shape();
    virtual void Draw(HDC hdc, bool preview = false) = 0;
    int X1() const { return startX; }
    int Y1() const { return startY; }
    int X2() const { return endX; }
    int Y2() const { return endY; }
};
