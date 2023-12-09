#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"

#include "shader.h"
#include "camera.h"
#include "model.hpp"
#include "globals.hpp"
#include "constants.hpp"
#include "calc.hpp"

#include <iostream>

extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

float initial_aspect_ratio = (float)SCR_WIDTH / (float)SCR_HEIGHT;
float current_aspect_ratio = initial_aspect_ratio;
float width_aspect = 1.f, height_aspect = 1.f;

int CURRENT_WIDTH = SCR_WIDTH;
int CURRENT_HEIGHT = SCR_HEIGHT;

MousePicker* mousePicker = nullptr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
unsigned int loadTexture(const char* path);

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = CURRENT_WIDTH / 2.0f;
float lastY = CURRENT_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Model *selectedModel = nullptr;
vector<Model*> models;

glm::vec3 getTranslation(const glm::mat4& m) {
    return glm::vec3(m[3][0], m[3][1], m[3][2]);    
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
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
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    mousePicker = new MousePicker(&camera, glm::mat4());

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

    const int N_POINTS_LIGHTS = 2;
    glm::vec3 pointLightsPositions[N_POINTS_LIGHTS] = {
        glm::vec3(7.f, 2.f, 4.0f),
        glm::vec3(1.f, 8.f, 4.0f)
    };


#pragma region floor
    // floor
    Shader floorShader("floor.vert", "floor.frag");
    unsigned int floor_diffuse = loadTexture("assets/cobble.jpg");
    floorShader.use();
    float dim_textura = 15.f;

    floorShader.setFloat("texture_scale", dim_textura);
    dim_textura = 1.f;
    GLfloat v[] = {
        // pozitie              normale             tex_coords
        -0.5f, 0.5f, 0.0f,     0, 0, 1,          0.0f, 1.0f * dim_textura,                      
        -0.5f, -0.5f, 0.0f,    0, 0, 1,          0.0f, 0.0f,                                   
        0.5f, 0.5f, 0.0f,      0, 0, 1,          1.0f * dim_textura, 1.0f * dim_textura,        

        -0.5f, -0.5f, 0.0f,    0, 0, 1,          0.0f, 0.0f,                                   
        0.5f, -0.5f, 0.0f,     0, 0, 1,          1.0f * dim_textura, 0.0f,                     
        0.5f, 0.5f, 0.0f,      0, 0, 1,          1.0f * dim_textura, 1.0f * dim_textura,        
    };

    cout << sizeof(v) << '\n';
    cout << sizeof(float) << '\n';
    unsigned int vbo_floor, vao_floor;
    glGenBuffers(1, &vbo_floor);
    glGenVertexArrays(1, &vao_floor);
    glBindVertexArray(vao_floor);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_floor);
    cout << sizeof(v) << '\n';
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), &v, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

    // generate color texture attachment pt reflectie
    unsigned int reflection_texture;
    glGenTextures(1, &reflection_texture);

    // texturi perete
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floor_diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, reflection_texture);

    floorShader.use();
    floorShader.setInt("texture_diffuse1", 0);
    floorShader.setInt("tex_reflection", 1);
#pragma endregion floor

#pragma region viewport
    dim_textura = 2.f;
    float scale = 1.f;
    GLfloat viewport_f[] = {
        // pozitie              normale             tex_coords
        -0.5f * scale, 0.5f * scale, 0.0f,              0.0f, 1.0f * dim_textura,
        -0.5f * scale, -0.5f * scale, 0.0f,             0.0f, 0.0f,
        0.5f * scale, 0.5f * scale, 0.0f,              1.0f * dim_textura, 1.0f * dim_textura,

        -0.5f * scale, -0.5f * scale, 0.0f,         0.0f, 0.0f,
        0.5f * scale, -0.5f * scale, 0.0f,             1.0f * dim_textura, 0.0f,
        0.5f * scale, 0.5f * scale, 0.0f,             1.0f * dim_textura, 1.0f * dim_textura,
    };

    unsigned int vbo_viewport, vao_viewport;
    glGenBuffers(1, &vbo_viewport);
    glGenVertexArrays(1, &vao_viewport);
    glBindVertexArray(vao_viewport);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_viewport);
    glBufferData(GL_ARRAY_BUFFER, sizeof(viewport_f), &viewport_f, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    // texturi perete
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, reflection_texture);

    Shader viewportShader("viewport.vert", "viewport.frag");
    viewportShader.use();
    viewportShader.setInt("tex", 0);

