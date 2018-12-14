#include "shapes.h"

void makeVBOSizeAndPush(object *newObject) {

    newObject->vboSize[0] = sizeof(GLfloat) * newObject->verticesCount * 3; // vertices buffer size
    newObject->vboSize[1] = sizeof(GLfloat) * newObject->verticesCount * 3; // normals buffer size
    newObject->vboSize[2] = sizeof(GLfloat) * newObject->verticesCount * 2; // uvs buffer size

    pushObject(newObject);

}

void fixRoundUVs(object *obj, int startIndex, int endIndex, v3 radius) {

    for(int i = startIndex; i < endIndex; i++) {

        v3 point = newV3(obj->vertices[(i * 3)], obj->vertices[(i * 3) + 1], obj->vertices[(i * 3) + 2]);

        float x = (point.x + radius.x) / (radius.x * 2);
        float y = (point.z + radius.y) / (radius.y * 2);

        obj->uvs[(i*2)] = x;
        obj->uvs[(i*2)+1] = y;
    }

}

void deleteVertices(object *obj) {

    free(obj->vertices);
    obj->vertices = malloc(sizeof(GLfloat));
    obj->verticesCount = 0;

}

void roundVerticesToCenter(object *obj, v3 center, v3 radius, int startIndex, int endIndex) {

    if(endIndex > obj->verticesCount) {
        printf("WARNING: Can't round vertices beyond bounds.\n");
        endIndex = obj->verticesCount;
    }

    for(int x = startIndex; x < endIndex; x++) {

        v3 vec = { .x = obj->vertices[(x*3)] - center.x, .y = obj->vertices[(x*3)+1] - center.y, .z = obj->vertices[(x*3)+2] - center.z };

        float value = dist(vec);

        obj->normals[(x*3)] = vec.x / value;
        obj->normals[(x*3)+1] = vec.y / value;
        obj->normals[(x*3)+2] = vec.z / value;

        obj->vertices[(x*3)] = vec.x / value * radius.x;
        obj->vertices[(x*3)+1] = vec.y / value * radius.y;
        obj->vertices[(x*3)+2] = vec.z / value * radius.z;

        obj->vertices[(x*3)] += center.x;
        obj->vertices[(x*3)+1] += center.y;
        obj->vertices[(x*3)+2] += center.z;

        //printf("x:%f, y:%f, z:%f \n", obj.vertices[(x*3)],obj.vertices[(x*3)+1],obj.vertices[(x*3)+2]);
    }
}


