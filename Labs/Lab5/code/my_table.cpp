#include "my_table.h"
#include <commctrl.h>

void MyTable::Insert(const Row& row) {
    if (!list) return;
    LVITEMW item = {};
    item.mask = LVIF_TEXT;
    item.iItem = ListView_GetItemCount(list);
    item.pszText = const_cast<wchar_t*>(row.name.c_str());
    int index = (int)SendMessageW(list, LVM_INSERTITEMW, 0, (LPARAM)&item);
    if (index < 0) return;
    int values[] = {row.x1, row.y1, row.x2, row.y2};
    for (int i = 0; i < 4; i++) {
        std::wstring text = std::to_wstring(values[i]);
        LVITEMW cell = {};
        cell.iSubItem = i + 1;
        cell.pszText = const_cast<wchar_t*>(text.c_str());
        SendMessageW(list, LVM_SETITEMTEXTW, index, (LPARAM)&cell);
    }
    ListView_EnsureVisible(list, index, FALSE);
}

void MyTable::Add(const wchar_t* name, 
    int x1, int y1, int x2, int y2) { 
    rows.push_back({name, x1, y1, x2, y2}); 
    Insert(rows.back()); 
} 

void MyTable::Clear() {
    rows.clear();
    if (list) ListView_DeleteAllItems(list);
}

void MyTable::Remove(int index) {
    if (index < 0 || index >= (int)rows.size()) return;
    rows.erase(rows.begin() + index);
    if (list) {
        ListView_DeleteItem(list, index);
        ListView_SetItemState(list, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);
    }
}

void MyTable::SetCallbacks(Callback select, Callback remove) {
    onSelect = select;
    onDelete = remove;
}

void MyTable::Toggle(HINSTANCE instance, HWND owner) {
    this->owner = owner;
    if (dialog) Close();
    else {
        dialog = CreateDialogParamW(instance, MAKEINTRESOURCEW(501), 
           owner, DialogProc, (LPARAM)this);
        if (dialog) ShowWindow(dialog, SW_SHOW);
    }
}

void MyTable::Close() {
    if (dialog) DestroyWindow(dialog);
}

INT_PTR CALLBACK MyTable::DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    MyTable* table = (MyTable*)GetWindowLongPtrW(hwnd, DWLP_USER);
    if (message == WM_INITDIALOG) {
        table = (MyTable*)lParam;
        SetWindowLongPtrW(hwnd, DWLP_USER, (LONG_PTR)table);
        table->dialog = hwnd;
        table->list = GetDlgItem(hwnd, 502);
        ListView_SetExtendedListViewStyle(table->list, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        const wchar_t* names[] = {L"Name", L"x1", L"y1", L"x2", L"y2"};
        for (int i = 0; i < 5; i++) {
            LVCOLUMNW column = {};
            column.mask = LVCF_TEXT | LVCF_WIDTH;
            column.pszText = const_cast<wchar_t*>(names[i]);
            column.cx = i == 0 ? 175 : 65;
            SendMessageW(table->list, LVM_INSERTCOLUMNW, i, (LPARAM)&column);
        }
        for (const Row& row : table->rows) table->Insert(row);
        return TRUE;
    }
    if (!table) return FALSE;
    switch (message) {
    case WM_NOTIFY: {
        NMHDR* header = (NMHDR*)lParam;
        if (header->idFrom == 502 && header->code == LVN_ITEMCHANGED) {
            NMLISTVIEW* item = (NMLISTVIEW*)lParam;
            if ((item->uChanged & LVIF_STATE) &&
                ((item->uOldState ^ item->uNewState) & LVIS_SELECTED)) {
                int index = ListView_GetNextItem(table->list, -1, LVNI_SELECTED);
                if (table->onSelect) table->onSelect(table->owner, index);
            }
        }
        break;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL) { table->Close(); return TRUE; }
        if (LOWORD(wParam) == 503) {
            int index = ListView_GetNextItem(table->list, -1, LVNI_SELECTED);
            if (index >= 0 && table->onDelete) table->onDelete(table->owner, index);
            return TRUE;
        }
        break;
    case WM_CLOSE:
        table->Close();
        return TRUE;
    case WM_DESTROY:
        table->dialog = table->list = NULL;
        if (table->onSelect) table->onSelect(table->owner, -1);
        return TRUE;
    }
    return FALSE;
}
