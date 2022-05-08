
#include<iostream>
#include"GL/glew.h"
#include"GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"
#include "arcball.h"
#include "glm/gtc/type_ptr.hpp"
#include"shaders.h"

unsigned int SCR_WIDTH = 900;
unsigned int SCR_HEIGHT = 900;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double x, double y);

// for arcball
float arcballSpeed = 0.2f;
static Arcball camArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
static Arcball modelArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
bool arcballCamRot = true;


int main() {
	GLFWwindow* window;
	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_SAMPLES, 16);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_ANY_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int width = SCR_WIDTH, height = SCR_HEIGHT;
	window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL); // *** Windoww creation ***
	if (!window) {
		std::cout << "Failed to create window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	GLuint program = LoadShaders("vshader.vs", "fshader.fs");
	GLint umodel = glGetUniformLocation(program, "model");
	GLint uview = glGetUniformLocation(program, "view");
	GLint uprojection = glGetUniformLocation(program, "projection");

	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	float cyl[18 * 5];
	float o = 0.0f;
	float o2 = 0.0f;
	for (int i = 0; i < 5; i++) {
		o2 += (glm::pi<float>() * 2) / 5;
		cyl[i * 18] = sin(o);
		cyl[i * 18 + 1] = -1.f;
		cyl[i * 18 + 2] = cos(o);
		cyl[i * 18 + 3] = sin(o2);
		cyl[i * 18 + 4] = -1.f;
		cyl[i * 18 + 5] = cos(o2);
		cyl[i * 18 + 6] = sin(o);
		cyl[i * 18 + 7] = 1.f;
		cyl[i * 18 + 8] = cos(o);
		cyl[i * 18 + 9] = sin(o2);
		cyl[i * 18 + 10] = 1.f;
		cyl[i * 18 + 11] = cos(o2);
		cyl[i * 18 + 12] = sin(o);
		cyl[i * 18 + 13] = 1.f;
		cyl[i * 18 + 14] = cos(o);
		cyl[i * 18 + 15] = sin(o2);
		cyl[i * 18 + 16] = -1.f;
		cyl[i * 18 + 17] = cos(o2);
		o += (glm::pi<float>() * 2) / 5;
	}

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cyl), cyl, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
	glBindVertexArray(0);


	float animation_time = 0;
	float frametime = 0;
	bool stop = 0;

	glViewport(0, 0, width, height);



	while (!glfwWindowShouldClose(window)) { 
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
		float radius = 5.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(0.0f, 3.0f, 7.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		view = view * camArcBall.createViewRotationMatrix();
		model = model * modelArcBall.createModelRotationMatrix(view);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		animation_time += frametime;
		glUseProgram(program);
		glEnableVertexAttribArray(0);
        glBindVertexArray(VAO); 
		glUniformMatrix4fv(umodel, 1, false, glm::value_ptr(model));
		glUniformMatrix4fv(uview, 1, false, glm::value_ptr(view));
		glUniformMatrix4fv(uprojection, 1, false, glm::value_ptr(projection));
		glDrawArrays(GL_TRIANGLES, 0, 6*18); 

		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	glfwTerminate();
	return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		camArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
		modelArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
	}
	else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		arcballCamRot = !arcballCamRot;
		if (arcballCamRot) {
			std::cout << "ARCBALL: camera rotation mode" << std::endl;
		}
		else {
			std::cout << "ARCBALL: model  rotation mode" << std::endl;
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (arcballCamRot)
		camArcBall.mouseButtonCallback(window, button, action, mods);
	else
		modelArcBall.mouseButtonCallback(window, button, action, mods);
}

void cursor_position_callback(GLFWwindow* window, double x, double y) {
	if (arcballCamRot)
		camArcBall.cursorCallback(window, x, y);
	else
		modelArcBall.cursorCallback(window, x, y);
}