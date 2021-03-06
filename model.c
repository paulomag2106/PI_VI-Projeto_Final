#include "model.h"

#define MODELCOUNT 10
#define MODELSCALE 7.f
#define MODELDIST_Y 2.f
#define MODELDIST_X 5.f
#define MODEL_Z 1.f
#define GROUPDIST 5.f
#define TURNSTOTEST 200
#define MOVEDISTANCE 25.f


v4 wolfColor = (v4) {0.5f, (v3){0.6f, 1.f, 1.f}};
v4 preyColor = (v4) {0.5f, (v3){0.6f, 1.f, 1.f}};


void changeModels() {
    
    for(int i = 0; i < sitesArrayCount; i++) {
        
        float preyDensity = (sites[i].prey.preyDensity);
        float wolfDensity = (sites[i].wolf.strength);
        
        if(preyDensity <= 0.0001f) preyDensity = 0.f;
        if(wolfDensity <= 0.0001f) wolfDensity = 0.f;
        
        for(int x = 0; x < 10; x++) {
            
            float c = (float) x / 10.f;
            
            ModelGroup wolfGroup = sites[i].wolf.models;
            ModelGroup preyGroup = sites[i].prey.models;
            
            if(c < 0.001f) c = 0.001f;
            
            if(preyDensity >= c) {
                
                if(simulation_speed == FAST) preyGroup.objectArray[x].color.vec.z = 1.f;
                
                else preyGroup.animate[x] = 1;
                
            }
            
            else  {
                
                if(simulation_speed == FAST) preyGroup.objectArray[x].color.vec.z = 0.f;
                
                else preyGroup.animate[x] = -1;
            }
            
            
            if(wolfDensity >= c) {
                
                if(simulation_speed == FAST) wolfGroup.objectArray[x].color.vec.z = 1.f;
                
                else wolfGroup.animate[x] = 1;
            }
            
            else {
                
                if(simulation_speed == FAST) wolfGroup.objectArray[x].color.vec.z = 0.f;
                
                else wolfGroup.animate[x] = -1;
            }
            
        }
    }
    
}


void setNearest(int site_index) {
    
    //treis dados
    //
    // media sitio sucesso caca
    // porcentagem de desnivel do terreno
    // razão média entre as populations
    
    int value = sqrt(sitesArrayCount);
    int near_index = 0;

    int yStart = -1, xStart = -1, yEnd = 1, xEnd = 1;

    if(site_index % value == 0) {
        //yStart = 0;
        xStart = 0;
    }

    else if((site_index + 1) % value == 0) {
        xEnd = 0;
        //yEnd = 0;
    }

    for(int y = yStart; y <= yEnd; y++) {

        for(int x = xStart; x <= xEnd; x++) {

            int index = site_index + (x + (y * value));

            if (index < 0 || index >= sitesArrayCount) sites[site_index].nearest[near_index++] = -1;
            else if(index != site_index) sites[site_index].nearest[near_index++] = index;

        }
    }

    // printf("Site %d with nearests: %d %d %d %d %d %d %d %d\n", site_index, sites[site_index].nearest[0], sites[site_index].nearest[1], sites[site_index].nearest[2], sites[site_index].nearest[3], sites[site_index].nearest[4], sites[site_index].nearest[5], sites[site_index].nearest[6], sites[site_index].nearest[7]);

}

