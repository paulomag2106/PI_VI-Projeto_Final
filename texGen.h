#pragma once
#include "functions.h"
#define TWIDTH 1024
#define RADIUS 128
#define NUMPOINTS (TWIDTH/RADIUS * TWIDTH/RADIUS)

v3 points[NUMPOINTS];
v3 voronoiPoints[NUMPOINTS*NUMPOINTS];

float getD(v3 a, v3 b, v3 c);
float getU(v3 a, v3 b);
float getV(v3 b, v3 c);
v3 getCircCenter(v3 a, v3 b, v3 c);
bool isInCircle(v3 point, v3 center, float radius);
void genVoronoiMap(uint32_t *tex, float variation); // variation 0 to 1
