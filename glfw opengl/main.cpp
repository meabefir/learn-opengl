#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "shader.h"
#include "camera.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 500;
const unsigned int SCR_HEIGHT = 800;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::mat4 projection = glm::ortho(0.f, (float)SCR_WIDTH, 0.f, (float)SCR_HEIGHT);
// glm::mat4 projection = glm::ortho(-1000, 1000, -1000, 1000);
glm::vec3 anch((float)SCR_WIDTH / 2.f, (float)SCR_HEIGHT * 1.f / 4.f, .0f);

glm::mat4 model(1.f);

float randf() {
    return (float)rand() / (float)RAND_MAX;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    float vertices[] = {
        -.5f, -5.f, 0.f,
        .5f, -.5f, 0.f,
        .5f, .5f, 0.f,
        -.5f, .5f, 0.f, // rect vertexes
    };

    const int circleFidelity = 20;
    float circleVertices[circleFidelity * 3];

    constexpr float step = 2.f * glm::pi<float>() / (float)circleFidelity;
    float current_angle = .0f;
    float radius = 100.f;
    for (int i = 0; i < circleFidelity; i++) {
        float x, y;
        x = cos(current_angle) / 2.f * radius;
        y = sin(current_angle) / 2.f * radius;
        
        circleVertices[i * 3] = x;
        circleVertices[i * 3 + 1] = y;
        circleVertices[i * 3 + 2] = .0f;

        current_angle += step;
    }


    circleVertices[0] = 100.f;
    circleVertices[1] = 100.f;
    circleVertices[2] = 0.f;
    circleVertices[3] = 300.f;
    circleVertices[4] = 100.f;
    circleVertices[5] = 0.f;
    circleVertices[6] = 300.f;
    circleVertices[7] = 400.f;
    circleVertices[8] = 0.f;
    circleVertices[9] = 100.f;
    circleVertices[10] = 400.f;
    circleVertices[11] = 0.f;
    for (int i = 0; i < circleFidelity; i++) {
        std::cout << circleVertices[i * 3] << ' ' << circleVertices[i * 3 + 1] << '\n';
    }

    unsigned int circleVBO, circleVAO;
    glGenBuffers(1, &circleVBO);
    glGenVertexArrays(1, &circleVAO);
    glBindVertexArray(circleVAO);

    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /*unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);*/

    // shaders
    Shader circleShader("shaders/vertex.vert", "shaders/color_fragment.frag");
    circleShader.use();
    circleShader.setVec3("inColor", glm::vec3(1.f, 0.f, 0.f));
    circleShader.setMat4("projection", glm::value_ptr(projection));

    Shader rectShader("shaders/vertex.vert", "shaders/color_fragment.frag");
    rectShader.use();
    rectShader.setVec3("inColor", glm::vec3(0.f, 0.f, 1.f));
    rectShader.setMat4("projection", glm::value_ptr(projection));

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw circles
       /* model = glm::mat4(1.f);
        model = glm::translate(model, anch);
        model = glm::scale(model, glm::vec3(30.f));*/

        circleShader.use();
        circleShader.setMat4("model", glm::value_ptr(model));

        /*glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
        glBindVertexArray(circleVAO);*/

        glDrawArrays(GL_QUADS, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteBuffers(1, &circleVBO);
    glDeleteVertexArrays(1, &circleVAO);

    //glDeleteVertexArrays(1, &cubeVAO);
    //glDeleteVertexArrays(1, &lightCubeVAO);
    //glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}