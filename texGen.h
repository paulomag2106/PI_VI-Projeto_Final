#pragma once
#include "functions.h"
#define TWIDTH 1024
#define RADIUS 64
#define NUMPOINTS (TWIDTH/RADIUS * TWIDTH/RADIUS)

typedef struct t_weightV2 {
    int x,y;
    float weight;
} weightV2;

weightV2 points[NUMPOINTS];

void genVoronoiMap(uint32_t *tex, float variation); // variation 0 to 1
