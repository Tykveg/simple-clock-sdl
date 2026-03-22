#include "SimpleClock.hpp"
#include <cstdlib>
#include <stdexcept>
#include <iostream>

void TSimpleClock::SetTime()
{
  std::time_t time_now = std::time(nullptr);
  time_changed = false;
  if (time_now - time_prev >= time_difference)
  {
    time_changed = true;
    // Www Mmm dd hh:mm:ss yyyy\n
    char* ctime_now = std::ctime(&time_now);
    bool not_same_date = false;
    size_t i;
    pm = false;
    if (ctime_now[11] == '1' && ctime_now[12] >= '2' || ctime_now[11] == '2')
    {
      if (p.do_ampm)
      {
        if (ctime_now[12] == '0' || ctime_now[12] == '1')
        {
          ctime_now[11] -= 1;
          ctime_now[12] += 10;
        }
        ctime_now[11] -= 1;
        ctime_now[12] -= 2;
      }
      pm = true;
    }
    for (i = 0; i < p.nums_size; i++)
      nums[i].SetChar(ctime_now[i + 11]);
    if (time_now - date_prev >= 86400) // 24*3600
    {
      if (pm && p.do_ampm)
        ctime_now = std::ctime(&time_now); // because I modified ctime string
      for (i = 0; i < 11; i++)
      {
        if (ctime_now[i] != date[i])
        {
          not_same_date = true;
          date[i] = ctime_now[i];
        }
      }
      for (i = 20; i < 25; i++)
      {
        if (ctime_now[i] != date[i-9])
        {
          not_same_date = true;
          date[i-9] = ctime_now[i];
        }
      }
      if (not_same_date || pm_prev != pm)
      {
        if (!SDL_SetWindowTitle(window, (window_title + (pm ? " PM " : " AM ") + date).c_str()))
        {
          throw std::runtime_error(SDL_GetError());
        }
        pm_prev = pm;
      }
      //date_prev = time_now - time_now % 86400; // doesn't work because for whatever reason it shows not 00:00 on some OSes
      date_prev = time_now - ((ctime_now[11] - '0') * 36000 + (ctime_now[12] - '0') * 3600 + (ctime_now[14] - '0') * 600 + (ctime_now[15] - '0') * 60);
    }
    time_prev = time_now - time_now % time_difference;
  }
}

void TSimpleClock::CreateIcon(void)
{
  const size_t sz = 32;
  Uint16 pixels[sz*sz];
  const Uint16 bgc = 0xf,  col = 0xffff;
  size_t i = 0;
  const size_t padding = sz/8;
  for (i = 0; i < sz*sz; i++)
  {
    pixels[i] = bgc;
  }
  for (i = 0; i < sz; i++)
  {
    pixels[i] = col;
    pixels[i+sz*(sz-1)] = col;
  }
  for (i = 0; i < sz; i++)
  {
    pixels[i*sz] = col;
    pixels[(i+1)*sz-1] = col;
  }

  pixels[padding+1+3*sz] = col;
  pixels[sz-padding-2+3*sz] = col;
  pixels[padding+1+(sz-padding-2)*sz] = col;
  pixels[sz-padding-2+(sz-padding-2)*sz] = col;

  pixels[sz/2-1+padding*sz] = col;
  pixels[sz/2+padding*sz] = col;
  pixels[sz/2-1+(padding+1)*sz] = col;
  pixels[sz/2+(padding+1)*sz] = col;

  pixels[sz/2-1+(sz-padding-1)*sz] = col;
  pixels[sz/2+(sz-padding-1)*sz] = col;
  pixels[sz/2-1+(sz-padding-2)*sz] = col;
  pixels[sz/2+(sz-padding-2)*sz] = col;

  pixels[padding+(sz/2-1)*sz] = col;
  pixels[padding+1+(sz/2-1)*sz] = col;
  pixels[padding+(sz/2)*sz] = col;
  pixels[padding+1+(sz/2)*sz] = col;

  pixels[sz-padding-1+(sz/2-1)*sz] = col;
  pixels[sz-padding-2+(sz/2-1)*sz] = col;
  pixels[sz-padding-1+(sz/2)*sz] = col;
  pixels[sz-padding-2+(sz/2)*sz] = col;

  i = (sz/2-1)+(sz/2-1)*sz;
  for (size_t j = 0; j < 2+sz/4-padding; j++)
  {
    pixels[i] = col;
    pixels[i+sz] = col;
    i++;
  }
  i = (sz/2-1)+(sz/2-2)*sz;
  for (size_t j = 0; j < sz/3-padding; j++)
  {
    pixels[i] = col;
    pixels[i+1] = col;
    i-=sz;
  }

  // SDL_Surface* icon_surf = SDL_CreateSurfaceFrom(pixels, sz, sz, sz*sizeof(Uint16), SDL_PIXELFORMAT_RGBA4444);
  SDL_Surface* icon_surf = SDL_CreateSurfaceFrom(sz, sz, SDL_PIXELFORMAT_RGBA4444, pixels, sz*sizeof(Uint16));
  if (icon_surf == nullptr)
  {
    throw std::runtime_error(SDL_GetError());
  }
  int status = SDL_SetWindowIcon(window, icon_surf);
  SDL_DestroySurface(icon_surf);
  if (!status)
  {
    const char* err_msg = SDL_GetError();
    throw std::runtime_error(err_msg);
  }
  SDL_DestroySurface(icon_surf);
}

