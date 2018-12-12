#include "texGen.h"
#include "float.h"

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

    if (d == 0) return (v3){-1,-1,-1};

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

	for(int i = 0; i < NUMPOINTS*NUMPOINTS; ++i) {
		voronoiPoints[i] = (v3){-1,-1,-1};
	}

	int index = 0;

    for(int a = 0; a < NUMPOINTS-2; ++a) {
        for(int b = a+1; b < NUMPOINTS-1; ++b) {
            for(int c = b+1; c < NUMPOINTS; ++c) {
                v3 center = getCircCenter(points[a], points[b], points[c]);

                if(center.x != -1 && center.y != -1 && center.z != -1) {

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
                        voronoiPoints[index++] = center;
					}
				}
			}
		}
	}

}

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
			points[index].z = frand(1.f);
		}
	}

	makeVoronoiVerts(); // create vertices where voronoi edges meet

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

			for(int k = 0; k < NUMPOINTS*NUMPOINTS; ++k) {
				int dx = fabsf(j-voronoiPoints[k].x);
				int dy = fabsf(i-voronoiPoints[k].y);
				int distance = intSqDist(dx, dy);
				if(distance < 4) {
					tex[index] = 0xff0000ff;
					paint = false;
					break;
				}
			}

			if(paint) {
				tex[index] = dist < 4 ? 0xff00ff00 : 0xff << 24 |
	 			 			 (int)(((points[nextClosest].z)) * 0xff) << 8;
			}
			// tex[index] = 0xff << 24 |
 			// 			 (int)(((points[nextClosest].z)) * 0xff) << 8;

        }
    }

}