void positionWolfModels(Site site) {
    
    for(int m = 0; m < MODELCOUNT; m++) {
        
        v3 wolfPosition = newV3(site.modelX, site.modelY, MODEL_Z);
        
        wolfPosition.x -= (TWIDTH/2.f);
        wolfPosition.y -= (TWIDTH/2.f);
        
        wolfPosition.x -= GROUPDIST;
        
        float xChange = MODELDIST_X, yChange = MODELDIST_Y;
        
        switch (m) {
            case 0:
                xChange = 0.f;
                yChange = 0.f;
                break;
            case 1:
                xChange = -MODELDIST_X;
                yChange = MODELDIST_Y;
                break;
            case 2:
                xChange = -MODELDIST_X;
                yChange = -MODELDIST_Y;
                break;
            case 3:
                xChange = -2.f * MODELDIST_X;
                yChange = 0.f;
                break;
            case 4:
                xChange = -2.f * MODELDIST_X;
                yChange = 2.f * MODELDIST_Y;
                break;
            case 5:
                xChange = -2.f * MODELDIST_X;
                yChange = -2.f * MODELDIST_Y;
                break;
            case 6:
                xChange = -3.f * MODELDIST_X;
                yChange = MODELDIST_Y;
                break;
            case 7:
                xChange = -3.f * MODELDIST_X;
                yChange = -MODELDIST_Y;
                break;
            case 8:
                xChange = -4.f * MODELDIST_X;
                yChange = MODELDIST_Y;
                break;
            case 9:
                xChange = -4.f * MODELDIST_X;
                yChange = -MODELDIST_Y;
                break;
        }
        
        wolfPosition.x += xChange;
        wolfPosition.y += yChange;
        
        site.wolf.models.objectArray[m].position = wolfPosition;
    }
}

void createInitialEnvironment() {

    sites = malloc(sizeof(Site) * sitesArrayCount);
    
    for(int i = 0; i < sitesArrayCount; i++) {

        sites[i].x = sitesArray[i].center.x;
        sites[i].y = sitesArray[i].center.y;
        
        sites[i].slopeAngle = sitesArray[i].center.z;
        
        //For CSV tests
        //sites[i].slopeAngle = 0.5f;
        
        sites[i].turnsPassed = 0;
        sites[i].wolfHistory = malloc(sizeof(float) * TURNSTOTEST);
        sites[i].preyHistory = malloc(sizeof(float) * TURNSTOTEST);
        sites[i].huntSuccess = malloc(sizeof(int) * TURNSTOTEST);
        
        float sumX = 0.f, sumY = 0.f;
        
        for(int p = 0; p < sitesArray[i].numPoints; p++) {
            
            sumX += (sitesArray[i].perimeter[p].x + sitesArray[i].center.x);
            sumY += (sitesArray[i].perimeter[p].y + sitesArray[i].center.y);
            
        }
        
        sites[i].modelX = sumX / sitesArray[i].numPoints;
        sites[i].modelY = sumY / sitesArray[i].numPoints;
        
        float preyDensity = frand(1.f) * clamp(sites[i].slopeAngle, 0.4f, 0.6f);
        float wolfDensity = frand(1.f) * clamp((1.f - sites[i].slopeAngle), 0.3f, 0.8f);
        
        //For CSV tests
        preyDensity = 0.5;
        wolfDensity = 0.5;
        
        ModelGroup wolfGroup = {malloc(sizeof(int) * MODELCOUNT), MODELCOUNT, malloc(sizeof(object) * MODELCOUNT), 0, MOVEDISTANCE};
        ModelGroup preyGroup = {malloc(sizeof(int) * MODELCOUNT), MODELCOUNT, malloc(sizeof(object) * MODELCOUNT), 0, MOVEDISTANCE};
        
        for(int m = 0; m < MODELCOUNT; m++) {
            
            object preyObj = loadOBJModel("models/deer.obj");
            object wolfObj = loadOBJModel("models/grey_wolf.obj");
            
            preyObj.color = preyColor;
            wolfObj.color = wolfColor;
            
            v3 preyPosition = newV3(sites[i].modelX, sites[i].modelY, MODEL_Z);
            v3 wolfPosition = newV3(sites[i].modelX, sites[i].modelY, MODEL_Z);
            
            wolfPosition.x -= (TWIDTH/2.f);
            wolfPosition.y -= (TWIDTH/2.f);
            preyPosition.x -= (TWIDTH/2.f);
            preyPosition.y -= (TWIDTH/2.f);
            
            wolfPosition.x -= GROUPDIST;
            preyPosition.x += GROUPDIST;
            
            wolfGroup.animate[m] = 0;
            preyGroup.animate[m] = 0;
            
            float xChange = MODELDIST_X, yChange = MODELDIST_Y;
            
            switch (m) {
                case 0:
                    xChange = 0.f;
                    yChange = 0.f;
                    break;
                case 1:
                    xChange = -MODELDIST_X;
                    yChange = MODELDIST_Y;
                    break;
                case 2:
                    xChange = -MODELDIST_X;
                    yChange = -MODELDIST_Y;
                    break;
                case 3:
                    xChange = -2.f * MODELDIST_X;
                    yChange = 0.f;
                    break;
                case 4:
                    xChange = -2.f * MODELDIST_X;
                    yChange = 2.f * MODELDIST_Y;
                    break;
                case 5:
                    xChange = -2.f * MODELDIST_X;
                    yChange = -2.f * MODELDIST_Y;
                    break;
                case 6:
                    xChange = -3.f * MODELDIST_X;
                    yChange = MODELDIST_Y;
                    break;
                case 7:
                    xChange = -3.f * MODELDIST_X;
                    yChange = -MODELDIST_Y;
                    break;
                case 8:
                    xChange = -4.f * MODELDIST_X;
                    yChange = MODELDIST_Y;
                    break;
                case 9:
                    xChange = -4.f * MODELDIST_X;
                    yChange = -MODELDIST_Y;
                    break;
            }
            
            wolfPosition.x += xChange;
            wolfPosition.y += yChange;

            preyPosition.x += -xChange;
            preyPosition.y += yChange;
        
            preyObj.position = preyPosition;
            wolfObj.position = wolfPosition;
            
            scaleObjTo(&preyObj, newV3(MODELSCALE, MODELSCALE, MODELSCALE));
            scaleObjTo(&wolfObj, newV3(MODELSCALE, MODELSCALE, MODELSCALE));
            
//            rotateObjBy(&preyObj, newV3(0, 0, 1), M_PI / 2.f);
//            rotateObjBy(&wolfObj, newV3(0, 0, 1), -M_PI / 2.f);
            
            wolfGroup.objectArray[m] = wolfObj;
            preyGroup.objectArray[m] = preyObj;
        }
        
        
        sites[i].prey = (Prey){preyGroup, preyDensity};
        sites[i].wolf = (Wolf){wolfGroup, wolfDensity};

        sites[i].nextSite = -1;

        sites[i].nearest[7] = -1;
        sites[i].nearest[6] = -1;
        sites[i].nearest[5] = -1;
        setNearest(i);
    }

    simulation_speed = FAST;
    changeModels();
    simulation_speed = SLOW;
}


