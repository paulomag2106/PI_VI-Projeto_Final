#pragma once
#include "shapes.h"

#define FONT_SEQUENCE "ABCDEFGHIJKLMNOPQRSTUVWXYZ()!?abcdefghijklmnopqrstuvwxyz*+-/0123456789@#$&|\\.,:;$[]{}<>^_="
//#define FONT_SEQUENCE "=_^><}{][%;:,.\\|&$#@9876543210/-+*zyxwvutsrqponmlkjihgfedcba?!)(ZYXWVUTSRQPONMLKJIHGFEDCBA"
#define FONT_THIN " OSabcdeghknopqsuvxyz?"
#define FONT_VERY_THIN "fjtr"
#define FONT_VERY_VERY_THIN "Ii!l|.,;:"
#define FONT_THIN_MULT 0.7
#define FONT_VERY_THIN_MULT 0.4
#define FONT_VERY_VERY_THIN_MULT 0.3
#define FONT_SIZE_MULT 0.075
#define TEXT_MAX_LINES 10
#define W_TO_H_RATIO 1.112195122
#define FONT_X 10
#define FONT_Y 9
#define FONT_SIZE 90

//bool font_loaded = false;
objtexture *font_texture;
float character_width;
float character_height;
float uvWidth;

typedef struct t_txtobj {
    
    char *text;
    int textCount;
    object obj;
    
} textobject;

typedef enum t_spacement {
    TEXT_LOOSE,
    TEXT_NORMAL,
    TEXT_COMPACT
} TextSpacement;

typedef enum t_alignment {
    TEXT_CENTER,
    TEXT_LEFT,
    TEXT_RIGHT,
    TEXT_JUSTIFIED
} TextAlignment;

textobject makeTextObject(const char *text, float textWidth, int fontSize, TextSpacement spacement, TextAlignment alignment, v3 textColor);
void freeFont(void);
void freeTextObject(textobject *obj);

