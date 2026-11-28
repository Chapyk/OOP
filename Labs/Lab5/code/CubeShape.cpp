#include "CubeShape.h"

CubeShape::CubeShape(int x1, int y1, int x2, int y2, COLORREF col)
    : Shape(x1, y1, x2, y2, col),
    // Передній квадрат
    front(x1, y1, x2, y2, RGB(0, 0, 255)),
    // Задній квадрат (зміщений вправо-вгору на 40 пікселів)
    back(x1 + 40, y1 - 40, x2 + 40, y2 - 40, RGB(0, 0, 255)),
    // Ребра, що з'єднують їх
    c1(x1, y1, x1 + 40, y1 - 40, RGB(0, 0, 255)),
    c2(x2, y1, x2 + 40, y1 - 40, RGB(0, 0, 255)),
    c3(x2, y2, x2 + 40, y2 - 40, RGB(0, 0, 255)),
    c4(x1, y2, x1 + 40, y2 - 40, RGB(0, 0, 255)) {}

void CubeShape::Draw(HDC hdc, bool preview) {
    front.Draw(hdc, preview);
    back.Draw(hdc, preview);
    c1.Draw(hdc, preview); c2.Draw(hdc, preview);
    c3.Draw(hdc, preview); c4.Draw(hdc, preview);
}
