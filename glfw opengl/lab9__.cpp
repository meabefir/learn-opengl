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

glm::mat4 myMatrix, matrRot;

GLuint
VaoId,
VboId,
EboId,
ColorBufferId,
ProgramIdv,
ProgramIdf,
viewLocation,
projLocation,
codColLocation,
depthLocation,
rendermode,
l1, l2,
ProgramId,
myMatrixLocation,
matrUmbraLocation,
matrRotlLocation,
lightColorLocation,
lightPosLocation,
viewPosLocation;
GLint objectColorLoc, lightColorLoc, lightDiffuseLoc, lightPosLoc, viewPosLoc;

int codCol;
float PI = 3.141592;

float width = 800, height = 600, xwmin = -800.f, xwmax = 800, ywmin = -600, ywmax = 600, znear = 0.1, zfar = 1, fov = 45;

// elemente pentru matricea de vizualizare
float Refx = 0.0f, Refy = 0.0f, Refz = 0.0f;
float alpha = PI / 8, beta = 0.0f, dist = 400.0f;
float Obsx, Obsy, Obsz;
float Vx = 0.0, Vy = 0.0, Vz = 1.0;
// matrice utilizate
glm::mat4 view, projection;

// sursa de lumina
float xL = 500.f, yL = 100.f, zL = 400.f;

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

