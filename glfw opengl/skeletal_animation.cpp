#include <glad/glad.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "shader.h"
#include "camera.h"
#include "model.hpp"
#include "animator.h"

#include <iostream>

extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

int CURRENT_WIDTH = SCR_WIDTH;
int CURRENT_HEIGHT = SCR_HEIGHT;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 spotlightPos = glm::vec3(2.56604, 15.1884, -4.42532);

glm::vec3 creature_positions[] = {
    {5.1f, -.73f, 1.32f},
    {-7.6f, -.95f, -1.68f},
    {-.47f, -3.31f, -11.86f}
};

float createure_rotations[] = {
    0.f,
    0.f,
    0.f
};

const int N_POINTS_LIGHTS = 6;
glm::vec3 pointLightPositions[N_POINTS_LIGHTS] = {
    glm::vec3(12.2864, 4.96267, 16.883),
    glm::vec3(-18.7989, 5.73609, 7.17032),
    glm::vec3(12.8172, 2.01248, -4.82285),
    { 4.91513, 5.78663, 2.11437 },
    { -7.58551, 3.3936, -1.06168 },
    { -0.73936, 1.14029, -11.5726 },
};

glm::vec3 pointLightColors[N_POINTS_LIGHTS] = {
    {1, .2, .2},
    {1, 1, .2},
    {.2, .2, 1},
    {1,1,1},
    {1,1,1},
    {1,1,1},
};

glm::vec3 getTranslation(const glm::mat4& m) {
    return glm::vec3(m[3][0], m[3][1], m[3][2]);
}

typedef std::pair<glm::vec3, glm::vec3> path_point;
const int path_size = 8;
path_point running_path1[] = {
    {{ -11.7169, -3.32293, -11.0594 }, { 0.712622, 0.262189, 0.650713 }},
    {{ -7.38956, -1.7797, -5.81981 }, { 0.724098, 0.224951, 0.651981 }},
    {{ -3.79064, -1.07775, -2.32254 }, { 0.846817, 0.116671, 0.51893 }},
    {{ 4.18428, -1.31179, -1.43709 }, { 0.866315, 0.0366438, -0.498153 }},
    {{ 7.00629, -2.68207, -7.14062 }, { 0.204838, -0.114937, -0.972024 }},
    {{ 4.30406, -3.8679, -13.4096 }, { -0.390397, 0.0993199, -0.915274 }},
    {{ 0.903631, -4.68388, -18.7095 }, { -0.932941, 0.00523609, -0.359991 }},
    {{ -9.4508, -4.24063, -16.4058 }, { -0.782422, 0.187381, 0.593889 }},
};

glm::mat4 createLookAtMatrix(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up) {
    glm::vec3 forward = glm::normalize(target - eye);
    glm::vec3 right = glm::normalize(glm::cross(up, forward));
    glm::vec3 newUp = glm::cross(forward, right);

    glm::mat4 result(1.0f);
    result[0] = glm::vec4(right, 0.0f);
    result[1] = glm::vec4(newUp, 0.0f);
    result[2] = glm::vec4(-forward, 0.0f);
    result[3] = glm::vec4(glm::vec3(), 1.0f);

    return result;
}

