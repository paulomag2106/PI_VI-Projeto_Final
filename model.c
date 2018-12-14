#include "model.h"

#define MODELSCALE 13.2f
#define MODELDIST 20.f
#define MODEL_Z 25.f
#define GROUPDIST 50.f


v4 standardColor = (v4) {1.f,1.f,1.f,1.f};


void setNearest(int site_index) {
    
    
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

void createInitialEnvironment() {

    sites = malloc(sizeof(Site) * sitesArrayCount);
    
    for(int i = 0; i < sitesArrayCount; i++) {

        sites[i].x = sitesArray[i].center.x;
        sites[i].y = sitesArray[i].center.y;
        
        sites[i].slopeAngle = sitesArray[i].center.z;
        
        float preyDensity = frand(1.f) * clamp(sites[i].slopeAngle, 0.4f, 0.6f);
        float wolfDensity = frand(1.f) * clamp((1.f - sites[i].slopeAngle), 0.3f, 0.8f);
        
        ModelGroup wolfGroup = {MODELCOUNT, malloc(sizeof(object) * MODELCOUNT)};
        ModelGroup preyGroup = {MODELCOUNT, malloc(sizeof(object) * MODELCOUNT)};
        
        for(int m = 0; m < MODELCOUNT; m++) {
            
            object preyObj = loadOBJModel("models/grey_wolf.obj");
            object wolfObj = loadOBJModel("models/deer.obj");
            
            preyObj.color = standardColor;
            wolfObj.color = standardColor;
            
            wolfGroup.objectArray[m] = wolfObj;
            preyGroup.objectArray[m] = preyObj;
            
            v3 preyPosition = newV3(sites[i].x + GROUPDIST, sites[i].y, MODEL_Z);
            v3 wolfPosition = newV3(sites[i].x - GROUPDIST, sites[i].y, MODEL_Z);
            
            float xChange = MODELDIST, yChange = MODELDIST;
            
            switch (m) {
                case 0:
                    xChange = 0.f;
                    yChange = 0.f;
                case 1:
                    xChange = MODELDIST;
                    yChange = -MODELDIST;
                case 2:
                    xChange = MODELDIST;
                    yChange = MODELDIST;
                case 3:
                    xChange = -MODELDIST;
                    yChange = -MODELDIST;
                case 4:
                    xChange = -MODELDIST;
                    yChange = MODELDIST;
                case 6:
                    xChange = 0.f;
                    yChange = MODELDIST * 2.f;
                case 7:
                    xChange = 0.f;
                    yChange = MODELDIST * -2.f;
                case 8:
                    xChange = MODELDIST * 2.f;
                    yChange = 0.f;
                case 9:
                    xChange = MODELDIST * -2.f;
                    yChange = 0.f;
            }
            
            wolfPosition.x += xChange;
            wolfPosition.y += yChange;
            
            preyPosition.x += -xChange;
            preyPosition.y += yChange;
            
            preyObj.position = preyPosition;
            wolfObj.position = wolfPosition;
            
            scaleObjTo(&preyObj, newV3(MODELSCALE*preyDensity, MODELSCALE*preyDensity, MODELSCALE*preyDensity));
            scaleObjTo(&wolfObj, newV3(MODELSCALE*wolfDensity, MODELSCALE*wolfDensity, MODELSCALE*wolfDensity));
            
            rotateObjBy(&preyObj, newV3(0, 0, 1), M_PI / 2.f);
            rotateObjBy(&wolfObj, newV3(0, 0, 1), -M_PI / 2.f);
        }
        
        
        sites[i].prey = (Prey){preyGroup, preyDensity};
        sites[i].wolf = (Wolf){wolfGroup, wolfDensity};

        sites[i].nextSite = -1;

        sites[i].nearest[7] = -1;
        sites[i].nearest[6] = -1;
        sites[i].nearest[5] = -1;
        setNearest(i);
    }

}


void timePasses() {

    printf("time has passed!\n");

    for(int i = 0; i < sitesArrayCount; i++) {

        float w = sites[i].wolf.strength;
        float p = sites[i].prey.preyDensity;

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

            }

            else {

                float noHuntEffect = clamp(frand(0.05f), 0.f, 0.05f);

                sites[i].prey.preyDensity += (p) * clamp(frand(0.05f), 0.01f, 0.05f);
                sites[i].prey.preyDensity = clamp(sites[i].prey.preyDensity, 0.f, 1.f);

                sites[i].wolf.strength -= noHuntEffect;
                sites[i].wolf.strength = clamp(sites[i].wolf.strength, 0.f, 1.f);

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

    for(int i = 0; i < sitesArrayCount; i++) {

        float preyDensity = (sites[i].prey.preyDensity);
        float wolfDensity = (sites[i].wolf.strength);

//        if(preyDensity <= 0.0001f) preyDensity = 0.f;
//        if(wolfDensity <= 0.0001f) wolfDensity = 0.f;

        for(int x = 0; x < 10; x++) {
            
            float c = (float) x / 10.f;
            
            if(c >= preyDensity) {
                sites[i].prey.models.objectArray[x].color = standardColor;
            }
            
            else {
                sites[i].prey.models.objectArray[x].color.w = 0.f;
            }
            
            
            if(c >= wolfDensity) {
                sites[i].wolf.models.objectArray[x].color = standardColor;
            }
            
            else {
                sites[i].wolf.models.objectArray[x].color.w = 0.f;
            }
            
        }
    }
}


void drawSites() {

    for(int i = 0; i < sitesArrayCount; i++) {
        
        for(int j = 0; j < MODELCOUNT; j++) {
            
            drawObject(&sites[i].prey.models.objectArray[j]);
            drawObject(&sites[i].wolf.models.objectArray[j]);
        }
    }

}

void freeSites() {

    for(int i = 0; i < sitesArrayCount; i++) {
        for(int j = 0; j < MODELCOUNT; j++) {
            
            freeObject(&sites[i].prey.models.objectArray[j]);
            freeObject(&sites[i].wolf.models.objectArray[j]);
        }
    }
    
    free(sites);
    free(sitesArray);
}
