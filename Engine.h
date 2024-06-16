#pragma once

#include <Windows.h>

#define _USE_MATH_DEFINES
#include <math.h>

enum EKey_Type
{
   EKT_Left,
   EKT_Right,
   EKT_Space
};

enum EBrick_Type
{
   EBT_None,
   EBT_Red,
   EBT_Blue
};

enum ELetter_Type
{
   ELT_None,
   ELT_O
};

//------------------------------------------------------------------------------------------------------------
const int Timer_ID = WM_USER + 1;
//------------------------------------------------------------------------------------------------------------
class ABall
{
public:
   void Draw_Ball(HDC hdc, RECT &paint_area);
   void Move_Ball();

   void Ball_Reflection_Wall();
   void Ball_Reflection_Brick();
   void Ball_Reflection_Platform();

   double Ball_X_Pos, Ball_Y_Pos;
   double Prev_Ball_X_Pos, Prev_Ball_Y_Pos;
   double Ball_Speed, Ball_Direction;
   double Cos_Ball_Direction, Sin_Ball_Direction;

   static const int Global_Scale = 3;
   static const int Ball_Size = 4;

   RECT Ball_Rect, Prev_Ball_Rect;
   HWND Hwnd;
   HPEN BG_Pen, Highlight_Pen, Letter_Pen, Red_Pen, Blue_Pen, Platform_Circle_Pen, Platform_Inner_Pen, White_Pen;
   HBRUSH BG_Brush, Red_Brush, Blue_Brush, Platform_Circle_Brush, Platform_Inner_Brush, White_Brush;
};
//------------------------------------------------------------------------------------------------------------
class AsEngine
{
public:
   AsEngine();

   void Init_Engine(HWND hwnd);
   void Draw_Frame(HDC hdc, RECT &paint_area);
   int On_Key_Down(EKey_Type key_type);
   int On_Timer();

   static const int Level_Width = 12, Level_Height = 14;                          // Width and Height expressed in cells

private:
   void Create_Pen_Brush(unsigned char r, unsigned char g, unsigned char b, HPEN &pen, HBRUSH &brush);
   void Redraw_Platform();
   void Generate_Bricks_Level_Rects();
   void Draw_Brick(HDC hdc, EBrick_Type brick_type, int x, int y);
   void Set_Brick_Letter_Colors(bool is_switch_color, EBrick_Type brick_type, HPEN &front_pen, HBRUSH &front_brush, HPEN &back_pen, HBRUSH &back_brush);
   void Draw_Brick_Letter(HDC hdc, int rotation_step, EBrick_Type brick_type, ELetter_Type letter_type, int x, int y);
   void Draw_Border(HDC hdc, int x, int y, bool top_border);
   void Draw_Level(HDC hdc);
   void Draw_Platform(HDC hdc, int x, int y);
   void Draw_Bounds(HDC hdc, RECT &paint_area);

   HWND Hwnd;
   HPEN BG_Pen, Highlight_Pen, Letter_Pen, Red_Pen, Blue_Pen, Platform_Circle_Pen, Platform_Inner_Pen, White_Pen;
   HBRUSH BG_Brush, Red_Brush, Blue_Brush, Platform_Circle_Brush, Platform_Inner_Brush, White_Brush;

   int Inner_Width;
   int Platform_X_Pos;
   int Platform_X_Step;
   int Platform_Width;
   int Timer_Frequency;      // Frame refresh time in msec

   RECT Platform_Rect, Prev_Platform_Rect;
   RECT Level_Rect;

   RECT Brick_Rect_01[Level_Height][Level_Width];

   ABall Ball;

   static const int Global_Scale = 3;
   static const int Brick_Width = 15, Brick_Height = 7;
   static const int Cell_Width = Brick_Width + 1, Cell_Height = Brick_Height + 1;
   static const int Level_X_Offset = 8, Level_Y_Offset = 6;                       // Coordinates of the beginning of the level
   static const int Circle_Size = 7;
   static const int Platform_Y_Pos = 185, Platform_Height = 7;
   static const int Level_X_Offset_Left = 8, Level_X_Offset_Right = Level_X_Offset_Left + Cell_Width * Level_Width;
   static const int Border_Top_Offset = 1, Border_Left_Offset = 2, Border_Right_Offset = 201, Border_Bottom_Offset = 99;
   static const int Border_X_Offset = 6, Border_Y_Offset = 4;
   static const int Max_X_Pos = 201;
};
//------------------------------------------------------------------------------------------------------------
