#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <ctime>
#include "SymbolDIY.hpp"

struct SParams
{
  size_t nums_size = 5; // 2 - hh ; 5 - hh:mm ; 8 - hh:mm:ss
  bool do_ampm = false;
  Uint32 delay = 50;
  SDL_Color background_color = SDL_Color{0, 0, 0};
  SDL_Color char_color = SDL_Color{255, 255, 255};
  int padx = 20;
  int pady = 20;
  int char_size = 50;
};

class TSimpleClock
{
private:
  const std::string title = "simple-clock-sdl";
  const std::string version = "v1.0";
  const std::string window_title = this->title + ' ' + this->version;
  SParams p;
  TSymbolDIY* nums = nullptr;
  bool pm = false;
  bool pm_prev = false;
  bool was_init = false;

  SDL_Window* window = nullptr;
  SDL_Surface* screen_surf = nullptr;

  bool running = true;
  bool time_changed = true;
  std::time_t time_prev = 0;
  std::time_t time_difference = 60; // seconds
  std::string date = "Www Mmm dd yyyy";
  std::time_t date_prev = 0;

  void SetTime(void);
  void CreateIcon(void);
public:
  void Init(const SParams& params);
  void Run(void);
  ~TSimpleClock();
};
