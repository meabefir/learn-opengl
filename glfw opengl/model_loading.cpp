#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "camera.h"
#include "model.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(-10.0f, 3.0f, 1.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// alien planet globals
glm::vec3 eggPosition = glm::vec3(0.0f, 10.0f, 0.0f);
glm::vec3 eggScale = glm::vec3(1.0f, 2.0f, 1.0f);

// tentacle alien globals
glm::vec3 tentacleScale = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 tentaclePosition = glm::vec3(0.0f, 0.0f, 2.0f);
glm::vec3 tentacleRotation = glm::vec3(0.0f, 0.0f, 0.0f);

// reptile alien globals
glm::vec3 reptileScale = glm::vec3(0.02f, 0.02f, 0.02f);
glm::vec3 reptilePosition = glm::vec3(7.0f, 0.0f, 0.0f);
glm::vec3 reptileRotation = glm::vec3(0.0f, 0.0f, 0.0f);

// dragon globals
glm::vec3 dragonScale = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 dragonPosition = glm::vec3(11.0f, 0.0f, 2.0f);
glm::vec3 dragonRotation = glm::vec3(0.0f, 0.0f, 0.0f);

// worm globals
glm::vec3 wormScale = glm::vec3(0.03f, 0.03f, 0.03f);
glm::vec3 wormPosition = glm::vec3(3.5f, 0.0f, 0.0f);
glm::vec3 wormRotation = glm::vec3(0.0f, 0.0f, 0.0f);

