#include "Shape.h"

Shape::Shape(int x1, int y1, int x2, int y2, COLORREF col)
    : startX(x1), startY(y1), endX(x2), endY(y2), color(col) {}

Shape::~Shape() {}
