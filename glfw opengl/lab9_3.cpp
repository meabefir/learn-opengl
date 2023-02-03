#include <glad/glad.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"

#include "shader.h"
#include "camera.h"
#include "model.h"

#include <iostream>

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// view matrix
glm::mat4 view = glm::mat4(1.0f);

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader shader1("lab7.vert", "lab7.frag");

    const glm::vec3 brown(139.f / 255, 69.f / 255, 19.f / 255);
    const glm::vec3 green(0.f / 255, 255.f / 255, 0.f / 255);

#define R .2f
#define N 13
#define N_V1 (N + 2)
#define N_V (N * 2 + 4)
    glm::vec3 vertices[N_V];
    unsigned int upperBaseOffset = N_V1;
    vertices[0] = glm::vec3(0.f, -1.f, 0.f);
    vertices[upperBaseOffset] = glm::vec3(0.f, 1.f, 0.f);
    float deltaAngle = glm::radians(360.f) / N;
    for (int i = 0; i <= N; i++) {
        float currentDelta = (i)*deltaAngle;
        vertices[i + 1] = glm::vec3(glm::cos(currentDelta) * R, -1.f, glm::sin(currentDelta) * R);
        vertices[i + 1 + upperBaseOffset] = glm::vec3(glm::cos(currentDelta) * R, 1.f, glm::sin(currentDelta) * R);
    }

    glm::vec3 colors[N_V];
    for (int i = 0; i < N_V; i++) {
        colors[i] = brown;
    }

    unsigned int indices[N_V + 6 * N];
    for (int i = 0; i < N_V; i++) {
        indices[i] = i;
    }
    int ii = 1;
    int jj = 1 + N_V1;
    for (int i = N_V; i < N_V + 6 * N; i += 6) {
        indices[i] = ii;
        indices[i + 1] = ii + 1;
        indices[i + 2] = jj;
        indices[i + 3] = jj;
        indices[i + 4] = jj + 1;
        indices[i + 5] = ii + 1;
        ii += 1;
        jj += 1;
    }

    unsigned int vbo, vbo_color, vao_cilinder, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &vbo_color);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao_cilinder);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(vao_cilinder);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);



    glm::vec3 vertices2[N_V1+1];
    vertices2[0] = glm::vec3(0.f, .0f, 0.f);
    vertices2[upperBaseOffset] = glm::vec3(0.f, .5f, 0.f);
    deltaAngle = glm::radians(360.f) / N;
    for (int i = 0; i <= N; i++) {
        float currentDelta = (i)*deltaAngle;
        vertices2[i + 1] = glm::vec3(glm::cos(currentDelta) * R, 0.f, glm::sin(currentDelta) * R);
    }
    glm::vec3 colors2[N_V1+1];
    for (int i = 0; i < N_V1+1; i++) {
        colors2[i] = green;
    }

    unsigned int indices2[N_V1 + 3 * N];
    for (int i = 0; i < N_V1; i++) {
        indices2[i] = i;
    }
    ii = 1;
    jj = N_V1;
    for (int i = N_V1; i < N_V1 + 3 * N; i += 3) {
        indices2[i] = ii;
        indices2[i + 1] = ii + 1;
        indices2[i + 2] = jj;
        ii += 1;
    }

    unsigned int vbo2, vbo_color2, vao_cone, ebo2;
    glGenBuffers(1, &vbo2);
    glGenBuffers(1, &vbo_color2);
    glGenBuffers(1, &ebo2);
    glGenVertexArrays(1, &vao_cone);
    glBindVertexArray(vao_cone);

    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_color2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors2), colors2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);

    glBindVertexArray(vao_cone);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);

    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_color2);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);;

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model(1.f);

        shader1.use();
        shader1.setMat4("view", glm::value_ptr(camera.GetViewMatrix()));
        shader1.setMat4("projection", glm::value_ptr(projection));
        shader1.setMat4("model", glm::value_ptr(model));

        glBindVertexArray(vao_cilinder);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLE_FAN, N_V1, GL_UNSIGNED_INT, (void*)0);
        glDrawElements(GL_TRIANGLE_FAN, N_V1, GL_UNSIGNED_INT, (void*)(sizeof(float) * N_V1));
        glDrawElements(GL_TRIANGLES, N * 6, GL_UNSIGNED_INT, (void*)(sizeof(float) * N_V));

        for (int i = 0; i < 3; i++) {
            model = glm::mat4(1.f);
            model = glm::translate(model, glm::vec3(0.f, .5f * (i+1), 0.f));
            model = glm::scale(model, glm::vec3(3.f - i * .5f));
            shader1.setMat4("model", glm::value_ptr(model));
            glBindVertexArray(vao_cone);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDrawElements(GL_TRIANGLE_FAN, N_V1, GL_UNSIGNED_INT, (void*)0);
            glDrawElements(GL_TRIANGLES, N * 3, GL_UNSIGNED_INT, (void*)(sizeof(float) * N_V1));
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}