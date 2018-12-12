#include "texGen.h"
#include "float.h"

void genVoronoiMap(uint32_t *tex, float variation) {

	for(int i = 0; i < TWIDTH/RADIUS; ++i) {
		for(int j = 0; j < TWIDTH/RADIUS; ++j) {
			int index = j + i*(TWIDTH/RADIUS);
			points[index].x = (frand(variation*RADIUS) -
							   variation*RADIUS/4) + i*RADIUS +
							   RADIUS/2;
			points[index].y = (frand(variation*RADIUS) -
							   variation*RADIUS/4) + j*RADIUS +
							   RADIUS/2;
			points[index].weight = frand(1.f);
		}
	}

	for(int i = 0; i < TWIDTH; ++i) {
        for(int j = 0; j < TWIDTH; ++j) {
            int index = j + i*TWIDTH;

            uint32_t dist = UINT_MAX;
            int nextClosest = 0;
            for(int k = 0; k < NUMPOINTS; k++) {
                int dx = abs(j-points[k].x);
                int dy = abs(i-points[k].y);
                int newDist = intSqDist(dx, dy);

                if(dist > newDist) {
                    dist = newDist;
                    nextClosest = k;
                }

            }

			// tex[index] = dist < 2 ? 0xffff0000 :
 			// 			 0xff << 24 |
 			// 			 (int)((points[nextClosest].weight) * 0xff) << 8;
			tex[index] = 0xff << 24 |
 						 (int)(((points[nextClosest].weight)) * 0xff) << 8;

        }
    }

}