void makeCircle(object *obj, v3 center, v3 pointB, v3 pointC, int subdivision) {

    int pointsQuantity = 8 + (subdivision * subdivision);
    int vertices_count = pointsQuantity * 3;

    //v3 verts[vertices_count];
    //v2 uvs[vertices_count];

	v3 *verts = malloc(sizeof(v3) * vertices_count);
	v2 *uvs = malloc(sizeof(v2) * vertices_count);

    v3 last_point = pointB;

    double degreeIteration = (M_PI * 2.f) / (pointsQuantity);
    double degree = degreeIteration;

    v3 dist_b = { .x = pointB.x - center.x, .y = pointB.y - center.y, .z = pointB.z - center.z };
    v3 dist_c = { .x = pointC.x - center.x, .y = pointC.y - center.y, .z = pointC.z - center.z };

    v3 face_normal = crossProduct(dist_b, dist_c);

    float normalize = dist(face_normal);
    face_normal.x = face_normal.x / normalize;
    face_normal.y = face_normal.y / normalize;
    face_normal.z = face_normal.z / normalize;

    float radiusA = dist(dist_b);
    float radiusB = dist(dist_c);

    for(int x = 0; x < pointsQuantity; x++) {

        float multA = 1.f, multB = 1.f;

        v3 a,b,c;
        v2 uv_a, uv_b, uv_c;

        a = center;
        b = last_point;

        uv_a = newV2(0.5f, 0.5f);

        if(face_normal.z > 0.1f || face_normal.z < -0.1f ) {

            if(face_normal.z < 0.1f) {
                if((pointB.x - center.x) < -0.01f) multA = -1.f;
                else multB = -1.f;
            }

            c = newV3(cosf(degree) * radiusA * multA, sinf(degree) * radiusB * multB, center.z);
            uv_b = newV2( (b.x - center.x + radiusA) / (2.f * radiusA), (b.y - center.y + radiusB) / (2.f * radiusB));
            uv_c = newV2( (c.x - center.x + radiusA) / (2.f * radiusA), (c.y - center.y + radiusB) / (2.f * radiusB));
        }

        else if(face_normal.y > 0.1f || face_normal.y < -0.1f ) {

            if(face_normal.y < 0.1f) {
                if((pointB.x - center.x) < -0.01f) multA = -1.f;
                else multB = -1.f;
            }

            c = newV3(cosf(degree) * radiusA * multA, center.y, sinf(degree) * radiusB * -multB);
            uv_b = newV2( (b.x - center.x + radiusA) / (2.f * radiusA), (b.z - center.z + radiusB) / (2.f * radiusB));
            uv_c = newV2( (c.x - center.x + radiusA) / (2.f * radiusA), (c.z - center.z + radiusB) / (2.f * radiusB));
        }

        else {

            if(face_normal.x < 0.1f) {
                if((pointC.y - center.y) < -0.01f) multA = -1.f;
                else multB = -1.f;
            }

            c = newV3(center.x, sinf(degree) * radiusB * multA, cosf(degree) * radiusA * -multB);
            uv_b = newV2( (b.z - center.z + radiusA) / (2.f * radiusA), (b.y - center.y + radiusB) / (2.f * radiusB));
            uv_c = newV2( (c.z - center.z + radiusA) / (2.f * radiusA), (c.y - center.y + radiusB) / (2.f * radiusB));
        }

        verts[(x*3)] = a;
        verts[(x*3)+1] = b;
        verts[(x*3)+2] = c;

        uvs[(x*3)] = uv_a;
        uvs[(x*3)+1] = uv_b;
        uvs[(x*3)+2] = uv_c;

        last_point = c;
        degree += degreeIteration;
    }

    int oldCount = obj->verticesCount;

    obj->verticesCount = obj->verticesCount + vertices_count;

    obj->vertices = realloc(obj->vertices, sizeof(GLfloat) * obj->verticesCount * 3);
    obj->normals = realloc(obj->normals, sizeof(GLfloat) * obj->verticesCount * 3);
    obj->uvs = realloc(obj->uvs, sizeof(GLfloat) * obj->verticesCount * 2);


    for(int i = oldCount; i < obj->verticesCount; i++) {

        int x = i - oldCount;

        obj->vertices[(i*3)] = verts[x].x;
        obj->vertices[(i*3)+1] = verts[x].y;
        obj->vertices[(i*3)+2] = verts[x].z;

        //printf("x:%f, y:%f, z:%f //",obj.vertices[(i*3)],obj.vertices[(i*3)+1],obj.vertices[(i*3)+2]);
//
//        if((x+1) % 3 == 0) {
//            printf("\n");
//        }

        obj->normals[(i*3)] = face_normal.x;
        obj->normals[(i*3)+1] = face_normal.y;
        obj->normals[(i*3)+2] = face_normal.z;

        obj->uvs[(i*2)] = uvs[x].x;
        obj->uvs[(i*2)+1] = uvs[x].y;
    }

	free(verts);
	free(uvs);

}

