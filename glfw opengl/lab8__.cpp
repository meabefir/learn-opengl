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
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


GLuint
VaoId,
VboId,
ColorBufferId,
ProgramIdv,
ProgramIdf,
viewLocation,
projLocation,
codColLocation,
depthLocation,
rendermode,
l1, l2,
codCol;
GLint objectColorLoc, lightColorLoc, lightDiffuseLoc, lightPosLoc, viewPosLoc;

// variabile pentru matricea de vizualizare
float Obsx = 0.0, Obsy = -600.0, Obsz = 0.f;
float Refx = 0.0f, Refy = 1000.0f, Refz = 0.0f;
float Vx = 0.0, Vy = 0.0, Vz = 1.0;

// variabile pentru matricea de proiectie
float width = 800, height = 600, znear = 0.1, fov = 45;

// matrice utilizate
glm::mat4 view, projection;

enum {
	Il_Frag, Il_Frag_Av, Il_Vert, Il_Vert_Av
};

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "tema8", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

#define N 30
#define BASE_OFFSET (N * 3 * 2)
#define BASE_OFFSET2 (BASE_OFFSET * 2)
	glm::vec3 vertices[(N * 3 * 2 + N * 6) * 2];
	float deltaAngle = glm::radians(360.f) / N;

	for (int i = 0; i < N; i++) {
		float currentDelta = (i)*deltaAngle;
		float nextDelta = (i + 1) * deltaAngle;

		vertices[6 * i + 0] = glm::vec3(glm::cos(currentDelta), -1.f, glm::sin(currentDelta));
		vertices[6 * i + 1] = glm::vec3(0, -1.f, 0);
		vertices[6 * i + 2] = glm::vec3(glm::cos(nextDelta), -1.f, glm::sin(nextDelta));
		vertices[6 * i + 3] = glm::vec3(0, -1.f, 0);
		vertices[6 * i + 4] = glm::vec3(0, -1.f, 0);
		vertices[6 * i + 5] = glm::vec3(0, -1.f, 0);

		vertices[BASE_OFFSET + 6 * i + 0] = glm::vec3(glm::cos(currentDelta), 1.f, glm::sin(currentDelta));
		vertices[BASE_OFFSET + 6 * i + 1] = glm::vec3(0, 1.f, 0);
		vertices[BASE_OFFSET + 6 * i + 2] = glm::vec3(glm::cos(nextDelta), 1.f, glm::sin(nextDelta));
		vertices[BASE_OFFSET + 6 * i + 3] = glm::vec3(0, 1.f, 0);
		vertices[BASE_OFFSET + 6 * i + 4] = glm::vec3(0, 1.f, 0);
		vertices[BASE_OFFSET + 6 * i + 5] = glm::vec3(0, 1.f, 0);

		vertices[BASE_OFFSET2 + 12 * i + 0] = glm::vec3(glm::cos(currentDelta), -1.f, glm::sin(currentDelta));
		vertices[BASE_OFFSET2 + 12 * i + 1] = glm::vec3(glm::cos(currentDelta), 0, glm::sin(currentDelta));
		vertices[BASE_OFFSET2 + 12 * i + 2] = glm::vec3(glm::cos(nextDelta), -1.f, glm::sin(nextDelta));
		vertices[BASE_OFFSET2 + 12 * i + 3] = glm::vec3(glm::cos(nextDelta), 0, glm::sin(nextDelta));
		vertices[BASE_OFFSET2 + 12 * i + 4] = glm::vec3(glm::cos(currentDelta), 1.f, glm::sin(currentDelta));
		vertices[BASE_OFFSET2 + 12 * i + 5] = glm::vec3(glm::cos(currentDelta), 0, glm::sin(currentDelta));

		vertices[BASE_OFFSET2 + 12 * i + 6] = glm::vec3(glm::cos(currentDelta), 1.f, glm::sin(currentDelta));
		vertices[BASE_OFFSET2 + 12 * i + 7] = glm::vec3(glm::cos(currentDelta), 0, glm::sin(currentDelta));
		vertices[BASE_OFFSET2 + 12 * i + 8] = glm::vec3(glm::cos(nextDelta), 1.f, glm::sin(nextDelta));
		vertices[BASE_OFFSET2 + 12 * i + 9] = glm::vec3(glm::cos(nextDelta), 0, glm::sin(nextDelta));
		vertices[BASE_OFFSET2 + 12 * i + 10] = glm::vec3(glm::cos(nextDelta), -1.f, glm::sin(nextDelta));
		vertices[BASE_OFFSET2 + 12 * i + 11] = glm::vec3(glm::cos(nextDelta), 0, glm::sin(nextDelta));
	}

	unsigned int vbo, vao;
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);



	Shader ProgramIdv = Shader("10_02v_Shader.vert", "10_02v_Shader.frag");
	Shader ProgramIdf = Shader("10_02f_Shader.vert", "10_02f_Shader.frag");
	
	objectColorLoc = glGetUniformLocation(ProgramIdf.ID, "objectColor");
	lightColorLoc = glGetUniformLocation(ProgramIdf.ID, "lightColor");
	lightDiffuseLoc = glGetUniformLocation(ProgramIdf.ID, "lightDiffuse");
	lightPosLoc = glGetUniformLocation(ProgramIdf.ID, "lightPos");
	viewPosLoc = glGetUniformLocation(ProgramIdf.ID, "viewPos");
	viewLocation = glGetUniformLocation(ProgramIdf.ID, "view");
	projLocation = glGetUniformLocation(ProgramIdf.ID, "projection");

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		ProgramIdf.use();
		//ProgramIdf.setMat4("view", GL_FALSE, camera.GetViewMatrix());
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
		projection = glm::infinitePerspective(fov, GLfloat(width) / GLfloat(height), znear);
		glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

		glm::mat4 model(1.f);
		model = glm::scale(model, glm::vec3(10.f));
		ProgramIdf.setMat4("view", glm::value_ptr(camera.GetViewMatrix()));
		ProgramIdf.setMat4("projection", glm::value_ptr(projection));
		ProgramIdf.setMat4("model", glm::value_ptr(model));

		// variabile uniforme pentru iluminare
		glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.4f);
		glUniform3f(lightDiffuseLoc, 1.0f, .0f, .0f);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, 100.f, 130.f, 100.f);
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);


		glBindVertexArray(vao);

		glDrawArrays(GL_TRIANGLES, 0, N * 3 * 2 + N * 6);

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
		rendermode = (rendermode + 1) % 4;
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