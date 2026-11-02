#include <windows.h>
#include <commctrl.h>
#include "Shape.h"

#pragma comment(lib, "comctl32.lib")

#define ID_FILE_CLEAR   101
#define ID_FILE_EXIT    102

#define ID_OBJ_POINT    201
#define ID_OBJ_LINE     202
#define ID_OBJ_RECT     203
#define ID_OBJ_ELLIPSE  204

HINSTANCE hInst;
HWND g_hWnd = NULL;
HWND g_hWndToolbar = NULL;

const int MAX_SHAPES = 121; // Варіант 21: розмір масиву 121
Shape** pcshape = nullptr; // Динамічний масив вказівників
int shapeCount = 0;

int currentShapeType = ID_OBJ_POINT;
bool isDrawing = false;
int startX = 0, startY = 0;
int currentX = 0, currentY = 0;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    hInst = hInstance;

    INITCOMMONCONTROLSEX icx;
    icx.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icx.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icx);

    WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"Lab3Class";
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

    g_hWnd = CreateWindowW(L"Lab3Class", L"Lab 3 - Variant 21",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 800, 600, nullptr, hMenu, hInstance, nullptr);

    if (!g_hWnd) return FALSE;

    // Створення панелі інструментів (Toolbar) з підтримкою підказок (tooltips)
    g_hWndToolbar = CreateWindowExW(0, TOOLBARCLASSNAMEW, NULL,
        WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS, 0, 0, 0, 0,
        g_hWnd, (HMENU)1, hInstance, NULL);

    SendMessageW(g_hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

    TBBUTTON tbb[4] = {
        { 0, ID_OBJ_POINT, TBSTATE_ENABLED | TBSTATE_CHECKED, BTNS_BUTTON, {0}, 0, (INT_PTR)L"Point" },
        { 1, ID_OBJ_LINE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)L"Line" },
        { 2, ID_OBJ_RECT, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)L"Rectangle" },
        { 3, ID_OBJ_ELLIPSE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)L"Ellipse" }
    };

    SendMessageW(g_hWndToolbar, TB_ADDBUTTONS, (WPARAM)4, (LPARAM)&tbb);
    SendMessageW(g_hWndToolbar, TB_AUTOSIZE, 0, 0); // Автоматичне підлаштування розміру тулбара

    // Виділення пам'яті під динамічний масив
    pcshape = new Shape * [MAX_SHAPES];
    for (int i = 0; i < MAX_SHAPES; i++) {
        pcshape[i] = nullptr;
    }

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Звільнення динамічної пам'яті
    for (int i = 0; i < shapeCount; i++) {
        delete pcshape[i];
    }
    delete[] pcshape;

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITMENUPOPUP: {
        HMENU hMenu = (HMENU)wParam;
        CheckMenuItem(hMenu, ID_OBJ_POINT, MF_BYCOMMAND | (currentShapeType == ID_OBJ_POINT ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem(hMenu, ID_OBJ_LINE, MF_BYCOMMAND | (currentShapeType == ID_OBJ_LINE ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem(hMenu, ID_OBJ_RECT, MF_BYCOMMAND | (currentShapeType == ID_OBJ_RECT ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem(hMenu, ID_OBJ_ELLIPSE, MF_BYCOMMAND | (currentShapeType == ID_OBJ_ELLIPSE ? MF_CHECKED : MF_UNCHECKED));
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
            currentShapeType = id;
            // Синхронізація вибору між меню та кнопками Toolbar
            SendMessageW(g_hWndToolbar, TB_CHECKBUTTON, ID_OBJ_POINT, (LPARAM)MAKELONG(id == ID_OBJ_POINT ? TRUE : FALSE, 0));
            SendMessageW(g_hWndToolbar, TB_CHECKBUTTON, ID_OBJ_LINE, (LPARAM)MAKELONG(id == ID_OBJ_LINE ? TRUE : FALSE, 0));
            SendMessageW(g_hWndToolbar, TB_CHECKBUTTON, ID_OBJ_RECT, (LPARAM)MAKELONG(id == ID_OBJ_RECT ? TRUE : FALSE, 0));
            SendMessageW(g_hWndToolbar, TB_CHECKBUTTON, ID_OBJ_ELLIPSE, (LPARAM)MAKELONG(id == ID_OBJ_ELLIPSE ? TRUE : FALSE, 0));
        }
        break;
    }
    case WM_NOTIFY: {
        // Обробка підказок (tooltips) для тулбара
        LPNMHDR pnmh = (LPNMHDR)lParam;
        if (pnmh->code == TTN_GETDISPINFOW) {
            LPNMTTDISPINFOW pdi = (LPNMTTDISPINFOW)lParam;
            if (pdi->hdr.idFrom == ID_OBJ_POINT) wcscpy_s(pdi->szText, L"Create Point");
            else if (pdi->hdr.idFrom == ID_OBJ_LINE) wcscpy_s(pdi->szText, L"Create Line");
            else if (pdi->hdr.idFrom == ID_OBJ_RECT) wcscpy_s(pdi->szText, L"Create Rectangle");
            else if (pdi->hdr.idFrom == ID_OBJ_ELLIPSE) wcscpy_s(pdi->szText, L"Create Ellipse");
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

            // Варіант 21: гумовий слід штриховою лінією (PS_DASH)
            HPEN hPenOld = (HPEN)SelectObject(hdc, CreatePen(PS_DASH, 1, RGB(0, 0, 0)));

            if (currentShapeType == ID_OBJ_LINE) {
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