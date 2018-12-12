#pragma once
#include "text.h"


typedef struct t_wolf {
    
    object obj;
    float strength;
    
} Wolf;

typedef struct t_prey {
    
    object obj;
    float preyDensity;
    
} Prey;


typedef struct t_site {
    
    float x, y;
    float slopeAngle;
    float migrationRate;
    int nearest[8];
    int nextSite;
    Wolf wolf;
    Prey prey;
    
    
} Site;


void createInitialEnvironment(void);
void drawSites(void);
void freeSites(void);
void timePasses(void);

