#include "my_editor.h"
#include <windowsx.h>

MyEditor::MyEditor() {
    pcshape = new Shape * [MAX_SHAPES];
    for (int i = 0; i < MAX_SHAPES; i++) {
        pcshape[i] = nullptr;
    }
    shapeCount = 0;
    selectedIndex = -1;
    currentShapeType = drawingType = 0;
    isDrawing = false;
    startX = startY = currentX = currentY = 0;
}

MyEditor::~MyEditor() {
    Clear();
    delete[] pcshape;
}

#if LAB_VARIANT % 2 == 0
MyEditor* MyEditor::instance = nullptr;
MyEditor& MyEditor::GetInstance() {
    if (!instance) instance = new MyEditor();
    return *instance;
}
void MyEditor::ReleaseInstance() {
    delete instance;
    instance = nullptr;
}
#else
MyEditor& MyEditor::GetInstance() { 
    static MyEditor instance; 
    return instance; 
}
void MyEditor::ReleaseInstance() {}
#endif

void MyEditor::RegisterShape(int type, const char* name, Creator creator) {
    factory[type] = creator;
    names[type] = name;
}

void MyEditor::SetShapeType(int type) {
    if (factory.count(type)) currentShapeType = type;
}

int MyEditor::GetShapeType() const { return currentShapeType; }
const char* MyEditor::GetShapeName() const { return names.at(drawingType).c_str(); }
const char* MyEditor::GetShapeName(int index) const { return names.at(shapeTypes[index]).c_str(); }
int MyEditor::GetShapeCount() const { return shapeCount; }
Shape* MyEditor::GetShape(int index) const { return pcshape[index]; }

Shape* MyEditor::AddShape(int type, int x1, int y1, int x2, int y2) {
    if (shapeCount >= MAX_SHAPES || !factory.count(type)) return nullptr;
    Shape* shape = factory.at(type)(x1, y1, x2, y2); 
    shapeTypes[shapeCount] = type; 
    pcshape[shapeCount++] = shape; 
    return shape;
}

void MyEditor::Select(int index) {
    selectedIndex = index >= 0 && index < shapeCount ? index : -1;
}

void MyEditor::Remove(int index) {
    if (index < 0 || index >= shapeCount) return;
    delete pcshape[index];
    for (int i = index; i < shapeCount - 1; i++) {
        pcshape[i] = pcshape[i + 1];
        shapeTypes[i] = shapeTypes[i + 1];
    }
    pcshape[--shapeCount] = nullptr;
    selectedIndex = -1;
}

void MyEditor::OnLButtonDown(HWND hwnd, LPARAM lParam) {
    if (!factory.count(currentShapeType) || shapeCount >= MAX_SHAPES) return;
    isDrawing = true;
    drawingType = currentShapeType;
    startX = currentX = GET_X_LPARAM(lParam);
    startY = currentY = GET_Y_LPARAM(lParam);
    SetCapture(hwnd);
}

void MyEditor::OnMouseMove(HWND hwnd, LPARAM lParam) {
    if (!isDrawing) return;
    currentX = GET_X_LPARAM(lParam);
    currentY = GET_Y_LPARAM(lParam);
    InvalidateRect(hwnd, NULL, FALSE);
}

Shape* MyEditor::OnLButtonUp(HWND hwnd, LPARAM lParam) {
    if (!isDrawing) return nullptr;
    isDrawing = false;
    ReleaseCapture();
    int endX = GET_X_LPARAM(lParam);
    int endY = GET_Y_LPARAM(lParam);
    Shape* shape = AddShape(drawingType, startX, startY, endX, endY); 
    InvalidateRect(hwnd, NULL, FALSE);
    return shape;
}

void MyEditor::CancelDrawing(HWND hwnd) {
    if (isDrawing) {
        isDrawing = false;
        InvalidateRect(hwnd, NULL, FALSE);
    }
}

void MyEditor::OnPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT r;
    GetClientRect(hwnd, &r);
    HDC mem = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, max(1, r.right), max(1, r.bottom));
    HGDIOBJ old = SelectObject(mem, bmp);
    FillRect(mem, &r, (HBRUSH)(COLOR_WINDOW + 1));
    for (int i = 0; i < shapeCount; i++) pcshape[i]->Draw(mem);
    if (selectedIndex >= 0) {
        Shape* shape = pcshape[selectedIndex];
        HPEN pen = CreatePen(PS_DASH, 1, RGB(255, 0, 0));
        HGDIOBJ oldPen = SelectObject(mem, pen);
        HGDIOBJ oldBrush = SelectObject(mem, GetStockObject(NULL_BRUSH));
        Rectangle(mem, min(shape->X1(), shape->X2()) - 6,
            min(shape->Y1(), shape->Y2()) - 6,
            max(shape->X1(), shape->X2()) + 6,
            max(shape->Y1(), shape->Y2()) + 6);
        SelectObject(mem, oldBrush);
        SelectObject(mem, oldPen);
        DeleteObject(pen);
    }
    if (isDrawing) {
        // Вимога завдання: гумовий слід завжди пунктирною (PS_DASH) лінією
        Shape* preview = factory.at(drawingType)(startX, startY, currentX, currentY); 
        preview->Draw(mem, true);
        delete preview;
    }
    BitBlt(hdc, 0, 0, r.right, r.bottom, mem, 0, 0, SRCCOPY);
    SelectObject(mem, old);
    DeleteObject(bmp);
    DeleteDC(mem);
    EndPaint(hwnd, &ps);
}

void MyEditor::Clear() {
    for (int i = 0; i < shapeCount; i++) {
        delete pcshape[i];
        pcshape[i] = nullptr;
    }
    shapeCount = 0;
    selectedIndex = -1;
    isDrawing = false;
}
