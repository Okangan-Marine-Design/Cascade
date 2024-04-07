#include <gl3w/GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

#define GLT_IMPLEMENTATION
#include "gltext.h"

// Define the size of the voxel grid
const int gridWidth = 10;
const int gridHeight = 10;
const int gridDepth = 10;

// Camera variables
GLfloat cameraPositionX = 0.0f;
GLfloat cameraPositionY = 0.0f;
GLfloat cameraPositionZ = -7.0f;

GLfloat cameraYaw = 0.0f;   // Yaw angle (rotation around the Y-axis)
GLfloat cameraPitch = 0.0f; // Pitch angle (rotation around the X-axis)
float cameraSpeed = 0.1f;
float sensitivity = 0.001f;

float degreesToRadians(float degrees) {
    return degrees * static_cast<float>(M_PI / 180.0);
}

// Function to handle keyboard input for camera movement
void processInput(GLFWwindow* window) {
    // Camera movement speed
    float cameraSpeed = 0.1f;

// Calculate movement direction based on yaw angle
float angleRad = degreesToRadians(cameraYaw);
float dx = sin(angleRad);
float dz = cos(angleRad);

if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    cameraPositionX += dx * cameraSpeed;
    cameraPositionZ += dz * cameraSpeed;
}
if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    cameraPositionX -= dx * cameraSpeed;
    cameraPositionZ -= dz * cameraSpeed;
}
if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    cameraPositionX += dz * cameraSpeed;
    cameraPositionZ -= dx * cameraSpeed;
}
if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    cameraPositionX -= dz * cameraSpeed;
    cameraPositionZ += dx * cameraSpeed;
}
if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    cameraPositionY += cameraSpeed;
if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    cameraPositionY -= cameraSpeed;
    }

void renderText(const std::string& data, float x, float y, float scale) {
    GLTtext *text = gltCreateText();
    gltSetText(text, data.c_str());

    // Begin text drawing (this for instance calls glUseProgram)
    gltBeginDraw();

    // Draw any amount of text between begin and end
    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    gltDrawText2D(text, x, y, scale);

    // Finish drawing text
    gltEndDraw();
}

// Function to draw a single voxel
void drawVoxel(float x, float y, float z, float size) {

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    // Front face
    glVertex3f(x - size / 2, y - size / 2, z + size / 2);
    glVertex3f(x + size / 2, y - size / 2, z + size / 2);
    glVertex3f(x + size / 2, y + size / 2, z + size / 2);
    glVertex3f(x - size / 2, y + size / 2, z + size / 2);
    // Back face
    glVertex3f(x - size / 2, y - size / 2, z - size / 2);
    glVertex3f(x + size / 2, y - size / 2, z - size / 2);
    glVertex3f(x + size / 2, y + size / 2, z - size / 2);
    glVertex3f(x - size / 2, y + size / 2, z - size / 2);
    // Top face
    glVertex3f(x - size / 2, y + size / 2, z + size / 2);
    glVertex3f(x + size / 2, y + size / 2, z + size / 2);
    glVertex3f(x + size / 2, y + size / 2, z - size / 2);
    glVertex3f(x - size / 2, y + size / 2, z - size / 2);
    // Bottom face
    glVertex3f(x - size / 2, y - size / 2, z + size / 2);
    glVertex3f(x + size / 2, y - size / 2, z + size / 2);
    glVertex3f(x + size / 2, y - size / 2, z - size / 2);
    glVertex3f(x - size / 2, y - size / 2, z - size / 2);
    // Right face
    glVertex3f(x + size / 2, y - size / 2, z + size / 2);
    glVertex3f(x + size / 2, y - size / 2, z - size / 2);
    glVertex3f(x + size / 2, y + size / 2, z - size / 2);
    glVertex3f(x + size / 2, y + size / 2, z + size / 2);
    // Left face
    glVertex3f(x - size / 2, y - size / 2, z + size / 2);
    glVertex3f(x - size / 2, y - size / 2, z - size / 2);
    glVertex3f(x - size / 2, y + size / 2, z - size / 2);
    glVertex3f(x - size / 2, y + size / 2, z + size / 2);
    glEnd();

    glColor3f(0.1f, 0.8f, 0.5f);
    glBegin(GL_LINES);
    // Front face
    glVertex3f(x - size / 2, y - size / 2, z + size / 2);
    glVertex3f(x + size / 2, y - size / 2, z + size / 2);

    glVertex3f(x + size / 2, y - size / 2, z + size / 2);
    glVertex3f(x + size / 2, y + size / 2, z + size / 2);

    glVertex3f(x + size / 2, y + size / 2, z + size / 2);
    glVertex3f(x - size / 2, y + size / 2, z + size / 2);

    glVertex3f(x - size / 2, y + size / 2, z + size / 2);
    glVertex3f(x - size / 2, y - size / 2, z + size / 2);
    // Back face
    glVertex3f(x - size / 2, y - size / 2, z - size / 2);
    glVertex3f(x + size / 2, y - size / 2, z - size / 2);

    glVertex3f(x + size / 2, y - size / 2, z - size / 2);
    glVertex3f(x + size / 2, y + size / 2, z - size / 2);

    glVertex3f(x + size / 2, y + size / 2, z - size / 2);
    glVertex3f(x - size / 2, y + size / 2, z - size / 2);

    glVertex3f(x - size / 2, y + size / 2, z - size / 2);
    glVertex3f(x - size / 2, y - size / 2, z - size / 2);
    // Connect front and back faces
    glVertex3f(x - size / 2, y - size / 2, z + size / 2);
    glVertex3f(x - size / 2, y - size / 2, z - size / 2);

    glVertex3f(x + size / 2, y - size / 2, z + size / 2);
    glVertex3f(x + size / 2, y - size / 2, z - size / 2);

    glVertex3f(x + size / 2, y + size / 2, z + size / 2);
    glVertex3f(x + size / 2, y + size / 2, z - size / 2);

    glVertex3f(x - size / 2, y + size / 2, z + size / 2);
    glVertex3f(x - size / 2, y + size / 2, z - size / 2);
    glEnd();
}