#pragma endregion viewport

#pragma region reflection

    unsigned int reflection_FBO;
    glGenFramebuffers(1, &reflection_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, reflection_FBO);

    float ss = 2.f;
    // generate color texture attachment
    glBindTexture(GL_TEXTURE_2D, reflection_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH * ss, SCR_HEIGHT * ss , 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflection_texture, 0);

    // generate depth buffer attachment
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH * ss, SCR_HEIGHT * ss);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

#pragma endregion reflection

    Shader modelShader("model_light.vert", "model_light.frag");
    modelShader.use();
    modelShader.setFloat("fogStr", .1f);
    modelShader.setVec3("fogColor", glm::vec3(1.f, 1.f, 0.f));
    // Shader modelShader("model_light.vert", "diffuse.frag");
    // Shader modelShader("model_light.vert", "depth_test.frag");
    Shader lightCubeShader("light_cube.vert", "light_cube.frag");
    Model ourModel("assets/sponza/model.obj");
     Model tren("assets/tren/model.obj");
     //Model tren("assets/turret/model.obj");
     stbi_set_flip_vertically_on_load(true);
     Model backpack("assets/backpack/model.obj");

     models.push_back(&backpack);
     models.push_back(&tren);

     const int nModels = 50;
     for (int i = 0; i < nModels; i++) {
         glm::mat4 mm;
         mm = glm::translate(mm, glm::vec3(8, 3, 0));
         mm = glm::scale(mm, glm::vec3(.8));



     }

     glm::mat4 mm;
     mm = glm::translate(mm, glm::vec3(8, 3, 0));
     mm = glm::scale(mm, glm::vec3(.8));
     backpack.transform = mm;
     
#pragma region lighting_shader_data
     // set shader lighting data
     modelShader.use();
     for (int i = 0; i < N_POINTS_LIGHTS; i++) {
         modelShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightsPositions[i]);
         modelShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", glm::vec3(.01f));
         modelShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", glm::vec3(.8f));
         modelShader.setVec3("pointLights[" + std::to_string(i) + "].specular", glm::vec3(1.f));

         modelShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
         modelShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.027f);
         modelShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.0028f);
     }
     floorShader.use();
     for (int i = 0; i < N_POINTS_LIGHTS; i++) {
         floorShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightsPositions[i]);
         floorShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", glm::vec3(.01f));
         floorShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", glm::vec3(.8f));
         floorShader.setVec3("pointLights[" + std::to_string(i) + "].specular", glm::vec3(1.f));

         floorShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
         floorShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.027f);
         floorShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.0028f);
     }

#pragma endregion lighting_shader_data

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

        // update camera pos for projection and view
        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        
        glm::mat4 projection = glm::infinitePerspective(glm::radians(camera.Zoom), (float)CURRENT_WIDTH / (float)CURRENT_HEIGHT, 0.1f);
        mousePicker->projectionMatrix = projection;
        mousePicker->update(lastX, lastY);

        modelShader.use();
        modelShader.setMat4("projection", glm::value_ptr(projection));
        modelShader.setMat4("view", glm::value_ptr(camera.GetViewMatrix()));

        floorShader.use();
        floorShader.setMat4("projection", glm::value_ptr(projection));
        floorShader.setMat4("view", glm::value_ptr(camera.GetViewMatrix()));
        floorShader.setFloat("aspect_ratio", initial_aspect_ratio/current_aspect_ratio);
        /*floorShader.setFloat("width_aspect", width_aspect);
        floorShader.setFloat("height_aspect", height_aspect);*/

        

        // DRAWING BEGINS
        modelShader.use();
        modelShader.setFloat("time", glfwGetTime());
        modelShader.setVec3("spotLight.diffuse", glm::vec3(1.0f));
        modelShader.setVec3("spotLight.specular", glm::vec3(1.0f));
        modelShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(3.f)));
        modelShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(12.f)));

        floorShader.use();
        floorShader.setFloat("time", glfwGetTime());
        floorShader.setVec3("spotLight.diffuse", glm::vec3(1.0f));
        floorShader.setVec3("spotLight.specular", glm::vec3(1.0f));
        floorShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(3.f)));
        floorShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(12.f)));

