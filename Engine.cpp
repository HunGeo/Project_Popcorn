#include "Engine.h"

char Level_01[AsEngine::Level_Height][AsEngine::Level_Width] =
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
AsEngine::AsEngine()
: Inner_Width(21), Platform_X_Pos(Level_X_Offset_Left), Platform_X_Step(Global_Scale), Platform_Width(28), Timer_Frequency(20)/*Frame refresh time in msec*/,
  Ball_X_Pos(20.0), Ball_Y_Pos(170.0), Ball_Speed(1.0), Ball_Direction(M_PI_4),
  Cos_Ball_Direction(cos(Ball_Direction)), Sin_Ball_Direction(sin(Ball_Direction))
{
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Init_Engine(HWND hwnd)
{//Setting up the game at startup
   Hwnd = hwnd;

   Highlight_Pen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
   Letter_Pen = CreatePen(PS_SOLID, Global_Scale, RGB(255, 255, 255));

   Create_Pen_Brush(15, 63, 31, BG_Pen, BG_Brush);
   Create_Pen_Brush(255, 79, 79, Red_Pen, Red_Brush);
   Create_Pen_Brush(85, 255, 255, Blue_Pen, Blue_Brush);
   Create_Pen_Brush(159, 0, 0, Platform_Circle_Pen, Platform_Circle_Brush);
   Create_Pen_Brush(0, 127, 191, Platform_Inner_Pen, Platform_Inner_Brush);
   Create_Pen_Brush(255, 255, 255, White_Pen, White_Brush);

   Level_Rect.left = Level_X_Offset * Global_Scale;
   Level_Rect.top = Level_Y_Offset * Global_Scale;
   Level_Rect.right = (Level_X_Offset + Cell_Width * Level_Width) * Global_Scale;
   Level_Rect.bottom = (Level_Y_Offset + Cell_Height * Level_Height) * Global_Scale;

   Redraw_Platform();

   Generate_Bricks_Level_Rects();

   SetTimer(Hwnd, Timer_ID, Timer_Frequency, 0);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Frame(HDC hdc, RECT& paint_area)
{ // Draw the game window

   RECT intersection_rect;
   if (IntersectRect(&intersection_rect, &paint_area, &Level_Rect))
      Draw_Level(hdc);

   if (IntersectRect(&intersection_rect, &paint_area, &Platform_Rect))
      Draw_Platform(hdc, Platform_X_Pos, Platform_Y_Pos);

   //for (int i = 0; i < 16; i++)
   //{
   //   Draw_Brick_Letter(hdc, i, EBT_Blue, ELT_O, 20 + i * Cell_Width * Global_Scale, 100);
   //   Draw_Brick_Letter(hdc, i, EBT_Red, ELT_O, 20 + i * Cell_Width * Global_Scale, 130);
   //}
   if (IntersectRect(&intersection_rect, &paint_area, &Ball_Rect))
      Draw_Ball(hdc, paint_area);

   Draw_Bounds(hdc, paint_area);

}
//------------------------------------------------------------------------------------------------------------
int AsEngine::On_Key_Down(EKey_Type key_type)
{//
   switch (key_type)
   {
   case EKT_Left:
      Platform_X_Pos -= Platform_X_Step;

      if (Platform_X_Pos < Border_X_Offset)
         Platform_X_Pos = Border_X_Offset;

      Redraw_Platform();
      break;

   case EKT_Right:
      Platform_X_Pos += Platform_X_Step;

      if (Platform_X_Pos > Max_X_Pos - Platform_Width)
         Platform_X_Pos = Max_X_Pos - Platform_Width;

      Redraw_Platform();
      break;

   case EKT_Space:
      break;
   }
   return 0;
}
//------------------------------------------------------------------------------------------------------------
int AsEngine::On_Timer()
{//
   Move_Ball();

   return 0;
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Create_Pen_Brush(unsigned char r, unsigned char g, unsigned char b, HPEN &pen, HBRUSH &brush)
{//
   pen = CreatePen(PS_SOLID, 0, RGB(r, g, b));
   brush = CreateSolidBrush(RGB(r, g, b));
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Redraw_Platform()
{//
   Prev_Platform_Rect = Platform_Rect;

   Platform_Rect.left = Platform_X_Pos * Global_Scale;
   Platform_Rect.top = Platform_Y_Pos * Global_Scale;
   Platform_Rect.right = Platform_Rect.left + Platform_Width * Global_Scale;
   Platform_Rect.bottom = Platform_Rect.top + Platform_Height * Global_Scale;

   InvalidateRect(Hwnd, &Prev_Platform_Rect, FALSE);
   InvalidateRect(Hwnd, &Platform_Rect, FALSE);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Generate_Bricks_Level_Rects()
{
   for (int j = 0; j < Level_Width; j++)
   {
      for (int i = 0; i < Level_Height; i++)
      {
         Brick_Rect_01[i][j].left = (Level_X_Offset_Left + j * Cell_Width) * Global_Scale;
         Brick_Rect_01[i][j].top = (Level_Y_Offset + i * Cell_Height) * Global_Scale;
         Brick_Rect_01[i][j].right = Brick_Rect_01[i][j].left + Cell_Width * Global_Scale;
         Brick_Rect_01[i][j].bottom = Brick_Rect_01[i][j].top + Cell_Height * Global_Scale;
      }
   }
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Brick(HDC hdc, EBrick_Type brick_type, int x, int y)
{//Brick output
   HPEN pen;
   HBRUSH brush;

   switch (brick_type)
   {
   case EBT_None:
      pen = BG_Pen;
      brush = BG_Brush;
      break;

   case EBT_Red:
      pen = Red_Pen;
      brush = Red_Brush;
      break;

   case EBT_Blue:
      pen = Blue_Pen;
      brush = Blue_Brush;
      break;

   default:
      return;
   }

   SelectObject(hdc, pen);
   SelectObject(hdc, brush);

   RoundRect(hdc, x * Global_Scale, y * Global_Scale, (x + Brick_Width) * Global_Scale, (y + Brick_Height) * Global_Scale, Global_Scale * 2, Global_Scale * 2);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Set_Brick_Letter_Colors(bool is_switch_color, EBrick_Type brick_type, HPEN &front_pen, HBRUSH &front_brush, HPEN& back_pen, HBRUSH& back_brush)
{//
   if (is_switch_color)
   {
      front_pen = Red_Pen;
      front_brush = Red_Brush;

      back_pen = Blue_Pen;
      back_brush = Blue_Brush;
   }
   else
   {
      front_pen = Blue_Pen;
      front_brush = Blue_Brush;

      back_pen = Red_Pen;
      back_brush = Red_Brush;
   }
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Brick_Letter(HDC hdc, int rotation_step, EBrick_Type brick_type, ELetter_Type letter_type, int x, int y)
{// A falling and inverted letter

   bool switch_color;
   double offset;
   double rotation_angle;                                                // Преобразование шага в угол поворота
   int brick_half_height = Brick_Height * Global_Scale / 2;
   int back_part_offset;
// int npi = rotation_angle / M_PI;                                      // Целая часть от деления на Пи
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
   xform.eM22 = (float)cos(rotation_angle /* - npi * M_PI*/);           // Растягивает/сужает и переворачивает изображение по Y-координате
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
void AsEngine::Draw_Border(HDC hdc, int x, int y, bool top_border)
{// Section of the border
   

   // Base
   SelectObject(hdc, Blue_Pen);
   SelectObject(hdc, Blue_Brush);

   if (top_border)
      Rectangle(hdc, x * Global_Scale, (y + 1) * Global_Scale, (x + 4) * Global_Scale, (y + 4) * Global_Scale);
   else
      Rectangle(hdc, (x + 1) * Global_Scale, y * Global_Scale, (x + 4) * Global_Scale, (y + 4) * Global_Scale);

   // White side
   SelectObject(hdc, White_Pen);
   SelectObject(hdc, White_Brush);

   if (top_border)
      Rectangle(hdc, x * Global_Scale, y * Global_Scale, (x + 4) * Global_Scale, (y + 1) * Global_Scale);
   else
      Rectangle(hdc, x * Global_Scale, y * Global_Scale, (x + 1) * Global_Scale, (y + 4) * Global_Scale);
   
   // Dot
   SelectObject(hdc, BG_Pen);
   SelectObject(hdc, BG_Brush);

   if (top_border)
      Rectangle(hdc, (x + 2) * Global_Scale, (y + 2) * Global_Scale, (x + 3) * Global_Scale, (y + 3) * Global_Scale);
   else
      Rectangle(hdc, (x + 2) * Global_Scale, (y + 1) * Global_Scale, (x + 3) * Global_Scale, (y + 2) * Global_Scale);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Level(HDC hdc)
{// Отрисовка всех кирпичей уровня
   int i;
   int j;

   for (i = 0; i < Level_Height; i++)
      for (j = 0; j < Level_Width; j++)
      {
         Draw_Brick(hdc, (EBrick_Type)Level_01[i][j], Level_X_Offset + j * Cell_Width, Level_Y_Offset + i * Cell_Height);
      }
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Platform(HDC hdc, int x, int y)
{//
   // Draw background
   SelectObject(hdc, BG_Pen);
   SelectObject(hdc, BG_Brush);

   Rectangle(hdc, Prev_Platform_Rect.left, Prev_Platform_Rect.top, Prev_Platform_Rect.right, Prev_Platform_Rect.bottom);

   // Draw balls on the sides
   SelectObject(hdc, Platform_Circle_Pen);
   SelectObject(hdc, Platform_Circle_Brush);

   Ellipse(hdc, x * Global_Scale, y * Global_Scale, (x + Circle_Size) * Global_Scale, (y + Circle_Size) * Global_Scale);
   Ellipse(hdc, (x + Inner_Width) * Global_Scale, y * Global_Scale, (x + Inner_Width + Circle_Size) * Global_Scale, (y + Circle_Size) * Global_Scale);
 
   // Draw highlights
   SelectObject(hdc, Highlight_Pen);
   Arc(hdc, (x + 1) * Global_Scale, (y + 1) * Global_Scale, (x + Circle_Size - 1) * Global_Scale, (y + Circle_Size - 1) * Global_Scale,
      (x + 1 + 1) * Global_Scale, (y + 1) * Global_Scale, (x + 1) * Global_Scale, (y + 1 + 2) * Global_Scale);

   // Draw platform
   SelectObject(hdc, Platform_Inner_Pen);
   SelectObject(hdc, Platform_Inner_Brush);

   RoundRect(hdc, (x + 4) * Global_Scale, (y + 1) * Global_Scale, (x + 3 + Inner_Width) * Global_Scale, (y + Circle_Size - 1) * Global_Scale, Global_Scale * 3, Global_Scale * 3);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Ball(HDC hdc, RECT &paint_area)
{
   // Draw background
   SelectObject(hdc, BG_Pen);
   SelectObject(hdc, BG_Brush);

   Rectangle(hdc, Prev_Ball_Rect.left, Prev_Ball_Rect.top, Prev_Ball_Rect.right, Prev_Ball_Rect.bottom);

   // Draw ball
   SelectObject(hdc, White_Pen);
   SelectObject(hdc, White_Brush);

   Ellipse(hdc, Ball_Rect.left, Ball_Rect.top, Ball_Rect.right - 1, Ball_Rect.bottom - 1);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Bounds(HDC hdc, RECT& paint_area)
{// 
   int i;
   
   // Left
   for (i = 0; i < 50; i++)
      Draw_Border(hdc, Border_Left_Offset, Border_Top_Offset + i * 4, false);
   // Right
   for (i = 0; i < 50; i++)
      Draw_Border(hdc, Border_Right_Offset, Border_Top_Offset + i * 4, false);
   // Top
   for (i = 0; i < 50; i++)
      Draw_Border(hdc, Border_Left_Offset + i * 4, Border_Top_Offset, true);

}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Ball_Reflection_Wall()
{//
   if (Ball_X_Pos < Border_X_Offset)                                                             // From the left
   {
      Cos_Ball_Direction = -Cos_Ball_Direction;
      Ball_X_Pos = 2 * Border_X_Offset - Ball_X_Pos;
   }
   if (Ball_X_Pos > Level_X_Offset_Right - Ball_Size)                                            // From the right
   {
      Cos_Ball_Direction = -Cos_Ball_Direction;
      Ball_X_Pos = 2 * (Max_X_Pos - Ball_Size) - Ball_X_Pos;
   }
   if (Ball_Y_Pos < Border_Y_Offset)                                                             // From the top
   {
      Sin_Ball_Direction = -Sin_Ball_Direction;
      Ball_Y_Pos = 2 * Border_Y_Offset - Ball_Y_Pos;
   }
   if (Ball_Y_Pos > 205 - Ball_Size)                                                             // From the bottom
   {
      Sin_Ball_Direction = -Sin_Ball_Direction;
      Ball_Y_Pos = 200;
   }
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Ball_Reflection_Brick()
{//
   RECT interseption_rect;

   int i, j;

   if (IntersectRect(&interseption_rect, &Ball_Rect, &Level_Rect)) //Checking for intersection with brick
   {
      // identification brick[i][j] 
      j = ((Ball_Rect.right + Ball_Rect.left) / 2 + Ball_Size * Cos_Ball_Direction * Global_Scale / 2 - Level_X_Offset_Left * Global_Scale) / (16 * Global_Scale);   //  int number brick in j-coordinates
      i = ((Ball_Rect.top + Ball_Rect.bottom) / 2 - Ball_Size * Sin_Ball_Direction * Global_Scale / 2 - Level_Y_Offset * Global_Scale) / (8 * Global_Scale);         //  int number brick in i-coordinates

      if (j == 12)
         j = 11;
      if (j == -1)
         j = 0;

      if (i == -1)
         i = 0;

      // checking if brick[i][j] isn't NULL
      if (Level_01[i][j] != 0)
      {
          IntersectRect(&interseption_rect, &Ball_Rect, &Brick_Rect_01[i][j]);

         if (((interseption_rect.right - interseption_rect.left) > 5 && (interseption_rect.bottom - interseption_rect.top < 12)) &&
            ((Sin_Ball_Direction > 0 && (interseption_rect.bottom + interseption_rect.top) > Brick_Rect_01[i][j].bottom + Brick_Rect_01[i][j].top) ||
               (Sin_Ball_Direction < 0 && (interseption_rect.bottom + interseption_rect.top) < Brick_Rect_01[i][j].bottom + Brick_Rect_01[i][j].top))) // Описание условия для вертикального отскока от кирппича
         {
            Sin_Ball_Direction = -Sin_Ball_Direction;
            Ball_Y_Pos = Prev_Ball_Y_Pos;
         }
         else
         {
            Cos_Ball_Direction = -Cos_Ball_Direction;
            Ball_X_Pos = Prev_Ball_X_Pos;
         }

         Level_01[i][j] = 0;
         InvalidateRect(Hwnd, &Level_Rect, FALSE); // Deleting the brick we reflect from
      }
   }
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Ball_Reflection_Platform()
{
   RECT interseption_rect;

   if (IntersectRect(&interseption_rect, &Ball_Rect, &Platform_Rect) && Sin_Ball_Direction < 0)
   {
      Sin_Ball_Direction = -Sin_Ball_Direction;
      Ball_Y_Pos = 2 * (Platform_Rect.top / Global_Scale - Ball_Size) - Ball_Y_Pos;
   }
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Move_Ball()
{//
   Prev_Ball_Rect = Ball_Rect;

   Prev_Ball_X_Pos = Ball_X_Pos;
   Prev_Ball_Y_Pos = Ball_Y_Pos;

   Ball_X_Pos += Cos_Ball_Direction * Ball_Speed;
   Ball_Y_Pos -= Sin_Ball_Direction * Ball_Speed;

   Ball_Reflection_Wall();

   Ball_Rect.left = (int)round((Ball_X_Pos) * Global_Scale);
   Ball_Rect.top = (int)round((Ball_Y_Pos) * Global_Scale);
   Ball_Rect.right = (int)round(Ball_Rect.left + Ball_Size * Global_Scale);
   Ball_Rect.bottom = (int)round(Ball_Rect.top + Ball_Size * Global_Scale);

   InvalidateRect(Hwnd, &Prev_Ball_Rect, FALSE);
   InvalidateRect(Hwnd, &Ball_Rect, FALSE);

   Ball_Reflection_Brick();
   Ball_Reflection_Wall();
   Ball_Reflection_Platform();
}
