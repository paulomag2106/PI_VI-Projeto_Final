#include "text.h"

void loadFont() {
    
    objtexture new = decodePNGtoTexture("roboto-font.png");
    font_texture = malloc(sizeof(objtexture));
    *font_texture = new;
}

void freeFont() {
    
    if(font_texture != NULL) free(font_texture->buffer);
}

void freeTextObject(textobject *obj) {
    
    freeObject(&obj->obj);
    font_texture = NULL;
}

char *getNextStringEndedWith(char *str, char end) {
    
    if(strlen(str) <= 0) {
        return NULL;
    }
    
    int index = -1;
    
    for(int i = 0; i < strlen(str); i++) {
        
        if(str[i] == end) {
            index = i+1;
            break;
        }
    }
    
    if(index < 0) index = (int)strlen(str);
    
    int endIndex = (int)(strlen(str) - index + 1);
    
    char *result = malloc(sizeof(char) * (index+1));
    char *new = malloc(sizeof(char) * endIndex);
    
    for(int c = 0; c < strlen(str); c++) {
        
        if(c < index) result[c] = str[c];
        else new[c-index] = str[c];
    }
    
    result[index] = '\0';
    new[endIndex-1] = '\0';
    strcpy(str, new);

    free(new);
    
    return result;
}

int findCharacterOnIndex(char c) {
    
    if(c == ' ') {
        return -1;
    }
    
    for (int x = 0; x < (FONT_X * FONT_Y); x++) {
        if(c == FONT_SEQUENCE[x]) {
            return x;
        }
    }
    
    return -1;
}

bool isThin(char c) {
    
    for (int x = 0; x < strlen(FONT_THIN); x++) {
        if(c == FONT_THIN[x]) {
            return true;
        }
    }
    
    return false;
}

bool isVeryThin(char c) {
    
    for (int x = 0; x < strlen(FONT_VERY_THIN); x++) {
        if(c == FONT_VERY_THIN[x]) {
            return true;
        }
    }
    
    return false;
}

bool isVeryVeryThin(char c) {
    
    for (int x = 0; x < strlen(FONT_VERY_VERY_THIN); x++) {
        if(c == FONT_VERY_VERY_THIN[x]) {
            return true;
        }
    }
    
    return false;
    
}

float getStringWidth(char *str) {
    
    float width = 0.f;
    
    for(int c = 0; c < strlen(str); c++) {
        
        if(isThin(str[c])) width += character_width * FONT_THIN_MULT;
        else if (isVeryThin(str[c])) width += character_width * FONT_VERY_THIN_MULT;
        else if (isVeryVeryThin(str[c])) width += character_width * FONT_VERY_VERY_THIN_MULT;
        else width += character_width;
        
    }
    
    return width;
}


object makeNewLine(object obj, int number, char *text, v2 limiters) {
    
    v3 pointA = newV3(limiters.x, 0.f, -character_height * number);
    v3 pointB = newV3(limiters.x, 0.f, -character_height * number);
    v3 pointC = newV3(limiters.x, 0.f, -character_height * (number + 1));
    
    float xChange, current_uv_width;
    
    if(isThin(text[0])) {
        pointA.x += character_width * FONT_THIN_MULT;
        pointC.x += character_width * FONT_THIN_MULT;
        xChange = character_width * FONT_THIN_MULT;
    }
    
    else if(isVeryThin(text[0])) {
        pointA.x += character_width * FONT_VERY_THIN_MULT;
        pointC.x += character_width * FONT_VERY_THIN_MULT;
        xChange = character_width * FONT_VERY_THIN_MULT;
    }
    
    else if(isVeryVeryThin(text[0])) {
        pointA.x += character_width * FONT_VERY_VERY_THIN_MULT;
        pointC.x += character_width * FONT_VERY_VERY_THIN_MULT;
        xChange = character_width * FONT_VERY_VERY_THIN_MULT;
    }
    
    else {
        pointA.x += character_width;
        pointC.x += character_width;
        xChange = character_width;
    }
    
