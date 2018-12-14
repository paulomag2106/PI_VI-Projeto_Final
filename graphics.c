#include "graphics.h"

GLuint loadShaders(const char *vertex_file_name, const char *fragment_file_name) {

    FILE *vertexFile = fopen(vertex_file_name, "r");
    FILE *fragmentFile = fopen(fragment_file_name, "r");


    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint result = GL_FALSE;
    int InfoLogLength;

    // Read Vertex Shader file
    size_t vertex_file_size;
    fseek(vertexFile, 0, SEEK_END);
    vertex_file_size = ftell(vertexFile);
    rewind(vertexFile);
    GLchar *vertexShaderContent = (GLchar *)calloc((vertex_file_size + 1), sizeof(GLchar));
    fread(vertexShaderContent, sizeof(char), vertex_file_size, vertexFile);
    fclose(vertexFile);

    printf("Compiling shader : %s\n", vertex_file_name);
    glShaderSource(VertexShaderID, 1, (const char *const *)&vertexShaderContent, NULL);
    glCompileShader(VertexShaderID);

    // Read Fragment Shader file
    size_t fragment_file_size;
    fseek(fragmentFile, 0, SEEK_END);
    fragment_file_size = ftell(fragmentFile);
    rewind(fragmentFile);
    GLchar *fragmentShaderContent = (GLchar *)calloc((fragment_file_size + 1), sizeof(GLchar));
    fread(fragmentShaderContent, sizeof(char), fragment_file_size, fragmentFile);
    fclose(fragmentFile);

    printf("Compiling shader : %s\n", fragment_file_name);
    glShaderSource(FragmentShaderID, 1, (const char *const *)&fragmentShaderContent, NULL);
    glCompileShader(FragmentShaderID);

    // Check vertex shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if(InfoLogLength > 0) {
        char *VertexShaderErrorMessage = calloc(InfoLogLength + 1, sizeof(char));
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
        free(VertexShaderErrorMessage);
    }

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
    	char *FragmentShaderErrorMessage = calloc(InfoLogLength + 1, sizeof(char));
    	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    	printf("%s\n", &FragmentShaderErrorMessage[0]);
    free(FragmentShaderErrorMessage);
    }

    // Link program
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    free(vertexShaderContent);
    free(fragmentShaderContent);

    return ProgramID;
}

object loadOBJModel(const char *file_name) {

    //Open file
    FILE *OBJFile = fopen(file_name, "r");

    //Check success
    if(OBJFile == NULL) {
        printf("Failed to open the file %s.\n", file_name);
        fclose(OBJFile);
        object nul;
        nul.drawMode = GL_TRIANGLES;
        return nul;
    }

    //Create arrays for vertices, normals and uv mapping
    v3 *vertex_array = malloc(sizeof(*vertex_array));
    v3 *normal_array = malloc(sizeof(*normal_array));
    v2 *uv_array = malloc(sizeof(*uv_array));

    //Create arrays for indexes used for every face
    unsigned int *vertex_index_array = malloc(sizeof(*vertex_index_array));
    unsigned int *uv_index_array = malloc(sizeof(*uv_index_array));
    unsigned int *normal_index_array = malloc(sizeof(*normal_index_array));

    //Count of vertices, normals, uvs, indexes, normal indexes, and lines readed.
    int v = 0, n = 0, u = 0, i = 0, line_count = 0;

    //Size and string used for getline()
    size_t size;
    char *line = calloc(500, sizeof(char));

