#include <windows.h>
#include <commctrl.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <commdlg.h>
#include "my_editor.h"
#include "my_table.h"
#include "PointShape.h"
#include "LineShape.h"
#include "RectShape.h"
#include "EllipseShape.h"
#include "LineWithCirclesShape.h"
#include "CubeShape.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")

#define ID_FILE_CLEAR 101
#define ID_FILE_EXIT 102
#define ID_TABLE 103
#define ID_FILE_LOAD 104
#define ID_OBJ_POINT 201
#define ID_OBJ_LINE 202
#define ID_OBJ_RECT 203
#define ID_OBJ_ELLIPSE 204
#define ID_OBJ_LINECIRCLE 205
#define ID_OBJ_CUBE 206

HINSTANCE hInst;
HWND g_hWndToolbar = NULL;
HIMAGELIST hImages = NULL;
MyEditor& editor = MyEditor::GetInstance(); 
MyTable table;

struct Tool {
    int id;
    const wchar_t* title;
    const char* className;
    MyEditor::Creator create;
};

const Tool tools[] = { 
    {ID_OBJ_POINT, L"Point", "PointShape", [](int x1, int y1, int, int) -> Shape* { return new PointShape(x1, y1, RGB(0, 0, 0)); }},
    {ID_OBJ_LINE, L"Line", "LineShape", [](int x1, int y1, int x2, int y2) -> Shape* { return new LineShape(x1, y1, x2, y2, RGB(0, 0, 0)); }},
    {ID_OBJ_RECT, L"Rectangle", "RectShape", [](int x1, int y1, int x2, int y2) -> Shape* { return new RectShape(x1, y1, x2, y2, RGB(0, 0, 0)); }},
    {ID_OBJ_ELLIPSE, L"Ellipse", "EllipseShape", [](int x1, int y1, int x2, int y2) -> Shape* { return new EllipseShape(x1, y1, x2, y2, RGB(0, 0, 0)); }},
    {ID_OBJ_LINECIRCLE, L"Line with Circles", "LineWithCirclesShape", [](int x1, int y1, int x2, int y2) -> Shape* { return new LineWithCirclesShape(x1, y1, x2, y2, RGB(0, 0, 0)); }},
    {ID_OBJ_CUBE, L"Cube", "CubeShape", [](int x1, int y1, int x2, int y2) -> Shape* { return new CubeShape(x1, y1, x2, y2, RGB(0, 0, 0)); }}
};
const int toolCount = sizeof(tools) / sizeof(tools[0]);

