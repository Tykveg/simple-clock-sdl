# simple-clock-sdl

This is a project I coded back in 2023. I haven't really modified the code, only edit some things to make the code work with the latest version of SDL3. For example, in older versions every SDL function used to return zero on success and non-zero on error, but now it returns true on success and false on error. Also added nix dev shell and direnv. Also added SANITIZE_ADDRESS option.

## Requirements
- cmake
- SDL3 (version 3.4.0 was tested)

## Usage
```
simple-clock-sdl [-ampm] [-nums_size 5] [-delay 50] [-background_color 0 0 0] [-char_color 255 255 255] [-padx 20] [-pady 20] [-char_size 50] [-file config.txt]
```
- ampm: 12-hours format instead of 24-hours
- nums_size: 2 - hh, 5 - hh:mm, 8 - hh:mm:ss; you can input a number between 1 and 8
- delay: main loop update interval in ms
- background_color: background color in rgb
- char_color: characters color in rgb
- padx: horizontal padding
- pady: vertical padding
- chars_size: characters size
- file: text file with parameters described above. Put the parameters the same way you put as command line arguments. But you can separate parameters (and it's values) with new line. Also you can include another file inside of a file. Max include depth is 10.