// Function to render the voxel grid
void renderGrid() {
    float voxelSize = 0.2f; // Size of each voxel

    // Loop through the voxel grid and draw each voxel
    for (int x = 0; x < gridWidth; ++x) {
        for (int y = 0; y < gridHeight; ++y) {
            for (int z = 0; z < gridDepth; ++z) {
                // Calculate the position of the voxel in 3D space
                float posX = x * voxelSize;
                float posY = y * voxelSize;
                float posZ = z * voxelSize;

                // Draw the voxel
                if(x%2==0 && y%2==0 && z%2==0)
                drawVoxel(posX, posY, posZ, voxelSize);
            }
        }
    }
}
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    static double lastX = xpos;
    static double lastY = ypos;

    double deltaX = xpos - lastX;
    double deltaY = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    cameraYaw -= deltaX * sensitivity;
    cameraPitch += deltaY * sensitivity;

    // Clamp pitch to avoid flipping
    if (cameraPitch > 89.0f) cameraPitch = 89.0f;
    if (cameraPitch < -89.0f) cameraPitch = -89.0f;
}// Function to set up the view matrix

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, nullptr);
    }
}

void setupCamera() {
    // Calculate the camera's direction vector
    GLfloat directionX = cos(cameraPitch) * sin(cameraYaw);
    GLfloat directionY = sin(cameraPitch);
    GLfloat directionZ = cos(cameraPitch) * cos(cameraYaw);

    // Calculate the right vector
    GLfloat rightX = sin(cameraYaw - M_PI/2.0f);
    GLfloat rightY = 0.0f;
    GLfloat rightZ = cos(cameraYaw - M_PI/2.0f);

    // Calculate the up vector
    GLfloat upX = directionY * rightZ - directionZ * rightY;
    GLfloat upY = directionZ * rightX - directionX * rightZ;
    GLfloat upZ = directionX * rightY - directionY * rightX;

    // Set up the view matrix
    GLfloat viewMatrix[16] = {
        rightX, upX, -directionX, 0.0f,
        rightY, upY, -directionY, 0.0f,
        rightZ, upZ, -directionZ, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    // Load the view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixf(viewMatrix);

    // Translate the camera to its position
    glTranslatef(-cameraPositionX, cameraPositionY, -cameraPositionZ);
}

// Function to set up the projection matrix
void setupProjection(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    float fov = 45.0f;
    float nearPlane = 0.1f; // Adjust this value
    float farPlane = 100.0f; // Adjust this value
    float top = tan(fov * M_PI / 360.0f) * nearPlane;
    float bottom = -top;
    float right = aspectRatio * top;
    float left = -right;
    glFrustum(left, right, bottom, top, nearPlane, farPlane);
    glMatrixMode(GL_MODELVIEW);
}

// Cross product helper function
GLfloat cross(GLfloat Ax, GLfloat Ay, GLfloat Az, GLfloat Bx, GLfloat By, GLfloat Bz) {
    return Ay * Bz - Az * By, Az * Bx - Ax * Bz, Ax * By - Ay * Bx;
}
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } 
}
int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "Voxel Grid", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    if (gl3wInit()) {
        printf("failed to initialize OpenGL\n");
        return -1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    if (gl3wInit()) {
        std::cerr << "Failed to initialize gl3w" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    if (!gltInit())
	{
		fprintf(stderr, "Failed to initialize glText\n");
		glfwTerminate();
		return EXIT_FAILURE;
	}

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Process input
        processInput(window);

        // Clear the buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up the view matrix
        setupCamera();
        setupProjection(800,600);

        // Render the voxel grid
        renderGrid();


        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Terminate GLFW
    gltTerminate();
    glfwTerminate();

    return 0;
}