void makeRect(object *obj, v3 pointA, v3 pointB, v3 pointC, int subdivisionH, int subdivisionW, v2 uvStart, v2 uvEnd) {

    int layers_w = 1 + (subdivisionW * subdivisionW);
    int layers_h = 1 + (subdivisionH * subdivisionH);
    int rects = layers_w * layers_h;

    float x_range = uvEnd.x - uvStart.x;
    float y_range = uvEnd.y - uvStart.y;

    //v3 verts[rects * 6];
    //v2 uvs[rects * 6];

	v3 *verts = malloc(sizeof(v3) * rects * 6);
	v2 *uvs = malloc(sizeof(v2) * rects * 6);

    v3 dist_a_c = {.x = pointA.x - pointC.x, .y = pointA.y - pointC.y, .z = pointA.z - pointC.z};
    v3 dist_b_c = {.x = pointB.x - pointC.x, .y = pointB.y - pointC.y, .z = pointB.z - pointC.z};

    v3 face_normal = crossProduct(dist_a_c, dist_b_c);

    float normalize = dist(face_normal);
    face_normal.x = face_normal.x / normalize;
    face_normal.y = face_normal.y / normalize;
    face_normal.z = face_normal.z / normalize;

    v3 current_point;

    float change_x_x = (pointB.x - pointA.x) / (float) layers_w;
    float change_x_y = (pointB.y - pointA.y) / (float) layers_w;
    float change_x_z = (pointB.z - pointA.z) / (float) layers_w;
    float change_y_x = (pointC.x - pointA.x) / (float) layers_h;
    float change_y_y = (pointC.y - pointA.y) / (float) layers_h;
    float change_y_z = (pointC.z - pointA.z) / (float) layers_h;


    for(int x = 0; x < layers_h; x++) {

        current_point.x = pointA.x + (change_y_x * x);
        current_point.y = pointA.y + (change_y_y * x);
        current_point.z = pointA.z + (change_y_z * x);

        for(int y = 0; y < layers_w; y++) {

            v3 a = current_point;
            v3 b = { .x = a.x + change_x_x, .y = a.y + change_x_y, .z = a.z + change_x_z };
            v3 c = { .x = a.x + change_y_x, .y = a.y + change_y_y, .z = a.z + change_y_z };
            v3 d = { .x = b.x + change_y_x, .y = b.y + change_y_y, .z = b.z + change_y_z };

            v2 uv_a = { .x = ((float) y / (float) layers_w), .y = ((float) x / (float) layers_h) };
            v2 uv_b = { .x = ((float) (y+1) / layers_w), .y = ((float) x / (float) layers_h) };
            v2 uv_c = { .x = ((float) y / layers_w), .y = ((float) (x+1) / (float) layers_h) };
            v2 uv_d = { .x = ((float) (y+1) / layers_w), .y = ((float) (x+1) / (float) layers_h) };

            uv_a.x = (uv_a.x * x_range) + uvStart.x;
            uv_a.y = (uv_a.y * y_range) + uvStart.y;
            uv_b.x = (uv_b.x * x_range) + uvStart.x;
            uv_b.y = (uv_b.y * y_range) + uvStart.y;
            uv_c.x = (uv_c.x * x_range) + uvStart.x;
            uv_c.y = (uv_c.y * y_range) + uvStart.y;
            uv_d.x = (uv_d.x * x_range) + uvStart.x;
            uv_d.y = (uv_d.y * y_range) + uvStart.y;

            int index = (x * layers_w * 6) + (y * 6);

            verts[index] = a;
            verts[index+1] = b;
            verts[index+2] = c;

            verts[index+3] = b;
            verts[index+4] = d;
            verts[index+5] = c;

            uvs[index] = uv_a;
            uvs[index+1] = uv_b;
            uvs[index+2] = uv_c;

            uvs[index+3] = uv_b;
            uvs[index+4] = uv_d;
            uvs[index+5] = uv_c;

            current_point = b;
        }

    }

    int oldCount = obj->verticesCount;

    obj->verticesCount = obj->verticesCount + (rects * 6);

    obj->vertices = realloc(obj->vertices, sizeof(GLfloat) * obj->verticesCount * 3);
    obj->normals = realloc(obj->normals, sizeof(GLfloat) * obj->verticesCount * 3);
    obj->uvs = realloc(obj->uvs, sizeof(GLfloat) * obj->verticesCount * 2);


    for(int i = oldCount; i < obj->verticesCount; i++) {

        int x = i - oldCount;

        obj->vertices[(i*3)] = verts[x].x;
        obj->vertices[(i*3)+1] = verts[x].y;
        obj->vertices[(i*3)+2] = verts[x].z;

        //printf("x:%f, y:%f, z:%f //",obj.vertices[(i*3)],obj.vertices[(i*3)+1],obj.vertices[(i*3)+2]);

        obj->normals[(i*3)] = face_normal.x;
        obj->normals[(i*3)+1] = face_normal.y;
        obj->normals[(i*3)+2] = face_normal.z;

        obj->uvs[(i*2)] = uvs[x].x;
        obj->uvs[(i*2)+1] = uvs[x].y;
    }

	free(verts);
	free(uvs);
}

float invertChance = 0.5f;
bool noiseUp = true;

float randomizeNoise(float noise) {

    float randomA = frand(1.f);

    if(randomA <= invertChance) {
        noiseUp = !noise;
        invertChance = 0.5f;
    }

    else {
        invertChance *= 1.18f;
    }


    float randomB = frand(1.f);

    if(noiseUp) return noise * randomB;
    else return ((noise * 0.1f) * randomB);

}

