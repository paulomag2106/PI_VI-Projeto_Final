#pragma once
#include "functions.h"
#define TWIDTH (1024/2)
#define RADIUS (512/8)
// #define VORONOIPOINTS RADIUS*16
#define NUMPOINTS (TWIDTH/RADIUS * TWIDTH/RADIUS)
#define NUMSITES (((TWIDTH/RADIUS)-2) * ((TWIDTH/RADIUS)-2))
#define INVALID (v3){-TWIDTH*5,-TWIDTH*5,-TWIDTH*5}

typedef struct t_siteObj {
    int numPoints;
    v3 center;
    v3 *perimeter;
}siteObj;

v3 points[NUMPOINTS];
// siteObj siteMeshes[NUMSITES];
siteObj siteMeshes[NUMPOINTS];
// v3 voronoiPoints[VORONOIPOINTS];

void printSitePoints(void);
float getD(v3 a, v3 b, v3 c);
float getU(v3 a, v3 b);
float getV(v3 b, v3 c);
v3 getCircCenter(v3 a, v3 b, v3 c);
bool isInCircle(v3 point, v3 center, float radius);
void genVoronoiMap(uint32_t *tex, float variation); // variation 0 to 1
int comp (const void * a, const void * b);
