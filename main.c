#include "model.h"

GLFWwindow *window;
int width, height, prevWidth, prevHeight;

int objectsCount = 0;
object *objectArray;

void createTerrain() {

    int size = sqrt(NUMPOINTS);
    int newSize = size - 2;

    siteObj sitesArray[newSize * newSize];
    objectArray = malloc(sizeof(object));

    for(int x = 1; x < (size-1); x++) {

        for(int y = 1; y < (size-1); y++) {

            sitesArray[(x-1) + ((y-1) * newSize)] = siteMeshes[x + (y * size)];

        }
    }

    for(int x = 0; x < newSize; x++) {

        for(int y = 0; y < newSize; y++) {

            siteObj site = sitesArray[x + (y * newSize)];

            for(int p = 0; p < site.numPoints; p++) {

                objectsCount++;

                v3 a,b,c;

                a = site.perimeter[p];
                c = site.center;

                c.x -= (TWIDTH/2.f);
                c.y -= (TWIDTH/2.f);
                c.z = 0.f;

                if(p+1 >= site.numPoints) b = site.perimeter[0];
                else b = site.perimeter[p+1];

                a.x = c.x + a.x;
                a.y = c.y + a.y;

                b.x = c.x + b.x;
                b.y = c.y + b.y;

                float red = randRange(0, 100) / 100.f;
                float green = randRange(0, 100) / 100.f;
                float blue = randRange(0, 100) / 100.f;

                object newObject = createNewObject(newV3(red, green, blue), NULL, GL_TRIANGLES, GL_DYNAMIC_DRAW);

                float accident = frand(1.f);

                makeNoisyTriangle(&newObject, a, b, c, 10, 10.f * accident);

                makeVBOSizeAndPush(&newObject);

                //v3 position = newV3(c.x - (TWIDTH/2.f), c.y - (TWIDTH/2.f), 0.f);

                //moveObjTo(&newObject, position);

                objectArray = realloc(objectArray, objectsCount * sizeof(object));
                objectArray[objectsCount-1] = newObject;



            }

        }
    }
}

int main() {
    srand((int)time(NULL));
    xOffset = yOffset = 0.f;
    zoom = WIDTH/10 + 50;
    //v3 camerapos = {0,0,zoom};

    double timepassed = 0.f;
    bool isPaused = true;
    double tickToPassTime = 1.f;

    width = WIDTH;
    height = HEIGHT;
    xAngle = -0.36f;
    yAngle = -0.8f;

    // Initialize GLFW
    if(!glfwInit()) {
        puts("Error loading GLFW.");
        return -1;
    }

    // Parametrize GLFW with Number of Samples(MultiSampling) and OpenGL Version
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Initialize window
    window = glfwCreateWindow(width, height, "PI VI", NULL, NULL);
    if(!window) {
        puts("Error opening GLFW window.");
        glfwTerminate();
        return -1;
    }

    // Set window as active, fix ratio and set Swap Interval(VSYNC)
    glfwMakeContextCurrent(window);
    glfwSetWindowAspectRatio(window, 16, 9);
    glfwSwapInterval(1);

    // Initialize GLEW
    if(glewInit() != GLEW_OK) {
        puts("Error loading GLEW.");
        glfwTerminate();
        return -1;
    }

    // Set Clear color
    glClearColor(0.153f, 0.067f, 0.200f, 0.f);

    glDisable(GL_DITHER | GL_MULTISAMPLE);
    glDisable(GL_DEPTH_CLAMP);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Enable face culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // Enable blend test
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create and compile our GLSL program from the shaders
    programID = loadShaders("vertex.shader", "fragment.shader");
    glUseProgram(programID);

    // Perspective Projection matrix
    float FOV = M_PI/4.f;
    float near = 0.5f;
    float far = 1000.f;
    float ratio = (float)width/height;
    m4x4 Projection = perspective(FOV, near, far, ratio);

    // Push Projection Matrix to GLSL
    GLuint matrixID = glGetUniformLocation(programID, "Projection");
    glUniformMatrix4fv(matrixID, 1, GL_TRUE, (GLfloat *)&Projection);

    // View Matrix
    m4x4 View = loadIdm4x4();
    GLuint ViewID = glGetUniformLocation(programID, "View");

    // Set Keys and Mouse functions callback on GLFW
    glfwSetKeyCallback(window, (void *)key_callback);
    glfwSetCursorPosCallback(window, (void *)cursor_position_callback);
    glfwSetScrollCallback(window, (void *)scroll_callback);


    uint32_t twidth = TWIDTH;
    objtexture tex;
    tex.buffer = (uint32_t *) malloc(sizeof(uint32_t)*twidth*twidth);
    tex.width = twidth;
    tex.height = twidth;
    tex.bufferSize = twidth * twidth;

    genVoronoiMap(tex.buffer, 0.75);
    printSitePoints();

    // Create Map
//    object map = makeShapeObject(RECT, (v3){WIDTH/20, WIDTH/20, 0.f}, (v3){1.f,1.f,1.f}, NULL,
//                                 GL_STATIC_DRAW, 0);
//    updateTexture(&map, &tex);

    createInitialEnvironment();
    createTerrain();


    //////////////////////////////////////////////////////////////////////////////
    //                                MAIN LOOP                                 //
    //////////////////////////////////////////////////////////////////////////////

    while(!glfwGetKey(window, GLFW_KEY_ESCAPE) &&
    !glfwWindowShouldClose(window)) {

        glfwGetWindowSize(window, &width, &height);
        glViewport(0,0, 2*width, 2*height);

        if(key_pressed[0]) {
            key_pressed[0] = false;
            timepassed = 0.f;
            isPaused = !isPaused;
        }

        if(key_pressed[1]) {
            key_pressed[1] = false;
            tickToPassTime = clamp(tickToPassTime - 0.25f, 0.01f, 3.f);
        }

        if(key_pressed[2]) {
            key_pressed[2] = false;
            tickToPassTime = clamp(tickToPassTime + 0.25f, 0.01f, 3.f);
        }

        // Get deltaTime and FPS
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if(!isPaused) {
            timepassed += deltaTime;

            if(timepassed > tickToPassTime) {

                timepassed = 0.f;
                timePasses();

            }
        }

        // Zoom View Matrix
        // Rotate and Zoom View Matrix
        v3 cameraPos = {0, 0, zoom};
        cameraPos = rotate(cameraPos, (v3){1,0,0}, xAngle+M_PI/2);
        cameraPos = rotate(cameraPos, (v3){0,0,1}, yAngle);
        View = lookAt(cameraPos,(v3){0,0,0},(v3){0,0,1}); // NOTE: camera is ALWAYS UPRIGHT!!!

        printf("X:%f e Y:%f\n", xAngle, yAngle);

        // Update View Matrix
        glUniformMatrix4fv(ViewID, 1, GL_TRUE, (GLfloat *)&View);

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw Objects
        for(int i = 0; i < objectsCount; i++) {
            drawObject(&objectArray[i]);
        }
        // drawSites();

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glDeleteProgram(programID);

    // Free objects
    for(int i = 0; i < objectsCount; i++) {
        freeObject(&objectArray[i]);
    }

    for(int i = 0; i < NUMPOINTS; ++i) {
        free(siteMeshes[i].perimeter);
    }
    freeSites();

    // Free textobjects

    // Close window
    glfwTerminate();

    return 0;
}