void makeNoisyTriangle(object *obj, v3 pointA, v3 pointB, v3 pointC, int subdivision, float noise) {

    int triangle_layers = 1 + subdivision;
    int triangles_on_face = triangle_layers * triangle_layers;

    bool hasLastD = false;
    v3 lastD = newV3(0, 0, 0);

    int dIndex = 0;
    int dCount = 0;

    float d_points[subdivision-1];
    float previous_d_points[subdivision-1];

    v3 *verts = malloc(sizeof(v3) * triangles_on_face * 3);
    v2 *uvs = malloc(sizeof(v2) * triangles_on_face * 3);
    v3 *normals = malloc(sizeof(v3) * triangles_on_face * 3);
    v3 curr_point = pointA;
    v3 start_point = pointA;

    v3 dist_a = {.x = pointA.x - pointC.x, .y = pointA.y - pointC.y, .z = pointA.z - pointC.z};
    v3 dist_b = {.x = pointB.x - pointC.x, .y = pointB.y - pointC.y, .z = pointB.z - pointC.z};

    v3 face_normal = crossProduct(dist_a, dist_b);

    float normalize = dist(face_normal);
    face_normal.x = face_normal.x / normalize;
    face_normal.y = face_normal.y / normalize;
    face_normal.z = face_normal.z / normalize;

    int index = 0;

    float change_y_x = (pointC.x - pointA.x) / (float) triangle_layers;
    float change_y_y = (pointC.y - pointA.y) / (float) triangle_layers;
    float change_y_z = (pointC.z - pointA.z) / (float) triangle_layers;

    float change_x_x = (pointB.x - pointA.x) / (float) triangle_layers;
    float change_x_y = (pointB.y - pointA.y) / (float) triangle_layers;
    float change_x_z = (pointB.z - pointA.z) / (float) triangle_layers;

    int base_points = triangle_layers + 1;

    for(int l = triangle_layers; l > 0; l--) {

        curr_point = start_point;

        int num_points = l + 1;
        int num_triangles = (2 * num_points) - 3;
        int point_number = 0;
        float plus = abs(l - triangle_layers) / 2.f;

        dIndex = 0;
        dCount = 0;

        for(int i = 0; i < (subdivision-1); i++) {
            previous_d_points[i] = d_points[i];
            d_points[i] = 0.f;

            // printf("%f\n", previous_d_points[i]);
        }

        for(int t = num_triangles; t > 0; t-= 2) {

            v3 a = curr_point;
            v3 b = { .x = a.x + change_x_x, .y = a.y + change_x_y, .z = a.z + change_x_z };
            v3 c = { .x = a.x + change_y_x, .y = a.y + change_y_y, .z = a.z + change_y_z };

            v3 safeB = b;

            if(l < triangle_layers) {

                if(t < num_triangles) {
                    a.z = previous_d_points[dIndex-1];
                }

                if((t-2) > 0) {
                    b.z = previous_d_points[dIndex];
                    dIndex++;
                }


            }

            if(hasLastD) {
                hasLastD = false;

                if((t-2) > 0) c = lastD;
            }

            verts[index] = a;
            verts[index + 1] = b;
            verts[index + 2] = c;

            v3 dist_a = {.x = a.x - c.x, .y = a.y - c.y, .z = a.z - c.z};
            v3 dist_b = {.x = b.x - c.x, .y = b.y - c.y, .z = b.z - c.z};

            v3 face_normal = crossProduct(dist_a, dist_b);

            float normalize = dist(face_normal);
            face_normal.x = face_normal.x / normalize;
            face_normal.y = face_normal.y / normalize;
            face_normal.z = face_normal.z / normalize;

            normals[index] = face_normal;
            normals[index + 1] = face_normal;
            normals[index + 2] = face_normal;

            v2 uv_a = { .x = (float)(point_number + plus) / (base_points - 1), .y = fabsf(l - (float) triangle_layers) / triangle_layers };
            v2 uv_b = { .x = (float)(point_number + plus + 1) / (base_points - 1), .y = fabsf(l - (float) triangle_layers) / triangle_layers };
            v2 uv_c = { .x = (float)(point_number + plus + 0.5) / (base_points - 1), .y = fabsf(l - 1 - (float) triangle_layers) / triangle_layers };

            if(change_y_y < 0.f) {

                uv_a.x = fabsf(uv_a.x - 1.f);
                uv_a.y = fabsf(uv_a.y - 1.f);
                uv_b.x = fabsf(uv_b.x - 1.f);
                uv_b.y = fabsf(uv_b.y - 1.f);
                uv_c.x = fabsf(uv_c.x - 1.f);
                uv_c.y = fabsf(uv_c.y - 1.f);
            }

            uvs[index] = uv_a;
            uvs[index + 1] = uv_b;
            uvs[index + 2] = uv_c;

            index += 3;

            if(t > 1) {

                v3 d = { .x = safeB.x + change_y_x, .y = safeB.y + change_y_y, .z = safeB.z + change_y_z };

                //point D is not on border
                if((t-4) > 0) {
                    d.z = randomizeNoise(noise);

                    hasLastD = true;
                    lastD = d;

                    d_points[dCount] = d.z;
                    dCount++;
                }

                verts[index] = b;
                verts[index + 1] = d;
                verts[index + 2] = c;

                v3 dist_b2 = {.x = b.x - c.x, .y = b.y - c.y, .z = b.z - c.z};
                v3 dist_d = {.x = d.x - c.x, .y = d.y - c.y, .z = d.z - c.z};

                v3 face_normal = crossProduct(dist_b2, dist_d);

                float normalize = dist(face_normal);
                face_normal.x = face_normal.x / normalize;
                face_normal.y = face_normal.y / normalize;
                face_normal.z = face_normal.z / normalize;

                normals[index] = face_normal;
                normals[index + 1] = face_normal;
                normals[index + 2] = face_normal;

                v2 uv_d = { .x = (float)(point_number + 1.5 + plus) / (base_points - 1), .y = fabsf(l - 1 - (float) triangle_layers) / triangle_layers };

                if(change_y_y < 0.f) {

                    uv_d.x = fabsf(uv_d.x - 1.f);
                    uv_d.y = fabsf(uv_d.y - 1.f);
                }

                uvs[index] = uv_b;
                uvs[index + 1] = uv_d;
                uvs[index + 2] = uv_c;

                index += 3;
            }

            point_number++;

            curr_point.x += change_x_x;
            curr_point.y += change_x_y;
            curr_point.z += change_x_z;
        }

        start_point.x += change_y_x;
        start_point.y += change_y_y;
        start_point.z += change_y_z;
    }

    int oldCount = obj->verticesCount;

    obj->verticesCount = obj->verticesCount + (triangles_on_face * 3);

    obj->vertices = realloc(obj->vertices, sizeof(GLfloat) * obj->verticesCount * 3);
    obj->normals = realloc(obj->normals, sizeof(GLfloat) * obj->verticesCount * 3);
    obj->uvs = realloc(obj->uvs, sizeof(GLfloat) * obj->verticesCount * 2);


    for(int i = oldCount; i < obj->verticesCount; i++) {

        int x = i - oldCount;

        obj->vertices[(i*3)] = verts[x].x;
        obj->vertices[(i*3)+1] = verts[x].y;
        obj->vertices[(i*3)+2] = verts[x].z;

        obj->normals[(i*3)] = normals[x].x;
        obj->normals[(i*3)+1] = normals[x].y;
        obj->normals[(i*3)+2] = normals[x].z;

        obj->uvs[(i*2)] = uvs[x].x;
        obj->uvs[(i*2)+1] = uvs[x].y;
    }

    free(normals);
    free(verts);
    free(uvs);

}

