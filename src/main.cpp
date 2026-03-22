#include <SDL3/SDL.h>
#include <stdexcept>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include "SimpleClock.hpp"

// because msvc hates unsafe code
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

// to prevent recursion
static int include_depth = 0;
static const int include_depth_max = 10;

const char** SplitFixedSpaces(const std::string& str, const int& sz)
{
  char** arr = new char*[sz];
  int str_sz = str.size();
  char quotes_type = 0;
  bool special_symb = false;
  std::string tmp;
  tmp.reserve(str_sz + 1);
  for (int i = 0, j = 0; i < str_sz + 1 && j < sz; i++)
  {
    if (special_symb)
    {
      special_symb = false;
      tmp += str[i];
      continue;
    }
    if (str[i] == ' ' && !quotes_type || i == str_sz)
    {
      arr[j] = new char[tmp.size() + 1];
      std::strcpy(arr[j], tmp.c_str());
      tmp.clear();
      tmp.reserve(str_sz + 1);
      j++;
      continue;
    }
    if (str[i] == '\\')
    {
      special_symb = true;
      continue;
    }
    if (str[i] == '\'' || str[i] == '"')
    {
      if (quotes_type)
      {
        if (str[i] == quotes_type)
        {
          quotes_type = 0;
          continue;
        }
      }
      else
      {
        quotes_type = str[i];
        continue;
      }
    }
    tmp += str[i];
  }
  return (const char**)arr;
}

std::string FixMultipleSpaces(const std::string& in, int* _spaces_count = nullptr)
{
  std::string out;
  int spaces_count = 0;
  int sz = in.size();
  bool spaces_now = false;
  char quotes_type = 0;
  bool special_symb = false;
  for (int i = 0; i < sz; i++)
  {
    if (special_symb)
    {
      special_symb = false;
      if (in[i] == '\'' || in[i] == '"' || in[i] == '\\')
      {
        out += in[i];
        continue;
      }
      else
      {
        out += '\\';
      }
    }
    if (in[i] == ' ' && !quotes_type)
    {
      spaces_now = true;
      continue;
    }
    if (in[i] == '"' || in[i] == '\'')
    {
      if (quotes_type)
      {
        if (in[i] == quotes_type)
        {
          quotes_type = 0;
        }
      }
      else
      {
        quotes_type = in[i];
      }
    }
    if ((in[i] != ' ' || quotes_type) && spaces_now)
    {
      spaces_now = false;
      out += ' ';
      spaces_count++;
    }
    if (in[i] == '\\')
    {
      special_symb = true;
    }
    out += in[i];
  }
  if (special_symb)
    out += '\\';
  if (quotes_type)
    out += quotes_type;
  if (_spaces_count)
    *_spaces_count = spaces_count;
  return out;
}

void ApplyParam(const int& sz, const char** arr, int& i, SParams& params)
{
  std::string argi = std::string(arr[i]);
  if (argi.empty())
  {
    return;
  }
  if (argi == "-ampm")
  {
    params.do_ampm = true;
    return;
  }
  if (argi == "-nums_size")
  {
    if (i + 1 < sz)
    {
      params.nums_size = atoi(arr[++i]);
      return;
    }
    else
    {
      throw std::runtime_error(argi + " requires one argument (uint , 1 <= x <= 8).");
    }
  }
  if (argi == "-delay")
  {
    if (i + 1 < sz)
    {
      params.delay = atoi(arr[++i]);
      return;
    }
    else
    {
      throw std::runtime_error(argi + " requires one argument (uint32 , x > 0).");
    }
  }
  if (argi == "-background_color")
  {
    if (i + 3 < sz)
    {
      Uint8 tmp[3];
      for (int j = 0; j < 3; j++)
      {
        tmp[j] = atoi(arr[++i]);
      }
      params.background_color = SDL_Color{tmp[0], tmp[1], tmp[2]};
      return;
    }
    else
    {
      throw std::runtime_error(argi + " requires 3 arguments (uint8[3]).");
    }
  }
  if (argi == "-char_color")
  {
    if (i + 3 < sz)
    {
      Uint8 tmp[3];
      for (int j = 0; j < 3; j++)
      {
        tmp[j] = atoi(arr[++i]);
      }
      params.char_color = SDL_Color{tmp[0], tmp[1], tmp[2]};
      return;
    }
    else
    {
      throw std::runtime_error(argi + " requires 3 arguments (uint8[3]).");
    }
  }
  if (argi == "-padx")
  {
    if (i + 1 < sz)
    {
      params.padx = atoi(arr[++i]);
      return;
    }
    else
    {
      throw std::runtime_error(argi + " requires one argument (int , x => 0).");
    }
  }
  if (argi == "-pady")
  {
    if (i + 1 < sz)
    {
      params.pady = atoi(arr[++i]);
      return;
    }
    else
    {
      throw std::runtime_error(argi + " requires one argument (int , x => 0).");
    }
  }
  if (argi == "-char_size")
  {
    if (i + 1 < sz)
    {
      params.char_size = atoi(arr[++i]);
      return;
    }
    else
    {
      throw std::runtime_error(argi + " requires one argument (int , x > 0).");
    }
  }
  if (argi == "-file")
  {
    if (i + 1 < sz)
    {
      if (include_depth >= include_depth_max)
      {
        i++;
        std::cout << argi << " reached max depth " << include_depth_max << ". Skipping '" << argi << ' ' << arr[i] << "'...\n";
        return;
      }
      std::ifstream f(arr[++i]);
      std::string output;
      std::vector<std::string> lines;
      if (f.is_open())
      {
        lines.reserve(16);
        include_depth++;
        while (std::getline(f, output))
        {
          lines.push_back(output);
        }
        f.close();
        for (auto& e : lines)
        {
          int tmp_arr_size, j;
          e = FixMultipleSpaces(e, &tmp_arr_size);
          tmp_arr_size++;
          const char** tmp_arr = SplitFixedSpaces(e, tmp_arr_size);
          for (j = 0; j < tmp_arr_size; j++)
          {
            ApplyParam(tmp_arr_size, tmp_arr, j, params);
          }
          for (j = 0; j < tmp_arr_size; j++)
          {
            delete[] tmp_arr[j];
          }
          delete[] tmp_arr;
          include_depth--;
        }
      }
      else
      {
        //throw std::runtime_error(argi + ' ' + arr[i] + " : file not found.");
        std::cout << argi << ' ' << arr[i] << " : file not found. Skipping...\n";
        return;
      }
      return;
    }
    else
    {
      throw std::runtime_error(argi + " requires one argument (string , path to file).");
    }
  }
  std::cout << "Unknown flag '" << argi << "'. Skipping...\n";
  return;
}

// const char* argv[] because it's more accurate because I don't want to modify it.
int main(const int argc, const char* argv[])
{
  SDL_SetHint(SDL_HINT_VIDEO_ALLOW_SCREENSAVER, "1");
  TSimpleClock app;
  SParams params;
  const char* include_default[] = {"-file", "config.txt"};
  int i = 0;
  std::cout << "Including default config " << include_default[1] << " if exists...\n";
  ApplyParam(2, include_default, i, params);
  for (i = 1; i < argc; i++)
  {
    ApplyParam(argc, argv, i, params);
  }
  app.Init(params);
  app.Run();
  return 0;
}