#pragma region draw_reflection
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, reflection_FBO);
        glViewport(0, 0, SCR_WIDTH * ss, SCR_HEIGHT * ss);
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        camera.flip();

        modelShader.use();
        modelShader.setFloat("time", glfwGetTime());
        modelShader.setVec3("viewPos", camera.Position);
        modelShader.setVec3("spotLight.position", camera.Position);
        modelShader.setVec3("spotLight.direction", camera.Front);

        glBindVertexArray(lightCubeVAO);
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", glm::value_ptr(projection));
        lightCubeShader.setMat4("view", glm::value_ptr(camera.GetViewMatrix()));
        lightCubeShader.setVec3("color", glm::vec3(1.f));
        for (int i = 0; i < N_POINTS_LIGHTS; i++) {
            glm::mat4 model(1.f);
            model = glm::translate(model, pointLightsPositions[i]);
            model = glm::scale(model, glm::vec3(.2f));

            lightCubeShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        modelShader.use();
        modelShader.setMat4("view", glm::value_ptr(camera.GetViewMatrix()));
        // draw the model
        modelShader.use();
        for (auto& model : models) {
            modelShader.setMat4("model", model->transform);
            modelShader.setBool("selected", model->selected ? true : false);

            model->Draw(modelShader);
        }
#pragma endregion draw_reflection

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, CURRENT_WIDTH, CURRENT_HEIGHT);
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        camera.flip();

        modelShader.use();
        modelShader.setFloat("time", glfwGetTime());
        modelShader.setVec3("viewPos", camera.Position);
        modelShader.setVec3("spotLight.position", camera.Position);
        modelShader.setVec3("spotLight.direction", camera.Front);
        floorShader.use();
        floorShader.setFloat("time", glfwGetTime());
        floorShader.setVec3("viewPos", camera.Position);
        floorShader.setVec3("spotLight.position", camera.Position);
        floorShader.setVec3("spotLight.direction", camera.Front);

# if 1
        //glEnable(GL_CLIP_DISTANCE0);
        // DRAW LIGHTS
        glBindVertexArray(lightCubeVAO);
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", glm::value_ptr(projection));
        lightCubeShader.setMat4("view", glm::value_ptr(camera.GetViewMatrix()));
        lightCubeShader.setVec3("color", glm::vec3(1.f));
        for (int i = 0; i < N_POINTS_LIGHTS; i++) {
            glm::mat4 model(1.f);
            model = glm::translate(model, pointLightsPositions[i]);
            model = glm::scale(model, glm::vec3(.2f));

            lightCubeShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // mouse picked world ray
# endif
        
        modelShader.use();
        modelShader.setMat4("view", glm::value_ptr(camera.GetViewMatrix()));

        // draw the model
        for (auto& model : models) {
            glm::mat4 m = glm::mat4(1.0f);
            modelShader.use();
            modelShader.setMat4("model", model->transform);
            modelShader.setBool("selected", model->selected ? true : false);

            model->Draw(modelShader);
            if (model->selected) {
                model->drawBB(lightCubeShader, lightCubeVAO);
            }
            else {
                if (selectedModel != nullptr && selectedModel->snap_idx != -1) {
                    model->drawBB(lightCubeShader, lightCubeVAO, true);
                }
            }
        }

        // draw the floor
        floorShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        model = glm::scale(model, glm::vec3(50.f));	// it's a bit too big for our scene, so scale it down
        floorShader.setMat4("model", model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floor_diffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, reflection_texture);
        glBindVertexArray(vao_floor);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // draw world ray cube
        /*if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
            lightCubeShader.use();
            lightCubeShader.setVec3("color", glm::vec3(1.f, 0.f, 0.f));
            glDisable(GL_DEPTH_TEST);
            model = glm::mat4(1.f);
            model = glm::translate(model, mousePicker->currentRay * 3.f + camera.Position);
            model = glm::scale(model, glm::vec3(.01f));
            lightCubeShader.setMat4("model", model);
            glBindVertexArray(lightCubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glEnable(GL_DEPTH_TEST);
        }*/

        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // test


        glDisable(GL_DEPTH_TEST);
        //glDisable(GL_CULL_FACE);
        model = glm::mat4(1.f);
        float s = .2f;
        model = glm::scale(model, glm::vec3(CURRENT_WIDTH * s, CURRENT_HEIGHT * s, 1.f));
        model = glm::translate(model, glm::vec3(.5, .5, 0.f));
        projection = glm::ortho(0.f, (float)CURRENT_WIDTH, 0.f, (float)CURRENT_HEIGHT, 0.f, 1000.f);
        //projection = glm::ortho(0, (int)10, 0, (int)10);
        viewportShader.use();
        viewportShader.setMat4("model", model);
        viewportShader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, reflection_texture);

        glBindVertexArray(vao_viewport);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

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

