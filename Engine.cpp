#include "Engine.h"

#define _USE_MATH_DEFINES
#include <math.h>

enum eBrick_Type
{
   EBT_None,
   EBT_Red,
   EBT_Blue
};

HPEN Highlight_Pen, Brick_Red_Pen, Brick_Blue_Pen, Platform_Circle_Pen, Platform_Inner_Pen;
HBRUSH Brick_Red_Brush, Brick_Blue_Brush, Platform_Circle_Brush, Platform_Inner_Brush;

const int Global_Scale = 3;
const int Brick_Width = 15;
const int Brick_Height = 7;
const int Cell_Width = Brick_Width + 1;
const int Cell_Height = Brick_Height + 1;
const int Level_X_Offset = 8;
const int Level_Y_Offset= 6;
const int Circle_Size = 7;

int Inner_Width = 21;

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

void Init()
{//Настройка игры при старте
   
   Highlight_Pen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));

   Create_Pen_Brush(255, 79, 79, Brick_Red_Pen, Brick_Red_Brush);
   Create_Pen_Brush(85, 255, 255, Brick_Blue_Pen, Brick_Blue_Brush);
   Create_Pen_Brush(159, 0, 0, Platform_Circle_Pen, Platform_Circle_Brush);
   Create_Pen_Brush(0, 127, 191, Platform_Inner_Pen, Platform_Inner_Brush);
}

//------------------------------------------------------------------------------------------------------------

void Draw_Brick(HDC hdc, eBrick_Type brick_type, int x, int y)
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

void Draw_Brick_Letter(HDC hdc, int rotation_step, int x, int y)
{// Вывод падающей буквы

   double offset;
   double rotation_angle = 2.0 * M_PI * (double)rotation_step / 16.0;   // Преобразование шага в угол поворота
   int brick_half_height = Brick_Height * Global_Scale / 2;
   int back_part_offset;
   int npi = rotation_angle / M_PI;                                     // Целая часть от деления на Пи
   XFORM xform, old_xform;

   SetGraphicsMode(hdc, GM_ADVANCED);

   xform.eM11 = 1.0f;                                                   // Растягивает/сужает и переворачивает изображение по Х-координате
   xform.eM12 = 0.0f;                                                   // Поворачивают по/против часовой стрелки и не только
   xform.eM21 = 0.0f;                                                   // Поворачивают по/против часовой стрелки и не только
   xform.eM22 = (float)cos(rotation_angle - npi * M_PI);                // Растягивает/сужает и переворачивает изображение по Y-координате
   xform.eDx = (float)x;                                                // Перемещение по Х-координате
   xform.eDy = (float)y + (float)brick_half_height;                     // Премещение по Y-координате
   
   if (fabs(xform.eM22) < 0.1)                                          // Переходное изображение положения переворачивающегося кирпича 
   {
      SelectObject(hdc, Brick_Red_Pen);
      SelectObject(hdc, Brick_Red_Brush);

      Rectangle(hdc, 0 + (int)xform.eDx, 0 + (int)xform.eDy, Brick_Width * Global_Scale + (int)xform.eDx, -Global_Scale + (int)xform.eDy);      // Используем значения xform.Edx, вместо SetWorldTransform, преобразовывающего ещё и форму кирпича 

      SelectObject(hdc, Brick_Blue_Pen);
      SelectObject(hdc, Brick_Blue_Brush);

      Rectangle(hdc, 0 + (int)xform.eDx, 0 + (int)xform.eDy, Brick_Width * Global_Scale + (int)xform.eDx, Global_Scale + (int)xform.eDy - 1);   // Используем значения xform.Edx, вместо SetWorldTransform, преобразовывающего ещё и форму кирпича 
   }
   else
   {
      GetWorldTransform(hdc, &old_xform);                               // Записывает текущую трансформацию мира в old_xform
      SetWorldTransform(hdc, &xform);                                   // Преобразует мир согласно xform

      SelectObject(hdc, Brick_Red_Pen);
      SelectObject(hdc, Brick_Red_Brush);

      offset = 3.0f * (1.0f - fabs(xform.eM22)) * (double)Global_Scale; // Отвечает за одновременный показ лицевой и обратной стороны кирпича
      back_part_offset = (int)round(offset);                            // Отвечает за одновременный показ лицевой и обратной стороны кирпича

      Rectangle(hdc, 0, -brick_half_height - back_part_offset, Brick_Width * Global_Scale, brick_half_height - back_part_offset);

      SelectObject(hdc, Brick_Blue_Pen);
      SelectObject(hdc, Brick_Blue_Brush);

      Rectangle(hdc, 0, -brick_half_height, Brick_Width * Global_Scale, brick_half_height);
   
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
         Draw_Brick(hdc, (eBrick_Type)Level_01[i][j], Level_X_Offset + j * Cell_Width, Level_Y_Offset+ i * Cell_Height);
      }
}

//------------------------------------------------------------------------------------------------------------

void Draw_Platform(HDC hdc, int x, int y)
{ // Отрисовка платформы

   // 1. Рисуем боковые шарики
   SelectObject(hdc, Platform_Circle_Pen);
   SelectObject(hdc, Platform_Circle_Brush);

   Ellipse(hdc, x * Global_Scale, y * Global_Scale, (x + Circle_Size) * Global_Scale, (y + Circle_Size) * Global_Scale);
   Ellipse(hdc, (x + Inner_Width) * Global_Scale, y * Global_Scale, (x + Inner_Width + Circle_Size) * Global_Scale, (y + Circle_Size) * Global_Scale);
 
   // 2. Рисуем блик
   SelectObject(hdc, Highlight_Pen);
   Arc(hdc, (x + 1) * Global_Scale, (y + 1) * Global_Scale, (x + Circle_Size - 1) * Global_Scale, (y + Circle_Size - 1) * Global_Scale,
      (x + 1 + 1) * Global_Scale, (y + 1) * Global_Scale, (x + 1) * Global_Scale, (y + 1 + 2) * Global_Scale);

   // 3. Рисуем платформу
   SelectObject(hdc, Platform_Inner_Pen);
   SelectObject(hdc, Platform_Inner_Brush);

   RoundRect(hdc, (x + 4) * Global_Scale, (y + 1) * Global_Scale, (x + 3 + Inner_Width) * Global_Scale, (y + Circle_Size - 1) * Global_Scale, Global_Scale * 3, Global_Scale * 3);
}

//------------------------------------------------------------------------------------------------------------

void Draw_Frame(HDC hdc)
{ // Отрисовка экрана игры

   //Draw_Level(hdc);

   //Draw_Platform(hdc, 50, 100);

   for (int i = 0; i < 16; i ++)
      Draw_Brick_Letter(hdc, i, 20 + i * Cell_Width * Global_Scale, 100);

}

//------------------------------------------------------------------------------------------------------------