void timePasses() {

    printf("time has passed!\n");
    
    for(int i = 0; i < sitesArrayCount; i++) {
        
        float w = sites[i].wolf.strength;
        float p = sites[i].prey.preyDensity;
        
        int turn = ++sites[i].turnsPassed;
        
        if(turn < TURNSTOTEST) {
        
            sites[i].preyHistory[turn] = p;
            sites[i].wolfHistory[turn] = w;
            sites[i].huntSuccess[turn] = -1;
        
        }
        

        float ratio;

        if(p <= 0.f) ratio = 99.f;
        else ratio = w / p;


        float migrate = ratio / (0.9f / 0.1f);
        migrate = clamp(migrate, 0.f, 1.f);
        //float hunt = 1.f - migrate;

        bool willMigrate = frand(1.f) <= migrate;

        if(willMigrate) {

            float wolfMigration = clamp(frand(0.5f), 0.3f, 0.5f) * w;
            wolfMigration = clamp(wolfMigration, wolfMigration, sites[i].wolf.strength);

            //sites[i].wolf.strength -= wolfMigration;

            int nearest = -1;
            float newRatio = 99999999.f;

            for(int j = 0; j < 8; j++) {

                if(sites[i].nearest[j] >= 0) {

                    Site checkSite = sites[ sites[i].nearest[j] ];

                    float nearestRatio = checkSite.wolf.strength / checkSite.prey.preyDensity;

                    if(nearestRatio < newRatio)  {
                        newRatio = nearestRatio;
                        nearest = j;
                    }
                }
            }

            sites[i].migrationRate = wolfMigration;
            sites[i].nextSite = nearest;

            sites[i].prey.preyDensity += (p) * clamp(frand(0.05f), 0.01f, 0.05f);
            sites[i].prey.preyDensity = clamp(sites[i].prey.preyDensity, 0.f, 1.f);
        }

        else {

            float huntSuccessChance = 1.f - sites[i].slopeAngle * 0.5f;
            huntSuccessChance = clamp(huntSuccessChance, 0.1f, 0.9f);

            bool huntSuccessful = frand(1.f) <= huntSuccessChance;

            if(huntSuccessful) {

                float huntPreyEffect = w * clamp(frand(0.15f), 0.05f, 0.15f);
                float huntWolfEffect = ((p + w)/2.f) * clamp(frand(0.15f), 0.05f, 0.15f);

                sites[i].prey.preyDensity -= huntPreyEffect;
                sites[i].prey.preyDensity = clamp(sites[i].prey.preyDensity, 0.f, 1.f);

                sites[i].wolf.strength += huntWolfEffect;
                sites[i].wolf.strength = clamp(sites[i].wolf.strength, 0.f, 1.f);
                
                sites[i].wolf.models.move = 1;
                sites[i].wolf.models.distance = MOVEDISTANCE;
                
                
                if(turn < TURNSTOTEST) sites[i].huntSuccess[turn] = 1;

            }

            else {

                float noHuntEffect = clamp(frand(0.05f), 0.f, 0.05f);

                sites[i].prey.preyDensity += (p) * clamp(frand(0.05f), 0.01f, 0.05f);
                sites[i].prey.preyDensity = clamp(sites[i].prey.preyDensity, 0.f, 1.f);

                sites[i].wolf.strength -= noHuntEffect;
                sites[i].wolf.strength = clamp(sites[i].wolf.strength, 0.f, 1.f);
                
                if(turn < TURNSTOTEST) sites[i].huntSuccess[turn] = 0;

            }
        }


    }


    for(int i = 0; i < sitesArrayCount; i++) {

        if(sites[i].nextSite >= 0) {

            int next = sites[i].nearest[sites[i].nextSite];

            //printf("Wolves of site %d will migrate to site %d\n", i,sites[i].nextSite);

            sites[next].wolf.strength += sites[i].migrationRate;
            sites[i].wolf.strength -= sites[i].migrationRate;

            sites[next].wolf.strength = clamp(sites[next].wolf.strength, 0.f, 1.f);
            sites[i].wolf.strength = clamp(sites[i].wolf.strength, 0.f, 1.f);

            sites[i].nextSite = -1;

        }
    }
    
    changeModels();
}