void TSimpleClock::Init(const SParams& params)
{
  p = params;
  if (p.delay == 0)
  {
    throw std::runtime_error("delay must be > 0.");
  }
  if (p.padx < 0)
  {
    throw std::runtime_error("padx must be >= 0.");
  }
  if (p.pady < 0)
  {
    throw std::runtime_error("pady must be >= 0.");
  }
  if (p.char_size <= 0)
  {
    throw std::runtime_error("char_size must be > 0.");
  }
  // hh:mm:ss
  // 1|2|3 -> 3600 ; 4 -> 600 ; 5|6 -> 60 ; 7 -> 10 ; 8 -> 1
  switch (p.nums_size)
  {
    case 8:
      time_difference = 1;
      break;
    case 7:
      time_difference = 10;
      break;
    case 6:
      p.nums_size--;
    case 5:
      time_difference = 60;
      break;
    case 4:
      time_difference = 600;
      break;
    case 3:
      p.nums_size--;
    case 2:
    case 1: // because 10 hours time_difference breaks after 11 P.M.
      time_difference = 3600;
      break;
    default:
      throw std::runtime_error("Incorrect nums_size value (must be in [1..8])");
      break;
  }
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    throw std::runtime_error(SDL_GetError());
  }
  nums = new TSymbolDIY[p.nums_size];
  nums[0].SetCharSize(p.char_size);
  nums[0].SetPos(p.padx, p.pady);
  int t = nums[0].GetBgRect().w;
  for (size_t i = 1; i < p.nums_size; i++)
  {
    nums[i].SetCharSize(p.char_size);
    nums[i].SetPos(p.padx + (t + p.padx)*i, p.pady);
  }
  window = SDL_CreateWindow(window_title.c_str(), t + nums[p.nums_size - 1].GetBgRect().x + p.padx, p.pady + p.pady + nums[0].GetBgRect().h, SDL_WINDOW_ALWAYS_ON_TOP);
  if (window == nullptr)
  {
    throw std::runtime_error(SDL_GetError());
  }
  CreateIcon();
  screen_surf = SDL_GetWindowSurface(window);
  if (screen_surf == nullptr)
  {
    throw std::runtime_error(SDL_GetError());
  }
  for (size_t i = 0; i < p.nums_size; i++)
  {
    nums[i].SetColorBGC(p.background_color, screen_surf);
    nums[i].SetColorCharC(p.char_color, screen_surf);
  }
  if (!SDL_FillSurfaceRect(screen_surf, nullptr, nums[0].GetColorBGC()))
    throw std::runtime_error(SDL_GetError());
  was_init = true;
  //Run();
}

void TSimpleClock::Run()
{
  if (!was_init)
    throw std::runtime_error("Can't Run() because " + title + " was not initialized. Call TSimpleClock::Init(const SParams& params).");
  while (true)
  {
    SDL_Event ev;
    while (SDL_PollEvent(&ev))
    {
      if (ev.type == SDL_EVENT_QUIT || ev.type == SDL_EVENT_TERMINATING)
      {
        running = false;
        break;
      }
    }
    if (!running)
      break;
    SetTime();
    if (time_changed)
    {
      for (size_t i = 0; i < p.nums_size; i++)
      {
        nums[i].Draw(screen_surf);
      }
      if (!SDL_UpdateWindowSurface(window))
        throw std::runtime_error(SDL_GetError());
    }
    SDL_Delay(p.delay);
  }
}

TSimpleClock::~TSimpleClock()
{
  if (nums != nullptr)
    delete[] nums;
  if (window != nullptr)
    SDL_DestroyWindow(window);
  SDL_Quit();
}
