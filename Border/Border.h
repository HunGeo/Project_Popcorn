#pragma once

#include <Windows.h>

//------------------------------------------------------------------------------------------------------------
class AsConfig
{
public:
	static const int Global_Scale = 3;

	static void Create_Pen_Brush(unsigned char r, unsigned char g, unsigned char b, HPEN &pen, HBRUSH &brush);
};
//------------------------------------------------------------------------------------------------------------
class AsBorder
{
public:
	void Init();
	void Draw(HDC hdc, RECT &paint_area, HPEN bg_pen, HBRUSH bg_brush);

	static const int Border_X_Offset = 6;
	static const int Border_Y_Offset = 4;

private:
	void Draw_Element(HDC hdc, int x, int y, bool top_boder, HPEN bg_pen, HBRUSH bg_brush);

	HPEN Border_Blue_Pen, Border_White_Pen;
	HBRUSH Border_Blue_Brush, Border_White_Brush;
};
//------------------------------------------------------------------------------------------------------------