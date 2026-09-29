#include "module2.h"
#include "utils.h"
#include "resource.h"
#include <string>

static INT_PTR CALLBACK Dialog2Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        // IDC_SCROLLBAR1 - стандартний ID повзунка
        SetScrollRange(GetDlgItem(hDlg, IDC_SCROLLBAR1), SB_CTL, 1, 100, TRUE);
        SetScrollPos(GetDlgItem(hDlg, IDC_SCROLLBAR1), SB_CTL, 50, TRUE);
        return (INT_PTR)TRUE;

    case WM_HSCROLL: {
        HWND hScroll = (HWND)lParam;
        int pos = GetScrollPos(hScroll, SB_CTL);
        int request = LOWORD(wParam);

        switch (request) {
        case SB_LINELEFT: pos -= 1; break;
        case SB_LINERIGHT: pos += 1; break;
        case SB_PAGELEFT: pos -= 10; break;
        case SB_PAGERIGHT: pos += 10; break;
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK: pos = HIWORD(wParam); break;
        }
        if (pos < 1) pos = 1;
        if (pos > 100) pos = 100;
        SetScrollPos(hScroll, SB_CTL, pos, TRUE);
        return (INT_PTR)TRUE;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            int pos = GetScrollPos(GetDlgItem(hDlg, IDC_SCROLLBAR1), SB_CTL);
            g_displayText = L"Selected number: " + std::to_wstring(pos);
            InvalidateRect(g_hMainWindow, NULL, TRUE);
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void ShowWork2Dialog(HINSTANCE hInstance, HWND parent) {
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG2), parent, Dialog2Proc);
}