    // ** Reading loop ** //
    while(1) {


        line_count++;

#if defined(_WIN32)
        if(getNextLine(line, 500, OBJFile) < 0) {
            break;
        }
#else
        //Check if getline() reached EOF
        if(getline(&line, &size, OBJFile) < 0) {
            break;
        }
#endif

        // *** Check if line starts with: "vt", "vn", "v", "f" *** //
        if(strncmp(line, "vt", strlen("vt")) == 0) {
            v2 uv;
            sscanf(line, "vt %f %f\n", &uv.x, &uv.y);

            if(u > 0) {
                uv_array = realloc(uv_array, (u+1) * sizeof(*uv_array));
            }

            uv_array[u] = uv;
            u++;

        }

        else if(strncmp(line, "vn", strlen("vn")) == 0) {
            v3 normal;
            sscanf(line, "vn %f %f %f\n", &normal.x, &normal.y, &normal.z);

            if(n > 0) {
                normal_array = realloc(normal_array, (n+1) * sizeof(*normal_array));
            }

            normal_array[n] = normal;
            n++;
        }

        else if(strncmp(line, "v", strlen("v")) == 0) {
            v3 vertex;

            sscanf(line, "v %f %f %f\n", &vertex.x, &vertex.y, &vertex.z);

            if(v > 0) {
                vertex_array = realloc(vertex_array, (v+1) * sizeof(*vertex_array));
            }

            vertex_array[v] = vertex;
            v++;
        }

        else if(strncmp(line, "f", strlen("f")) == 0) {

            char *sub_line;
            unsigned int vertex_index, uv_index = 1, normal_index = 1;

            //Split line in strings separated by " "
            sub_line = strtok(line, " ");


            while(sub_line != NULL) {

                if(sscanf(sub_line,  "%d/%d/%d", &vertex_index, &uv_index, &normal_index) == 3) {

                    if(i > 0) {
                        vertex_index_array = realloc(vertex_index_array, (i+1) * sizeof(*vertex_index_array));
                        uv_index_array = realloc(uv_index_array, (i+1) * sizeof(*uv_index_array));
                        normal_index_array = realloc(normal_index_array, (i+1) * sizeof(*normal_index_array));
                    }

                    vertex_index_array[i] = vertex_index;
                    uv_index_array[i] = uv_index;
                    normal_index_array[i] = normal_index;

                    i++;

                }

                else if(sscanf(sub_line, "%d//%d", &vertex_index, &normal_index) == 2) {

                    if(i > 0) {
                        vertex_index_array = realloc(vertex_index_array, (i+1) * sizeof(*vertex_index_array));
                        uv_index_array = realloc(uv_index_array, (i+1) * sizeof(*uv_index_array));
                        normal_index_array = realloc(normal_index_array, (i+1) * sizeof(*normal_index_array));
                    }

                    vertex_index_array[i] = vertex_index;
                    uv_index_array[i] = uv_index;
                    normal_index_array[i] = normal_index;

                    i++;

                }

                else if(sscanf(sub_line, "%d/%d", &vertex_index, &normal_index) == 2) {

                    if(i > 0) {
                        vertex_index_array = realloc(vertex_index_array, (i+1) * sizeof(*vertex_index_array));
                        uv_index_array = realloc(uv_index_array, (i+1) * sizeof(*uv_index_array));
                        normal_index_array = realloc(normal_index_array, (i+1) * sizeof(*normal_index_array));
                    }

                    vertex_index_array[i] = vertex_index;
                    uv_index_array[i] = uv_index;
                    normal_index_array[i] = normal_index;

                    i++;

                }

                sub_line = strtok(NULL, " ");
            }

            free(sub_line);
        }
    }

    free(line);

    int total = (i * 3);

    //Arrays for drawing
    //GLfloat vertices[total], uvs[i * 2], normals[total];

    GLfloat *vertices = malloc(sizeof(GLfloat) * total);
    GLfloat *normals = malloc(sizeof(GLfloat) * total);
    GLfloat *uvs = malloc(sizeof(GLfloat) *i * 2);

    //Pass the faces informations to the arrays
    for(int x = 0; x < i; x++) {

        int v_index = vertex_index_array[x] - 1, u_index = uv_index_array[x] - 1, n_index = normal_index_array[x] - 1;

        if(v_index >= v || (u_index >= u && u > 0) || v_index < 0 || u_index < 0 || n_index >= n || n_index < 0) {
            printf("WARNING: Some index is beyond bounds.\n");
            printf("vertex index: %d, uv index: %d, normal index: %d \n", v_index, u_index, n_index);
        }

        else {
            vertices[(x * 3)] = vertex_array[v_index].x;
            vertices[(x * 3) + 1] = vertex_array[v_index].y;
            vertices[(x * 3) + 2] = vertex_array[v_index].z;

            uvs[(x * 2)] = uv_array[u_index].x;
            uvs[(x * 2) + 1] = uv_array[u_index].y;

            normals[(x * 3)] = normal_array[n_index].x;
            normals[(x * 3) + 1] = normal_array[n_index].y;
            normals[(x * 3) + 2] = normal_array[n_index].z;
        }
    }


