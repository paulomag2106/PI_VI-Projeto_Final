#pragma once
#include "text.h"

typedef enum t_sp {
    SLOW,
    NORMAL,
    FAST
} SimulationSpeed;

typedef struct t_mg {
    
    int *animate;
    int count;
    object *objectArray;
    int move;
    float distance;
    
} ModelGroup;

typedef struct t_wolf {
    
    ModelGroup models;
    float strength;
    
} Wolf;

typedef struct t_prey {
    
    ModelGroup models;
    float preyDensity;
    
} Prey;


typedef struct t_site {
    
    float x, y;
    float slopeAngle;
    float migrationRate;
    float modelX, modelY;
    float *wolfHistory;
    float *preyHistory;
    int *huntSuccess;
    int turnsPassed;
    int nearest[8];
    int nextSite;
    Wolf wolf;
    Prey prey;
    
    
} Site;


Site *sites;
siteObj *sitesArray;
int sitesArrayCount;

SimulationSpeed simulation_speed;

void createInitialEnvironment(void);
void drawSites(void);
void freeSites(void);
void timePasses(void);
void createCSV(void);
void printHistory(void);
