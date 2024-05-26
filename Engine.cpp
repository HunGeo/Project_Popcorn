#include "Engine.h"

#define _USE_MATH_DEFINES
#include <math.h>

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

HWND Hwnd;
HPEN BG_Pen, Highlight_Pen, Letter_Pen, Brick_Red_Pen, Brick_Blue_Pen, Platform_Circle_Pen, Platform_Inner_Pen;
HBRUSH BG_Brush, Brick_Red_Brush, Brick_Blue_Brush, Platform_Circle_Brush, Platform_Inner_Brush;

const int Global_Scale = 3;
const int Brick_Width = 15;
const int Brick_Height = 7;
const int Cell_Width = Brick_Width + 1;
const int Cell_Height = Brick_Height + 1;
const int Level_X_Offset = 8;
const int Level_Y_Offset= 6;
const int Circle_Size = 7;
const int Platform_Y_Pos = 185;
const int Platform_Height = 7;

int Inner_Width = 21;
int Platform_X_Pos = 0;
int Platform_X_Step = Global_Scale;
int Platform_Width = 28;

RECT Platform_Rect, Prev_Platform_Rect;

char Level_01[14][12] =
{
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
//------------------------------------------------------------------------------------------------------------
void Create_Pen_Brush(unsigned char r, unsigned char g, unsigned char b, HPEN &pen, HBRUSH &brush)
{
   pen = CreatePen(PS_SOLID, 0, RGB(r, g, b));
   brush = CreateSolidBrush(RGB(r, g, b));
}
//------------------------------------------------------------------------------------------------------------
void Redraw_Platform()
{

   Prev_Platform_Rect = Platform_Rect;

   Platform_Rect.left = Platform_X_Pos * Global_Scale;
   Platform_Rect.top = Platform_Y_Pos * Global_Scale;
   Platform_Rect.right = Platform_Rect.left + Platform_Width * Global_Scale;
   Platform_Rect.bottom = Platform_Rect.top + Platform_Height * Global_Scale;

   InvalidateRect(Hwnd, &Prev_Platform_Rect, FALSE);
   InvalidateRect(Hwnd, &Platform_Rect, FALSE);
}
//------------------------------------------------------------------------------------------------------------
void Init_Engine(HWND hwnd)
{//Настройка игры при старте
   
   Hwnd = hwnd;

   Highlight_Pen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
   Letter_Pen = CreatePen(PS_SOLID, Global_Scale, RGB(255, 255, 255));

   Create_Pen_Brush(15, 63, 31, BG_Pen, BG_Brush);
   Create_Pen_Brush(255, 79, 79, Brick_Red_Pen, Brick_Red_Brush);
   Create_Pen_Brush(85, 255, 255, Brick_Blue_Pen, Brick_Blue_Brush);
   Create_Pen_Brush(159, 0, 0, Platform_Circle_Pen, Platform_Circle_Brush);
   Create_Pen_Brush(0, 127, 191, Platform_Inner_Pen, Platform_Inner_Brush);

   Redraw_Platform();
}
//------------------------------------------------------------------------------------------------------------
void Draw_Brick(HDC hdc, EBrick_Type brick_type, int x, int y)
{//Вывод керпича
   HPEN pen;
   HBRUSH brush;

   switch (brick_type)
   {
   case EBT_None:
      return;

   case EBT_Red:
      pen = Brick_Red_Pen;
      brush = Brick_Red_Brush;
      break;

   case EBT_Blue:
      pen = Brick_Blue_Pen;
      brush = Brick_Blue_Brush;
      break;

   default:
      return;

   }

   SelectObject(hdc, pen);
   SelectObject(hdc, brush);

   RoundRect(hdc, x * Global_Scale, y * Global_Scale, (x + Brick_Width) * Global_Scale, (y + Brick_Height) * Global_Scale, Global_Scale * 2, Global_Scale * 2);

}
//------------------------------------------------------------------------------------------------------------
void Set_Brick_Letter_Colors(bool is_switch_color, EBrick_Type brick_type, HPEN &front_pen, HBRUSH &front_brush, HPEN& back_pen, HBRUSH& back_brush)
{//
   if (is_switch_color)
   {
      front_pen = Brick_Red_Pen;
      front_brush = Brick_Red_Brush;

      back_pen = Brick_Blue_Pen;
      back_brush = Brick_Blue_Brush;
   }
   else
   {
      front_pen = Brick_Blue_Pen;
      front_brush = Brick_Blue_Brush;

      back_pen = Brick_Red_Pen;
      back_brush = Brick_Red_Brush;
   }
}
//------------------------------------------------------------------------------------------------------------
void Draw_Brick_Letter(HDC hdc, int rotation_step, EBrick_Type brick_type, ELetter_Type letter_type, int x, int y)
{// Вывод падающей буквы

   bool switch_color;
   double offset;
   double rotation_angle;   // Преобразование шага в угол поворота
   int brick_half_height = Brick_Height * Global_Scale / 2;
   int back_part_offset;
//   int npi = rotation_angle / M_PI;                                     // Целая часть от деления на Пи
   HPEN front_pen, back_pen;
   HBRUSH front_brush, back_brush;
   XFORM xform, old_xform;

   SetGraphicsMode(hdc, GM_ADVANCED);

   if (sin(2.0 * M_PI * (double)rotation_step / 16.0) > -0.1)
   {
      rotation_angle = 2.0 * M_PI * (double)rotation_step / 16.0;
   }
   else
   {
      rotation_angle = 2.0 * M_PI * (double)(8 - rotation_step) / 16.0;
   }

   // Настраиваем матрицу "переворота"
   xform.eM11 = 1.0f;                                                   // Растягивает/сужает и переворачивает изображение по Х-координате
   xform.eM12 = 0.0f;                                                   // Поворачивают по/против часовой стрелки и не только
   xform.eM21 = 0.0f;                                                   // Поворачивают по/против часовой стрелки и не только
   xform.eM22 = (float)cos(rotation_angle /* - npi * M_PI*/);                // Растягивает/сужает и переворачивает изображение по Y-координате
   xform.eDx = (float)x;                                                // Перемещение по Х-координате
   xform.eDy = (float)y + (float)brick_half_height;                     // Премещение по Y-координате
   
   if (!(brick_type == EBT_Blue || brick_type == EBT_Red))
      return;  // Падающие буквы могут быть только от кирпичей такого типа

   if ((cos(2.0 * M_PI * (double)rotation_step / 16.0) < -0.1) || sin(2.0 * M_PI * (double)rotation_step / 16.0) < -0.9)   // Значиние косинуса для диапазона от Пи/2 до 3Пи/2, от сплюснутого до развёрнутого
      switch_color = brick_type == EBT_Blue;
   else
      switch_color = brick_type == EBT_Red;

   Set_Brick_Letter_Colors(switch_color, brick_type, front_pen, front_brush, back_pen, back_brush);

   if (fabs(xform.eM22) < 0.1)   // Значиние косинуса для Пи/2 и 3Пи/2, переходное изображение положения переворачивающегося кирпича, 
   {  
      // Выводим обратную сторону падающего переворачивающегося кирпича
      SelectObject(hdc, back_pen);
      SelectObject(hdc, back_brush);

      Rectangle(hdc, 0 + (int)xform.eDx, 0 + (int)xform.eDy, Brick_Width * Global_Scale + (int)xform.eDx, -Global_Scale + (int)xform.eDy);      // Используем значения xform.Edx, вместо SetWorldTransform, преобразовывающего ещё и форму кирпича 
      
      // Выводим лицевую сторону падающего переворачивающегося кирпича
      SelectObject(hdc, front_pen);
      SelectObject(hdc, front_brush);

      Rectangle(hdc, 0 + (int)xform.eDx, 0 + (int)xform.eDy, Brick_Width * Global_Scale + (int)xform.eDx, Global_Scale + (int)xform.eDy - 1);   // Используем значения xform.Edx, вместо SetWorldTransform, преобразовывающего ещё и форму кирпича 
   }
   else                                                                 // Описывает все значения косинусов
   {
      GetWorldTransform(hdc, &old_xform);                               // Записывает текущую трансформацию мира в old_xform
      SetWorldTransform(hdc, &xform);                                   // Преобразует мир согласно xform
      
      // Выводим обратную сторону падающего переворачивающегося кирпича
      SelectObject(hdc, back_pen);
      SelectObject(hdc, back_brush);

      offset = 3.0f * (1.0f - fabs(xform.eM22)) * (double)Global_Scale; // Отвечает за одновременный показ лицевой и обратной стороны кирпича
      back_part_offset = (int)round(offset);                            // Отвечает за одновременный показ лицевой и обратной стороны кирпича

      Rectangle(hdc, 0, -brick_half_height - back_part_offset, Brick_Width * Global_Scale, brick_half_height - back_part_offset);
      
      // Выводим лицевую сторону падающего переворачивающегося кирпича
      SelectObject(hdc, front_pen);
      SelectObject(hdc, front_brush);

      Rectangle(hdc, 0, -brick_half_height, Brick_Width * Global_Scale, brick_half_height);

      if (cos(2.0 * M_PI * (double)rotation_step / 16.0) < -0.1)
      {
         if (letter_type == ELT_O)
         {
            SelectObject(hdc, Letter_Pen);

            Ellipse(hdc, (0 + 5)* Global_Scale, -brick_half_height + 1 * Global_Scale, (Brick_Width - 5)* Global_Scale, brick_half_height - 1 * Global_Scale);
         }
      }

      SetWorldTransform(hdc, &old_xform);                               // Преобразует мир согласно old_xform
   }
}
//------------------------------------------------------------------------------------------------------------
void Draw_Level(HDC hdc)
{// Отрисовка всех кирпичей уровня
   int i;
   int j;

   for (i = 0; i < 14; i++)
      for (j = 0; j < 12; j++)
      {
         Draw_Brick(hdc, (EBrick_Type)Level_01[i][j], Level_X_Offset + j * Cell_Width, Level_Y_Offset+ i * Cell_Height);
      }
}
//------------------------------------------------------------------------------------------------------------
void Draw_Platform(HDC hdc, int x, int y)
{ // Отрисовка платформы

   // Draw background
   SelectObject(hdc, BG_Pen);
   SelectObject(hdc, BG_Brush);

   Rectangle(hdc, Prev_Platform_Rect.left, Prev_Platform_Rect.top, Prev_Platform_Rect.right * Global_Scale, Prev_Platform_Rect.bottom * Global_Scale);

   // Draw balls
   SelectObject(hdc, Platform_Circle_Pen);
   SelectObject(hdc, Platform_Circle_Brush);

   Ellipse(hdc, x * Global_Scale, y * Global_Scale, (x + Circle_Size) * Global_Scale, (y + Circle_Size) * Global_Scale);
   Ellipse(hdc, (x + Inner_Width) * Global_Scale, y * Global_Scale, (x + Inner_Width + Circle_Size) * Global_Scale, (y + Circle_Size) * Global_Scale);
 
   // Draw highlights
   SelectObject(hdc, Highlight_Pen);
   Arc(hdc, (x + 1) * Global_Scale, (y + 1) * Global_Scale, (x + Circle_Size - 1) * Global_Scale, (y + Circle_Size - 1) * Global_Scale,
      (x + 1 + 1) * Global_Scale, (y + 1) * Global_Scale, (x + 1) * Global_Scale, (y + 1 + 2) * Global_Scale);

   // Рисуем платформу
   SelectObject(hdc, Platform_Inner_Pen);
   SelectObject(hdc, Platform_Inner_Brush);

   RoundRect(hdc, (x + 4) * Global_Scale, (y + 1) * Global_Scale, (x + 3 + Inner_Width) * Global_Scale, (y + Circle_Size - 1) * Global_Scale, Global_Scale * 3, Global_Scale * 3);
}
//------------------------------------------------------------------------------------------------------------
void Draw_Frame(HDC hdc)
{ // Отрисовка экрана игры

   //Draw_Level(hdc);

   Draw_Platform(hdc, Platform_X_Pos, Platform_Y_Pos);

   //for (int i = 0; i < 16; i++)
   //{
   //   Draw_Brick_Letter(hdc, i, EBT_Blue, ELT_O, 20 + i * Cell_Width * Global_Scale, 100);
   //   Draw_Brick_Letter(hdc, i, EBT_Red, ELT_O, 20 + i * Cell_Width * Global_Scale, 130);
   //}
}
//------------------------------------------------------------------------------------------------------------
int On_Key_Down(EKey_Type key_type)
{
   switch (key_type)
   {
   case EKT_Left:
      Platform_X_Pos -= Platform_X_Step;
      Redraw_Platform();
      break;

   case EKT_Right:
      Platform_X_Pos += Platform_X_Step;
      Redraw_Platform();
      break;
   
   case EKT_Space:
      break;
   }
   return 0;
}
//------------------------------------------------------------------------------------------------------------