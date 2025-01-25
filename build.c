#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#include <raylib.h>
#include <raymath.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#include "types.h"

#include "resources.c"
#include "custom_style.h"
#include "theme.c"
#include "ui.c"

#include "scene.c"
#include "intro.c"
#include "ending.c"
#include "training.c"
#include "main.c"