void makeTriangle(object *obj, v3 pointA, v3 pointB, v3 pointC, int subdivision) {

    int triangle_layers = 1 + (subdivision * subdivision);
    int triangles_on_face = triangle_layers * triangle_layers;

    //v2 uvs[triangles_on_face * 3];
    //v3 verts[triangles_on_face * 3];

	v3 *verts = malloc(sizeof(v3) * triangles_on_face * 3);
	v2 *uvs = malloc(sizeof(v2) * triangles_on_face * 3);
    v3 curr_point = pointA;
    v3 start_point = pointA;

    v3 dist_a = {.x = pointA.x - pointC.x, .y = pointA.y - pointC.y, .z = pointA.z - pointC.z};
    v3 dist_b = {.x = pointB.x - pointC.x, .y = pointB.y - pointC.y, .z = pointB.z - pointC.z};

    v3 face_normal = crossProduct(dist_a, dist_b);

    float normalize = dist(face_normal);
    face_normal.x = face_normal.x / normalize;
    face_normal.y = face_normal.y / normalize;
    face_normal.z = face_normal.z / normalize;

    int index = 0;

    float change_y_x = (pointC.x - pointA.x) / (float) triangle_layers;
    float change_y_y = (pointC.y - pointA.y) / (float) triangle_layers;
    float change_y_z = (pointC.z - pointA.z) / (float) triangle_layers;

    float change_x_x = (pointB.x - pointA.x) / (float) triangle_layers;
    float change_x_y = (pointB.y - pointA.y) / (float) triangle_layers;
    float change_x_z = (pointB.z - pointA.z) / (float) triangle_layers;

    int base_points = triangle_layers + 1;

    for(int l = triangle_layers; l > 0; l--) {

        curr_point = start_point;

        int num_points = l + 1;
        int num_triangles = (2 * num_points) - 3;
        int point_number = 0;
        float plus = abs(l - triangle_layers) / 2.f;

        for(int t = num_triangles; t > 0; t-= 2) {

            v3 a = curr_point;
            v3 b = { .x = a.x + change_x_x, .y = a.y + change_x_y, .z = a.z + change_x_z };
            v3 c = { .x = a.x + change_y_x, .y = a.y + change_y_y, .z = a.z + change_y_z };

            verts[index] = a;
            verts[index + 1] = b;
            verts[index + 2] = c;

            v2 uv_a = { .x = (float)(point_number + plus) / (base_points - 1), .y = fabsf(l - (float) triangle_layers) / triangle_layers };
            v2 uv_b = { .x = (float)(point_number + plus + 1) / (base_points - 1), .y = fabsf(l - (float) triangle_layers) / triangle_layers };
            v2 uv_c = { .x = (float)(point_number + plus + 0.5) / (base_points - 1), .y = fabsf(l - 1 - (float) triangle_layers) / triangle_layers };

            if(change_y_y < 0.f) {

                uv_a.x = fabsf(uv_a.x - 1.f);
                uv_a.y = fabsf(uv_a.y - 1.f);
                uv_b.x = fabsf(uv_b.x - 1.f);
                uv_b.y = fabsf(uv_b.y - 1.f);
                uv_c.x = fabsf(uv_c.x - 1.f);
                uv_c.y = fabsf(uv_c.y - 1.f);
            }

            uvs[index] = uv_a;
            uvs[index + 1] = uv_b;
            uvs[index + 2] = uv_c;

            index += 3;

            if(t > 1) {

                v3 d = { .x = b.x + change_y_x, .y = b.y + change_y_y, .z = b.z + change_y_z };

                verts[index] = b;
                verts[index + 1] = d;
                verts[index + 2] = c;

                v2 uv_d = { .x = (float)(point_number + 1.5 + plus) / (base_points - 1), .y = fabsf(l - 1 - (float) triangle_layers) / triangle_layers };

                if(change_y_y < 0.f) {

                    uv_d.x = fabsf(uv_d.x - 1.f);
                    uv_d.y = fabsf(uv_d.y - 1.f);
                }

                uvs[index] = uv_b;
                uvs[index + 1] = uv_d;
                uvs[index + 2] = uv_c;

                index += 3;
            }

            point_number++;

            curr_point.x += change_x_x;
            curr_point.y += change_x_y;
            curr_point.z += change_x_z;
        }

        start_point.x += change_y_x;
        start_point.y += change_y_y;
        start_point.z += change_y_z;
    }

    int oldCount = obj->verticesCount;

    obj->verticesCount = obj->verticesCount + (triangles_on_face * 3);

    obj->vertices = realloc(obj->vertices, sizeof(GLfloat) * obj->verticesCount * 3);
    obj->normals = realloc(obj->normals, sizeof(GLfloat) * obj->verticesCount * 3);
    obj->uvs = realloc(obj->uvs, sizeof(GLfloat) * obj->verticesCount * 2);


    for(int i = oldCount; i < obj->verticesCount; i++) {

        int x = i - oldCount;

        obj->vertices[(i*3)] = verts[x].x;
        obj->vertices[(i*3)+1] = verts[x].y;
        obj->vertices[(i*3)+2] = verts[x].z;

        obj->normals[(i*3)] = face_normal.x;
        obj->normals[(i*3)+1] = face_normal.y;
        obj->normals[(i*3)+2] = face_normal.z;

        obj->uvs[(i*2)] = uvs[x].x;
        obj->uvs[(i*2)+1] = uvs[x].y;
    }

	free(verts);
	free(uvs);
}


