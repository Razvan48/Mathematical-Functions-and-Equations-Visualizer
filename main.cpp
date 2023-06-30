#include <iostream>
#include <vector>

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using namespace std;

const double WINDOW_WIDTH = 1024.0;
const double WINDOW_HEIGHT = 1024.0;

const char* vertexShaderSource =
"#version 330 core \n"
"\n"
"layout (location = 0) in vec2 vertexPosition; \n"
"uniform mat4 ortho; \n"
"\n"
"void main() \n"
"{ \n"
"\n"
"   gl_Position = ortho * vec4(vertexPosition.x, vertexPosition.y, 0.0, 1.0); \n"
"\n"
"} \n"
"\0";

const char* fragmentShaderSource =
"#version 330 core \n"
"\n"
"out vec4 vertexColour; \n"
"uniform vec3 colour; \n"
"\n"
"void main() \n"
"{ \n"
"\n"
"   vertexColour = vec4(colour, 1.0); \n"
"\n"
"} \n"
"\0";

unsigned int VAO;
unsigned int VBO;

double currentTime;
double previousTime;
double deltaTime;

class Camera
{
public:

    double x;
    double y;

    double movementSpeed;
    double zoomSpeed;

    double scale;

    const double MAX_SCALE = 1000.0;
    const double MIN_SCALE = 1.0;

    Camera(double x, double y, double movementSpeed, double zoomSpeed, double scale) :
        x(x), y(y), movementSpeed(movementSpeed), zoomSpeed(zoomSpeed), scale(scale) {}

    ~Camera() {};

private:

};

void updateDeltaTime()
{
    currentTime = glfwGetTime();
    deltaTime = currentTime - previousTime;
    previousTime = currentTime;
}

int POINTS = 1000;

int POINTS_X = 1024;
int POINTS_Y = 1024;

double EPSILON_EQUATION = 0.001;

void handleInput(GLFWwindow* window, Camera* camera)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->y += camera->movementSpeed * camera->scale / 50.0 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->y -= camera->movementSpeed * camera->scale / 50.0 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->x -= camera->movementSpeed * camera->scale / 50.0 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->x += camera->movementSpeed * camera->scale / 50.0 * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        camera->scale -= camera->zoomSpeed * camera->scale / 10.0 * deltaTime;
        camera->scale = max(camera->scale, camera->MIN_SCALE);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        camera->scale += camera->zoomSpeed * camera->scale / 10.0 * deltaTime;
        camera->scale = min(camera->scale, camera->MAX_SCALE);
    }

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        POINTS -= 10;
        POINTS = max(POINTS, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        POINTS += 10;
        POINTS = min(POINTS, 10000);
    }

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    {
        POINTS_X -= 10;
        POINTS_X = max(POINTS_X, 0);

        POINTS_Y -= 10;
        POINTS_Y = max(POINTS_Y, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        POINTS_X += 10;
        POINTS_X = min(POINTS_X, 10000);

        POINTS_Y += 10;
        POINTS_Y = min(POINTS_Y, 10000);
    }

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        EPSILON_EQUATION -= 0.01;

        EPSILON_EQUATION = max(EPSILON_EQUATION, 0.0);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        EPSILON_EQUATION += 0.01;

        EPSILON_EQUATION = min(EPSILON_EQUATION, 10000.0);
    }
}

bool isPrime(long long x)
{
    for (long long i = 2; i * i <= x; i++)
        if (x % i == 0) return false;

    return true;
}

//const double EPSILON = 0.01;

//x * sin(x) * cos(x)
//sqrt(sin(x)* x);
//sqrt(x) + x * sin(x) - x * cos(x);


double function(double x)
{
    return x;
}

vector<double> vertices;
vector<double> origin;
vector<double> axes;

void drawLine(double x1, double y1, double x2, double y2)
{
    vertices.emplace_back(x1);
    vertices.emplace_back(y1);

    vertices.emplace_back(x2);
    vertices.emplace_back(y2);
}

void drawOrigin(double x, double y)
{
    origin.emplace_back(x);
    origin.emplace_back(y);
}

void drawAxes(double x1, double y1, double x2, double y2)
{
    axes.emplace_back(x1);
    axes.emplace_back(y1);

    axes.emplace_back(x2);
    axes.emplace_back(y2);
}

int colourPath;

void drawFunction(Camera* camera)
{
    axes.clear();
    vertices.clear();
    origin.clear();

    double distXWindow = WINDOW_WIDTH / (1.0 * POINTS - 1.0);

    double crtXWindow = -WINDOW_WIDTH / 2.0;
    double antXWindow = -WINDOW_WIDTH / 2.0;

    for (int i = 1; i <= POINTS; i++)
    {
        double crtX = crtXWindow / (WINDOW_WIDTH / 2.0) * camera->scale + camera->x;
        double antX = antXWindow / (WINDOW_WIDTH / 2.0) * camera->scale + camera->x;

        double crtY = function(crtX);
        double antY = function(antX);

        double antYWindow = (antY - camera->y) / camera->scale * WINDOW_HEIGHT / 2.0;
        double crtYWindow = (crtY - camera->y) / camera->scale * WINDOW_HEIGHT / 2.0;

        drawLine(antXWindow, antYWindow, crtXWindow, crtYWindow);

        antXWindow = crtXWindow;
        crtXWindow += distXWindow;
    }

    drawAxes(((camera->x - camera->scale) - camera->x) / camera->scale * WINDOW_WIDTH / 2.0, -camera->y / camera->scale * WINDOW_HEIGHT / 2.0, ((camera->x + camera->scale) - camera->x) / camera->scale * WINDOW_WIDTH / 2.0, -camera->y / camera->scale * WINDOW_HEIGHT / 2.0);
    drawAxes(-camera->x / camera->scale * WINDOW_WIDTH / 2.0, ((camera->y - camera->scale) - camera->y) / camera->scale * WINDOW_HEIGHT / 2.0, -camera->x / camera->scale * WINDOW_WIDTH / 2.0, ((camera->y + camera->scale) - camera->y) / camera->scale * WINDOW_HEIGHT / 2.0);

    drawOrigin(-camera->x / camera->scale * WINDOW_WIDTH / 2.0, -camera->y / camera->scale * WINDOW_HEIGHT / 2.0);

    if (axes.size() > 0)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(double) * axes.size(), &(axes.front()), GL_DYNAMIC_DRAW);

        glUniform3f(colourPath, 1.0, 1.0, 1.0);

        glDrawArrays(GL_LINES, 0, axes.size() / 2);
    }

    if (vertices.size() > 0)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(double) * vertices.size(), &(vertices.front()), GL_DYNAMIC_DRAW);

        glUniform3f(colourPath, 1.0, 0.0, 0.0);

        glDrawArrays(GL_LINES, 0, vertices.size() / 2);
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * origin.size(), &(origin.front()), GL_DYNAMIC_DRAW);

    glUniform3f(colourPath, 1.0, 0.0, 0.0);

    glDrawArrays(GL_POINTS, 0, origin.size() / 2);
}

