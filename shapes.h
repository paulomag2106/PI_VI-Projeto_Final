#pragma once
#include "graphics.h"

object makeShapeObject(objEnum type, v3 radius, v3 color, objtexture *texture, GLenum usage, int subdivision);
void makeRect(object *obj, v3 pointA, v3 pointB, v3 pointC, int subdivisionH, int subdivisionW, v2 uvStart, v2 uvEnd);
void makeVBOSizeAndPush(object *newObject);
