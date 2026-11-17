#pragma once
#include <windows.h>
#include "Shape.h"

#define ID_OBJ_POINT       201
#define ID_OBJ_LINE        202
#define ID_OBJ_RECT        203
#define ID_OBJ_ELLIPSE     204
#define ID_OBJ_LINECIRCLE  205
#define ID_OBJ_CUBE        206

const int MAX_SHAPES = 121;

class MyEditor { 
private:
    Shape** pcshape; 
    int shapeCount; 
    int currentShapeType; 
    bool isDrawing;
    int startX, startY;
    int currentX, currentY;

public:
    MyEditor() {
        pcshape = new Shape * [MAX_SHAPES]; 
        for (int i = 0; i < MAX_SHAPES; i++) {
            pcshape[i] = nullptr;
        }
        shapeCount = 0;
        currentShapeType = ID_OBJ_POINT;
        isDrawing = false;
        startX = startY = currentX = currentY = 0;
    }

    ~MyEditor() {
        for (int i = 0; i < shapeCount; i++) {
            delete pcshape[i];
        }
        delete[] pcshape;
    }

    void SetShapeType(int type) {
        currentShapeType = type;
    }

    int GetShapeType() const {
        return currentShapeType;
    }

    void OnLButtonDown(HWND hwnd, LPARAM lParam) { 
        isDrawing = true;
        startX = LOWORD(lParam);
        startY = HIWORD(lParam);
        currentX = startX;
        currentY = startY;
        SetCapture(hwnd);
    }

    void OnMouseMove(HWND hwnd, LPARAM lParam) { 
        if (isDrawing) {
            HDC hdc = GetDC(hwnd);
            SetROP2(hdc, R2_NOTXORPEN);

            // Вимога завдання: гумовий слід завжди пунктирною (PS_DASH) лінією
            HPEN hPenOld = (HPEN)SelectObject(hdc, CreatePen(PS_DASH, 1, RGB(0, 0, 0))); 

            if (currentShapeType == ID_OBJ_LINE || currentShapeType == ID_OBJ_LINECIRCLE) {
                MoveToEx(hdc, startX, startY, NULL); LineTo(hdc, currentX, currentY);
                MoveToEx(hdc, startX, startY, NULL); LineTo(hdc, LOWORD(lParam), HIWORD(lParam));
            }
            else if (currentShapeType == ID_OBJ_RECT) {
                int w1 = abs(currentX - startX);
                int h1 = abs(currentY - startY);
                Rectangle(hdc, startX - w1, startY - h1, startX + w1, startY + h1);
                int w2 = abs(LOWORD(lParam) - startX);
                int h2 = abs(HIWORD(lParam) - startY);
                Rectangle(hdc, startX - w2, startY - h2, startX + w2, startY + h2);
            }
            else if (currentShapeType == ID_OBJ_ELLIPSE) {
                Ellipse(hdc, startX, startY, currentX, currentY);
                Ellipse(hdc, startX, startY, LOWORD(lParam), HIWORD(lParam));
            }
            else if (currentShapeType == ID_OBJ_CUBE) {
                Rectangle(hdc, startX, startY, currentX, currentY);
                Rectangle(hdc, startX, startY, LOWORD(lParam), HIWORD(lParam));
            }

            currentX = LOWORD(lParam);
            currentY = HIWORD(lParam);

            DeleteObject(SelectObject(hdc, hPenOld));
            ReleaseDC(hwnd, hdc);
        }
    }

    void OnLButtonUp(HWND hwnd, LPARAM lParam) { 
        if (isDrawing) {
            isDrawing = false;
            ReleaseCapture();

            int endX = LOWORD(lParam);
            int endY = HIWORD(lParam);

            if (shapeCount < MAX_SHAPES) {
                if (currentShapeType == ID_OBJ_POINT) {
                    pcshape[shapeCount++] = new PointShape(startX, startY, RGB(0, 0, 0));
                }
                else if (currentShapeType == ID_OBJ_LINE) {
                    pcshape[shapeCount++] = new LineShape(startX, startY, endX, endY, RGB(0, 0, 0));
                }
                else if (currentShapeType == ID_OBJ_RECT) {
                    pcshape[shapeCount++] = new RectShape(startX, startY, endX, endY, RGB(0, 0, 0));
                }
                else if (currentShapeType == ID_OBJ_ELLIPSE) {
                    pcshape[shapeCount++] = new EllipseShape(startX, startY, endX, endY, RGB(0, 0, 0));
                }
                else if (currentShapeType == ID_OBJ_LINECIRCLE) {
                    pcshape[shapeCount++] = new LineWithCirclesShape(startX, startY, endX, endY, RGB(0, 0, 0));
                }
                else if (currentShapeType == ID_OBJ_CUBE) {
                    pcshape[shapeCount++] = new CubeShape(startX, startY, endX, endY, RGB(0, 0, 0));
                }
            }
            InvalidateRect(hwnd, NULL, TRUE);
        }
    }

    void OnPaint(HWND hwnd) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        for (int i = 0; i < shapeCount; i++) {
            if (pcshape[i] != nullptr) {
                pcshape[i]->Draw(hdc);
            }
        }
        EndPaint(hwnd, &ps);
    }

    void Clear() {
        for (int i = 0; i < shapeCount; i++) {
            delete pcshape[i];
            pcshape[i] = nullptr;
        }
        shapeCount = 0;
    }
};