// global transformation variables
glm::vec3 scaleFactors = glm::vec3(1.0f, 1.0f, 1.0f);
bool enableCombinedTransformations = false;
glm::vec3 parentPosition = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 childPosition = glm::vec3(2.0f, 0.0f, 0.0f); // Offset from parent
glm::vec3 parentRotation = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 childRotation = glm::vec3(0.0f, 0.0f, 0.0f);

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Model Load", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

     // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    vector<std::string> faces
    {
        // // the working cubmemap images:

        // "cubemap/right.png",
        // "cubemap/left.png",
        // "cubemap/top.png",
        // "cubemap/bottom.png",
        // "cubemap/frontsnow.png",
        // "cubemap/back.png"
    };

    glEnable(GL_DEPTH_TEST);
    std::cout << glGetError();
    // build and compile shaders
    // -------------------------
    Shader ourShader("shaders/1.model_loading.vs", "shaders/1.model_loading.fs");
    Shader skyboxShader("shaders/skybox.vs", "shaders/skybox.fs");
    /// Shader lightShader("shaders/light.vs", "shaders/light.fs");
    Model egg("models/AlienPlanet/AlienPlanet.obj");
    Model tentacle("models/Tentacle Monster/tentacle monster 1.obj");
    Model dragon("models/xmmlekophyio-Dragon/Dragon.obj");
    Model reptile("models/Reptile Alien Creature-OBJ/Reptile Alien Creature.obj");
    Model worm("models/Frew Worm Monster-OBJ/Frew Worm Monster.obj");
    ourShader.use();
    ourShader.setInt("main", 0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // creating matrix for each loaded obj file (for transformations)
        glm::mat4 model = glm::mat4(1.0f);

        ourShader.setVec3("lightColor", glm::vec3(0.8f, 0.8f, 0.8f)); // Light gray specular color
        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
        // ourShader.setVec3("light.position", lightPos);
        // ourShader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        // ourShader.setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        // ourShader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        // float eggShininess = 32.0f; // Example shininess value
        // ourShader.setFloat("shininess", eggShininess);

        glm::mat4 model_egg = glm::mat4(1.0f);
        model_egg = glm::scale(model_egg, eggScale);
        model_egg = glm::translate(model_egg, eggPosition);
        // Calculate the rotation angle based on the time to create a continuous rotation
        float angle = glfwGetTime() * glm::radians(6.0f); // 6.0f is the speed of the rotation
        // Apply the rotation around the y-axis (0, 1, 0) to the model matrix
        model_egg = glm::rotate(model_egg, angle, glm::vec3(0.0f, 1.0f, 0.0f));

        ourShader.setMat4("model", model_egg);
        egg.Draw(ourShader);
        //std::cout << glGetError();

        // ----------------------------------------------------------------------------------------------
        
        glm::mat4 model_tentacle = glm::mat4(1.0f);
        model_tentacle = glm::translate(model_tentacle, tentaclePosition);
        model_tentacle = glm::scale(model_tentacle, tentacleScale);
        // Calculate the rotation angle based on the time to create a continuous rotation
        // float angle = glfwGetTime() * glm::radians(3.0f); // 3.0f is the speed of the rotation

        // Apply the rotation around the y-axis (0, 1, 0) to the model matrix
        // model_tentacle = glm::rotate(model_tentacle, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model_tentacle);
        tentacle.Draw(ourShader);
        
        // ----------------------------------------------------------------------------------------------
        
        glm::mat4 model_dragon = glm::mat4(1.0f);
        model_dragon = glm::translate(model_dragon, dragonPosition);
        model_dragon = glm::scale(model_dragon, dragonScale);
        // Calculate the rotation angle based on the time to create a continuous rotation
        // float angle = glfwGetTime() * glm::radians(3.0f); // 3.0f is the speed of the rotation

        // Apply the rotation around the y-axis (0, 1, 0) to the model matrix
        // model_tentacle = glm::rotate(model_tentacle, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model_dragon);
        dragon.Draw(ourShader);
        
        // ----------------------------------------------------------------------------------------------
        glm::mat4 model_reptile = glm::mat4(1.0f);
        model_reptile = glm::translate(model_reptile, reptilePosition);
        model_reptile = glm::scale(model_reptile, reptileScale);
        // Calculate the rotation angle based on the time to create a continuous rotation
        // float angle = glfwGetTime() * glm::radians(3.0f); // 3.0f is the speed of the rotation


        // Apply the rotation around the y-axis (0, 1, 0) to the model matrix
        // model_tentacle = glm::rotate(model_tentacle, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model_reptile);
        reptile.Draw(ourShader);
        // ----------------------------------------------------------------------------------------------

        glm::mat4 model_worm = glm::mat4(1.0f);
        model_worm = glm::translate(model_worm, wormPosition);
        model_worm = glm::scale(model_worm, wormScale);
        // Calculate the rotation angle based on the time to create a continuous rotation
        // float angle = glfwGetTime() * glm::radians(3.0f); // 3.0f is the speed of the rotation
        model_worm = glm::rotate(model_worm, glm::radians(wormRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model_worm = glm::rotate(model_worm, glm::radians(wormRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model_worm = glm::rotate(model_worm, glm::radians(wormRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        // Apply the rotation around the y-axis (0, 1, 0) to the model matrix
        // model_tentacle = glm::rotate(model_tentacle, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model_worm);
        worm.Draw(ourShader);

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        unsigned int cubemapTexture = loadCubemap(faces);
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        glfwSwapBuffers(window);
        glfwPollEvents(); // polling IO events (for camera movement)
    }

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);


    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    float speed = 2.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        dragonPosition.y += speed;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        dragonPosition.y -= speed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        dragonPosition.x -= speed;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        dragonPosition.x += speed;
    }
// Transformation controls
float rotationSpeed = 45.0f; // degrees per second

    // Rotate around the x-axis when key '1' is pressed
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        wormRotation.x += rotationSpeed * deltaTime;
    }
    // Rotate around the y-axis when key '2' is pressed
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        wormRotation.y += rotationSpeed * deltaTime;
    }
    // Rotate around the z-axis when key '3' is pressed
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        wormRotation.z += rotationSpeed * deltaTime;
    }
    // Rotate around the y-axis in the opposite direction when key '4' is pressed
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        wormRotation.y -= rotationSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        // Translate in the x-direction
        reptilePosition.x += 1.0f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        // Translate in the y-direction
        reptilePosition.y += 1.0f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        // Translate in the z-direction
        reptilePosition.z += 1.0f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        // Uniform scaling
        reptileScale += glm::vec3(0.1f * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        // Non-uniform scaling - X
        reptileScale.x += 0.1f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        // Non-uniform scaling - Y
        reptileScale.y += 0.1f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        // Non-uniform scaling - Z
        reptileScale.z += 0.1f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        // Toggle combined transformations
        enableCombinedTransformations = !enableCombinedTransformations;
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
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

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}