void toggleMouseCapture(GLFWwindow* window) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void dragObjects() {
    if (selectedModel != nullptr) {
        float plane_x = 0, plane_y = 1, plane_z = 0, plane_d = 0;

        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(selectedModel->transform, scale, rotation, translation, skew, perspective);

        // find vertex of other model that is closest only if item is grabbed by bb point
        bool overlapping_point = false;
        glm::vec3 closest;
        if (selectedModel->snap_idx != -1) {
            float min_dist = 999999.f;
            for (auto& model : models) {
                if (model == selectedModel) continue;

                int idx = model->getClosestPointToWorldRay();
                if (idx == -1) continue;

                glm::vec3 point_to_world = model->getSnapPointWorldCoord(idx);
                float dist = model->getPointDistanceToWorldRay(point_to_world);
                if (dist < min_dist) {
                    min_dist = dist;
                    closest = point_to_world;
                    overlapping_point = true;
                }
            }
        }
        if (selectedModel->snap_idx != -1) {
            plane_d = -selectedModel->getSnapPointWorldCoord().y;
        }
        else {
            plane_d = -translation.y;
        }

        glm::vec3 v = getLinePlaneIntersection(mousePicker->world_ray, camera.Position, plane_x, plane_y, plane_z, plane_d);        

        glm::vec3 move;
        glm::vec3 offset = translation;
        if (selectedModel->snap_idx != -1)
            offset = selectedModel->getSnapPointWorldCoord();
        move = v - offset;
        if (overlapping_point) {
            move = closest - offset;
        }
        selectedModel->transform = glm::translate(selectedModel->transform, glm::vec3(move.x, move.y, move.z));

    }
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

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        dragObjects();
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    {
        toggleMouseCapture(window);
    }
    else if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
        camera.Yaw -= 15.f;
        camera.updateCameraVectors();
    }
    else if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        camera.Yaw += 15.f;
        camera.updateCameraVectors();
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        glfwSetWindowSize(window, CURRENT_WIDTH * 1.1, CURRENT_HEIGHT * 1.1);
    }
}

void handleClick() {
    for (auto* model : models) {
        if (mousePicker->intersectsModel(model)) {

            if (selectedModel != nullptr)
                selectedModel->selected = false;
            selectedModel = model;
            selectedModel->selected = true;
            return;
        }
        else {
            if (selectedModel != nullptr) {
                // check if colliding with bb first
                selectedModel->snap_idx = selectedModel->getClosestPointToWorldRay();
                if (selectedModel->snap_idx == -1) {
                    selectedModel->selected = false;
                    selectedModel = nullptr;
                }
            }
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) return;
        handleClick();
        for (auto& model : models) {
            model->mouseDown();
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) return;
        for (auto& model : models) {
            model->mouseUp();
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
    CURRENT_WIDTH = width;
    CURRENT_HEIGHT = height;
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

    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
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