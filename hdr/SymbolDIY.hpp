#pragma once

#include <SDL3/SDL.h>

/*
   0
   __
5 |  | 1
 6 __
4 |  | 2
   __
   3
*/

class TSymbolDIY
{
private:
  static const char l_ch[][7];
private:
  SDL_Rect bg;
  SDL_Rect lines[7];
  const char* l_correct = nullptr;
  int line_width = 1;
  void CalcLineWidth() noexcept;
  void CalcRectsPosAndSize() noexcept;
  void CalcLinesCorrectColorArray() noexcept;
private:
  bool changed = false;
  char c = ' ';
  int char_size = 1;
  int x = 0, y = 0;
  Uint32 bgc = 0;
  Uint32 char_color = 0;
public:
  void SetChar(const char& _c);
  char GetChar(void) const noexcept;
  void SetCharSize(const int& _char_size);
  int GetCharSize(void) const noexcept;
  void SetPos(const int& _x, const int& _y);
  int GetPosX(void) const noexcept;
  int GetPosY(void) const noexcept;
  void SetColorBGC(const SDL_Color& _bgc, const SDL_Surface* surf);
  void SetColorCharC(const SDL_Color& _char_color, const SDL_Surface* surf);
  void SetColorBGC(const Uint32& _bgc) noexcept;
  void SetColorCharC(const Uint32& _char_color) noexcept;
  Uint32 GetColorBGC(void) const noexcept;
  Uint32 GetColorCharC(void) const noexcept;
  void Draw(SDL_Surface* surf) const;
  void DrawIfChanged(SDL_Surface* surf);
  SDL_Rect GetBgRect(void) const noexcept;
};
