/* Vivencial02 - código adaptado de HelloIllumination3D.cpp e https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Modificado por Gabriela Spanemberg Bado
 * para a disciplina de Computação Gráfica - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 30/05/2026
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
	glm::vec3 scale;
	glm::vec3 rotation;
};

struct Material
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct Light
{
	glm::vec3 position;
	glm::vec3 color;
	float intensity;
	bool enabled;
};

// ---- VARIÁVEIS GLOBAIS ----
vector<Object3D> objects = {
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

int selectedObjectIndex = 0;
int nVertices = 0;

const GLuint WIDTH = 1000, HEIGHT = 1000;
const float MOVEMENT_STEP = 0.1f;
const float SCALE_STEP = 0.1f;
const float ROTATION_STEP = 0.1f;

GLuint gShaderID = 0;

Light keyLight = {
		glm::vec3(2.0f, 2.0f, 2.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		1.0f,
		true};

Light fillLight = {
		glm::vec3(-2.0f, 1.0f, 2.0f),
		glm::vec3(0.6f, 0.6f, 0.8f),
		0.5f,
		true};

Light backLight = {
		glm::vec3(0.0f, 3.0f, -2.0f),
		glm::vec3(0.8f, 0.8f, 0.8f),
		0.7f,
		true};

// ---- DECLARAÇÃO DE FUNÇÕES ----
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

void handleRotationKeys(int key);
void handleMovementKeys(int key);
void handleScaleKeys(int key);

void prepareFrame();
glm::mat4 buildObject3DModelMatrix(const Object3D &object);
Object3D &getSelectedObject3D();

void showControlsGuide();

void updateThreePointLighting();

void renderObject3D(
		const Object3D &object,
		GLuint objectVAO,
		GLint modelMatrixLocation);

int setupShader();
string loadTexturePathFromMTL(const string &mtlPath);
GLuint loadTexture(const string &texturePath);
GLuint loadSimpleOBJ(string filePath, int &nVertices);

Material loadMaterialFromMTL(const string &mtlPath);

// ---- SHADERS ----
const GLchar *vertexShaderSource = "#version 450\n"
																	 "layout (location = 0) in vec3 position;\n"
																	 "layout (location = 1) in vec3 color;\n"
																	 "layout (location = 2) in vec2 texCoord;\n"
																	 "layout (location = 3) in vec3 normal;\n"
																	 "uniform mat4 model;\n"
																	 "uniform mat4 view;\n"
																	 "uniform mat4 projection;\n"
																	 "out vec2 fragTexCoord;\n"
																	 "out vec3 fragNormal;\n"
																	 "out vec3 fragPosition;\n"
																	 "void main()\n"
																	 "{\n"
																	 "   vec4 worldPos = model * vec4(position, 1.0);\n"
																	 "   gl_Position = projection * view * worldPos;\n"
																	 "   fragTexCoord = texCoord;\n"
																	 "   fragNormal = normalize(mat3(transpose(inverse(model))) * normal);\n"
																	 "   fragPosition = vec3(worldPos);\n"
																	 "}\0";

const GLchar *fragmentShaderSource = "#version 450\n"
																		 "in vec2 fragTexCoord;\n"
																		 "in vec3 fragNormal;\n"
																		 "in vec3 fragPosition;\n"
																		 "out vec4 color;\n"

																		 "uniform sampler2D texture1;\n"

																		 "uniform vec3 materialAmbient;\n"
																		 "uniform vec3 materialDiffuse;\n"
																		 "uniform vec3 materialSpecular;\n"

																		 "uniform vec3 keyLightPosition;\n"
																		 "uniform vec3 fillLightPosition;\n"
																		 "uniform vec3 backLightPosition;\n"

																		 "uniform vec3 keyLightColor;\n"
																		 "uniform vec3 fillLightColor;\n"
																		 "uniform vec3 backLightColor;\n"

																		 "uniform float keyLightIntensity;\n"
																		 "uniform float fillLightIntensity;\n"
																		 "uniform float backLightIntensity;\n"

																		 "uniform vec3 viewPosition;\n"

																		 "float attenuation(float distance)\n"
																		 "{\n"
																		 "    float constant = 1.0;\n"
																		 "    float linear = 0.09;\n"
																		 "    float quadratic = 0.032;\n"

																		 "    return 1.0 / (constant + linear * distance + quadratic * distance * distance);\n"
																		 "}\n"

																		 "vec3 calculateLight(\n"
																		 "    vec3 lightPosition,\n"
																		 "    vec3 lightColor,\n"
																		 "    float lightIntensity,\n"
																		 "    vec3 normal,\n"
																		 "    vec3 fragPosition,\n"
																		 "    vec3 viewDir)\n"
																		 "{\n"

																		 "    vec3 lightDir = normalize(lightPosition - fragPosition);\n"

																		 "    float distance = length(lightPosition - fragPosition);\n"
																		 "    float att = attenuation(distance);\n"

																		 "    vec3 reflectDir = reflect(-lightDir, normal);\n"

																		 "    float diff = max(dot(normal, lightDir), 0.0);\n"

																		 "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);\n"

																		 "    vec3 ambient = materialAmbient * lightColor * lightIntensity;\n"

																		 "    vec3 diffuse = materialDiffuse * diff * lightColor * lightIntensity;\n"

																		 "    vec3 specular = materialSpecular * spec * lightColor * lightIntensity;\n"

																		 "    return (ambient + diffuse + specular) * att;\n"
																		 "}\n"

																		 "void main()\n"
																		 "{\n"

																		 "    vec3 normal = normalize(fragNormal);\n"

																		 "    vec3 viewDir = normalize(viewPosition - fragPosition);\n"

																		 "    vec3 phong = vec3(0.0);\n"

																		 "    phong += calculateLight(\n"
																		 "        keyLightPosition,\n"
																		 "        keyLightColor,\n"
																		 "        keyLightIntensity,\n"
																		 "        normal,\n"
																		 "        fragPosition,\n"
																		 "        viewDir);\n"

																		 "    phong += calculateLight(\n"
																		 "        fillLightPosition,\n"
																		 "        fillLightColor,\n"
																		 "        fillLightIntensity,\n"
																		 "        normal,\n"
																		 "        fragPosition,\n"
																		 "        viewDir);\n"

																		 "    phong += calculateLight(\n"
																		 "        backLightPosition,\n"
																		 "        backLightColor,\n"
																		 "        backLightIntensity,\n"
																		 "        normal,\n"
																		 "        fragPosition,\n"
																		 "        viewDir);\n"

																		 "    vec4 texColor = texture(texture1, fragTexCoord);\n"

																		 "    color = vec4(phong, 1.0) * texColor;\n"
																		 "}\0";

// ---- FUNÇÃO MAIN ----
int main()
{
	glfwInit();

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Olá Atividade Vivencial 02 -- Gabriela Bado", nullptr, nullptr);
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
	gShaderID = shaderID;
	GLuint VAO = loadSimpleOBJ("../assets/Modelos3D/Suzanne/Suzanne.obj", nVertices);
	string textureName = loadTexturePathFromMTL("../assets/Modelos3D/Suzanne/Suzanne.mtl");
	GLuint textureID = loadTexture("../assets/Modelos3D/Suzanne/" + textureName);
	Material material = loadMaterialFromMTL("../assets/Modelos3D/Suzanne/Suzanne.mtl");

	glUseProgram(shaderID);

	glm::vec3 cameraPosition(0.0f, 2.0f, 3.0f);

	glUniform3fv(
			glGetUniformLocation(shaderID, "materialAmbient"),
			1,
			glm::value_ptr(material.ambient));

	glUniform3fv(
			glGetUniformLocation(shaderID, "materialDiffuse"),
			1,
			glm::value_ptr(material.diffuse));

	glUniform3fv(
			glGetUniformLocation(shaderID, "materialSpecular"),
			1,
			glm::value_ptr(material.specular));

	glUniform3fv(
			glGetUniformLocation(shaderID, "viewPosition"),
			1,
			glm::value_ptr(cameraPosition));

	updateThreePointLighting();

	glUniform3fv(
			glGetUniformLocation(shaderID, "keyLightPosition"),
			1,
			glm::value_ptr(keyLight.position));

	glUniform3fv(
			glGetUniformLocation(shaderID, "fillLightPosition"),
			1,
			glm::value_ptr(fillLight.position));

	glUniform3fv(
			glGetUniformLocation(shaderID, "backLightPosition"),
			1,
			glm::value_ptr(backLight.position));

	glUniform3fv(
			glGetUniformLocation(shaderID, "keyLightColor"),
			1,
			glm::value_ptr(keyLight.color));

	glUniform3fv(
			glGetUniformLocation(shaderID, "fillLightColor"),
			1,
			glm::value_ptr(fillLight.color));

	glUniform3fv(
			glGetUniformLocation(shaderID, "backLightColor"),
			1,
			glm::value_ptr(backLight.color));

	glUniform1f(
			glGetUniformLocation(shaderID, "keyLightIntensity"),
			keyLight.intensity);

	glUniform1f(
			glGetUniformLocation(shaderID, "fillLightIntensity"),
			fillLight.intensity);

	glUniform1f(
			glGetUniformLocation(shaderID, "backLightIntensity"),
			backLight.intensity);

	glUniform1i(glGetUniformLocation(shaderID, "texture1"), 0);
	GLint modelMatrixLocation = glGetUniformLocation(shaderID, "model");

	glm::mat4 view = glm::lookAt(
			glm::vec3(0.0f, 2.0f, 3.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
	GLint viewLoc = glGetUniformLocation(shaderID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::perspective(
			glm::radians(45.0f),
			(float)WIDTH / (float)HEIGHT,
			0.1f,
			100.0f);
	GLint projectionLoc = glGetUniformLocation(shaderID, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		prepareFrame();

		updateThreePointLighting();

		glUniform3fv(
				glGetUniformLocation(shaderID, "keyLightPosition"),
				1,
				glm::value_ptr(keyLight.position));

		glUniform3fv(
				glGetUniformLocation(shaderID, "fillLightPosition"),
				1,
				glm::value_ptr(fillLight.position));

		glUniform3fv(
				glGetUniformLocation(shaderID, "backLightPosition"),
				1,
				glm::value_ptr(backLight.position));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		for (const Object3D &object : objects)
		{
			renderObject3D(
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

Material loadMaterialFromMTL(const string &mtlPath)
{
	Material material = {
			glm::vec3(0.0f),
			glm::vec3(0.0f),
			glm::vec3(0.0f)};

	ifstream file(mtlPath);

	if (!file.is_open())
	{
		cout << "Erro ao abrir MTL" << endl;
		return material;
	}

	string line;

	while (getline(file, line))
	{
		istringstream ss(line);

		string word;
		ss >> word;

		if (word == "Ka")
		{
			ss >> material.ambient.r >> material.ambient.g >> material.ambient.b;
		}
		else if (word == "Kd")
		{
			ss >> material.diffuse.r >> material.diffuse.g >> material.diffuse.b;
		}
		else if (word == "Ks")
		{
			ss >> material.specular.r >> material.specular.g >> material.specular.b;
		}
	}

	return material;
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
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Object3D &getSelectedObject3D()
{
	return objects[selectedObjectIndex];
}

glm::mat4 buildObject3DModelMatrix(const Object3D &object)
{
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	modelMatrix = glm::translate(
			modelMatrix,
			object.position);

	modelMatrix = glm::scale(
			modelMatrix,
			object.scale);

	modelMatrix = glm::rotate(
			modelMatrix,
			object.rotation.x,
			glm::vec3(1.0f, 0.0f, 0.0f));

	modelMatrix = glm::rotate(
			modelMatrix,
			object.rotation.y,
			glm::vec3(0.0f, 1.0f, 0.0f));

	modelMatrix = glm::rotate(
			modelMatrix,
			object.rotation.z,
			glm::vec3(0.0f, 0.0f, 1.0f));

	return modelMatrix;
}

void updateThreePointLighting()
{
	Object3D &mainObject = getSelectedObject3D();

	glm::vec3 center = mainObject.position;

	float distance =
			glm::max(
					glm::max(
							mainObject.scale.x,
							mainObject.scale.y),
					mainObject.scale.z) *
			5.0f;

	keyLight.position =
			center +
			glm::vec3(
					distance,
					distance,
					distance);

	fillLight.position =
			center +
			glm::vec3(
					-distance,
					distance * 0.5f,
					distance);

	backLight.position =
			center +
			glm::vec3(
					0.0f,
					distance,
					-distance);
}

void renderObject3D(const Object3D &object, GLuint objectVAO, GLint modelMatrixLocation)
{
	glm::mat4 modelMatrix = buildObject3DModelMatrix(object);

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

		cout << "Object3D selected: "
				 << selectedObjectIndex + 1
				 << endl;

		return;
	}

	if (key == GLFW_KEY_1)
	{
		keyLight.enabled = !keyLight.enabled;
		cout << "Key Light: " << (keyLight.enabled ? "ON" : "OFF") << endl;
	}

	if (key == GLFW_KEY_2)
	{
		fillLight.enabled = !fillLight.enabled;
		cout << "Fill Light: " << (fillLight.enabled ? "ON" : "OFF") << endl;
	}

	if (key == GLFW_KEY_3)
	{
		backLight.enabled = !backLight.enabled;
		cout << "Back Light: " << (backLight.enabled ? "ON" : "OFF") << endl;
	}

	handleRotationKeys(key);
	handleMovementKeys(key);
	handleScaleKeys(key);

	updateThreePointLighting();

	glUseProgram(gShaderID);

	glUniform1f(
			glGetUniformLocation(gShaderID, "keyLightIntensity"),
			keyLight.enabled ? keyLight.intensity : 0.0f);

	glUniform1f(
			glGetUniformLocation(gShaderID, "fillLightIntensity"),
			fillLight.enabled ? fillLight.intensity : 0.0f);

	glUniform1f(
			glGetUniformLocation(gShaderID, "backLightIntensity"),
			backLight.enabled ? backLight.intensity : 0.0f);
}

void handleRotationKeys(int key)
{
	Object3D &object = getSelectedObject3D();

	if (key == GLFW_KEY_X)
		object.rotation.x += ROTATION_STEP;

	if (key == GLFW_KEY_Y)
		object.rotation.y += ROTATION_STEP;

	if (key == GLFW_KEY_Z)
		object.rotation.z += ROTATION_STEP;
}

void handleMovementKeys(int key)
{
	Object3D &object = getSelectedObject3D();

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
	Object3D &object = getSelectedObject3D();

	if (key == GLFW_KEY_LEFT_BRACKET)
	{
		object.scale.x = glm::max(object.scale.x - SCALE_STEP, 0.2f);
		object.scale.y = glm::max(object.scale.y - SCALE_STEP, 0.2f);
		object.scale.z = glm::max(object.scale.z - SCALE_STEP, 0.2f);
	}

	if (key == GLFW_KEY_RIGHT_BRACKET)
	{
		object.scale.x = glm::min(object.scale.x + SCALE_STEP, 1.2f);
		object.scale.y = glm::min(object.scale.y + SCALE_STEP, 1.2f);
		object.scale.z = glm::min(object.scale.z + SCALE_STEP, 1.2f);
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
	cout << " Bem-vindo a Atividade Vivencial 02 " << endl;
	cout << " Controle seu cubo utilizando as seguintes teclas:" << endl;
	cout << "------------------------------------------" << endl;
	cout << " Selecionar : TAB (troca cubo ativo)" << endl;
	cout << " Movimento X : A | D  ou  <- | ->" << endl;
	cout << " Movimento Y : I | J  ou  /\\ | \\/" << endl;
	cout << " Movimento Z : W | S" << endl;
	cout << " Rotacao     : X | Y | Z" << endl;
	cout << " Escala      : [ | ]" << endl;
	cout << " Luz Key     : 1" << endl;
	cout << " Luz Fill    : 2" << endl;
	cout << " Luz Back    : 3" << endl;
	cout << " Sair        : ESC" << endl;
	cout << "==========================================" << endl;
	cout << endl;
}