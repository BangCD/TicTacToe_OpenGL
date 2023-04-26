#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
//#include<debugapi.h>
//#include <GL/glew.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <glm.hpp>
#include <vec3.hpp>
#include <Windows.h>
#include<debugapi.h>
#include <tuple>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//#include <debugapi.h>

using namespace std;

std::vector<glm::vec3> vertices; //

std::vector<unsigned int> indices;

// GameState

int gameState[3][3] =
{
    {0,0,0},
    {0,0,0},
    {0,0,0}
};

double boardBoundary[3][3][4] =
{
    { {0,0,0,0},{0,0,0,0},{0,0,0,0} },
    { {0,0,0,0},{0,0,0,0},{0,0,0,0} },
    { {0,0,0,0},{0,0,0,0},{0,0,0,0} }

};

double xoPlacement[3][3][2] = {
    {{0,0} ,{0,0},{0,0}},
    {{0,0} ,{0,0},{0,0}},
    {{0,0} ,{0,0},{0,0}}
};




void updateBoardBoundary(int c, int r, double x1, double y1, double x2, double y2) {



    if (x1 > x2) {
        boardBoundary[c][r][0] = x2;
        boardBoundary[c][r][1] = x1;
    }
    else {
        boardBoundary[c][r][0] = x1;
        boardBoundary[c][r][1] = x2;
    }

    if (y1 > y2) {
        boardBoundary[c][r][2] = y2;
        boardBoundary[c][r][3] = y1;
    }
    else {
        boardBoundary[c][r][2] = y1;
        boardBoundary[c][r][3] = y2;
    }

    double m1 = (x1 + x2) / 2;
    double m2 = (y1 + y2) / 2;

    xoPlacement[c][r][0] = m1;
    xoPlacement[c][r][1] = m2;




    cout << "game boundary" << endl;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 4; k++) {
                cout << " " << boardBoundary[i][j][k];
            }
            cout << "\t";
        }
        cout << endl;
    }

    cout << "XO Placement" << endl;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 2; k++) {
                cout << " " << xoPlacement[i][j][k];
            }
            cout << "\t";
        }
        cout << endl;
    }

}




void updateGameState(int c, int r,int XorO) {
    cout << "update state called" << endl;
    gameState[c][r] = XorO;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cout << "\t" << gameState[i][j];
        }
        cout << endl;
    }
}



// object id variables
unsigned int vertexBuffer;
unsigned int vertexArray;
unsigned int elementBuffer;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec3 color;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(color, 1.0f);\n"
"}\n\0";



typedef struct vec2 {
    float x;
    float y;
} vec2;

vec2* vec2_init(float _x, float _y) {
    vec2* v = (vec2*)malloc(sizeof(vec2));
    v->x = _x;
    v->y = _y;
    return v;
}

typedef struct vec3 {
    float x;
    float y;
    float z;
} vec3;

vec3* vec3_init(float _x, float _y, float _z) {
    vec3* v = (vec3*)malloc(sizeof(vec3));
    v->x = _x;
    v->y = _y;
    v->z = _z;
    return v;
}

typedef struct line {
    int shaderProgram;
    unsigned int VBO, VAO;      //vertex buffer object , vertex array object 
    float* vertices;
    vec2* startPoint;
    vec2* endPoint;
    vec3* lineColor;
} line;

