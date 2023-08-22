#!/bin/sh
clear
set -ex
output="main.out"
libraries="-lpthread -lm -lSDL2 -lSDL2_ttf -lSDL2_gfx -lSDL2_image"
# flags="-std=c11 -Wall -Wextra -Wpedantic -Werror -g"
flags="-std=c11 -Wall -Wextra -Wpedantic -Werror -Ofast -march=native"
gcc main.c  $flags $libraries -o $output