    for(int c = 0; c < strlen(text); c++) {
        
        int value = findCharacterOnIndex(text[c]);
        
        current_uv_width = (xChange * uvWidth) / character_width;
        char str[2] = {text[c+1]};
        xChange = getStringWidth(str);
        
//        if(isThin(text[c+1])) xChange = character_width * FONT_THIN_MULT;
//        //current_uv_width = (xChange * uvWidth) / character_width;
//        else if(isVeryThin(text[c+1])) xChange = character_width * FONT_VERY_THIN_MULT;
//        //current_uv_width = uvWidth * FONT_VERY_THIN_MULT;
//        else xChange = character_width;
//        //current_uv_width = uvWidth;
        
        
        if(value < 0) {
            makeRect(&obj, pointA, pointB, pointC, 0, 0, newV2(0.f, 0.f), newV2(0.f, 0.f));
        }
        
        else {
            
            int x = (value % FONT_X);
            int y = FONT_Y - (value / FONT_X);
            
            v2 uv_start = newV2((float)x / (float)FONT_X, (float)(y-1) / (float)FONT_Y);
            v2 uv_end = newV2((float)(x+1) / (float)FONT_X, (float)y / (float)FONT_Y);
            
            uv_start.x = uv_start.x + ( ( (1.f / FONT_X) - current_uv_width) / 2.f);
            uv_end.x = uv_start.x + current_uv_width;
            
            makeRect(&obj, pointA, pointB, pointC, 0, 0, uv_end, uv_start);
        }
        
        //printf("%f vs %f at %d\n", getStringWidth(str), xChange, c);
        
        pointB.x = pointA.x;
        pointA.x += xChange;
        pointC.x += xChange;
    }
    
    return obj;
}


textobject makeTextObject(const char *text, float textWidth, int fontSize, TextSpacement spacement, TextAlignment alignment, v3 textColor) {
    
    object obj;
    obj.usage = GL_DYNAMIC_DRAW;
    obj.drawMode = GL_TRIANGLES;
    
    obj.vertices = malloc(sizeof(GLfloat));
    obj.normals = malloc(sizeof(GLfloat));
    obj.color = textColor;
    obj.uvs = malloc(sizeof(GLfloat));
    obj.verticesCount = 0;
    
    int textCount = (int) strlen(text);
    
    if(!font_texture) loadFont();
    
    obj.tex = font_texture;
    
    switch (spacement) {
        case TEXT_NORMAL:
            uvWidth = (1.f / FONT_X) / 1.35f;
            break;
        case TEXT_LOOSE:
            uvWidth = 1.f / FONT_X;
            break;
        case TEXT_COMPACT:
            uvWidth = (1.f / FONT_X) / 1.6f;
            break;
    }
    
    character_height = fontSize * FONT_SIZE_MULT;
    character_width = character_height / W_TO_H_RATIO;
    
    
    char *text_copy = calloc(strlen(text), sizeof(char));
    strcpy(text_copy, text);
    
    char *lines[TEXT_MAX_LINES];
    char *line = calloc(strlen(text), sizeof(char));
    int number_lines = 1;
    char *sub_line = getNextStringEndedWith(text_copy, ' ');

    printf("line is %s\n", line);
    
    while(1) {
        
        float current_width = getStringWidth(line) + getStringWidth(sub_line);
        
        if(current_width > textWidth) {
            lines[number_lines-1] = calloc(strlen(line), sizeof(char));
            strcpy(lines[number_lines-1], line);
            number_lines++;
            for(int c = 0; c < strlen(line); c++) {
                line[c] = 0;
            }
        }
        
        strcat(line, sub_line);
        sub_line = getNextStringEndedWith(text_copy, ' ');
        printf("line is %s\n", line);
        
        if(!sub_line) {
            lines[number_lines-1] = calloc(strlen(line), sizeof(char));
            strcpy(lines[number_lines-1], line);
            
            break;
        }
    }
    
    free(sub_line);
    free(line);
    
    printf("result:%s\n", lines[0]);
    
    for(int current = 0; current < number_lines; current++) {
        
        char *current_line = lines[current];
        float line_width = getStringWidth(current_line);
        float x_start, x_end;
        
        switch (alignment) {
            case TEXT_LEFT:
                x_start = (-textWidth / 2.f);
                x_end = line_width;
                break;
            case TEXT_RIGHT:
                x_start = (textWidth / 2.f) - line_width;
                x_end = textWidth;
                break;
            case TEXT_CENTER:
                x_start = (-textWidth / 2.f) + ((textWidth - line_width) / 2.f);
                x_end = x_start + line_width;
                break;
            case TEXT_JUSTIFIED:
                x_start = (-textWidth / 2.f);
                x_end = (textWidth / 2.f);
                break;
        }
        
        obj = makeNewLine(obj, current, current_line, newV2(x_start, x_end));
        free(current_line);
    }
    
    
    makeVBOSizeAndPush(&obj);
    
    textobject new = { (char *)text, textCount, obj };
    
    
    return new;
}