line* line_init(vec2* start, vec2* end) {

    line* l = (line*)malloc(sizeof(line));

    float x1 = start->x;
    float y1 = start->y;
    float x2 = end->x;
    float y2 = end->y;


    // convert 3d world space position 2d screen space position
    //x1 = 2 * x1 / w - 1;
    //y1 = 2 * y1 / h - 1;

    //x2 = 2 * x2 / w - 1;
    //y2 = 2 * y2 / h - 1;

    start->x = x1;
    start->y = y1;
    end->x = x2;
    end->y = y2;

    l->startPoint = start;
    l->endPoint = end;
    l->lineColor = vec3_init(1.0f, 1.0f, 1.0f);

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // link shaders
    l->shaderProgram = glCreateProgram();
    glAttachShader(l->shaderProgram, vertexShader);
    glAttachShader(l->shaderProgram, fragmentShader);
    glLinkProgram(l->shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // setting vertex data
    l->vertices = (float*)(malloc(sizeof(float) * 6));
    l->vertices[0] = start->x;
    l->vertices[1] = start->y;
    l->vertices[2] = end->x;
    l->vertices[3] = end->y;

    glGenVertexArrays(1, &l->VAO);
    glGenBuffers(1, &l->VBO);
    glBindVertexArray(l->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, l->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(l->vertices) * 4,
        &l->vertices[0],
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
        2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return l;
}


int line_draw(line* l) {

    glUseProgram(l->shaderProgram);
    //glUniform3fv(glGetUniformLocation(l->shaderProgram, "color"),
    //    1, &l->lineColor->x);

    glBindVertexArray(l->VAO);
    glDrawArrays(GL_LINES, 0, 2);
    return 0;
}


void buildCircle(float radius, int vCount,float pointX,float pointY)
{
    vertices.clear();
    indices.clear();
    float angle = 360.0f / vCount;

    int triangleCount = vCount - 2;

    std::vector<glm::vec3> temp;
    // positions
    for (int i = 0; i < vCount; i++)
    {
        float currentAngle = angle * i;
        float x = pointX + radius * cos(glm::radians(currentAngle));
        float y = pointY + radius * sin(glm::radians(currentAngle));
        float z =  0.0f;

        vertices.push_back(glm::vec3(x, y, z));
    }

    // push indexes of each triangle points
    for (int i = 0; i < triangleCount; i++)
    {
        indices.push_back(0);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
    }

    glGenVertexArrays(1, &vertexArray);

    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &elementBuffer);

    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

 

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

}



void buildX(float pointX, float pointY)
{

    line* line5 = line_init(vec2_init(pointX+0.1, pointY+0.1), vec2_init(pointX - 0.1, pointY - 0.1));
    line* line6 = line_init(vec2_init(pointX - 0.1, pointY + 0.1), vec2_init(pointX + 0.1, pointY - 0.1));
    line_draw(line5);
    line_draw(line6);




}





bool centerFlag = false;
bool centerRight = false;

bool flag1 = false;
double x = 0, y = 0;
double xpos, ypos = 0;
void mouse_button(GLFWwindow* window, int button, int action, int mods)
{

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        //Converting window coordinates to openGL coords
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        x = (xpos / width) * 2 - 1;
        y = (1 - ypos / height) * 2 - 1;
        char out2[256];
        snprintf(out2, sizeof(out2), "%G , %G \n", x, y);
        OutputDebugStringA(out2);
 
 

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if ((x > boardBoundary[i][j][0] && x < boardBoundary[i][j][1]) && (y > boardBoundary[i][j][2] && y < boardBoundary[i][j][3])) 
                {
                    gameState[i][j] = 1;
                    
                }
            }
        }




    }


    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        x = (xpos / width) * 2 - 1;
        y = (1 - ypos / height) * 2 - 1;
        cout << "right mouse pressed" << endl;


        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if ((x > boardBoundary[i][j][0] && x < boardBoundary[i][j][1]) && (y > boardBoundary[i][j][2] && y < boardBoundary[i][j][3]))
                {
                    gameState[i][j] = 2;

                }
            }
        }
       
    }


}



int main(int argc, char** argv)
{
    // initialize GLFW
    if (!glfwInit())
    {
        return -1;
    }

    // create a window pointer
    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello Circle", NULL, NULL);
    // error check
    if (window == NULL)
    {
        cout << "Error. I could not create a window at all!" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }
    
    glfwSetMouseButtonCallback(window, mouse_button);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

  


    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    int shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragmentShader);
    glLinkProgram(shaderProg);

    line* line1 = line_init(vec2_init(0.7, 0.4), vec2_init(-0.7, 0.4)); 
    line* line2 = line_init(vec2_init(0.3, 0.8), vec2_init(0.3, -0.8));
    line* line3 = line_init(vec2_init(-0.3, 0.8), vec2_init(-0.3, -0.8));
    line* line4 = line_init(vec2_init(0.7, -0.4), vec2_init(-0.7, -0.4));

    updateBoardBoundary(0, 2, 0.7, 0.4, 0.3, 0.8);  //Start of L1 & Start of L2 [top right box]
    updateBoardBoundary(0, 0, -0.7, 0.4, -0.3, 0.8); // Start of L3 & End of L1 [top left box]
    updateBoardBoundary(2, 2, 0.7, -0.4, 0.3, -0.8); //Start of L4 & End L2 [bot right]
    updateBoardBoundary(2, 0, -0.7, -0.4, -0.3, -0.8); //End of L4 & End L3 [Bot left]

    updateBoardBoundary(0, 1, -0.3, 0.4, 0.3, 0.8); // Start of L2 & Intersection of L3 , L1 [top mid]
    updateBoardBoundary(1, 0, -0.7, -0.4, -0.3, 0.4); // End of L4 & Intersection of L3 , L1 [Left mid]
    updateBoardBoundary(1, 2, 0.7, 0.4,0.3, -0.4); // Start of L4 & Intersection of L4 , L2 [right mid]
    updateBoardBoundary(2, 1, -0.3, -0.4, 0.3, -0.8); // End of L2 & Intersection of L3 , L4 [bot mid]
    updateBoardBoundary(1, 1, -0.3, 0.4, 0.3, -0.4); // Intersection of L1,L2 & L4,L3 [mid]







    while (!glfwWindowShouldClose(window))
    {
        glfwWaitEvents();
        glClearColor(255.0f, 255.0f, 255.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);



        line_draw(line1);
        line_draw(line2);
        line_draw(line3);
        line_draw(line4);


        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (gameState[i][j] == 1) {
                    buildCircle(0.1, 20, xoPlacement[i][j][0], xoPlacement[i][j][1]);
                }
                if (gameState[i][j] == 2) {
                    buildX(xoPlacement[i][j][0], xoPlacement[i][j][1]);
                }
            
            
            }
        }

        glUseProgram(shaderProg);

        glBindVertexArray(vertexArray);

        glfwSwapBuffers(window);

    }
}