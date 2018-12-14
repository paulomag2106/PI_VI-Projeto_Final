#include "functions.h"

float distV2(v2 vector) {
    return sqrt((vector.x*vector.x) + (vector.y*vector.y));
}

float dist(v3 vector) {
    return sqrt((vector.x*vector.x) + (vector.y*vector.y) + (vector.z*vector.z));
}

float sqDist(float x, float y) {
    return x*x + y*y;
}

// NOTE: For textures
uint32_t intSqDist(int x, int y) {
    return x*x + y*y;
}
uint32_t intManDist(int x, int y) {
    return abs(x) + abs(y);
}

v2 newV2(float x, float y) {

    v2 new = {.x = x, .y = y};
    return new;
}


v3 newV3(float x, float y, float z) {

    v3 new = {.x = x, .y = y, .z = z};
    return new;
}

v3 addV3(v3 a, v3 b) {
    return (v3){a.x + b.x,
                a.y + b.y,
                a.z + b.z};
}

v3 scalarProduct(v3 a, float b) {
    return (v3){a.x * b,
                a.y * b,
                a.z * b};
}

float dotProduct(v3 a, v3 b) {
  return (a.x * b.x) +
         (a.y * b.y) +
         (a.z * b.z);
}

v3 crossProduct(v3 a, v3 b) {
  return (v3){(a.y*b.z) - (a.z*b.y),
              (a.z*b.x) - (a.x*b.z),
              (a.x*b.y) - (a.y*b.x)};
}

v3 v3Lerp(v3 a, float t, v3 b) {
  return (v3) {lerp(a.x, t, b.x),
               lerp(a.y, t, b.y),
               lerp(a.z, t, b.z)};
}

v3 normalize(v3 vector) {
    float norm = dist(vector);
    return (v3) {
        vector.x / norm, vector.y / norm, vector.z / norm
    };
}

v4 quaternionMult(v4 a, v4 b) {
    // q = (s, v3)
    float sA = a.w;
    v3 vA = a.vec;
    float sB = b.w;
    v3 vB = b.vec;

    // q v3
    v3 qV3 = addV3(scalarProduct(vB,sA), scalarProduct(vA,sB));
    qV3 = addV3(qV3, crossProduct(vA,vB));

    return (v4){(sA*sB) - dotProduct(vA, vB), {qV3.x, qV3.y, qV3.z}};
}

v3 rotate(v3 a, v3 b, float theta) {
    float qS = cosf(theta/2.f); // Real Part of quaternion and q^-1
    v3 qN = scalarProduct(b, sinf(theta/2.f)); // Vector(Imaginary) Part of quaternion

    v4 q = {qS, {qN.x, qN.y, qN.z}};
    v4 qi = {qS, {-qN.x, -qN.y, -qN.z}};
    v4 q_a = {0, {a.x, a.y, a.z}};

    v4 result = quaternionMult(q, quaternionMult(q_a, qi));
    return result.vec;
}

m4x4 multm4x4(m4x4 a, m4x4 b) {
    m4x4 result;

    // First row
    result.m11 = (a.m11*b.m11) + (a.m12*b.m21) + (a.m13*b.m31) + (a.m14*b.m41);
    result.m12 = (a.m11*b.m12) + (a.m12*b.m22) + (a.m13*b.m32) + (a.m14*b.m42);
    result.m13 = (a.m11*b.m13) + (a.m12*b.m23) + (a.m13*b.m33) + (a.m14*b.m43);
    result.m14 = (a.m11*b.m14) + (a.m12*b.m24) + (a.m13*b.m34) + (a.m14*b.m44);

    // Second row
    result.m21 = (a.m21*b.m11) + (a.m22*b.m21) + (a.m23*b.m31) + (a.m24*b.m41);
    result.m22 = (a.m21*b.m12) + (a.m22*b.m22) + (a.m23*b.m32) + (a.m24*b.m42);
    result.m23 = (a.m21*b.m13) + (a.m22*b.m23) + (a.m23*b.m33) + (a.m24*b.m43);
    result.m24 = (a.m21*b.m14) + (a.m22*b.m24) + (a.m23*b.m34) + (a.m24*b.m44);

    // Third row
    result.m31 = (a.m31*b.m11) + (a.m32*b.m21) + (a.m33*b.m31) + (a.m34*b.m41);
    result.m32 = (a.m31*b.m12) + (a.m32*b.m22) + (a.m33*b.m32) + (a.m34*b.m42);
    result.m33 = (a.m31*b.m13) + (a.m32*b.m23) + (a.m33*b.m33) + (a.m34*b.m43);
    result.m34 = (a.m31*b.m14) + (a.m32*b.m24) + (a.m33*b.m34) + (a.m34*b.m44);

    // Fourth row
    result.m41 = (a.m41*b.m11) + (a.m42*b.m21) + (a.m43*b.m31) + (a.m44*b.m41);
    result.m42 = (a.m41*b.m12) + (a.m42*b.m22) + (a.m43*b.m32) + (a.m44*b.m42);
    result.m43 = (a.m41*b.m13) + (a.m42*b.m23) + (a.m43*b.m33) + (a.m44*b.m43);
    result.m44 = (a.m41*b.m14) + (a.m42*b.m24) + (a.m43*b.m34) + (a.m44*b.m44);

    return result;
}

