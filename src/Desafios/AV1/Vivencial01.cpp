/* Vivencial 01 - código adaptado de HelloTextures3D.cpp e https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Modificado por Gabriela Spanemberg Bado
 * para a disciplina de Computação Gráfica - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 11/05/2026
 */

// ---- HEADERS GLFW, GLAD, GLM ----
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <assert.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

using namespace std;

// ---- STRUCTS ----
struct Object3D
{
	glm::vec3 position;
	float scale;

	bool rotateX;
	bool rotateY;
	bool rotateZ;

	float angleX;
	float angleY;
	float angleZ;
};

// ---- VARIÁVEIS GLOBAIS ----
vector<Object3D> objects = {
		{glm::vec3(-0.5f, -0.5f, 0.0f),
		 0.4f,
		 false, false, false,
		 0.0f, 0.0f, 0.0f},
		{glm::vec3(0.5f, -0.5f, 0.0f),
		 0.4f,
		 false, false, false,
		 0.0f, 0.0f, 0.0f},
		{glm::vec3(-0.5f, 0.5f, 0.0f),
		 0.4f,
		 false, false, false,
		 0.0f, 0.0f, 0.0f},
		{glm::vec3(0.5f, 0.5f, 0.0f),
		 0.4f,
		 false, false, false,
		 0.0f, 0.0f, 0.0f}};

int selectedObjectIndex = 0;
int nVertices = 0;

const GLuint WIDTH = 1000, HEIGHT = 1000;
const float MOVEMENT_STEP = 0.1f;
const float SCALE_STEP = 0.1f;

// ---- DECLARAÇÃO DE FUNÇÕES ----
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

void handleRotationKeys(int key);
void handleMovementKeys(int key);
void handleScaleKeys(int key);

void prepareFrame();
void updateObjectsRotation(float frameDelta);
glm::mat4 buildObjectModelMatrix(const Object3D &object);
Object3D &getSelectedObject();

void showControlsGuide();

void renderObject(
		const Object3D &object,
		GLuint objectVAO,
		GLint modelMatrixLocation);

int setupShader();
string loadTexturePathFromMTL(const string &mtlPath);
GLuint loadTexture(const string &texturePath);
GLuint loadSimpleOBJ(string filePath, int &nVertices);

// ---- SHADERS ----
const GLchar *vertexShaderSource = "#version 450\n"
																	 "layout (location = 0) in vec3 position;\n"
																	 "layout (location = 1) in vec3 color;\n"
																	 "layout (location = 2) in vec2 texCoord;\n"
																	 "uniform mat4 model;\n"
																	 "out vec2 fragTexCoord;\n"
																	 "void main()\n"
																	 "{\n"
																	 "   gl_Position = model * vec4(position, 1.0);\n"
																	 "   fragTexCoord = texCoord;\n"
																	 "}\0";

const GLchar *fragmentShaderSource = "#version 450\n"
																		 "in vec2 fragTexCoord;\n"
																		 "out vec4 color;\n"
																		 "uniform sampler2D texture1;\n"
																		 "void main()\n"
																		 "{\n"
																		 "   color = texture(texture1, fragTexCoord);\n"
																		 "}\n\0";

// ---- FUNÇÃO MAIN ----
int main()
{
	glfwInit();

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Atividade Vivencial 01 -- Gabriela Bado", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	showControlsGuide();

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	GLuint shaderID = setupShader();
	GLuint VAO = loadSimpleOBJ("../assets/Modelos3D/Cube.obj", nVertices);
	string textureName = loadTexturePathFromMTL("../assets/Modelos3D/Cube.mtl");
	GLuint textureID = loadTexture("../assets/Modelos3D/" + textureName);

	glUseProgram(shaderID);

	glUniform1i(glGetUniformLocation(shaderID, "texture1"), 0);
	GLint modelMatrixLocation = glGetUniformLocation(shaderID, "model");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	float previousFrameTime = (float)glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		float frameTime = (float)glfwGetTime();
		float frameDelta = frameTime - previousFrameTime;
		previousFrameTime = frameTime;

		glfwPollEvents();
		updateObjectsRotation(frameDelta);

		prepareFrame();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		for (const Object3D &object : objects)
		{
			renderObject(
					object,
					VAO,
					modelMatrixLocation);
		}

		glfwSwapBuffers(window);
	}

	glDeleteTextures(1, &textureID);
	glDeleteVertexArrays(1, &VAO);

	glfwTerminate();
	return 0;
}

// ---- IMPLEMENTAÇÃO DAS FUNÇÕES ----
string loadTexturePathFromMTL(const string &mtlPath)
{
	ifstream file(mtlPath);

	if (!file.is_open())
	{
		cout << "Erro ao abrir MTL"
				 << endl;
		return "";
	}

	string line;

	while (getline(file, line))
	{
		istringstream ss(line);
		string word;
		ss >> word;
		if (word == "map_Kd")
		{
			string textureName;
			ss >> textureName;
			return textureName;
		}
	}

	return "";
}

