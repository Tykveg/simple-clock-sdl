#include "SymbolDIY.hpp"
#include <stdexcept>
#include <iostream>

/*
   0
   __
5 |  | 1
 6 __
4 |  | 2
   __
   3
*/

const char TSymbolDIY::l_ch[][7] = {
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 1, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}, // 9
  {1, 1, 1, 1, 1, 1, 1}, // : // swap bgc and char_color
  {0, 0, 0, 0, 0, 0, 0}, // [space]
};

void TSymbolDIY::CalcLineWidth() noexcept
{
  line_width = char_size / 5 + 1;
}

void TSymbolDIY::CalcRectsPosAndSize() noexcept
{
  lines[0] = SDL_Rect{ x + line_width, y, char_size - line_width - line_width, line_width };
  lines[1] = SDL_Rect{ x + char_size - line_width, y, line_width, char_size };
  lines[2] = SDL_Rect{ lines[1].x, lines[1].y + line_width + char_size, line_width, char_size};
  lines[3] = SDL_Rect{ lines[0].x, lines[2].y + char_size - line_width, lines[0].w, line_width};
  lines[5] = SDL_Rect{ x, lines[1].y, line_width, char_size};
  lines[4] = SDL_Rect{ x, lines[2].y, line_width, char_size };
  lines[6] = SDL_Rect{ lines[0].x, lines[5].y + char_size, lines[0].w, line_width};

  bg = SDL_Rect{ x, y, char_size, line_width + lines[1].h + lines[2].h};
}

void TSymbolDIY::CalcLinesCorrectColorArray() noexcept
{
  if (c >= '0' && c <= ':') // ':' == 58 , '9' == 57 (lucky lol)
  {
    l_correct = l_ch[c - '0'];
  }
  else if (c == ' ')
  {
    l_correct = l_ch[11];
  }
  else
  {
    //throw std::runtime_error(std::string("char ") + c + " not found.");
    std::cout << "char " << c << " not found. Replacing with '.'.\n";
    l_correct = l_ch[11];
  }
}

void TSymbolDIY::SetChar(const char& _c)
{
  if (c != _c)
  {
    changed = true;
    c = _c;
    CalcLinesCorrectColorArray();
  }
}

char TSymbolDIY::GetChar(void) const noexcept
{
  return c;
}

void TSymbolDIY::SetCharSize(const int& _char_size)
{
  if (_char_size < 0)
  {
    throw std::runtime_error("char_size must be >= 0.");
  }
  if (char_size != _char_size)
  {
    changed = true;
    char_size = _char_size;
    CalcLineWidth();
    CalcRectsPosAndSize();
  }
}

int TSymbolDIY::GetCharSize(void) const noexcept
{
  return char_size;
}

void TSymbolDIY::SetPos(const int& _x, const int& _y)
{
  if (_x < 0)
  {
    throw std::runtime_error("x must be >= 0.");
  }
  if (_y < 0)
  {
    throw std::runtime_error("y must be >= 0.");
  }
  if (_x != x || _y != y)
  {
    changed = true;
    x = _x;
    y = _y;
    CalcRectsPosAndSize();
  }
}

int TSymbolDIY::GetPosX(void) const noexcept
{
  return x;
}

int TSymbolDIY::GetPosY(void) const noexcept
{
  return y;
}

void TSymbolDIY::SetColorBGC(const SDL_Color& _bgc, const SDL_Surface* surf)
{
  // SetColorBGC(SDL_MapRGBA(surf->format, NULL, _bgc.r, _bgc.g, _bgc.b, _bgc.a)); // API change
  SetColorBGC(SDL_MapRGBA(SDL_GetPixelFormatDetails(surf->format), NULL, _bgc.r, _bgc.g, _bgc.b, _bgc.a));
}

void TSymbolDIY::SetColorCharC(const SDL_Color& _char_color, const SDL_Surface* surf)
{
  // SetColorCharC(SDL_MapRGBA(surf->format, NULL, _char_color.r, _char_color.g, _char_color.b, _char_color.a)); // API change
  SetColorCharC(SDL_MapRGBA(SDL_GetPixelFormatDetails(surf->format), NULL, _char_color.r, _char_color.g, _char_color.b, _char_color.a));
}

void TSymbolDIY::SetColorBGC(const Uint32& _bgc) noexcept
{
  changed = true;
  bgc = _bgc;
}

void TSymbolDIY::SetColorCharC(const Uint32& _char_color) noexcept
{
  changed = true;
  char_color = _char_color;
}

Uint32 TSymbolDIY::GetColorBGC(void) const noexcept
{
  return bgc;
}

Uint32 TSymbolDIY::GetColorCharC(void) const noexcept
{
  return char_color;
}

void TSymbolDIY::Draw(SDL_Surface* surf) const
{
  Uint32 bg_color, lines_color;
  if (c != ':')
  {
    bg_color = bgc;
    lines_color = char_color;
  }
  else // ':' should has reversed colors
  {
    bg_color = char_color;
    lines_color = bgc;
  }
  if (!SDL_FillSurfaceRect(surf, &bg, bg_color))
    throw std::runtime_error(SDL_GetError());
  for (size_t i = 0; i < 7; i++)
  {
    if (!SDL_FillSurfaceRect(surf, &(lines[i]), l_correct[i] ? lines_color : bg_color))
      throw std::runtime_error(SDL_GetError());
  }
}

void TSymbolDIY::DrawIfChanged(SDL_Surface* surf)
{
  if (changed)
  {
    Draw(surf);
    changed = false;
  }
}

SDL_Rect TSymbolDIY::GetBgRect(void) const noexcept
{
  return bg;
}