object makeShapeObject(objEnum type, v3 radius, v4 color, objtexture *texture, GLenum usage, int subdivision) {


    subdivision = clamp(subdivision, 0, 10);

    object newObject = createNewObject(color, texture, GL_TRIANGLES, usage);

    //MARK: TRIANGLE
    if(type == TRIANGLE) {

        v3 pointA = {.x = -radius.x, .y = -radius.y, .z = 0.f};
        v3 pointB = {.x = radius.x, .y = -radius.y, .z = 0.f};
        v3 pointC = {.x = 0.f, .y = radius.y, .z = 0.f};

        makeTriangle(&newObject, pointA, pointB, pointC, subdivision);
    }

    //MARK: RECT
    else if(type == RECT) {

        v3 a = newV3(-radius.x, -radius.y, 0.f);
        v3 b = newV3( radius.x, -radius.y, 0.f);
        v3 c = newV3(-radius.x,  radius.y, 0.f);

        makeRect(&newObject, a, b, c, subdivision, subdivision, newV2(0.f, 0.f), newV2(1.f, 1.f));
    }

    //MARK: BOX
    else if(type == BOX) {

        makeRect(&newObject,
                 newV3(-radius.x, -radius.y,  -radius.z),
                 newV3( radius.x, -radius.y,  -radius.z),
                 newV3(-radius.x, -radius.y,   radius.z), subdivision, subdivision, newV2(0.f, 0.f), newV2(1.f, 1.f));
        makeRect(&newObject,
                 newV3( radius.x, -radius.y,  -radius.z),
                 newV3( radius.x,  radius.y,  -radius.z),
                 newV3( radius.x, -radius.y,   radius.z), subdivision, subdivision, newV2(0.f, 0.f), newV2(1.f, 1.f));
        makeRect(&newObject,
                 newV3( radius.x,  radius.y,  -radius.z),
                 newV3(-radius.x,  radius.y,  -radius.z),
                 newV3( radius.x,  radius.y,   radius.z), subdivision, subdivision, newV2(0.f, 0.f), newV2(1.f, 1.f));
        makeRect(&newObject,
                 newV3(-radius.x,  radius.y,  -radius.z),
                 newV3(-radius.x, -radius.y,  -radius.z),
                 newV3(-radius.x,  radius.y,   radius.z), subdivision, subdivision, newV2(0.f, 0.f), newV2(1.f, 1.f));
        makeRect(&newObject,
                 newV3(-radius.x, -radius.y,   radius.z),
                 newV3( radius.x, -radius.y,   radius.z),
                 newV3(-radius.x,  radius.y,   radius.z), subdivision, subdivision, newV2(0.f, 0.f), newV2(1.f, 1.f));
        makeRect(&newObject,
                 newV3(-radius.x,  radius.y,  -radius.z),
                 newV3( radius.x,  radius.y,  -radius.z),
                 newV3(-radius.x, -radius.y,  -radius.z), subdivision, subdivision, newV2(0.f, 0.f), newV2(1.f, 1.f));

    }