GLuint loadTexture(
		const string &texturePath)
{
	GLuint textureID;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);

	int width;
	int height;
	int channels;

	unsigned char *data = stbi_load(
			texturePath.c_str(),
			&width,
			&height,
			&channels,
			0);

	if (!data)
	{
		cout << "Erro ao carregar textura: "
				 << texturePath
				 << endl;

		return 0;
	}
	GLenum format = (channels == 4)
											? GL_RGBA
											: GL_RGB;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	return textureID;
}

void prepareFrame()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Object3D &getSelectedObject()
{
	return objects[selectedObjectIndex];
}

void updateObjectsRotation(float frameDelta)
{
	const float ROTATION_FACTOR = 1.0f;

	for (Object3D &object : objects)
	{
		if (object.rotateX)
			object.angleX += ROTATION_FACTOR * frameDelta;

		if (object.rotateY)
			object.angleY += ROTATION_FACTOR * frameDelta;

		if (object.rotateZ)
			object.angleZ += ROTATION_FACTOR * frameDelta;
	}
}

glm::mat4 buildObjectModelMatrix(const Object3D &object)
{
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	modelMatrix = glm::translate(
			modelMatrix,
			object.position);

	modelMatrix = glm::scale(
			modelMatrix,
			glm::vec3(object.scale));

	modelMatrix = glm::rotate(
			modelMatrix,
			object.angleX,
			glm::vec3(1.0f, 0.0f, 0.0f));

	modelMatrix = glm::rotate(
			modelMatrix,
			object.angleY,
			glm::vec3(0.0f, 1.0f, 0.0f));

	modelMatrix = glm::rotate(
			modelMatrix,
			object.angleZ,
			glm::vec3(0.0f, 0.0f, 1.0f));

	return modelMatrix;
}

void renderObject(const Object3D &object, GLuint objectVAO, GLint modelMatrixLocation)
{
	glm::mat4 modelMatrix = buildObjectModelMatrix(object);

	glUniformMatrix4fv(
			modelMatrixLocation,
			1,
			GL_FALSE,
			glm::value_ptr(modelMatrix));

	glBindVertexArray(objectVAO);
	glDrawArrays(GL_TRIANGLES, 0, nVertices);
	glBindVertexArray(0);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (action != GLFW_PRESS && action != GLFW_REPEAT)
		return;

	if (key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_TAB)
	{
		selectedObjectIndex =
				(selectedObjectIndex + 1) % objects.size();

		cout << "Object selected: "
				 << selectedObjectIndex + 1
				 << endl;

		return;
	}

	handleRotationKeys(key);
	handleMovementKeys(key);
	handleScaleKeys(key);
}

void handleRotationKeys(int key)
{
	Object3D &object = getSelectedObject();

	if (key == GLFW_KEY_X)
		object.rotateX = !object.rotateX;

	if (key == GLFW_KEY_Y)
		object.rotateY = !object.rotateY;

	if (key == GLFW_KEY_Z)
		object.rotateZ = !object.rotateZ;
}

void handleMovementKeys(int key)
{
	Object3D &object = getSelectedObject();

	if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT)
		object.position.x -= MOVEMENT_STEP;

	if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT)
		object.position.x += MOVEMENT_STEP;

	if (key == GLFW_KEY_I || key == GLFW_KEY_UP)
		object.position.y += MOVEMENT_STEP;

	if (key == GLFW_KEY_J || key == GLFW_KEY_DOWN)
		object.position.y -= MOVEMENT_STEP;

	if (key == GLFW_KEY_W)
		object.position.z -= MOVEMENT_STEP;

	if (key == GLFW_KEY_S)
		object.position.z += MOVEMENT_STEP;
}

void handleScaleKeys(int key)
{
	Object3D &object = getSelectedObject();

	if (key == GLFW_KEY_LEFT_BRACKET && object.scale > 0.2f)
		object.scale -= SCALE_STEP;

	if (key == GLFW_KEY_RIGHT_BRACKET && object.scale < 1.2f)
		object.scale += SCALE_STEP;
}

int setupShader()
{

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
							<< infoLog << std::endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
							<< infoLog << std::endl;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
							<< infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

void showControlsGuide()
{
	cout << endl;
	cout << "==========================================" << endl;
	cout << " Bem-vindo ao programa da Atividade Vivencial 01 " << endl;
	cout << " Controle seu objeto utilizando as seguintes teclas:" << endl;
	cout << "------------------------------------------" << endl;
	cout << " Selecionar : TAB (troca objeto ativo)" << endl;
	cout << " Movimento X : A | D  ou  <- | ->" << endl;
	cout << " Movimento Y : I | J  ou  /\\ | \\/" << endl;
	cout << " Movimento Z : W | S" << endl;
	cout << " Rotacao     : X | Y | Z" << endl;
	cout << " Escala      : [ | ]" << endl;
	cout << " Sair        : ESC" << endl;
	cout << "==========================================" << endl;
	cout << endl;
}