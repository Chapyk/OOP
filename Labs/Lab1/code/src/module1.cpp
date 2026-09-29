#include "module1.h"
#include "utils.h"
#include "resource.h" 

static INT_PTR CALLBACK Dialog1Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            wchar_t buffer[256];
            // IDC_EDIT1 - це стандартний ID поля вводу. Якщо компілятор його не знайде, 
            // перевір його ID у властивостях поля в Resource View
            GetDlgItemText(hDlg, IDC_EDIT1, buffer, 256);
            g_displayText = buffer;

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

void ShowWork1Dialog(HINSTANCE hInstance, HWND parent) {
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), parent, Dialog1Proc);
}