    //MARK: ELLIPSE
    else if(type == ELLIPSOID_2D) {

        makeCircle(&newObject, newV3(0.f, 0.f, 0.f), newV3(radius.x, 0.f, 0.f), newV3(0.f, radius.y, 0.f), subdivision);
    }


    //MARK: CAPSULE
    else if(type == CAPSULE) {

        int startIndex = 0;
        int points = 1 + (subdivision * subdivision) + 1;
        int total_per_triangle = (points-1) * (points-1) * 3;

        v3 center_up = newV3(0.f, 0.f, -radius.z - radius.x);
        v3 a_up = newV3(-radius.x, 0.f, -radius.z);
        v3 b_up = newV3(0.f, radius.y, -radius.z);
        v3 c_up = newV3(radius.x, 0.f, -radius.z);
        v3 d_up = newV3(0.f, -radius.y, -radius.z);

        v3 center_down = newV3(0.f, 0.f, radius.z + radius.x);
        v3 a_down = newV3(radius.x, 0.f, radius.z);
        v3 b_down = newV3(0.f, radius.y, radius.z);
        v3 c_down = newV3(-radius.x, 0.f, radius.z);
        v3 d_down = newV3(0.f, -radius.y, radius.z);

        makeTriangle(&newObject, a_down, b_down, center_down, subdivision);
        makeTriangle(&newObject, b_down, c_down, center_down, subdivision);
        makeTriangle(&newObject, c_down, d_down, center_down, subdivision);
        makeTriangle(&newObject, d_down, a_down, center_down, subdivision);
        roundVerticesToCenter(&newObject, newV3(0.f, 0.f, radius.z), newV3(radius.x, radius.y, radius.x), startIndex, newObject.verticesCount);
        fixRoundUVs(&newObject, startIndex, newObject.verticesCount, radius);

        v3 previous_point = newV3(newObject.vertices[0], newObject.vertices[1], newObject.vertices[2]);

        float min_x = 0.f, max_x = 0.f;

        for(int x = 0; x < 4; x++) {

            for(int y = 0; y < points-1; y++) {

                int index = (x * total_per_triangle) + (y * 6) + 1;

                min_x = (x * 0.5f) + (0.5f * (y) / (points - 1));
                max_x = (x * 0.5f) + (0.5f * (y+1) / (points - 1));

                v3 a = previous_point;
                v3 b = newV3(newObject.vertices[(index * 3)], newObject.vertices[(index * 3)+1], newObject.vertices[(index * 3)+2]);
                v3 c = newV3(b.x, b.y, -b.z);

                makeRect(&newObject, b, a, c, subdivision, 0, newV2(min_x, 0.f), newV2(max_x, 1.f));


                previous_point = b;
            }
        }

        startIndex = newObject.verticesCount;

        makeTriangle(&newObject, a_up, b_up, center_up, subdivision);
        makeTriangle(&newObject, b_up, c_up, center_up, subdivision);
        makeTriangle(&newObject, c_up, d_up, center_up, subdivision);
        makeTriangle(&newObject, d_up, a_up, center_up, subdivision);
        roundVerticesToCenter(&newObject, newV3(0.f, 0.f, -radius.z), newV3(radius.x, radius.y, radius.x), startIndex, newObject.verticesCount);
        fixRoundUVs(&newObject, startIndex, newObject.verticesCount, radius);

    }

