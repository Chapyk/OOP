#include <windows.h>
#include "Shape.h"
#define ID_FILE_CLEAR   101
#define ID_FILE_EXIT    102
#define ID_OBJ_POINT    201
#define ID_OBJ_LINE     202
#define ID_OBJ_RECT     203
#define ID_OBJ_ELLIPSE  204

HINSTANCE hInst;
HWND g_hWnd = NULL;

const int MAX_SHAPES = 120;
Shape* pcshape[MAX_SHAPES] = { nullptr }; 
int shapeCount = 0;

int currentShapeType = 1;
bool isDrawing = false;
int startX = 0, startY = 0;
int currentX = 0, currentY = 0;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) { 
    hInst = hInstance;
    WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"Lab2Class";
    RegisterClassExW(&wcex);

    HMENU hMenu = CreateMenu(); 
    HMENU hFileMenu = CreateMenu();
    HMENU hObjMenu = CreateMenu();
    HMENU hHelpMenu = CreateMenu();

    AppendMenuW(hFileMenu, MF_STRING, ID_FILE_CLEAR, L"Clear");
    AppendMenuW(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hFileMenu, MF_STRING, ID_FILE_EXIT, L"Exit");

    AppendMenuW(hObjMenu, MF_STRING, ID_OBJ_POINT, L"Point");
    AppendMenuW(hObjMenu, MF_STRING, ID_OBJ_LINE, L"Line");
    AppendMenuW(hObjMenu, MF_STRING, ID_OBJ_RECT, L"Rectangle");
    AppendMenuW(hObjMenu, MF_STRING, ID_OBJ_ELLIPSE, L"Ellipse");

    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hObjMenu, L"Objects");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"Help");

    g_hWnd = CreateWindowW(L"Lab2Class", L"Lab 2 - Variant 20",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 800, 600, nullptr, hMenu, hInstance, nullptr);

    if (!g_hWnd) return FALSE;
    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) { 
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    for (int i = 0; i < shapeCount; i++) {
        delete pcshape[i];
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITMENUPOPUP: { 
        HMENU hMenu = (HMENU)wParam; 
        CheckMenuItem(hMenu, ID_OBJ_POINT, MF_BYCOMMAND | (currentShapeType == 1 ? MF_CHECKED : MF_UNCHECKED)); 
        CheckMenuItem(hMenu, ID_OBJ_LINE, MF_BYCOMMAND | (currentShapeType == 2 ? MF_CHECKED : MF_UNCHECKED)); 
        CheckMenuItem(hMenu, ID_OBJ_RECT, MF_BYCOMMAND | (currentShapeType == 3 ? MF_CHECKED : MF_UNCHECKED)); 
        CheckMenuItem(hMenu, ID_OBJ_ELLIPSE, MF_BYCOMMAND | (currentShapeType == 4 ? MF_CHECKED : MF_UNCHECKED)); 
        break;
    }
    case WM_COMMAND: { 
        int id = LOWORD(wParam);
        if (id == ID_FILE_CLEAR) {
            for (int i = 0; i < shapeCount; i++) {
                delete pcshape[i];
                pcshape[i] = nullptr;
            }
            shapeCount = 0;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        else if (id == ID_FILE_EXIT) {
            DestroyWindow(hWnd);
        }
        else if (id >= ID_OBJ_POINT && id <= ID_OBJ_ELLIPSE) {
            currentShapeType = id - ID_OBJ_POINT + 1; 
        }
        break;
    }
    case WM_LBUTTONDOWN: { 
        isDrawing = true;
        startX = LOWORD(lParam);
        startY = HIWORD(lParam);
        currentX = startX;
        currentY = startY;
        SetCapture(hWnd);
        break;
    }
    case WM_MOUSEMOVE: { 
        if (isDrawing) {
            HDC hdc = GetDC(hWnd);
            SetROP2(hdc, R2_NOTXORPEN); 

            HPEN hPenOld = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)));

            if (currentShapeType == 2) {
                MoveToEx(hdc, startX, startY, NULL); LineTo(hdc, currentX, currentY);
                MoveToEx(hdc, startX, startY, NULL); LineTo(hdc, LOWORD(lParam), HIWORD(lParam));
            }
            else if (currentShapeType == 3) {
                Rectangle(hdc, startX, startY, currentX, currentY);
                Rectangle(hdc, startX, startY, LOWORD(lParam), HIWORD(lParam));
            }
            else if (currentShapeType == 4) {
                Ellipse(hdc, startX, startY, currentX, currentY);
                Ellipse(hdc, startX, startY, LOWORD(lParam), HIWORD(lParam));
            }

            currentX = LOWORD(lParam);
            currentY = HIWORD(lParam);

            DeleteObject(SelectObject(hdc, hPenOld));
            ReleaseDC(hWnd, hdc);
        }
        break;
    }
    case WM_LBUTTONUP: { 
        if (isDrawing) {
            isDrawing = false;
            ReleaseCapture();

            int endX = LOWORD(lParam);
            int endY = HIWORD(lParam);

            if (shapeCount < MAX_SHAPES) {
                if (currentShapeType == 1) {
                    pcshape[shapeCount++] = new PointShape(startX, startY, RGB(0, 0, 0));
                }
                else if (currentShapeType == 2) {
                    pcshape[shapeCount++] = new LineShape(startX, startY, endX, endY, RGB(0, 0, 0));
                }
                else if (currentShapeType == 3) {
                    pcshape[shapeCount++] = new RectShape(startX, startY, endX, endY, RGB(0, 0, 0));
                }
                else if (currentShapeType == 4) {
                    pcshape[shapeCount++] = new EllipseShape(startX, startY, endX, endY, RGB(0, 0, 0));
                }
            }
            InvalidateRect(hWnd, NULL, TRUE); 
        }
        break;
    }
    case WM_PAINT: { 
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        for (int i = 0; i < shapeCount; i++) {
            if (pcshape[i] != nullptr) {
                pcshape[i]->Draw(hdc); 
            }
        }

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}