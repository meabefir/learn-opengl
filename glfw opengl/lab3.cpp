#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "shader.h"
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float blend_amount = .2f;

// view matrix
glm::mat4 view = glm::mat4(1.0f);

glm::vec3 p1(200.f, 400.f, 0.f);
glm::vec3 p2(550.f, 550.f, 0.f);

float a = 1, b = -1, c = -150.f;
float cubex, cubey;

float progress = .5f;

int main()
{
    float x1, y1, x2, y2;
    x1 = 0.f;
    x2 = 800;

    y1 = (-c - a * x1) / b;
    y2 = (-c - a * x2) / b;

    std::cout << x1 << ' ' << y1 << '\n' << x2 << ' ' << y2;

    //glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    //glm::mat4 trans = glm::mat4(1.0f);
    //trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
    //vec = trans * vec;
    //std::cout << vec.x << vec.y << vec.z << std::endl;

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


    // build and compile our shader program
    // ------------------------------------
    // we skipped compile log checks this time for readability (if you do encounter issues, add the compile-checks! see previous code samples)
    Shader sh1("lab3.vert", "lab3.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float vertices[] = {
        200.f, 500.f, 0.f, 1.f, 0.f, 1.f,
        300.f, 400.f, 0.f, 1.f, 0.f, 1.f,
        250.f, 350.f, 0.f, 1.f, 0.f, 1.f,
        200.f, 330.f, 0.f, 1.f, 0.f, 1.f,
        150.f, 370.f, 0.f, 1.f, 0.f, 1.f, 

        500.f, 600.f, 0.f, 1.f, 0.f, 1.f, // 5
        550.f, 550.f, 0.f, 1.f, 0.f, 1.f,
        550.f, 520.f, 0.f, 1.f, 0.f, 1.f,
        550.f, 550.f, 0.f, 1.f, 0.f, 1.f,
        550.f, 520.f, 0.f, 1.f, 0.f, 1.f,
        600.f, 620.f, 0.f, 1.f, 0.f, 1.f,

        0.f, 0.f, 0.f,  1.f, 0.f, 0.f, // 11
        400.f, 0.f, 0.f, 1.f, 1.f, 0.f,
        400.f, 600.f, 0.f, 1.f, 1.f, 0.f,
        0.f, 0.f, 0.f,  1.f, 0.f, 0.f,
        0.f, 600.f, 0.f,1.f, 0.f, 0.f,
        400.f, 600.f, 0.f, 1.f, 1.f, 0.f,

        400.f, 0.f, 0.f, 1.f, 1.f, 0.f, // 17
        800.f, 0.f, 0.f, 0.f, 0.f, 1.f,
        800.f, 600.f, 0.f, 0.f, 0.f, 1.f,
        400.f, 0.f, 0.f, 1.f, 1.f, 0.f,
        400.f, 600.f, 0.f, 1.f, 1.f, 0.f,
        800.f, 600.f, 0.f, 0.f, 0.f, 1.f,

        x1, y1, 0.f, 1.f, 1.f, 1.f, // 23
        x2, y2, 0.f, 1.f, 1.f, 1.f,

        -1.f, -1.f, 0.f, 0.f, 0.f, 1.f, // 25
        1.f, -1.f, 0.f, 0.f, 0.f, 1.f,
        1.f, 1.f, 0.f, 0.f, 0.f, 1.f,
        -1.f, 1.f, 0.f, 0.f, 0.f, 1.f,
        
    };

    ///////////////////////////////////////////////////////////////// texture
    const char* texture_paths[] = { "assets/container.jpg", "assets/awesomeface.png" };
    unsigned int textures[2];
    glGenTextures(2, textures);
    stbi_set_flip_vertically_on_load(true);
    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, i == -1 ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, i == -1 ? GL_LINEAR : GL_NEAREST);

        // load and generate the texture
        int width, height, nrChannels;
        unsigned char* data = stbi_load(texture_paths[i], &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, i == 0 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            std::cout << "loaded " << texture_paths[i] << '\n';
        }
        else
        {
            std::cout << "Failed to load texture " << texture_paths[i] << std::endl;
        }
        stbi_image_free(data);
    }
    //////////////////////////////////////////////////////////////////

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO); // we can also generate multiple VAOs or buffers at the same time
    glGenBuffers(1, &VBO);
    // glGenBuffers(1, &EBO);
    // first triangle setup
    // --------------------
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   /* glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); 
    /* glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);*/
    // glBindVertexArray(0); // no need to unbind at all as we directly bind a different VAO the next few lines
    // second triangle setup

    // glBindVertexArray(0); // not really necessary as well, but beware of calls that could affect VAOs while this one is bound (like binding element buffer objects, or enabling/disabling vertex attributes)
   /* int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;*/

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // render loop
    // -----------

    sh1.use();
    /*sh1.setInt("texture1", 0);
    sh1.setInt("texture2", 1);*/

    //glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
    //glBindTexture(GL_TEXTURE_2D, textures[0]);

    //glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture
    //glBindTexture(GL_TEXTURE_2D, textures[1]);

    // create transform of rotation 90 degrees around z axis and pass it to the vertex shader
    /*glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));*/

    // note that we're translating the scene in the reverse direction of where we want to move
    // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    // projection matrix
    glm::mat4 projection;
    // projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    float scale = 3.f;
    // projection = glm::ortho(.0f - 800.f * scale / 2.f, 800.f * scale / 2.f, .0f - 600.f * scale / 2.f, 600.f * scale / 2.f);
    projection = glm::ortho(.0f, 800.f, .0f, 600.f);

    while (!glfwWindowShouldClose(window))
    {
        /*a = cos(glfwGetTime());
        b = sin(glfwGetTime());

        y1 = (-c - a * x1) / b;
        y2 = (-c - a * x2) / b;*/

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // now when we draw the triangle we first use the vertex and orange fragment shader from the first program
        // sh1.setFloat("blend_amount", blend_amount);

        // apply rotation
       /* glm::mat4 trans = glm::mat4(1.f);
        trans = glm::translate(trans, glm::vec3(.5f, -.5f, .0f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));*/

        glm::mat4 model = glm::mat4(1.0f);
        /*float scale = 100.f;
        model = glm::translate(model, glm::vec3(250.f, 250.f, .0f));
        model = glm::scale(model, glm::vec3(scale));*/
        // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        unsigned int modelLoc = glGetUniformLocation(sh1.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        /*unsigned int viewLoc = glGetUniformLocation(sh1.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));*/
        unsigned int projectionLoc = glGetUniformLocation(sh1.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        // 2
        glDrawArrays(GL_TRIANGLES, 11, 12);

        // 1
        glDrawArrays(GL_TRIANGLE_FAN, 0, 5);

        glDrawArrays(GL_TRIANGLES, 5, 6);

        // 3
        glm::vec3 mid = (p1 + p2) / 2.f;
        glm::vec3 p2_to_mid = (mid - p2);
        glm::vec3 p1_to_mid = (mid - p1);

        model = glm::mat4(1.0f);
        // model = glm::translate(model, -p1_to_mid);
        model = glm::translate(model, p1_to_mid);
        model = glm::translate(model, p1);
        model = glm::rotate(model, glm::radians(50.f * (float)glfwGetTime()), glm::vec3(0.f, 0.f, 1.f));
        model = glm::translate(model, -p1_to_mid);
        model = glm::translate(model, -p1);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLE_FAN, 0, 5);

        model = glm::mat4(1.0f);
        // model = glm::translate(model, -p1_to_mid);
        model = glm::translate(model, p2_to_mid);
        model = glm::translate(model, p2);
        model = glm::rotate(model, glm::radians(50.f * (float)glfwGetTime()), glm::vec3(0.f, 0.f, 1.f));
        model = glm::translate(model, -p2_to_mid);
        model = glm::translate(model, -p2);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLES, 5, 6);

        // 5
        model = glm::mat4(1.f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glLineWidth(5.f);
        glDrawArrays(GL_LINES, 23, 2);

        // progress = abs(sin(glfwGetTime() * .2f));
        float xmapped = x1 + progress * (x2 - x1);
        float ymapped = y1 + progress * (y2 - y1);

        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(xmapped, ymapped, 0.f));
        model = glm::rotate(model, glm::radians(60.f * (float)glfwGetTime()), glm::vec3(.0f, .0f, 1.f));
        model = glm::scale(model, glm::vec3(20.f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_QUADS, 25, 4);

        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // second object on the top left, translate and scale

        /*trans = glm::mat4(1.f);
        float _sin = sin(glfwGetTime());
        trans = glm::translate(trans, glm::vec3(-.5f, .5f, .0f));
        trans = glm::scale(trans, glm::vec3(_sin, _sin, _sin));

        glUniformMatrix4fv(transform_location, 1, GL_FALSE, glm::value_ptr(trans));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);*/

        // then we draw the second triangle using the data from the second VAO
        // when we draw the second triangle we want to use a different shader program so we switch to the shader program with our 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // -----------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &EBO);
    glDeleteTextures(2, textures);

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
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        progress = std::min(1.f, progress + d);
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        progress = std::max(0.f, progress - d);

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