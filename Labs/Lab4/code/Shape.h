
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
    virtual void Draw(HDC hdc) = 0;
};

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

class RectShape : public Shape {
public:
    RectShape(int x1, int y1, int x2, int y2, COLORREF col) : Shape(x1, y1, x2, y2, col) {}
    void Draw(HDC hdc) override {
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
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
};

class EllipseShape : public Shape {
public:
    EllipseShape(int x1, int y1, int x2, int y2, COLORREF col) : Shape(x1, y1, x2, y2, col) {}
    void Draw(HDC hdc) override {
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
        HBRUSH hBrush = CreateSolidBrush(RGB(200, 220, 255));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        Ellipse(hdc, startX, startY, endX, endY);
        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);
        DeleteObject(hBrush);
    }
};

// Нова фігура: Лінія з кружечками (через композицію, без множинного успадкування)
class LineWithCirclesShape : public Shape {
    LineShape line;
    EllipseShape startCircle;
    EllipseShape endCircle;
public:
    LineWithCirclesShape(int x1, int y1, int x2, int y2, COLORREF col)
        : Shape(x1, y1, x2, y2, col),
        line(x1, y1, x2, y2, col),
        startCircle(x1 - 4, y1 - 4, x1 + 4, y1 + 4, col),
        endCircle(x2 - 4, y2 - 4, x2 + 4, y2 + 4, col) {
    }

    void Draw(HDC hdc) override {
        line.Draw(hdc);
        startCircle.Draw(hdc);
        endCircle.Draw(hdc);
    }
};

class CubeShape : public Shape {
    RectShape front; // передній квадрат
    RectShape back; // задній квадрат
    LineShape c1, c2, c3, c4; // поєднувальні лінії
public:
    CubeShape(int x1, int y1, int x2, int y2, COLORREF col)
        : Shape(x1, y1, x2, y2, col),
        // передій квадрат
        front(x1, y1, x2, y2, RGB(0, 0, 255)),
        // задній квадрат (зміщений вправо-вгору на 40 пікселів)
        back(x1 + 40, y1 - 40, x2 + 40, y2 - 40, RGB(0, 0, 255)),
        // ребра що поєднують їх
        c1(x1, y1, x1 + 40, y1 - 40, RGB(0, 0, 255)),
        c2(x2, y1, x2 + 40, y1 - 40, RGB(0, 0, 255)),
        c3(x2, y2, x2 + 40, y2 - 40, RGB(0, 0, 255)),
        c4(x1, y2, x1 + 40, y2 - 40, RGB(0, 0, 255)) {
    }

    void Draw(HDC hdc) override { 
        front.Draw(hdc);
        back.Draw(hdc);
        c1.Draw(hdc); c2.Draw(hdc); c3.Draw(hdc); c4.Draw(hdc);
    }
};
    