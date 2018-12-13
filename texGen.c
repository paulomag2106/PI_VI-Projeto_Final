#include "texGen.h"
#include "float.h"

void printSitePoints() {
	for(int i = 0; i < NUMSITES; ++i) {
		printf("Site N.%d:\n", i);
		for(int j = 0; j < siteMeshes[i].numPoints; ++j) {
			printf("\tPoint %d: (%f,%f,%f)\n", j, siteMeshes[i].perimeter[j].x,
											 	  siteMeshes[i].perimeter[j].y,
										 	 	  siteMeshes[i].perimeter[j].z);
		}
	}
}

float getD(v3 a, v3 b, v3 c) {
	return ((a.x - b.x)*(b.y - c.y)) - ((b.x - c.x)*(a.y-b.y));
}

float getU(v3 a, v3 b) {
	return ((((a.x * a.x) - (b.x * b.x))+((a.y * a.y) - (b.y * b.y)))/2);
}

float getV(v3 b, v3 c) {
	return ((((b.x * b.x) - (c.x * c.x))+((b.y * b.y) - (c.y * c.y)))/2);
}

v3 getCircCenter(v3 a, v3 b, v3 c) {

    float d = getD(a, b, c);

    if (d == 0) return INVALID;

    float u = getU(a, b);
    float v = getV(b, c);

    float x = (u*(b.y-c.y)-v*(a.y-b.y))/d;
    float y = (v*(a.x-b.x)-u*(b.x-c.x))/d;

    return (v3){x, y, 0};

}

bool isInCircle(v3 point, v3 center, float radius) {

	v3 vec = (v3){
		center.x - point.x,
		center.y - point.y,
		center.z - point.z
	};
    float pointDist = dist(vec);
    return pointDist <= radius;

}

void makeVoronoiVerts() {

	for(int i = 0; i < VORONOIPOINTS; ++i) {
		voronoiPoints[i] = INVALID;
	}

	int index = 0;

    for(int a = 0; a < NUMPOINTS; ++a) {

		// int value = sqrt(NUMPOINTS);
		//
		// int yStart = -1, xStart = -1, yEnd = 1, xEnd = 1;
		//
	    // if(a % value == 0) {
	    //     //yStart = 0;
	    //     xStart = 0;
	    // }
		//
	    // else if((a + 1) % value == 0) {
	    //     xEnd = 0;
	    //     //yEnd = 0;
	    // }

        for(int b = a+1; b < NUMPOINTS; ++b) {
            for(int c = b+1; c < NUMPOINTS; ++c) {
				// if(a != b && a != c && b != c) {
	                v3 center = getCircCenter(points[a], points[b], points[c]);

					if(center.x != INVALID.x && center.y != INVALID.y && center.z != INVALID.z) {

						v3 radiusV = (v3) {
							center.x - points[a].x,
							center.y - points[a].y,
							center.z - points[a].z
						};
	                    float radius = dist(radiusV);
	                    bool flag = true;

	                    for(int point = 0; point < NUMPOINTS; ++point) {
	                        if(point != a && point != b && point != c) {
	                            if(isInCircle(points[point], center, radius)) {
	                                flag = false;
	                                break;
								}
							}
						}

	                    if(flag) {
	                        voronoiPoints[index] = center;
							// printf("Voronoi Point %d: (%f,%f,%f)\n", index,
							// 										 voronoiPoints[index].x,
							// 									 	 voronoiPoints[index].y,
							// 									 	 voronoiPoints[index].z);
							++index;

							siteMeshes[a].perimeter = realloc(siteMeshes[a].perimeter,
													  		  (siteMeshes[a].numPoints+1)*
															  sizeof(v3));
						    siteMeshes[a].perimeter[siteMeshes[a].numPoints++] =
								(v3){
									center.x - siteMeshes[a].center.x,
									center.y - siteMeshes[a].center.y,
									0,
								};

							siteMeshes[b].perimeter = realloc(siteMeshes[b].perimeter,
													  		  (siteMeshes[b].numPoints+1)*
															  sizeof(v3));
						    siteMeshes[b].perimeter[siteMeshes[b].numPoints++] =
								(v3){
									center.x - siteMeshes[b].center.x,
									center.y - siteMeshes[b].center.y,
									0,
								};

							siteMeshes[c].perimeter = realloc(siteMeshes[c].perimeter,
													  		  (siteMeshes[c].numPoints+1)*
															  sizeof(v3));
						    siteMeshes[c].perimeter[siteMeshes[c].numPoints++] =
								(v3){
									center.x - siteMeshes[c].center.x,
									center.y - siteMeshes[c].center.y,
									0,
								};

							// siteMeshes[b].perimeter = realloc(sizeof(++siteMeshes[b].numPoints)*
							// 								  sizeof(v3));
							// siteMeshes[c].perimeter = realloc(sizeof(++siteMeshes[c].numPoints)*
							// 								  sizeof(v3));
						}
					}
				// }
			}
		}
	}

}

