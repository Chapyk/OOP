#pragma once
#include <windows.h>
#include <string>
#include <vector>

class MyTable { 
public:
    using Callback = void (*)(HWND, int);
private:
    struct Row { 
        std::wstring name; 
        int x1, y1, x2, y2; 
    }; 
    std::vector<Row> rows; 
    HWND dialog = NULL;
    HWND list = NULL;
    HWND owner = NULL;
    Callback onSelect = nullptr;
    Callback onDelete = nullptr;
    static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void Insert(const Row& row);
public:
    void Add(const wchar_t* name, int x1, int y1, int x2, int y2);
    void Clear();
    void Remove(int index);
    void SetCallbacks(Callback select, Callback remove);
    void Toggle(HINSTANCE instance, HWND owner);
    void Close();
    HWND GetWindow() const { return dialog; }
};
