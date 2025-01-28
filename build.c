#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

#include <raylib.h>
#include <raymath.h>

#include "types.h"
// #include "mem.c"

#include "app.c"

#include "pixel_operator_font.c"
#include "resources.c"
#include "theme.c"
#include "ui.c"

#include "sequence.c"
#include "game.c"

#include "scene.c"
#include "intro.c"
#include "ending.c"
#include "gameplay.c"
#include "game_over.c"
#include "credits.c"

#include "main.c"
