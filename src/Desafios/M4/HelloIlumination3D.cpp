/* Hello Ilumination 3D - código adaptado de HelloTextures3D.cpp e https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Modificado por Gabriela Spanemberg Bado
 * para a disciplina de Computação Gráfica - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 07/05/2026
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
struct Cube
{
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
};

// ---- VARIÁVEIS GLOBAIS ----
vector<Cube> cubes = {
		{glm::vec3(-0.5f, -0.5f, 0.0f),
		 glm::vec3(0.4f),
		 glm::vec3(0.0f)},
		{glm::vec3(0.5f, -0.5f, 0.0f),
		 glm::vec3(0.4f),
		 glm::vec3(0.0f)},
		{glm::vec3(-0.5f, 0.5f, 0.0f),
		 glm::vec3(0.4f),
		 glm::vec3(0.0f)},
		{glm::vec3(0.5f, 0.5f, 0.0f),
		 glm::vec3(0.4f),
		 glm::vec3(0.0f)}};

int selectedCubeIndex = 0;
int nVertices = 0;

const GLuint WIDTH = 1000, HEIGHT = 1000;
const float MOVEMENT_STEP = 0.1f;
const float SCALE_STEP = 0.1f;
const float ROTATION_STEP = 0.1f;

// ---- DECLARAÇÃO DE FUNÇÕES ----
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

void handleRotationKeys(int key);
void handleMovementKeys(int key);
void handleScaleKeys(int key);

void prepareFrame();
glm::mat4 buildCubeModelMatrix(const Cube &cube);
Cube &getSelectedCube();

void showControlsGuide();

void renderCube(
		const Cube &cube,
		GLuint cubeVAO,
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
																	 "layout (location = 3) in vec3 normal;\n"
																	 "uniform mat4 model;\n"
																	 "out vec2 fragTexCoord;\n"
																	 "out vec3 fragNormal;\n"
																	 "void main()\n"
																	 "{\n"
																	 "   gl_Position = model * vec4(position, 1.0);\n"
																	 "   fragTexCoord = texCoord;\n"
																	 "   fragNormal = normal;\n"
																	 "}\0";

const GLchar *fragmentShaderSource = "#version 450\n"
																		 "in vec2 fragTexCoord;\n"
																		 "in vec3 fragNormal;\n"
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

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Olá Textures 3D -- Gabriela Bado", nullptr, nullptr);
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

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		prepareFrame();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		for (const Cube &cube : cubes)
		{
			renderCube(
					cube,
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

Cube &getSelectedCube()
{
	return cubes[selectedCubeIndex];
}

glm::mat4 buildCubeModelMatrix(const Cube &cube)
{
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	modelMatrix = glm::translate(
			modelMatrix,
			cube.position);

	modelMatrix = glm::scale(
			modelMatrix,
			cube.scale);

	modelMatrix = glm::rotate(
			modelMatrix,
			cube.rotation.x,
			glm::vec3(1.0f, 0.0f, 0.0f));

	modelMatrix = glm::rotate(
			modelMatrix,
			cube.rotation.y,
			glm::vec3(0.0f, 1.0f, 0.0f));

	modelMatrix = glm::rotate(
			modelMatrix,
			cube.rotation.z,
			glm::vec3(0.0f, 0.0f, 1.0f));

	return modelMatrix;
}

void renderCube(const Cube &cube, GLuint cubeVAO, GLint modelMatrixLocation)
{
	glm::mat4 modelMatrix = buildCubeModelMatrix(cube);

	glUniformMatrix4fv(
			modelMatrixLocation,
			1,
			GL_FALSE,
			glm::value_ptr(modelMatrix));

	glBindVertexArray(cubeVAO);
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
		selectedCubeIndex =
				(selectedCubeIndex + 1) % cubes.size();

		cout << "Cube selected: "
				 << selectedCubeIndex + 1
				 << endl;

		return;
	}

	handleRotationKeys(key);
	handleMovementKeys(key);
	handleScaleKeys(key);
}

void handleRotationKeys(int key)
{
	Cube &cube = getSelectedCube();

	if (key == GLFW_KEY_X)
		cube.rotation.x += ROTATION_STEP;

	if (key == GLFW_KEY_Y)
		cube.rotation.y += ROTATION_STEP;

	if (key == GLFW_KEY_Z)
		cube.rotation.z += ROTATION_STEP;
}

void handleMovementKeys(int key)
{
	Cube &cube = getSelectedCube();

	if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT)
		cube.position.x -= MOVEMENT_STEP;

	if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT)
		cube.position.x += MOVEMENT_STEP;

	if (key == GLFW_KEY_I || key == GLFW_KEY_UP)
		cube.position.y += MOVEMENT_STEP;

	if (key == GLFW_KEY_J || key == GLFW_KEY_DOWN)
		cube.position.y -= MOVEMENT_STEP;

	if (key == GLFW_KEY_W)
		cube.position.z -= MOVEMENT_STEP;

	if (key == GLFW_KEY_S)
		cube.position.z += MOVEMENT_STEP;
}

void handleScaleKeys(int key)
{
	Cube &cube = getSelectedCube();

	if (key == GLFW_KEY_LEFT_BRACKET)
	{
		cube.scale.x = glm::max(cube.scale.x - SCALE_STEP, 0.2f);
		cube.scale.y = glm::max(cube.scale.y - SCALE_STEP, 0.2f);
		cube.scale.z = glm::max(cube.scale.z - SCALE_STEP, 0.2f);
	}

	if (key == GLFW_KEY_RIGHT_BRACKET)
	{
		cube.scale.x = glm::min(cube.scale.x + SCALE_STEP, 1.2f);
		cube.scale.y = glm::min(cube.scale.y + SCALE_STEP, 1.2f);
		cube.scale.z = glm::min(cube.scale.z + SCALE_STEP, 1.2f);
	}
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
	cout << " Bem-vindo ao Hello Ilumination 3D! " << endl;
	cout << " Controle seu cubo utilizando as seguintes teclas:" << endl;
	cout << "------------------------------------------" << endl;
	cout << " Selecionar : TAB (troca cubo ativo)" << endl;
	cout << " Movimento X : A | D  ou  <- | ->" << endl;
	cout << " Movimento Y : I | J  ou  /\\ | \\/" << endl;
	cout << " Movimento Z : W | S" << endl;
	cout << " Rotacao     : X | Y | Z" << endl;
	cout << " Escala      : [ | ]" << endl;
	cout << " Sair        : ESC" << endl;
	cout << "==========================================" << endl;
	cout << endl;
}