void genVoronoiMap(uint32_t *tex, float variation) {

	for(int i = 0; i < TWIDTH/RADIUS; ++i) {
		for(int j = 0; j < TWIDTH/RADIUS; ++j) {
			int index = j + i*(TWIDTH/RADIUS);

			if(i > 0 && i < (TWIDTH/RADIUS)-1 &&
				j > 0 && j < (TWIDTH/RADIUS)-1) {
				points[index].x = (frand(variation*RADIUS) -
								   variation*RADIUS/4) + i*RADIUS +
								   RADIUS/2;
				points[index].y = (frand(variation*RADIUS) -
								   variation*RADIUS/4) + j*RADIUS +
								   RADIUS/2;
				points[index].z = frand(1.f);
			} else {
				points[index].x = i*RADIUS + RADIUS/2;
				points[index].y = j*RADIUS + RADIUS/2;
				points[index].z = 0;
			}
			// if(i > 0 && i < (TWIDTH/RADIUS)-1 &&
			// 	j > 0 && j < (TWIDTH/RADIUS)-1) {
				siteMeshes[index].center = points[index];
				siteMeshes[index].perimeter = malloc((siteMeshes[index].numPoints)*
													sizeof(v3));
			// }
		}
	}

	makeVoronoiVerts(); // create vertices where voronoi edges meet

	for(int i = 0; i < NUMPOINTS; ++i) {
		int numPoints = siteMeshes[i].numPoints;
		qsort(siteMeshes[i].perimeter, numPoints, sizeof(v3), comp);
	}

	for(int i = 0; i < TWIDTH; ++i) {
        for(int j = 0; j < TWIDTH; ++j) {
            int index = j + i*TWIDTH;

            uint32_t dist = UINT_MAX;
            int nextClosest = 0;

            for(int k = 0; k < NUMPOINTS; k++) {
                int dx = fabsf(j-points[k].x);
                int dy = fabsf(i-points[k].y);
                int newDist = intSqDist(dx, dy);

                if(dist > newDist) {
                    dist = newDist;
                    nextClosest = k;
                }

            }

			bool paint = true;

			for(int k = 0; k < VORONOIPOINTS; ++k) {
				int dx = fabsf(j-voronoiPoints[k].x);
				int dy = fabsf(i-voronoiPoints[k].y);
				int distance = (int)intSqDist(dx, dy);
				if(distance < 2) {
					tex[index] = 0xff0000ff;
					paint = false;
					break;
				}
			}

			if(paint) {
#if 1
				tex[index] = dist < 4 ? 0xff00ff00 : 0xff << 24 |
	 			 			 (int)(((points[nextClosest].z)) * 0xff) << 8;
#else
				tex[index] = dist < 4 ? 0xff00ff00 : 0xff << 24 |
							 (int)(((float)nextClosest/NUMPOINTS) * 0xff) << 16 |
	 			 			 (int)(((float)nextClosest/NUMPOINTS) * 0xff) << 8 |
							 (int)(((float)nextClosest/NUMPOINTS) * 0xff);
#endif
			}
			// tex[index] = 0xff << 24 |
 			// 			 (int)(((points[nextClosest].z)) * 0xff) << 8;

        }
    }

}

// QSORT COMPARATOR FOR ORDERING VERTICES
int comp (const void * a, const void * b)
{
    v3 *orderA = (v3 *)a;
    v3 *orderB = (v3 *)b;

    float angleA = atan2(orderA->y, orderA->x) > 0 ? atan2(orderA->y, orderA->x) : atan2(orderA->y, orderA->x) + 2*M_PI;
    float angleB = atan2(orderB->y, orderB->x) > 0 ? atan2(orderB->y, orderB->x) : atan2(orderB->y, orderB->x) + 2*M_PI;

    int order = angleA - angleB > 0 ? 1 : angleA - angleB < 0 ? -1 : 0;

    return order;
}