float matrUmbra[4][4];

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
	float h = 50.f;
	float radius = 25.f;
	for (int i = 0; i < N; i++) {
		float currentDelta = (i)*deltaAngle;
		float nextDelta = (i + 1) * deltaAngle;

		vertices[6 * i + 0] = glm::vec3(glm::cos(currentDelta) * radius, -1.f * h, glm::sin(currentDelta) * radius);
		vertices[6 * i + 1] = glm::vec3(0, -1.f, 0);
		vertices[6 * i + 2] = glm::vec3(glm::cos(nextDelta) * radius, -1.f * h, glm::sin(nextDelta) * radius);
		vertices[6 * i + 3] = glm::vec3(0, -1.f, 0);
		vertices[6 * i + 4] = glm::vec3(0, -1.f * h, 0);
		vertices[6 * i + 5] = glm::vec3(0, -1.f, 0);

		vertices[BASE_OFFSET + 6 * i + 0] = glm::vec3(glm::cos(currentDelta) * radius, 1.f * h, glm::sin(currentDelta) * radius);
		vertices[BASE_OFFSET + 6 * i + 1] = glm::vec3(0, 1.f, 0);
		vertices[BASE_OFFSET + 6 * i + 2] = glm::vec3(glm::cos(nextDelta) * radius, 1.f * h, glm::sin(nextDelta) * radius);
		vertices[BASE_OFFSET + 6 * i + 3] = glm::vec3(0, 1.f, 0);
		vertices[BASE_OFFSET + 6 * i + 4] = glm::vec3(0, 1.f * h, 0);
		vertices[BASE_OFFSET + 6 * i + 5] = glm::vec3(0, 1.f, 0);

		vertices[BASE_OFFSET2 + 12 * i + 0] = glm::vec3(glm::cos(currentDelta) * radius, -1.f * h, glm::sin(currentDelta) * radius);
		vertices[BASE_OFFSET2 + 12 * i + 1] = glm::vec3(glm::cos(currentDelta), 0, glm::sin(currentDelta));
		vertices[BASE_OFFSET2 + 12 * i + 2] = glm::vec3(glm::cos(nextDelta) * radius, -1.f * h, glm::sin(nextDelta) * radius);
		vertices[BASE_OFFSET2 + 12 * i + 3] = glm::vec3(glm::cos(nextDelta), 0, glm::sin(nextDelta));
		vertices[BASE_OFFSET2 + 12 * i + 4] = glm::vec3(glm::cos(currentDelta) * radius, 1.f * h, glm::sin(currentDelta) * radius);
		vertices[BASE_OFFSET2 + 12 * i + 5] = glm::vec3(glm::cos(currentDelta), 0, glm::sin(currentDelta));

		vertices[BASE_OFFSET2 + 12 * i + 6] = glm::vec3(glm::cos(currentDelta) * radius, 1.f * h, glm::sin(currentDelta) * radius);
		vertices[BASE_OFFSET2 + 12 * i + 7] = glm::vec3(glm::cos(currentDelta), 0, glm::sin(currentDelta));
		vertices[BASE_OFFSET2 + 12 * i + 8] = glm::vec3(glm::cos(nextDelta) * radius, 1.f * h, glm::sin(nextDelta) * radius);
		vertices[BASE_OFFSET2 + 12 * i + 9] = glm::vec3(glm::cos(nextDelta), 0, glm::sin(nextDelta));
		vertices[BASE_OFFSET2 + 12 * i + 10] = glm::vec3(glm::cos(nextDelta) * radius, -1.f * h, glm::sin(nextDelta) * radius);
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


	GLfloat Vertices[] =
	{
		// coordonate                   // culori			// normale
		// varfuri "ground"
	   -1500.0f,  -1500.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		1500.0f,  -1500.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		1500.0f,  1500.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	   -1500.0f,  1500.0f,  0.0f,  0.0f, 0.0f, 1.0f,

	   -50.0f,  -50.0f, 50.0f,   -1.0f, -1.0f, -1.0f,
		 50.0f,  -50.0f,  50.0f,   1.0f, -1.0f, -1.0f,
		 50.0f,  50.0f,  50.0f,  1.0f, 1.0f, -1.0f,
		-50.0f,  50.0f, 50.0f,   -1.0f, 1.0f, -1.0f,
		-50.0f,  -50.0f, 150.0f,   -1.0f, -1.0f, 1.0f,
		 50.0f,  -50.0f,  150.0f,   1.0f, -1.0f, 1.0f,
		 50.0f,  50.0f,  150.0f,   1.0f, 1.0f, 1.0f,
		-50.0f,  50.0f, 150.0f,  -1.0f, 1.0f, 1.0f,
	};

	GLubyte Indices[] =
	{
		// fetele "ground"
		 1, 2, 0,   2, 0, 3,

		 5, 6, 4,   6, 4, 7,
		 6, 7, 10, 10, 7, 11,
		 11, 7, 8,   8, 7, 4,
		 8, 4, 9,   9, 4, 5,
		 5, 6, 9,   9, 6, 10,
		 9, 10, 8,  8, 10, 11,
	};

	glGenVertexArrays(1, &VaoId);
	glGenBuffers(1, &VboId);
	glGenBuffers(1, &EboId);
	glBindVertexArray(VaoId);

	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	// atributul 1 = normale
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));


	Shader ProgramId = Shader("10_04_Shader.vert", "10_04_Shader.frag");
	ProgramId.use();
	ProgramId.setVec3("fogColor", glm::vec3(.3, .3, .3));
	ProgramId.setFloat("fogStr", .9f);
	myMatrix = glm::mat4(1.0f);
	myMatrixLocation = glGetUniformLocation(ProgramId.ID, "myMatrix");
	matrUmbraLocation = glGetUniformLocation(ProgramId.ID, "matrUmbra");
	viewLocation = glGetUniformLocation(ProgramId.ID, "view");
	projLocation = glGetUniformLocation(ProgramId.ID, "projection");
	lightColorLocation = glGetUniformLocation(ProgramId.ID, "lightColor");
	lightPosLocation = glGetUniformLocation(ProgramId.ID, "lightPos");
	viewPosLocation = glGetUniformLocation(ProgramId.ID, "viewPos");
	codColLocation = glGetUniformLocation(ProgramId.ID, "codCol");

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);


		// matrice de vizualizare + proiectie
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &camera.GetViewMatrix()[0][0]);
		projection = glm::infinitePerspective(fov, GLfloat(width) / GLfloat(height), znear);
		glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

		/*cout << xL << ' ' << yL << ' ' << zL << '\n';
		cout << camera.Position.x << ' ' << camera.Position.y << ' ' << camera.Position.z << "\n\n";*/

		// matricea pentru umbra
		float D = 50.f;
		matrUmbra[0][0] = zL + D; matrUmbra[0][1] = 0; matrUmbra[0][2] = 0; matrUmbra[0][3] = 0;
		matrUmbra[1][0] = 0; matrUmbra[1][1] = zL + D; matrUmbra[1][2] = 0; matrUmbra[1][3] = 0;
		matrUmbra[2][0] = -xL; matrUmbra[2][1] = -yL; matrUmbra[2][2] = D; matrUmbra[2][3] = -1;
		matrUmbra[3][0] = -D * xL; matrUmbra[3][1] = -D * yL; matrUmbra[3][2] = -D * zL; matrUmbra[3][3] = zL;
		glUniformMatrix4fv(matrUmbraLocation, 1, GL_FALSE, &matrUmbra[0][0]);

		// variabile uniforme pentru iluminare
		glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLocation, xL, yL, zL);
		glUniform3f(viewPosLocation, camera.Position.x, camera.Position.y, camera.Position.z);

		// plan
		myMatrixLocation = glGetUniformLocation(ProgramId.ID, "myMatrix");
		glBindVertexArray(VaoId);
		codCol = 0;
		glUniform1i(codColLocation, codCol);
		myMatrix = glm::translate(glm::mat4(122.0f), glm::vec3(5.f, 0.f, 5.0f));
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

		////// cilindru
		glBindVertexArray(vao);
		myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(550.f, 0.f, -350.0));
		myMatrix = glm::scale(myMatrix, glm::vec3(1.f));
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, N * 3 * 2 + N * 6);

		////// umbra cilindru
		codCol = 1;
		glUniform1i(codColLocation, codCol);
		myMatrix = glm::mat4(1.0f);
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
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