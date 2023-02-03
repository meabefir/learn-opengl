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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// view matrix
glm::mat4 view = glm::mat4(1.0f);

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader shader1("lab6.vert", "lab6.frag");

#define N 100
#define R 10
    float vertices1[(int)(N * R) * 3];
    float delta = glm::radians(360.f / N);
    float pi2 = 3.141592653f * 2.f;
    float a = 5.f, b = 7.f, c = 2.2f;
    for (int i = 0; i < N * R; i++) {
        float t = i * delta;
        vertices1[i * 3] = (a - b) * glm::cos(t) + c * glm::cos((a / b - 1) * t);
        vertices1[i * 3 + 1] = (a - b) * glm::sin(t) - c * glm::sin((a / b - 1) * t);

        vertices1[i * 3 + 2] = 0.f;
    }
    glm::vec3 color1[(int)(N*R)];
    for (int i = 0; i < N*R; i++) {
        color1[i] = glm::vec3(1.f);
    }

    unsigned int indices1[(int)(N*R)];
    for (int i = 0; i < N*R; i++) {
        indices1[i] = i;
    }

    unsigned int vbo, vbo_color, vao, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &vbo_color);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color1), color1, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);

    glm::mat4 projection;
    projection = glm::ortho(.0f, 800.f, .0f, 600.f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {

        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 model(1.f);
        model = glm::translate(model, glm::vec3(300.f, 300.f, 0.f));
        model = glm::scale(model, glm::vec3(30.f));

        shader1.use();
        shader1.setMat4("model", glm::value_ptr(model));
        shader1.setMat4("projection", glm::value_ptr(projection));
        glDrawElements(GL_LINE_STRIP, (int)(N*R), GL_UNSIGNED_INT, (void*)0);

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
    float d = .0001f;
    float d_translate = .001f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        view = glm::translate(view, glm::vec3(-d_translate, 0.f, 0.f));
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        view = glm::translate(view, glm::vec3(d_translate, 0.f, 0.f));
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        view = glm::translate(view, glm::vec3(0.f, -d_translate, 0.f));
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        view = glm::translate(view, glm::vec3(0.f, d_translate, 0.f));
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}