    object newObject;
    newObject.usage = GL_STATIC_DRAW;
    newObject.drawMode = GL_TRIANGLES;
    newObject.tex = NULL;

    newObject.vboSize[0] = sizeof(GLfloat) * total; // vertices buffer size
    newObject.vertices = malloc(newObject.vboSize[0]);
    memcpy(newObject.vertices, vertices, newObject.vboSize[0]);

    newObject.vboSize[1] = sizeof(GLfloat) * total; // normals buffer size
    newObject.normals = malloc(newObject.vboSize[1]);
    memcpy(newObject.normals, normals, newObject.vboSize[1]);


    newObject.vboSize[2] = sizeof(GLfloat) * (i * 2); // uvs buffer size
    newObject.uvs = malloc(newObject.vboSize[2]);
    memcpy(newObject.uvs, uvs, newObject.vboSize[2]);

    newObject.color = (v4){1.f, {1.f, 1.f, 1.f}};

    free(vertex_array);
    free(normal_array);
    free(uv_array);
    free(vertex_index_array);
    free(uv_index_array);
    free(normal_index_array);
    free(vertices);
    free(uvs);
    free(normals);

    fclose(OBJFile);

    pushObject(&newObject);

    return newObject;
}

void updateTexture(object *newObject, objtexture *tex) {

    if(newObject->tex) {
        if(newObject->tex->buffer) {
            free(newObject->tex->buffer);
        }
    }

    glBindVertexArray(newObject->vao);
    newObject->tex = tex;

    if(newObject->tex) {
        glBindTexture(GL_TEXTURE_2D, (GLuint)newObject->vbo[4]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, newObject->tex->width, newObject->tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, newObject->tex->buffer);
        // NOTE: Must pass texture from RGBA to BGRA so R G and B are read in this order
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        uint32_t texcolor = 0xffffffff;
        glBindTexture(GL_TEXTURE_2D, (GLuint)newObject->vbo[4]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texcolor);
        // NOTE: Must pass texture from RGBA to BGRA so R G and B are read in this order
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindVertexArray(0);
}

void pushObject(object *newObject) {
    glGenVertexArrays(1, &newObject->vao);
    glBindVertexArray(newObject->vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    //glEnableVertexAttribArray(3);

    // Generate, bind and attribute vertex buffer
    if(newObject->vboSize[0] > 0) {
        glGenBuffers(1, &newObject->vbo[0]);
        glBindBuffer(GL_ARRAY_BUFFER, newObject->vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, newObject->vboSize[0], newObject->vertices, newObject->usage);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    // Generate, bind and attribute normals
    if(newObject->vboSize[1] > 0) {
        glGenBuffers(1, &newObject->vbo[1]);
        glBindBuffer(GL_ARRAY_BUFFER, newObject->vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, newObject->vboSize[1], newObject->normals, newObject->usage);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    // Generate, bind and attribute UVs
    if(newObject->vboSize[2] > 0) {
        glGenBuffers(1, &newObject->vbo[2]);
        glBindBuffer(GL_ARRAY_BUFFER, newObject->vbo[2]);
        glBufferData(GL_ARRAY_BUFFER, newObject->vboSize[2], newObject->uvs, newObject->usage);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    // Generate, bind and attribute position
    newObject->position = (v3){0,0,0};
    newObject->uniform[0] = glGetUniformLocation(programID, "pos");
    glUniform3fv(newObject->uniform[0],1,(GLfloat *)&newObject->position);

    // Generate, bind and attribute scale
    newObject->scale = (v3){1,1,1};
    newObject->uniform[1] = glGetUniformLocation(programID, "scale");
    glUniform3fv(newObject->uniform[1],1,(GLfloat *)&newObject->scale);

    // Generate, bind and attribute rotation
    newObject->rotation = (v4){0,{0,0,0}};
    newObject->uniform[2] = glGetUniformLocation(programID, "rotation");
    glUniform4fv(newObject->uniform[2],1,(GLfloat *)&newObject->rotation);

    newObject->uniform[3] = glGetUniformLocation(programID, "color");
    glUniform4fv(newObject->uniform[3],1,(GLfloat *)&newObject->color);

    if(newObject->tex) {
        glGenTextures(1, &newObject->vbo[4]);
        glBindTexture(GL_TEXTURE_2D, (GLuint)newObject->vbo[4]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, newObject->tex->width, newObject->tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, newObject->tex->buffer);
        // NOTE: Must pass texture from RGBA to BGRA so R G and B are read in this order
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        uint32_t color = 0xffffffff;

        glGenTextures(1, &newObject->vbo[4]);
        glBindTexture(GL_TEXTURE_2D, (GLuint)newObject->vbo[4]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_BGRA, GL_UNSIGNED_BYTE, &color);

        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, newObject->tex.width, newObject->tex.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, newObject->tex.buffer);
        // NOTE: Must pass texture from RGBA to BGRA so R G and B are read in this order
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);


    }

    glBindVertexArray(0);
}

void drawObject(object *toDraw) {
    glBindVertexArray(toDraw->vao);
    glUniform3fv(toDraw->uniform[0],1,(GLfloat *)&toDraw->position);
    glUniform3fv(toDraw->uniform[1],1,(GLfloat *)&toDraw->scale);
    glUniform4fv(toDraw->uniform[2],1,(GLfloat *)&toDraw->rotation);
    glUniform4fv(toDraw->uniform[3],1,(GLfloat *)&toDraw->color);
    glBindTexture(GL_TEXTURE_2D, (GLuint)toDraw->vbo[4]);
    glDrawArrays(toDraw->drawMode, 0,  (toDraw->vboSize[0]/(3*sizeof(GLfloat)) ));
    glBindTexture(GL_TEXTURE_2D,0);
    glBindVertexArray(0);
}

void freeObject(object *toFree) {
    glBindVertexArray(toFree->vao);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    free(toFree->vertices);
    free(toFree->normals);
    free(toFree->uvs);

    if(toFree->tex != NULL) if(toFree->tex->buffer != NULL) free(toFree->tex->buffer);

    glBindVertexArray(0);
}

object createNewObject(v4 color, objtexture *texture, GLenum drawMode, GLenum usage) {

    object obj;
    obj.vertices = malloc(sizeof(GLfloat));
    obj.uvs = malloc(sizeof(GLfloat));
    obj.normals = malloc(sizeof(GLfloat));
    obj.usage = usage;
    obj.tex = texture;
    obj.drawMode = drawMode;
    obj.color = color;
    obj.verticesCount = 0;

    return obj;
}

objtexture decodePNGtoTexture(const char* filename) {
    unsigned error;
    unsigned char* image = 0;
    unsigned width = 0, height = 0;
    unsigned char* png = 0;
    size_t pngsize;

    error = lodepng_load_file(&png, &pngsize, filename);
    if(!error) error = lodepng_decode32(&image, &width, &height, png, pngsize);
    if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

    free(png);


    int fixed_index = 0;
    int pixel_size = 4;
    unsigned char *image_fixed = malloc(sizeof(char *) * width * height * pixel_size);


    for(int h = 0; h < height; h++) {

        for(int w = 0; w < width; w++) {

            int index = (height - h - 1) * width * pixel_size;
            index += w * pixel_size;

            image_fixed[fixed_index] = image[index];
            image_fixed[fixed_index+1] = image[index+1];
            image_fixed[fixed_index+2] = image[index+2];
            image_fixed[fixed_index+3] = image[index+3];

            fixed_index += 4;
        }

    }

    objtexture new = { image_fixed, 0, width, height };

    return new;
}

void moveObjTo(object *obj, v3 position) {
    obj->position = position;
}

void moveObjBy(object *obj, v3 offset) {
    v3 currentPos = obj->position;
    obj->position = (v3){
        currentPos.x + offset.x,
        currentPos.y + offset.y,
        currentPos.z + offset.z,
    };
}

void rotateObjTo(object *obj, v3 axis, float angle) {
    obj->rotation = (v4) {
        cosf(angle/2.f), scalarProduct(axis, sinf(angle/2.f))
    };
}

void rotateObjBy(object *obj, v3 axis, float angle) {
    // to implement
}

void scaleObjTo(object *obj, v3 scale) {
    obj->scale = scale;
}

void scaleObjBy(object *obj, v3 offset) {
    obj->scale = (v3) {
        obj->scale.x * offset.x,
        obj->scale.y * offset.y,
        obj->scale.z * offset.z,
    };
}
