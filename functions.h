#pragma once
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <time.h>


#define WIDTH 1280
#define HEIGHT 720
#define MAX_FPS 60.f
#define cotan(a) 1.f/tan(a)
#define frand(a) (((float)rand()/(float)(RAND_MAX)) * a)
#define randRange(a, b) (a + (rand()%(b-a+1)))
#define min(a,b) (a > b ? b : a)
#define max(a,b) (a < b ? b : a)
#define lerp(a, t, b) (t <= 0 ? a : t >= 1 ? b : ((1.f-t)*a) + (t*b))
#define clamp(value, a, b) (value < a ? a : value > b ? b : value)

#if defined(__APPLE__)
    #define clear() printf("\033[H\033[J")
#elif defined(_WIN32)
    #define clear() 24
#elif defined(__linux__)
    #define clear() 42
#endif


float xOffset;
float yOffset;
float yAngle, xAngle;
bool leftClick;

#define MAXSPD 3.f

double oldx, oldy, mouseX, mouseY; // for orbiting
double zoom; // for zooming camera

double old;
double deltaTime, lastTime;
float direction;

//SPACE - Z - X - C - V
bool key_pressed[5];

GLuint VAO;
typedef struct t_v2i {
    int32_t x, y;
} v2i;

typedef struct t_v2 {
    float x, y;
} v2;

typedef struct t_v3i {
    uint32_t x, y, z;
} v3i;

typedef struct t_v3 {
    float x, y, z;
} v3;

// NOTE: For quaternions
typedef struct t_v4 {
    float w;
    v3 vec;
} v4;

// NOTE: This is a _ROW_ major representation of matrix 4 by 4
typedef struct t_m4x4 {
    float m11, m12, m13, m14,
          m21, m22, m23, m24,
          m31, m32, m33, m34,
          m41, m42, m43, m44;
} m4x4;

// Floats
float dist(v3 vector);
float distV2(v2 vector);
float sqDist(float x, float y);
float dotProduct(v3 a, v3 b);
float points_distance(v3 pointA, v3 pointB);

// Ints
uint32_t intSqDist(int x, int y);
uint32_t intManDist(int x, int y);

// Vector 2
v2 newV2(float x, float y);

// Vector 3
v3 newV3(float x, float y, float z);
v3 addV3(v3 a, v3 b);
v3 scalarProduct(v3 a, float b);
v3 crossProduct(v3 a, v3 b);
v3 rotate(v3 a, v3 b, float theta);
v3 v3Lerp(v3 a, float t, v3 b);
v3 normalize(v3 vector);

// Vector 4
v4 quaternionMult(v4 a, v4 b);

// Matrices 4 by 4
m4x4 multm4x4(m4x4 a, m4x4 b);
m4x4 rotAngleX(float angle);
m4x4 rotAngleY(float angle);
m4x4 loadIdm4x4(void);
m4x4 orthographic(float near, float far, float right, float left,
                  float top, float bottom);
m4x4 perspective(float FOV, float near, float far, float ratio);
m4x4 lookAt(v3 eye, v3 target, v3 up);
m4x4 transpose(m4x4 matrix);


// Voids
void moveToV3(float *data, int size, v3 pos);
void moveModel(m4x4 a, v3 b);
void rotateModel(m4x4 a, v3 b, float theta);
void scaleModel(m4x4 a, v3 b);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursor_position_callback(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void blockFramerate(void);

int getNextLine(char *line, int size, FILE *file);