glm::mat4 getTransformOnPath(float t, path_point path[], size_t numPoints) {
    // Ensure t is within the valid range [0, 1]
    t = std::fmod(t, 1.0f);

    float total_dist = 0.f;
    for (int i = 0; i < numPoints; i++) {
        int j = (i + 1) % numPoints;
        total_dist += glm::distance(path[i].first, path[j].first);
    }

    float perc_to_dist = t * total_dist;

    float curr_dist = 0.f;
    float local_t = -1.f;
    int idx1 = -1;
    for (int i = 0; i < numPoints; i++) {
        int j = (i + 1) % numPoints;
        auto seg_dist = glm::distance(path[i].first, path[j].first);
        curr_dist += seg_dist;

        if (curr_dist > perc_to_dist) {
            idx1 = i;
            curr_dist -= seg_dist;
            float distance_traveled_in_segment = perc_to_dist - curr_dist;
            local_t = distance_traveled_in_segment / seg_dist;
            break;
        }
    }

    int idx2 = (idx1 + 1) % numPoints;
    
    glm::vec3 position = glm::mix(path[idx1].first, path[idx2].first, local_t);

    auto rot = createLookAtMatrix(path[idx2].first, path[idx1].first, glm::vec3(0, 1, 0));
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * rot;

    return transform;
}

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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, keyCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // stbi_set_flip_vertically_on_load(true);

    float vertices[] = {
        // positions
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    unsigned int lightCubeVBO, lightCubeVAO;
    glGenBuffers(1, &lightCubeVBO);
    glGenVertexArrays(1, &lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(lightCubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Shader modelShader("sk.vert", "sk.frag");

    modelShader.use();
    modelShader.setFloat("fogStr", .5f);
    modelShader.setVec3("fogColor", glm::vec3(0.f, 0.f, 0.f));

    Shader lightCubeShader("light_cube.vert", "light_cube.frag");
 
    Model creatureModel("models/creature/creature.dae");
    Model runningCreatureModel("models/running_creature/model.dae");
    Model terrain("models/rock_terrain/model.obj");

    Animation roarAnimation("models/creature/creature.dae", &creatureModel);
    Animation flexAnimation("models/creature/creature_flex.dae", &creatureModel);
    Animation runningAnimation("models/running_creature/model.dae", &runningCreatureModel);

    const int N = 3;
    
    float timeOffsets[N];
    float t = 0.f;
    for (int i = 0; i < N; i++) {
        timeOffsets[i] = (t += 330.f);
    }

    Animation* creature_animations[] = {
        &roarAnimation,
        &flexAnimation,
        &roarAnimation,
    };

    Animator animators[N];
    for (int i = 0; i < N; i++) {
        animators[i].setAnimation(creature_animations[i]);
        animators[i].setCurrentTime(timeOffsets[i]);
    }

    Animator runningAnimator;
    runningAnimator.setAnimation(&runningAnimation);
    runningAnimator.setCurrentTime(0.f);


    pair<glm::vec3, float> pos[N];
    float delta_angle = 3.141592653f * 2.f / N;
    float current_angle = 0.f;
    float radius = 8.f;
    for (int i = 0; i < N; i++) {
        current_angle += delta_angle;
        pos[i] = { glm::vec3(glm::cos(current_angle) * radius, 0.f, glm::sin(current_angle) * radius), current_angle - 3.141592653f * .5f};
    }

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
        for (int i = 0; i < N; i++) {
            animators[i].UpdateAnimation(deltaTime);
        }
        runningAnimator.UpdateAnimation(deltaTime);

        // render
        // ------
        // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        modelShader.use();

        // update camera pos for projection and view
        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 projection = glm::infinitePerspective(glm::radians(50.f), (float)CURRENT_WIDTH / (float)CURRENT_HEIGHT, 0.1f);
        glm::mat4 view = camera.GetViewMatrix();
        modelShader.setMat4("projection", glm::value_ptr(projection));
        modelShader.setMat4("view", glm::value_ptr(view));

        // draw the lights
# if 1
        glBindVertexArray(lightCubeVAO);
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", glm::value_ptr(projection));
        lightCubeShader.setMat4("view", glm::value_ptr(view));
        for (int i = 0; i < N_POINTS_LIGHTS; i++) {
            glm::mat4 model(1.f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(.2f));

            lightCubeShader.setMat4("model", model);
            lightCubeShader.setVec3("color", pointLightColors[i]);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
# endif

        // pointLightsPositions[0] = camera.Position;
        // set shader lighting data
        modelShader.use();
        modelShader.setFloat("time", glfwGetTime());
        modelShader.setVec3("viewPos", camera.Position);
        for (int i = 0; i < N_POINTS_LIGHTS; i++) {
            modelShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
            modelShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", glm::vec3(.01f) * pointLightColors[i]);
            modelShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", glm::vec3(.8f) * pointLightColors[i]);
            modelShader.setVec3("pointLights[" + std::to_string(i) + "].specular", glm::vec3(1.f) * pointLightColors[i]);

            modelShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
            modelShader.setFloat("pointLights[" + std::to_string(i) + "].linear", i < 0 ? 0.027f : 0.007);
            modelShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", i < 0 ? 0.0028f : 0.0002);
        }

        modelShader.setVec3("spotLight.position", spotlightPos);
        glm::vec3 spotlight_dir = glm::vec3(0, -10, 0) + glm::vec3(glm::cos(currentFrame), 0, glm::sin(currentFrame)) * 3.f;
        modelShader.setVec3("spotLight.direction", spotlight_dir);
        modelShader.setVec3("spotLight.diffuse", glm::vec3(1,0,1));
        modelShader.setVec3("spotLight.specular", glm::vec3(0,1,0));
        modelShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(5.f)));
        modelShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(20.f)));
        
        // draw creatures
        for (int i = 0; i < N; i++) {

            auto transforms = animators[i].GetFinalBoneMatrices();
            for (int j = 0; j < transforms.size(); ++j)
                modelShader.setMat4("finalBonesMatrices[" + std::to_string(j) + "]", transforms[j]);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, creature_positions[i]);
            model = glm::rotate(model, glm::radians(createure_rotations[i]), glm::vec3(0, 1, 0));
            model = glm::scale(model, glm::vec3(1.f));
            modelShader.setMat4("model", model);

            modelShader.setBool("calcBones", true);

            creatureModel.Draw(modelShader);
        }
        
        // draw running monster
        static float run_progress = 0.f;
        run_progress += deltaTime * .1f;
        if (run_progress >= 1.f) {
            run_progress -= 1.f;
        }
        size_t size = sizeof(running_path1) / sizeof(path_point);
        auto trans = getTransformOnPath(run_progress, running_path1, size);

        auto transforms = runningAnimator.GetFinalBoneMatrices();
        for (int j = 0; j < transforms.size(); ++j)
            modelShader.setMat4("finalBonesMatrices[" + std::to_string(j) + "]", transforms[j]);
        modelShader.setMat4("model", trans);
        runningCreatureModel.Draw(modelShader);

        // draw terrain
        modelShader.setBool("calcBones", false);
        modelShader.setMat4("model", glm::translate(glm::scale(glm::mat4(), glm::vec3(2.f)), glm::vec3(0,-4.5,0)));
        terrain.Draw(modelShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
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
    
   /* if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        auto& cam_pos = camera.Position;
        std::cout << cam_pos.x << ' ' << cam_pos.y << ' ' << cam_pos.z << '\n';
    }*/
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    //if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_T:
            {
                auto& cam_pos = camera.Position;
                std::cout << "{{ " << cam_pos.x << ", " << cam_pos.y << ", " << cam_pos.z << " }, ";
                std::cout << "{ " << camera.Front.x << ", " << camera.Front.y << ", " << camera.Front.z << " }},\n";
                break;
            }
        }
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