// Euler Angles rotation
m4x4 rotAngleX(float angle) {
    return (m4x4) {
        1,           0,            0, 0,
        0, cosf(angle), -sinf(angle), 0,
        0, sinf(angle),  cosf(angle), 0,
        0,           0,            0, 1
    };
}

m4x4 rotAngleY(float angle) {
    return (m4x4) {
        cosf(angle), 0, sinf(angle), 0,
                  0, 1,           0, 0,
       -sinf(angle), 0, cosf(angle), 0,
                  0, 0,           0, 1
    };
}

m4x4 loadIdm4x4() {
    return (m4x4) {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

m4x4 orthographic(float near, float far, float right, float left, float top, float bottom) {

    float A =  2.f/(right-left);
    float B =  2.f/(top-bottom);
    float C = -2.f/(far-near);
    float D = -(right+left)/(right-left);
    float E = -(top+bottom)/(top-bottom);
    float F = -(far+near)/(far-near);

    m4x4 result = {
        A, 0, 0, D,
        0, B, 0, E,
        0, 0, C, F,
        0, 0, 0, 1
    };

    return result;

}

m4x4 perspective(float FOV, float near, float far, float ratio) {
    float n = near;
    float f = far;

    float A = (1.f/(ratio)) * cotan(FOV/2.f);
    float B = cotan(FOV/2);
    // float C = -(n+f)/(n-f);
    // float D = 2*(f*n)/(n-f);
    float C = -(f+n)/(f-n);
    float D = (-2*f*n)/(f-n);


    m4x4 result = {
        A, 0, 0, 0,
        0, B, 0, 0,
        0, 0, C, D,
        0, 0, -1, 0
    };

    return result;
}

m4x4 lookAt(v3 eye, v3 target, v3 up) {
    // camera Z axis
    v3 forward = (v3) {
        eye.x - target.x, eye.y - target.y, eye.z - target.z
    };
    forward = normalize(forward);

    // camera X axis
    v3 right = crossProduct(up, forward);
    right = normalize(right);

    // camera Y axis
    v3 newUp = crossProduct(forward, right);

    // camera Position
    v3 newEye = (v3) {
        -eye.x,
        -eye.y,
        -eye.z
    };
    newEye = (v3) {
        dotProduct(right, newEye),
        dotProduct(newUp, newEye),
        dotProduct(forward, newEye)
    };

    // Final View Matrix
    return (m4x4) {
          right.x,   right.y,   right.z, newEye.x,
          newUp.x,   newUp.y,   newUp.z, newEye.y,
        forward.x, forward.y, forward.z, newEye.z,
                0,         0,         0,        1
    };

}

m4x4 transpose(m4x4 matrix) {
    return (m4x4) {
        matrix.m11, matrix.m21, matrix.m31, matrix.m41,
        matrix.m12, matrix.m22, matrix.m32, matrix.m42,
        matrix.m13, matrix.m23, matrix.m33, matrix.m43,
        matrix.m14, matrix.m24, matrix.m34, matrix.m44
    };
}

void moveToV3(float *data, int size, v3 pos) {
    for(int i = 0; i < size;) {
        data[i++] += pos.x;
        data[i++] += pos.y;
        data[i++] += pos.z;
    }
}

float points_distance(v3 pointA, v3 pointB) {

    return sqrtf((fabsf(pointA.x - pointB.x) * fabsf(pointA.x - pointB.x)) +
                 (fabsf(pointA.y - pointB.y) * fabsf(pointA.y - pointB.y)) +
                 (fabsf(pointA.z - pointB.z) * fabsf(pointA.z - pointB.z)));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    // int vertical = 0;
    // int horizontal = 0;
//    bool d = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
//    bool w = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
//    bool a = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
//    bool s = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
//
//    if(s) {
//        yOffset = lerp(yOffset, 0.25, -MAXSPD);
//    } else if(w) {
//        yOffset = lerp(yOffset, 0.25, MAXSPD);
//    } else {
//        yOffset = 0;
//    }
//
//    if(a) {
//        xOffset = lerp(xOffset, 0.25, -MAXSPD);
//    } else if(d) {
//        xOffset = lerp(xOffset, 0.25, MAXSPD);
//    } else {
//        xOffset = 0;
//    }
    //
     if(key == GLFW_KEY_SPACE && action == GLFW_RELEASE) key_pressed[0] = true;
     if(key == GLFW_KEY_Q && action == GLFW_RELEASE) key_pressed[1] = true;
     if(key == GLFW_KEY_E && action == GLFW_RELEASE) key_pressed[2] = true;
//     if(key == GLFW_KEY_C && action == GLFW_RELEASE) key_pressed[3] = true;
//     if(key == GLFW_KEY_V && action == GLFW_RELEASE) key_pressed[4] = true;

//    printf("%d\n", (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE));
//
//    key_pressed[0] = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE;
//    key_pressed[1] = glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE;
//    key_pressed[2] = glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE;
//    key_pressed[3] = glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE;
//    key_pressed[4] = glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE;
//
//    printf("%d%d%d%d%d\n", key_pressed[0],key_pressed[1],key_pressed[2],key_pressed[3],key_pressed[4]);

}

void cursor_position_callback(GLFWwindow* window) {

    if(!(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)) {
        leftClick = false;
    }

    if(leftClick) {

        double oldx = mouseX;
        double oldy = mouseY;

        glfwGetCursorPos(window, &mouseX, &mouseY);
//        int width, height;
//        glfwGetWindowSize(window, &width, &height);

        double deltax = mouseX - oldx;
        double deltay = mouseY - oldy;

        const float speed = 1.f;

        xAngle += speed * -deltay * deltaTime;
        yAngle += speed * -deltax * deltaTime;

        //xAngle = clamp(xAngle, -(M_PI-0.1f)/2, (M_PI-0.1f)/2);

//        if(deltax > 0) {
//            yAngle += speed * deltay * deltaTime;
//        } else if(deltax < 0) {
//            yAngle += (float)(height/3)/(height*M_PI*2);
//        }
//
//        if(deltay < 0) {
//            xAngle += (float)(width/3)/(width*M_PI*2);
//        } else if(deltay > 0) {
//            xAngle -= (float)(width/3)/(width*M_PI*2);
//        }
//
//        // NOTE: Don't allow X camera angle to go OVER the -PI/2 to 0 degrees, with
//        // small margin of 0.05 radians so object won't disappear from view
       xAngle = clamp(xAngle, -(M_PI-0.1f)/2, -0.05f);

    }

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        leftClick = true;
        glfwGetCursorPos(window, &mouseX, &mouseY);
    }

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    zoom += yoffset;
    zoom = clamp(zoom, 15.f, WIDTH/10 +500);
}

void blockFramerate() {
    if (old == 0) {
        old = glfwGetTime();
    }
    while (glfwGetTime() - old < 1.0 / MAX_FPS) {
    }
    old = glfwGetTime();
}

int getNextLine(char *line, int size, FILE *file) {

    int count = 0;


    while(count < size) {

        char c = getc(file);
        line[count] = c;

        if(c == '\n') {
            break;
        }

        else if(c == EOF) {
            line[count + 1] = '\0';
            return -1;
        }

        count++;
    }

    if(count >= size) line[size - 1] = '\0';
    else line[count + 1] = '\0';

    return 0;
}