void CreateIcons(HWND hwnd) {
    hImages = ImageList_Create(24, 24, ILC_COLOR24 | ILC_MASK, toolCount, 0);
    HDC hdc = GetDC(hwnd);
    HDC memDC = CreateCompatibleDC(hdc);
    for (int i = 0; i < toolCount; i++) {
        HBITMAP bmp = CreateCompatibleBitmap(hdc, 24, 24);
        HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, bmp);
        RECT r = {0, 0, 24, 24};
        FillRect(memDC, &r, (HBRUSH)GetStockObject(WHITE_BRUSH));
        SelectObject(memDC, GetStockObject(BLACK_PEN));
        SelectObject(memDC, GetStockObject(NULL_BRUSH));
        if (i == 0) {
            SelectObject(memDC, GetStockObject(BLACK_BRUSH));
            Ellipse(memDC, 10, 10, 14, 14);
        }
        else if (i == 1) {
            MoveToEx(memDC, 4, 20, NULL); LineTo(memDC, 20, 4);
        }
        else if (i == 2) Rectangle(memDC, 4, 6, 20, 18);
        else if (i == 3) Ellipse(memDC, 4, 6, 20, 18);
        else if (i == 4) {
            MoveToEx(memDC, 4, 20, NULL); LineTo(memDC, 20, 4);
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

void SaveShape(HWND hwnd, const char* name, Shape* shape) {
    std::ofstream file("shapes.txt", std::ios::app); 
    file << name << '\t' << shape->X1() << '\t' << shape->Y1()  
         << '\t' << shape->X2() << '\t' << shape->Y2() << '\n'; 
    file.close();
    if (!file) MessageBoxW(hwnd, L"Cannot write shapes.txt", L"Error", MB_OK | MB_ICONERROR);
}

void AddTableRow(const char* name, Shape* shape) {
    std::wstring wideName(name, name + lstrlenA(name));
    table.Add(wideName.c_str(), shape->X1(), shape->Y1(), shape->X2(), shape->Y2());
}

void SaveAll(HWND hwnd) {
    std::ofstream file("shapes.tmp", std::ios::trunc);
    for (int i = 0; i < editor.GetShapeCount(); i++) {
        Shape* shape = editor.GetShape(i);
        file << editor.GetShapeName(i) << '\t' << shape->X1() << '\t' << shape->Y1()
             << '\t' << shape->X2() << '\t' << shape->Y2() << '\n';
    }
    file.close();
    if (!file || !MoveFileExW(L"shapes.tmp", L"shapes.txt", MOVEFILE_REPLACE_EXISTING)) {
        DeleteFileW(L"shapes.tmp");
        MessageBoxW(hwnd, L"Cannot update shapes.txt", L"Error", MB_OK | MB_ICONERROR);
    }
}

void OnTableSelect(HWND hwnd, int index) { 
    editor.Select(index); 
    if (IsWindow(hwnd)) InvalidateRect(hwnd, NULL, FALSE); 
} 

void OnTableDelete(HWND hwnd, int index) { 
    editor.Remove(index); 
    table.Remove(index); 
    editor.Select(-1); 
    SaveAll(hwnd); 
    InvalidateRect(hwnd, NULL, FALSE); 
} 

bool LoadShapes(HWND hwnd, const wchar_t* path) { 
    std::ifstream file{std::filesystem::path(path)};
    if (!file) return false;
    struct Record {
        int type, x1, y1, x2, y2;
    };
    std::vector<Record> records;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream input(line);
        std::string name, extra;
        Record row = {};
        if (!(input >> name >> row.x1 >> row.y1 >> row.x2 >> row.y2) || input >> extra)
            return false;
        for (const Tool& tool : tools) {
            if (name == tool.className) row.type = tool.id;
        }
        if (!row.type || records.size() >= MAX_SHAPES) return false;
        int values[] = {row.x1, row.y1, row.x2, row.y2};
        for (int value : values) {
            if (value < -32768 || value > 32767) return false;
        }
        records.push_back(row);
    }
    if (!file.eof()) return false;

    editor.Clear();
    if (GetCapture() == hwnd) ReleaseCapture();
    table.Clear();
    for (const Record& row : records) {
        Shape* shape = editor.AddShape(row.type, row.x1, row.y1, row.x2, row.y2); 
        if (shape) AddTableRow(editor.GetShapeName(editor.GetShapeCount() - 1), shape);
    }
    InvalidateRect(hwnd, NULL, FALSE);
    return true;
}

void OpenShapes(HWND hwnd) {
    wchar_t path[MAX_PATH] = {};
    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    if (!GetOpenFileNameW(&ofn)) return;
    if (MessageBoxW(hwnd, L"Replace the current drawing and shapes.txt?", L"Load",
        MB_YESNO | MB_ICONQUESTION) != IDYES) return;
    if (LoadShapes(hwnd, path)) SaveAll(hwnd);
    else MessageBoxW(hwnd, L"Invalid file or too many shapes (maximum 121).",
        L"Error", MB_OK | MB_ICONERROR);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITMENUPOPUP:
        for (const Tool& tool : tools)
            CheckMenuItem((HMENU)wParam, tool.id, MF_BYCOMMAND | (editor.GetShapeType() == tool.id ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem((HMENU)wParam, ID_TABLE, MF_BYCOMMAND | (table.GetWindow() ? MF_CHECKED : MF_UNCHECKED));
        break;
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == ID_FILE_CLEAR) {
            if (MessageBoxW(hwnd, L"Clear the drawing and shapes.txt?", L"Clear",
                MB_YESNO | MB_ICONQUESTION) != IDYES) break;
            editor.Clear();
            if (GetCapture() == hwnd) ReleaseCapture();
            table.Clear();
            SaveAll(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        else if (id == ID_FILE_LOAD) OpenShapes(hwnd);
        else if (id == ID_FILE_EXIT) DestroyWindow(hwnd);
        else if (id == ID_TABLE) table.Toggle(hInst, hwnd);
        else {
            editor.SetShapeType(id);
            for (const Tool& tool : tools)
                SendMessageW(g_hWndToolbar, TB_CHECKBUTTON, tool.id, MAKELONG(editor.GetShapeType() == tool.id, 0));
        }
        break;
    }
    case WM_NOTIFY: {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        if (pnmh->code == TTN_GETDISPINFOW) {
            LPNMTTDISPINFOW pdi = (LPNMTTDISPINFOW)lParam;
            for (const Tool& tool : tools)
                if (pdi->hdr.idFrom == (UINT_PTR)tool.id) {
                    lstrcpyW(pdi->szText, L"Create ");
                    lstrcpynW(pdi->szText + 7, tool.title, 73);
                }
        }
        break;
    }
    case WM_LBUTTONDOWN:
        editor.OnLButtonDown(hwnd, lParam);
        break;
    case WM_MOUSEMOVE:
        editor.OnMouseMove(hwnd, lParam);
        break;
    case WM_LBUTTONUP: {
        Shape* shape = editor.OnLButtonUp(hwnd, lParam); 
        if (shape) { 
            const char* name = editor.GetShapeName(); 
            AddTableRow(name, shape); 
            SaveShape(hwnd, name, shape); 
        }
        break;
    }
    case WM_CAPTURECHANGED:
        editor.CancelDrawing(hwnd);
        break;
    case WM_PAINT:
        editor.OnPaint(hwnd);
        break;
    case WM_ERASEBKGND:
        return 1;
    case WM_SIZE:
        if (g_hWndToolbar) SendMessageW(g_hWndToolbar, TB_AUTOSIZE, 0, 0);
        InvalidateRect(hwnd, NULL, FALSE);
        break;
    case WM_DESTROY:
        table.Close();
        if (g_hWndToolbar) SendMessageW(g_hWndToolbar, TB_SETIMAGELIST, 0, 0);
        if (hImages) ImageList_Destroy(hImages);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
    return 0;
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {
    hInst = hInstance;
    INITCOMMONCONTROLSEX icx = {sizeof(icx), ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES};
    InitCommonControlsEx(&icx);
    for (const Tool& tool : tools) 
    editor.RegisterShape(tool.id, tool.className, tool.create);
    editor.SetShapeType(ID_OBJ_POINT);
    table.SetCallbacks(OnTableSelect, OnTableDelete); 

    WNDCLASSEXW wc = {sizeof(wc)};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"Lab5Class";
    RegisterClassExW(&wc);

    HMENU menu = CreateMenu();
    HMENU file = CreatePopupMenu();
    HMENU objects = CreatePopupMenu();
    HMENU help = CreatePopupMenu();
    AppendMenuW(file, MF_STRING, ID_FILE_LOAD, L"Load");
    AppendMenuW(file, MF_STRING, ID_FILE_CLEAR, L"Clear");
    AppendMenuW(file, MF_SEPARATOR, 0, NULL);
    AppendMenuW(file, MF_STRING, ID_FILE_EXIT, L"Exit");
    for (const Tool& tool : tools) AppendMenuW(objects, MF_STRING, tool.id, tool.title);
    AppendMenuW(menu, MF_POPUP, (UINT_PTR)file, L"File");
    AppendMenuW(menu, MF_POPUP, (UINT_PTR)objects, L"Objects");
    AppendMenuW(menu, MF_STRING, ID_TABLE, L"Table");
    AppendMenuW(menu, MF_POPUP, (UINT_PTR)help, L"Help");
    std::wstring title = L"Lab 5 - Variant " + std::to_wstring(LAB_VARIANT);
    HWND hwnd = CreateWindowW(L"Lab5Class", title.c_str(), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, 0, 850, 650, NULL, menu, hInstance, NULL);
    if (!hwnd) { MyEditor::ReleaseInstance(); return 0; }

    // Створення панелі інструментів на 6 кнопок із підказками
    g_hWndToolbar = CreateWindowExW(0, TOOLBARCLASSNAMEW, NULL,
        WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS, 0, 0, 0, 0, hwnd, (HMENU)1, hInstance, NULL);
    SendMessageW(g_hWndToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    CreateIcons(g_hWndToolbar);
    SendMessageW(g_hWndToolbar, TB_SETIMAGELIST, 0, (LPARAM)hImages);
    TBBUTTON tbb[toolCount] = {};
    for (int i = 0; i < toolCount; i++) {
        tbb[i].iBitmap = i;
        tbb[i].idCommand = tools[i].id;
        tbb[i].fsState = TBSTATE_ENABLED | (i == 0 ? TBSTATE_CHECKED : 0);
        tbb[i].fsStyle = BTNS_CHECKGROUP;
        tbb[i].iString = -1;
    }
    SendMessageW(g_hWndToolbar, TB_ADDBUTTONS, toolCount, (LPARAM)tbb);
    SendMessageW(g_hWndToolbar, TB_AUTOSIZE, 0, 0);
    if (GetFileAttributesW(L"shapes.txt") != INVALID_FILE_ATTRIBUTES) {
        if (!LoadShapes(hwnd, L"shapes.txt")) {
            MessageBoxW(hwnd, L"Cannot load shapes.txt. Move or correct this file before starting Lab5.",
                L"Error", MB_OK | MB_ICONERROR);
            DestroyWindow(hwnd);
            MyEditor::ReleaseInstance();
            return 0;
        }
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        if (!table.GetWindow() || !IsDialogMessageW(table.GetWindow(), &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    MyEditor::ReleaseInstance();
    return (int)msg.wParam;
}
