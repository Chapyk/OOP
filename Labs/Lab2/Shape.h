#pragma once
#include <windows.h>

 class Shape { 
protected:
    int startX, startY;
    int endX, endY;
    COLORREF color;
public:
    Shape(int x1, int y1, int x2, int y2, COLORREF col)
        : startX(x1), startY(y1), endX(x2), endY(y2), color(col) {
    }
    virtual ~Shape() {}
     virtual void Draw(HDC hdc) = 0; // Чисто віртуальна функція (поліморфізм)
};

// point
 class PointShape : public Shape {
public:
    PointShape(int x1, int y1, COLORREF col) : Shape(x1, y1, x1, y1, col) {}
    void Draw(HDC hdc) override {
        HPEN hPen = CreatePen(PS_SOLID, 1, color);
        HBRUSH hBrush = CreateSolidBrush(color);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

        Ellipse(hdc, startX - 2, startY - 2, startX + 2, startY + 2);

        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);
        DeleteObject(hBrush);
    }
};

// line
class LineShape : public Shape { 
public:
    LineShape(int x1, int y1, int x2, int y2, COLORREF col) : Shape(x1, y1, x2, y2, col) {}
    void Draw(HDC hdc) override {
        HPEN hPen = CreatePen(PS_SOLID, 1, color);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        MoveToEx(hdc, startX, startY, NULL); 
        LineTo(hdc, endX, endY); 
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }
};

// rectanlge
class RectShape : public Shape { 
public:
    RectShape(int x1, int y1, int x2, int y2, COLORREF col) : Shape(x1, y1, x2, y2, col) {}
    void Draw(HDC hdc) override {
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); // Чорний контур
        HBRUSH hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH); // Біле заповнення
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

        Rectangle(hdc, startX, startY, endX, endY); 

        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);
    }
};

// 4. ellips
class EllipseShape : public Shape { 
public:
    EllipseShape(int x1, int y1, int x2, int y2, COLORREF col) : Shape(x1, y1, x2, y2, col) {}
    void Draw(HDC hdc) override {
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); // Чорний контур
        HBRUSH hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH); // Білий фон (без заповнення)
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

        Ellipse(hdc, startX, startY, endX, endY); 

        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);
    }
};

