#pragma once
#include "Shape.h"
#include <map>
#include <string>

#define LAB_VARIANT 21

const int MAX_SHAPES = 121;

class MyEditor {
public:
    using Creator = Shape* (*)(int, int, int, int); 
private:
    Shape** pcshape;
    int shapeCount;
    int shapeTypes[MAX_SHAPES];
    int selectedIndex;
    int currentShapeType;
    bool isDrawing;
    int startX, startY, currentX, currentY;
    int drawingType;
    std::map<int, Creator> factory;
    std::map<int, std::string> names;
    MyEditor(); 
    ~MyEditor(); 
    MyEditor(const MyEditor&) = delete;  
    MyEditor& operator=(const MyEditor&) = delete; 
#if LAB_VARIANT % 2 == 0
    static MyEditor* instance;
#endif
public:
    static MyEditor& GetInstance();
    static void ReleaseInstance();
    void RegisterShape(int type, const char* name, Creator creator);
    void SetShapeType(int type);
    int GetShapeType() const;
    const char* GetShapeName() const;
    const char* GetShapeName(int index) const;
    int GetShapeCount() const;
    Shape* GetShape(int index) const;
    Shape* AddShape(int type, int x1, int y1, int x2, int y2);
    void Select(int index);
    void Remove(int index);
    void OnLButtonDown(HWND hwnd, LPARAM lParam);
    void OnMouseMove(HWND hwnd, LPARAM lParam);
    Shape* OnLButtonUp(HWND hwnd, LPARAM lParam);
    void CancelDrawing(HWND hwnd);
    void OnPaint(HWND hwnd);
    void Clear();
};
