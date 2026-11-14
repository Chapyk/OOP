#include <windows.h>
#include <commctrl.h>
#include "MyEditor.h"

#pragma comment(lib, "comctl32.lib")

#define ID_FILE_CLEAR   101
#define ID_FILE_EXIT    102

HINSTANCE hInst;
HWND g_hWnd = NULL;
HWND g_hWndToolbar = NULL;

// Згідно з варіантом 21 (непарний номер): створюємо глобальний статичний об'єкт MyEditor
MyEditor editor;

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
    wcex.lpszClassName = L"Lab4Class";
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
    AppendMenuW(hObjMenu, MF_STRING, ID_OBJ_LINECIRCLE, L"Line with Circles");
    AppendMenuW(hObjMenu, MF_STRING, ID_OBJ_CUBE, L"Cube");

    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hObjMenu, L"Objects");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"Help");

    g_hWnd = CreateWindowW(L"Lab4Class", L"Lab 4 - Variant 21",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 850, 650, nullptr, hMenu, hInstance, nullptr);

    if (!g_hWnd) return FALSE;

    // Створення панелі інструментів на 6 кнопок із підказками
    g_hWndToolbar = CreateWindowExW(0, TOOLBARCLASSNAMEW, NULL,
        WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS, 0, 0, 0, 0,
        g_hWnd, (HMENU)1, hInstance, NULL);

    SendMessageW(g_hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

    TBBUTTON tbb[6] = {
        { 0, ID_OBJ_POINT, TBSTATE_ENABLED | TBSTATE_CHECKED, BTNS_BUTTON, {0}, 0, (INT_PTR)L"Point" },
        { 1, ID_OBJ_LINE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)L"Line" },
        { 2, ID_OBJ_RECT, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)L"Rectangle" },
        { 3, ID_OBJ_ELLIPSE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)L"Ellipse" },
        { 4, ID_OBJ_LINECIRCLE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)L"Line+Circle" },
        { 5, ID_OBJ_CUBE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)L"Cube" }
    };

    SendMessageW(g_hWndToolbar, TB_ADDBUTTONS, (WPARAM)6, (LPARAM)&tbb);
    SendMessageW(g_hWndToolbar, TB_AUTOSIZE, 0, 0);

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITMENUPOPUP: {
        HMENU hMenu = (HMENU)wParam;
        int cur = editor.GetShapeType();
        CheckMenuItem(hMenu, ID_OBJ_POINT, MF_BYCOMMAND | (cur == ID_OBJ_POINT ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem(hMenu, ID_OBJ_LINE, MF_BYCOMMAND | (cur == ID_OBJ_LINE ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem(hMenu, ID_OBJ_RECT, MF_BYCOMMAND | (cur == ID_OBJ_RECT ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem(hMenu, ID_OBJ_ELLIPSE, MF_BYCOMMAND | (cur == ID_OBJ_ELLIPSE ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem(hMenu, ID_OBJ_LINECIRCLE, MF_BYCOMMAND | (cur == ID_OBJ_LINECIRCLE ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem(hMenu, ID_OBJ_CUBE, MF_BYCOMMAND | (cur == ID_OBJ_CUBE ? MF_CHECKED : MF_UNCHECKED));
        break;
    }
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == ID_FILE_CLEAR) {
            editor.Clear();
            InvalidateRect(hWnd, NULL, TRUE);
        }
        else if (id == ID_FILE_EXIT) {
            DestroyWindow(hWnd);
        }
        else if (id >= ID_OBJ_POINT && id <= ID_OBJ_CUBE) {
            editor.SetShapeType(id);
            SendMessageW(g_hWndToolbar, TB_CHECKBUTTON, ID_OBJ_POINT, (LPARAM)MAKELONG(id == ID_OBJ_POINT ? TRUE : FALSE, 0));
            SendMessageW(g_hWndToolbar, TB_CHECKBUTTON, ID_OBJ_LINE, (LPARAM)MAKELONG(id == ID_OBJ_LINE ? TRUE : FALSE, 0));
            SendMessageW(g_hWndToolbar, TB_CHECKBUTTON, ID_OBJ_RECT, (LPARAM)MAKELONG(id == ID_OBJ_RECT ? TRUE : FALSE, 0));
            SendMessageW(g_hWndToolbar, TB_CHECKBUTTON, ID_OBJ_ELLIPSE, (LPARAM)MAKELONG(id == ID_OBJ_ELLIPSE ? TRUE : FALSE, 0));
            SendMessageW(g_hWndToolbar, TB_CHECKBUTTON, ID_OBJ_LINECIRCLE, (LPARAM)MAKELONG(id == ID_OBJ_LINECIRCLE ? TRUE : FALSE, 0));
            SendMessageW(g_hWndToolbar, TB_CHECKBUTTON, ID_OBJ_CUBE, (LPARAM)MAKELONG(id == ID_OBJ_CUBE ? TRUE : FALSE, 0));
        }
        break;
    }
    case WM_NOTIFY: {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        if (pnmh->code == TTN_GETDISPINFOW) {
            LPNMTTDISPINFOW pdi = (LPNMTTDISPINFOW)lParam;
            if (pdi->hdr.idFrom == ID_OBJ_POINT) wcscpy_s(pdi->szText, L"Create Point");
            else if (pdi->hdr.idFrom == ID_OBJ_LINE) wcscpy_s(pdi->szText, L"Create Line");
            else if (pdi->hdr.idFrom == ID_OBJ_RECT) wcscpy_s(pdi->szText, L"Create Rectangle");
            else if (pdi->hdr.idFrom == ID_OBJ_ELLIPSE) wcscpy_s(pdi->szText, L"Create Ellipse");
            else if (pdi->hdr.idFrom == ID_OBJ_LINECIRCLE) wcscpy_s(pdi->szText, L"Create Line with Circles");
            else if (pdi->hdr.idFrom == ID_OBJ_CUBE) wcscpy_s(pdi->szText, L"Create Cube");
        }
        break;
    }
    case WM_LBUTTONDOWN:
        editor.OnLButtonDown(hWnd, lParam);
        break;
    case WM_MOUSEMOVE:
        editor.OnMouseMove(hWnd, lParam);
        break;
    case WM_LBUTTONUP:
        editor.OnLButtonUp(hWnd, lParam);
        break;
    case WM_PAINT:
        editor.OnPaint(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}