void drawPoint(double x, double y)
{
    vertices.emplace_back(x);
    vertices.emplace_back(y);
}

bool equation(double x, double y)
{
    //return abs((x - 3) * (x - 3) + (y - 5) * (y - 5) - 200) < EPSILON_EQUATION;
    return abs((x - 5) * (x - 5) / 7 + (y - 5) * (y - 5) - 200) < EPSILON_EQUATION;
}

void drawEquation(Camera* camera)
{
    axes.clear();
    vertices.clear();
    origin.clear();

    double distXWindow = WINDOW_WIDTH / (1.0 * POINTS_X - 1.0);
    double distYWindow = WINDOW_HEIGHT / (1.0 * POINTS_Y - 1.0);

    double crtXWindow = -WINDOW_WIDTH / 2.0;

    for (int i = 0; i < POINTS_X; i++)
    {
        double crtYWindow = -WINDOW_HEIGHT / 2.0;

        for (int j = 0; j < POINTS_Y; j++)
        {
            double crtX = crtXWindow / (WINDOW_WIDTH / 2.0) * camera->scale + camera->x;
            double crtY = crtYWindow / (WINDOW_HEIGHT / 2.0) * camera->scale + camera->y;

            if (equation(crtX, crtY))
            {
                drawPoint(crtXWindow, crtYWindow);
            }

            crtYWindow += distYWindow;
        }

        crtXWindow += distXWindow;
    }

    drawAxes(((camera->x - camera->scale) - camera->x) / camera->scale * WINDOW_WIDTH / 2.0, -camera->y / camera->scale * WINDOW_HEIGHT / 2.0, ((camera->x + camera->scale) - camera->x) / camera->scale * WINDOW_WIDTH / 2.0, -camera->y / camera->scale * WINDOW_HEIGHT / 2.0);
    drawAxes(-camera->x / camera->scale * WINDOW_WIDTH / 2.0, ((camera->y - camera->scale) - camera->y) / camera->scale * WINDOW_HEIGHT / 2.0, -camera->x / camera->scale * WINDOW_WIDTH / 2.0, ((camera->y + camera->scale) - camera->y) / camera->scale * WINDOW_HEIGHT / 2.0);

    drawOrigin(-camera->x / camera->scale * WINDOW_WIDTH / 2.0, -camera->y / camera->scale * WINDOW_HEIGHT / 2.0);

    if (axes.size() > 0)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(double) * axes.size(), &(axes.front()), GL_DYNAMIC_DRAW);

        glUniform3f(colourPath, 1.0, 1.0, 1.0);

        glDrawArrays(GL_LINES, 0, axes.size() / 2);
    }

    if (vertices.size() > 0)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(double) * vertices.size(), &(vertices.front()), GL_DYNAMIC_DRAW);

        glUniform3f(colourPath, 1.0, 0.0, 0.0);

        glDrawArrays(GL_POINTS, 0, vertices.size() / 2);
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * origin.size(), &(origin.front()), GL_DYNAMIC_DRAW);

    glUniform3f(colourPath, 1.0, 0.0, 0.0);

    glDrawArrays(GL_POINTS, 0, origin.size() / 2);
}

void displayCoordinates(Camera* camera)
{
    cout << "Current point coordinates: " << camera->x << ' ' << camera->y << '\n';
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Mathematical Functions and Equations Visualizer", 0, 0);
    //glfwGetPrimaryMonitor();

    glfwMakeContextCurrent(window);

    glewInit();

    Camera* camera = new Camera(0.0, 0.0, 75.0, 5.0, 10.0);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);

    colourPath = glGetUniformLocation(shaderProgram, "colour");
    int orthoPath = glGetUniformLocation(shaderProgram, "ortho");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 2 * sizeof(double), (void*)0);
    glEnableVertexAttribArray(0);

    glm::mat4 ortho = glm::ortho(-0.5 * WINDOW_WIDTH, 0.5 * WINDOW_WIDTH, -0.5 * WINDOW_HEIGHT, 0.5 * WINDOW_HEIGHT);
    glUniformMatrix4fv(orthoPath, 1, GL_FALSE, glm::value_ptr(ortho));

    while (!glfwWindowShouldClose(window))
    {
        updateDeltaTime();

        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        handleInput(window, camera);

        //drawFunction(camera);
        drawEquation(camera);

        displayCoordinates(camera);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}