#pragma once
// #include "functions.h"
#include "texGen.h"
#include "lodepng.h"
// #include "physics.h"

GLuint programID;

typedef struct t_texture {

    void *buffer;
    int bufferSize;
    int width;
    int height;

} objtexture;

typedef enum t_camEnum {
  ORTHOGRAPHIC,
  PERSPECTIVE
} camEnum;

typedef struct t_camera {
  m4x4 matrixID[2]; // Projection, View
  m4x4 projection, view;
  camEnum type;
} camera;

typedef enum t_lightEnum {
  POINT,
  DIRECTIONAL
} lightEnum;

typedef struct t_light {
  float intensity; // use 0 to 1 values
  v3 position, color;
  v4 rotation;
  lightEnum type;
} light;

typedef enum t_objEnum {
    TRIANGLE,
    RECT,
    BOX,
    ELLIPSOID_2D,
    ELLIPSOID_3D,
    CYLINDER,
    CAPSULE,
    OCTAHEDRON
} objEnum;

typedef struct t_object {
    GLuint vao;
    GLuint vbo[5]; // vertexBuffer, normalsBuffer, uvBuffer, colorBuffer, texBuffer
    GLuint uniform[4]; // position, scale, rotation, color
    GLuint vboSize[3];
    GLenum usage;
    GLenum drawMode;
    GLfloat *vertices,
            *normals,
            *uvs;
    v3 position, scale;
    v4 rotation; // quaternion
    v3 color;

    int verticesCount;
    objtexture *tex;
} object;

// GLuints
GLuint loadShaders(const char *vertex_file_name, const char *fragment_file_name);

// Voids
void pushObject(object *newObject);
void drawObject(object *toDraw);
void freeObject(object *toFree);
void updateTexture(object *newObject, objtexture *tex);

// Objects
object createNewObject(v3 color, objtexture *texture, GLenum drawMode, GLenum usage);
object loadOBJModel(const char *file_name);

// Manipulate Objects
void moveObjTo(object *obj, v3 position);
void moveObjBy(object *obj, v3 offset);
void rotateObjTo(object *obj, v3 axis, float angle);
void rotateObjBy(object *obj, v3 axis, float angle);
void scaleObjTo(object *obj, v3 scale);
void scaleObjBy(object *obj, v3 offset);

// Texture
objtexture decodePNGtoTexture(const char* filename);