void printHistory() {
    
    for(int s = 0; s < sitesArrayCount; s++) {
        
        Site site = sites[s];
        
        printf("Sitio %d\n", s);
        
        for(int t = 0; t < TURNSTOTEST; t++) {
            
            printf("Densidade de Lobos: %.5f, Ungulados: %.5f, ", site.wolfHistory[t], site.preyHistory[t]);
            
            if(site.huntSuccess[t] == 0) {
                printf("Caça falhou.\n");
            }
            
            else if(site.huntSuccess[t] == 1) {
                printf("Caça bem sucedida.\n");
            }
            
            else {
                printf("Lobos migraram.\n");
            }
        }
        
        printf("\n");
    }

    
}

void createCSV() {
    
    FILE *file = fopen("results-pop.csv", "w");
    
    for(int s = 0; s < sitesArrayCount; s++) {
        
        Site site = sites[s];
        
        fprintf(file, "Sitio %d\n", s);
        
        for(int t = 0; t < TURNSTOTEST; t++) {
            
            fprintf(file, "%f, %f, %d\n", site.wolfHistory[t], site.preyHistory[t], site.huntSuccess[t]);
        }
        
        fprintf(file, "\n");
    }
    
    fclose(file);
    
}

void drawSites() {

    for(int i = 0; i < sitesArrayCount; i++) {
        
        for(int j = 0; j < MODELCOUNT; j++) {
            
            bool animatedPrey = false, animatedWolf = false;
            
            if(simulation_speed != FAST) {
                
                //PREY FADE IN OUT
                if(sites[i].prey.models.animate[j] < 0) {
                    
                    animatedPrey = true;
                    
                    if(simulation_speed == NORMAL)
                        sites[i].prey.models.objectArray[j].color.vec.z -= (deltaTime / 0.25f);
                    
                    else
                        sites[i].prey.models.objectArray[j].color.vec.z -= (deltaTime / 0.5f);
                }
                
                else if(sites[i].prey.models.animate[j] > 0) {
                    
                    animatedPrey = true;
                    
                    if(simulation_speed == NORMAL)
                        sites[i].prey.models.objectArray[j].color.vec.z += (deltaTime / 0.25f);
                    
                    else
                        sites[i].prey.models.objectArray[j].color.vec.z += (deltaTime / 0.5f);
                    
                }
                
                
                //WOLF FADE IN OUT
                if(sites[i].wolf.models.animate[j] < 0) {
                    
                    animatedWolf = true;
                    
                    if(simulation_speed == NORMAL)
                        sites[i].wolf.models.objectArray[j].color.vec.z -= (deltaTime / 0.25f);
                    
                    else
                        sites[i].wolf.models.objectArray[j].color.vec.z -= (deltaTime / 0.5f);
                }
                
                else if(sites[i].wolf.models.animate[j] > 0) {
                    
                    animatedWolf = true;
                    
                    if(simulation_speed == NORMAL)
                        sites[i].wolf.models.objectArray[j].color.vec.z += (deltaTime / 0.25f);
                    
                    else
                        sites[i].wolf.models.objectArray[j].color.vec.z += (deltaTime / 0.5f);
                    
                }
                
                
                if(animatedPrey) {
                    if(sites[i].prey.models.objectArray[j].color.vec.z <= 0.001f || sites[i].prey.models.objectArray[j].color.vec.z > 0.999f ) {
                        sites[i].prey.models.animate[j] = 0;
                    }
                }
                
                if(animatedWolf) {
                    if(sites[i].wolf.models.objectArray[j].color.vec.z <= 0.001f || sites[i].wolf.models.objectArray[j].color.vec.z > 0.999f ) {
                        sites[i].wolf.models.animate[j] = 0;
                    }
                }
            }
            
            float value = 0.f;
            
            if(simulation_speed == NORMAL) {
                value = (deltaTime / 0.4);
            }
            
            else if(simulation_speed == SLOW) {
                value = (deltaTime / 0.8);
            }
            
            
            if(sites[i].wolf.models.move == 1) {
                
                moveObjBy(&sites[i].wolf.models.objectArray[j], newV3(value * MOVEDISTANCE, 0, 0));
                sites[i].wolf.models.distance -= value * MOVEDISTANCE;
                
                if(sites[i].wolf.models.distance < 0.f) {
                    sites[i].wolf.models.distance = 0.f;
                    sites[i].wolf.models.move = 2;
                }
            }
            
            else if(sites[i].wolf.models.move == 2) {
                
                moveObjBy(&sites[i].wolf.models.objectArray[j], newV3(-value * MOVEDISTANCE, 0, 0));
                sites[i].wolf.models.distance += value * MOVEDISTANCE;
                
                if(sites[i].wolf.models.distance > MOVEDISTANCE) {
                    sites[i].wolf.models.distance = MOVEDISTANCE;
                    sites[i].wolf.models.move = 0;
                    positionWolfModels(sites[i]);
                }
            }
            
            
            
            drawObject(&sites[i].prey.models.objectArray[j]);
            drawObject(&sites[i].wolf.models.objectArray[j]);
        }
    }

}

void freeSites() {

    for(int i = 0; i < sitesArrayCount; i++) {
        
        free(sites[i].huntSuccess);
        free(sites[i].wolfHistory);
        free(sites[i].preyHistory);
        
        for(int j = 0; j < MODELCOUNT; j++) {
            
            freeObject(&sites[i].prey.models.objectArray[j]);
            freeObject(&sites[i].wolf.models.objectArray[j]);
        }
    }
    
    free(sites);
    free(sitesArray);
}