    //MARK: CYLINDER
    else if(type == CYLINDER) {


        makeCircle(&newObject, newV3(0.f, 0.f, radius.z), newV3(radius.x, 0.f, radius.z), newV3(0.f, radius.y, radius.z), subdivision);
        makeCircle(&newObject, newV3(0.f, 0.f, -radius.z), newV3(radius.x, 0.f, -radius.z), newV3(0.f, -radius.y, -radius.z), subdivision);

        int count = newObject.verticesCount / 2;

        float min_x = 0.f, max_x = 0.f;

        for(int i = 0; i < count; i+= 3) {

            min_x = 1.f - (1.f * (i) / (float)(count/2));
            max_x = 1.f - (1.f * (i+1) / (float)(count/2));

            int indexA = (i + 1) * 3;

            v3 a = newV3(newObject.vertices[indexA], newObject.vertices[indexA+1], newObject.vertices[indexA+2]);
            v3 b = newV3(newObject.vertices[indexA+3], newObject.vertices[indexA+4], newObject.vertices[indexA+5]);
            v3 c = newV3(b.x, b.y, -b.z);

            makeRect(&newObject, b, a, c, subdivision, 0, newV2(min_x, 0.f), newV2(max_x, 1.f));
        }

    }

    //MARK: ELLIPSOID and OCTAHEDRON
    else if(type == ELLIPSOID_3D || type == OCTAHEDRON) {

        for(int f = 0; f < 8; f++) {

            v3 point_a = { .x = 0.f, .y = 0.f, .z = 0.f };
            v3 point_b = { .x = 0.f, .y = 0.f, .z = 0.f };
            v3 point_c = { .x = 0.f, .y = 0.f, .z = 0.f };

            switch (f) {
                case 0:
                    point_a.x = -radius.x;
                    point_b.z = radius.z;
                    point_c.y = radius.y;
                    break;
                case 1:
                    point_a.z = radius.z;
                    point_b.x = radius.x;
                    point_c.y = radius.y;
                    break;
                case 2:
                    point_a.x = radius.x;
                    point_b.z = -radius.z;
                    point_c.y = radius.y;
                    break;
                case 3:
                    point_a.z = -radius.z;
                    point_b.x = -radius.x;
                    point_c.y = radius.y;
                    break;
                case 4:
                    point_a.z = radius.z;
                    point_b.x = -radius.x;
                    point_c.y = -radius.y;
                    break;
                case 5:
                    point_a.x = radius.x;
                    point_b.z = radius.z;
                    point_c.y = -radius.y;
                    break;
                case 6:
                    point_a.z = -radius.z;
                    point_b.x = radius.x;
                    point_c.y = -radius.y;
                    break;
                case 7:
                    point_a.x = -radius.x;
                    point_b.z = -radius.z;
                    point_c.y = -radius.y;
                    break;
                default:
                    break;
            }

            makeTriangle(&newObject, point_a, point_b, point_c, subdivision);
        }

        v3 center = {.x = 0.f, .y = 0.f, .z = 0.f};

        if(type == ELLIPSOID_3D) {
            roundVerticesToCenter(&newObject, center, radius, 0, newObject.verticesCount);
            fixRoundUVs(&newObject, 0, newObject.verticesCount, radius);
        }
    }

    makeVBOSizeAndPush(&newObject);

    return newObject;
}
