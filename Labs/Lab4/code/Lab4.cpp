
#include <windows.h>
#include <commctrl.h>
#include "MyEditor.h"

#pragma comment(lib, "comctl32.lib")

#define ID_FILE_CLEAR   101
#define ID_FILE_EXIT    102

HINSTANCE hInst;
HWND g_hWnd = NULL;
HWND g_hWndToolbar = NULL;
HIMAGELIST hImages = NULL;


MyEditor editor; 

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void CreateIcons(HWND hwnd) {
    hImages = ImageList_Create(24, 24, ILC_COLOR24 | ILC_MASK, 6, 0);

    HDC hdc = GetDC(hwnd);
    HDC memDC = CreateCompatibleDC(hdc);

    for (int i = 0; i < 6; i++) {
        HBITMAP bmp = CreateCompatibleBitmap(hdc, 24, 24);
        HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, bmp);

        RECT r = { 0, 0, 24, 24 };
        FillRect(memDC, &r, (HBRUSH)GetStockObject(WHITE_BRUSH));

        SelectObject(memDC, GetStockObject(BLACK_PEN));
        SelectObject(memDC, GetStockObject(NULL_BRUSH));

        if (i == 0) {
            SelectObject(memDC, GetStockObject(BLACK_BRUSH));
            Ellipse(memDC, 10, 10, 14, 14);
        }
        else if (i == 1) {
            MoveToEx(memDC, 4, 20, NULL);
            LineTo(memDC, 20, 4);
        }
        else if (i == 2) {
            Rectangle(memDC, 4, 6, 20, 18);
        }
        else if (i == 3) {
            Ellipse(memDC, 4, 6, 20, 18);
        }
        else if (i == 4) {
            MoveToEx(memDC, 4, 20, NULL);
            LineTo(memDC, 20, 4);
            SelectObject(memDC, GetStockObject(BLACK_BRUSH));
            Ellipse(memDC, 2, 18, 7, 23);
            Ellipse(memDC, 17, 1, 22, 6);
        }
        else if (i == 5) {
            Rectangle(memDC, 4, 9, 16, 21);
            Rectangle(memDC, 9, 4, 21, 16);
            MoveToEx(memDC, 4, 9, NULL); LineTo(memDC, 9, 4);
            MoveToEx(memDC, 16, 9, NULL); LineTo(memDC, 21, 4);
            MoveToEx(memDC, 16, 21, NULL); LineTo(memDC, 21, 16);
            MoveToEx(memDC, 4, 21, NULL); LineTo(memDC, 9, 16);
        }

        SelectObject(memDC, oldBmp);
        ImageList_AddMasked(hImages, bmp, RGB(255, 255, 255));
        DeleteObject(bmp);
    }

    DeleteDC(memDC);
    ReleaseDC(hwnd, hdc);
}

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

    CreateIcons(g_hWndToolbar);
    SendMessageW(g_hWndToolbar, TB_SETIMAGELIST, 0, (LPARAM)hImages);

    TBBUTTON tbb[6] = { 
        { 0, ID_OBJ_POINT, TBSTATE_ENABLED | TBSTATE_CHECKED, BTNS_CHECKGROUP, {0}, 0, -1 },
        { 1, ID_OBJ_LINE, TBSTATE_ENABLED, BTNS_CHECKGROUP, {0}, 0, -1 },
        { 2, ID_OBJ_RECT, TBSTATE_ENABLED, BTNS_CHECKGROUP, {0}, 0, -1 },
        { 3, ID_OBJ_ELLIPSE, TBSTATE_ENABLED, BTNS_CHECKGROUP, {0}, 0, -1 },
        { 4, ID_OBJ_LINECIRCLE, TBSTATE_ENABLED, BTNS_CHECKGROUP, {0}, 0, -1 },
        { 5, ID_OBJ_CUBE, TBSTATE_ENABLED, BTNS_CHECKGROUP, {0}, 0, -1 }
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
        SendMessageW(g_hWndToolbar, TB_SETIMAGELIST, 0, 0);
        if (hImages != NULL) ImageList_Destroy(hImages);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
