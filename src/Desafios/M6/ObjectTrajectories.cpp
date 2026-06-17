/* Object Trajectories - código adaptado de FirstPersonCamera.cpp e https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Modificado por Gabriela Spanemberg Bado
 * para a disciplina de Computação Gráfica - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 16/06/2026
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

// ---- STRUCTS E ENUMS ----
struct Object3D
{
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;

	std::vector<glm::vec3> trajectoryPoints;

	bool trajectoryEnabled = false;
	int currentPoint = 0;
	float trajectorySpeed = 1.0f;
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

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

struct Camera
{
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	float yaw;
	float pitch;
	float lastX;
	float lastY;

	bool firstMouse;

	float movementSpeed;
	float mouseSensitivity;

	Camera(
			glm::vec3 position = glm::vec3(0.0f, 0.0f, 4.0f),
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
			float yaw = -90.0f,
			float pitch = 0.0f)
			: position(position),
				front(glm::vec3(0.0f, 0.0f, -1.0f)),
				up(glm::vec3(0.0f, 1.0f, 0.0f)),
				right(glm::vec3(0.0f, 0.0f, 0.0f)),
				worldUp(up),
				yaw(yaw),
				pitch(pitch),
				lastX(0.0f),
				lastY(0.0f),
				firstMouse(true),
				movementSpeed(2.5f),
				mouseSensitivity(0.1f)
	{
		updateCameraVectors();
	}

	glm::mat4 getViewMatrix() const
	{
		return glm::lookAt(position, position + front, up);
	}

	void processKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = movementSpeed * deltaTime;
		if (direction == FORWARD)
			position += front * velocity;
		if (direction == BACKWARD)
			position -= front * velocity;
		if (direction == LEFT)
			position -= right * velocity;
		if (direction == RIGHT)
			position += right * velocity;
	}

	void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
	{
		xoffset *= mouseSensitivity;
		yoffset *= mouseSensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (constrainPitch)
		{
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}

		updateCameraVectors();
	}

	void updateCameraVectors()
	{
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(direction);
		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}
};

// ---- VARIÁVEIS GLOBAIS ----
vector<Object3D> objects = {
		{glm::vec3(-1.0f, 0.0f, 0.0f),
		 glm::vec3(0.5f),
		 glm::vec3(0.0f)},
		{glm::vec3(1.0f, 0.0f, 0.0f),
		 glm::vec3(0.5f),
		 glm::vec3(0.0f)}};

int selectedObjectIndex = 0;
int nVertices = 0;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

const GLuint WIDTH = 1000, HEIGHT = 1000;
const float MOVEMENT_STEP = 0.1f;
const float SCALE_STEP = 0.1f;
const float ROTATION_STEP = 0.1f;
const float LIGHT_DISTANCE_FACTOR = 5.0f;
const float CAMERA_FOV = 45.0f;
const float MIN_SCALE = 0.2f;
const float MAX_SCALE = 1.2f;
const float TRAJECTORY_POINT_THRESHOLD = 0.05f;

GLuint gShaderID = 0;

Light keyLight = {
		glm::vec3(2.0f, 2.0f, 2.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		0.8f,
		true};

Light fillLight = {
		glm::vec3(-2.0f, 1.0f, 2.0f),
		glm::vec3(0.6f, 0.6f, 0.8f),
		0.15f,
		true};

Light backLight = {
		glm::vec3(0.0f, 3.0f, -2.0f),
		glm::vec3(0.7f, 0.8f, 1.0f),
		0.5f,
		true};

Camera camera(glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

// ---- DECLARAÇÃO DE FUNÇÕES ----
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

void handleRotationKeys(int key);
void handleMovementKeys(int key);
void handleScaleKeys(int key);
void handleObjectSelection(int key);
void handleLightToggle(int key);
void handleTrajectoryToggle(int key);
void processCameraInput(GLFWwindow *window, float deltaTime);
void loadTrajectory(Object3D &object, const string &filename);

void prepareFrame();
glm::mat4 buildObject3DModelMatrix(const Object3D &object);
Object3D &getSelectedObject3D();

void updateTrajectory(Object3D &object, float deltaTime);

glm::vec3 parseVec3(std::istringstream &ss);
void setUniformVec3(GLuint shaderID, const char *name, const glm::vec3 &value);
void setUniformFloat(GLuint shaderID, const char *name, float value);
void uploadMaterialToShader(GLuint shaderID, const Material &material);
void uploadLightPositions(GLuint shaderID);
void uploadLightColors(GLuint shaderID);
void uploadLightIntensities(GLuint shaderID);
GLuint compileShader(GLenum shaderType, const GLchar *source);
void checkShaderCompileErrors(GLuint shader, const std::string &type);

void showControlsGuide();

void updateThreePointLightPositions();

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
																		 "    float linear = 0.14;\n"
																		 "    float quadratic = 0.07;\n"

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

																		 "    return ambient + (diffuse + specular) * att;\n"
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

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Trajetórias de Objetos -- Gabriela Bado", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

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
	loadTrajectory(objects[0], "../assets/objectTrajectory1.txt");
	loadTrajectory(objects[1], "../assets/objectTrajectory2.txt");
	objects[0].trajectorySpeed = 1.0f;
	objects[1].trajectorySpeed = 1.5f;

	glUseProgram(shaderID);

	uploadMaterialToShader(shaderID, material);
	setUniformVec3(shaderID, "viewPosition", camera.position);

	updateThreePointLightPositions();
	uploadLightPositions(shaderID);
	uploadLightColors(shaderID);
	uploadLightIntensities(shaderID);

	glUniform1i(glGetUniformLocation(shaderID, "texture1"), 0);
	GLint modelMatrixLocation = glGetUniformLocation(shaderID, "model");

	glm::mat4 view = camera.getViewMatrix();
	GLint viewLoc = glGetUniformLocation(shaderID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::perspective(
			glm::radians(CAMERA_FOV),
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
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		processCameraInput(window, deltaTime);

		prepareFrame();

		for (Object3D &object : objects)
		{
			updateTrajectory(
					object,
					deltaTime);
		}

		view = camera.getViewMatrix();

		glUniformMatrix4fv(
				viewLoc,
				1,
				GL_FALSE,
				glm::value_ptr(view));

		setUniformVec3(
				shaderID,
				"viewPosition",
				camera.position);

		updateThreePointLightPositions();
		uploadLightPositions(shaderID);

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

	glDeleteProgram(shaderID);
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

glm::vec3 parseVec3(std::istringstream &ss)
{
	glm::vec3 value(0.0f);
	ss >> value.r >> value.g >> value.b;
	return value;
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
			material.ambient = parseVec3(ss);
		}
		else if (word == "Kd")
		{
			material.diffuse = parseVec3(ss);
		}
		else if (word == "Ks")
		{
			material.specular = parseVec3(ss);
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

	modelMatrix = glm::translate(modelMatrix, object.position);
	modelMatrix = glm::scale(modelMatrix, object.scale);
	modelMatrix = glm::rotate(modelMatrix, object.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, object.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, object.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

	return modelMatrix;
}

void setUniformVec3(GLuint shaderID, const char *name, const glm::vec3 &value)
{
	GLint location = glGetUniformLocation(shaderID, name);
	glUniform3fv(location, 1, glm::value_ptr(value));
}

void setUniformFloat(GLuint shaderID, const char *name, float value)
{
	GLint location = glGetUniformLocation(shaderID, name);
	glUniform1f(location, value);
}

void uploadMaterialToShader(GLuint shaderID, const Material &material)
{
	setUniformVec3(shaderID, "materialAmbient", material.ambient);
	setUniformVec3(shaderID, "materialDiffuse", material.diffuse);
	setUniformVec3(shaderID, "materialSpecular", material.specular);
}

void uploadLightPositions(GLuint shaderID)
{
	setUniformVec3(shaderID, "keyLightPosition", keyLight.position);
	setUniformVec3(shaderID, "fillLightPosition", fillLight.position);
	setUniformVec3(shaderID, "backLightPosition", backLight.position);
}

void uploadLightColors(GLuint shaderID)
{
	setUniformVec3(shaderID, "keyLightColor", keyLight.color);
	setUniformVec3(shaderID, "fillLightColor", fillLight.color);
	setUniformVec3(shaderID, "backLightColor", backLight.color);
}

void uploadLightIntensities(GLuint shaderID)
{
	setUniformFloat(shaderID, "keyLightIntensity", keyLight.enabled ? keyLight.intensity : 0.0f);
	setUniformFloat(shaderID, "fillLightIntensity", fillLight.enabled ? fillLight.intensity : 0.0f);
	setUniformFloat(shaderID, "backLightIntensity", backLight.enabled ? backLight.intensity : 0.0f);
}

GLuint compileShader(GLenum shaderType, const GLchar *source)
{
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	checkShaderCompileErrors(shader, shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
	return shader;
}

void checkShaderCompileErrors(GLuint shader, const std::string &type)
{
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n"
							<< infoLog << std::endl;
	}
}

void updateThreePointLightPositions()
{
	Object3D &mainObject = getSelectedObject3D();

	glm::vec3 center = mainObject.position;

	float distance =
			glm::max(
					glm::max(
							mainObject.scale.x,
							mainObject.scale.y),
					mainObject.scale.z) *
			LIGHT_DISTANCE_FACTOR;

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

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
		return;

	if (camera.firstMouse)
	{
		camera.lastX = xpos;
		camera.lastY = ypos;
		camera.firstMouse = false;
	}

	float xoffset = xpos - camera.lastX;
	float yoffset = camera.lastY - ypos;

	camera.lastX = xpos;
	camera.lastY = ypos;

	camera.processMouseMovement(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			camera.firstMouse = true;
			cout << "Mouse capturado. Clique direito para liberar o cursor." << endl;
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			cout << "Mouse liberado. Clique esquerdo dentro da janela para capturar novamente." << endl;
		}
	}
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (action != GLFW_PRESS && action != GLFW_REPEAT)
		return;

	if (key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GL_TRUE);

	handleObjectSelection(key);
	handleLightToggle(key);
	handleTrajectoryToggle(key);

	handleRotationKeys(key);
	handleMovementKeys(key);
	handleScaleKeys(key);

	updateThreePointLightPositions();

	glUseProgram(gShaderID);

	uploadLightPositions(gShaderID);
	uploadLightIntensities(gShaderID);
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

	if (key == GLFW_KEY_LEFT)
		object.position.x -= MOVEMENT_STEP;

	if (key == GLFW_KEY_RIGHT)
		object.position.x += MOVEMENT_STEP;

	if (key == GLFW_KEY_UP)
		object.position.y += MOVEMENT_STEP;

	if (key == GLFW_KEY_DOWN)
		object.position.y -= MOVEMENT_STEP;
}

void handleScaleKeys(int key)
{
	Object3D &object = getSelectedObject3D();

	if (key == GLFW_KEY_LEFT_BRACKET)
	{
		object.scale.x = glm::max(object.scale.x - SCALE_STEP, MIN_SCALE);
		object.scale.y = glm::max(object.scale.y - SCALE_STEP, MIN_SCALE);
		object.scale.z = glm::max(object.scale.z - SCALE_STEP, MIN_SCALE);
	}

	if (key == GLFW_KEY_RIGHT_BRACKET)
	{
		object.scale.x = glm::min(object.scale.x + SCALE_STEP, MAX_SCALE);
		object.scale.y = glm::min(object.scale.y + SCALE_STEP, MAX_SCALE);
		object.scale.z = glm::min(object.scale.z + SCALE_STEP, MAX_SCALE);
	}
}

void handleObjectSelection(int key)
{
	if (key != GLFW_KEY_TAB)
		return;

	selectedObjectIndex = (selectedObjectIndex + 1) % objects.size();

	cout << "Object3D selected: "
			 << selectedObjectIndex + 1
			 << endl;
}

void handleLightToggle(int key)
{
	if (key == GLFW_KEY_1)
	{
		keyLight.enabled = !keyLight.enabled;

		cout << "Key Light: "
				 << (keyLight.enabled ? "ON" : "OFF")
				 << endl;
	}

	if (key == GLFW_KEY_2)
	{
		fillLight.enabled = !fillLight.enabled;

		cout << "Fill Light: "
				 << (fillLight.enabled ? "ON" : "OFF")
				 << endl;
	}

	if (key == GLFW_KEY_3)
	{
		backLight.enabled = !backLight.enabled;

		cout << "Back Light: "
				 << (backLight.enabled ? "ON" : "OFF")
				 << endl;
	}
}

void handleTrajectoryToggle(int key)
{
	if (key != GLFW_KEY_T)
		return;

	Object3D &object = getSelectedObject3D();

	object.trajectoryEnabled = !object.trajectoryEnabled;

	cout << "Trajetoria "
			 << (object.trajectoryEnabled ? "ON" : "OFF")
			 << endl;
}

void processCameraInput(GLFWwindow *window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, deltaTime);
}

void updateTrajectory(Object3D &object, float deltaTime)
{
	if (!object.trajectoryEnabled)
		return;

	if (object.trajectoryPoints.size() < 2)
		return;

	glm::vec3 target = object.trajectoryPoints[object.currentPoint];

	glm::vec3 direction = target - object.position;

	float distance = glm::length(direction);

	if (distance < TRAJECTORY_POINT_THRESHOLD)
	{
		object.currentPoint =
				(object.currentPoint + 1) % object.trajectoryPoints.size();
		return;
	}

	object.position +=
			glm::normalize(direction) * object.trajectorySpeed * deltaTime;
}

void loadTrajectory(Object3D &object, const string &filename)
{
	ifstream file(filename);

	if (!file.is_open())
	{
		cout << "Erro ao abrir arquivo: "
				 << filename
				 << endl;
		return;
	}

	float x, y, z;

	while (file >> x >> y >> z)
	{
		object.trajectoryPoints.push_back(glm::vec3(x, y, z));
	}

	cout << object.trajectoryPoints.size()
			 << " pontos carregados de "
			 << filename
			 << endl;
}

int setupShader()
{
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	GLint success;
	GLchar infoLog[512];

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
	cout << " Bem-vindo a Object Trajectories! " << endl;
	cout << " Controle seu objeto ou câmera utilizando as seguintes teclas:" << endl;
	cout << "------------------------------------------" << endl;
	cout << " Selecionar         : TAB (troca objeto ativo)" << endl;
	cout << " Capturar Mouse     : Clique Esquerdo" << endl;
	cout << " Liberar Mouse      : Clique Direito" << endl;
	cout << " Camera Movimento   : W | A | S | D" << endl;
	cout << " Camera Olhar       : Movimento do Mouse" << endl;
	cout << " Movimento Objeto X : <- | ->" << endl;
	cout << " Movimento Objeto Y :  /\\ | \\/" << endl;
	cout << " Rotacao            : X | Y | Z" << endl;
	cout << " Escala             : [ | ]" << endl;
	cout << " Iniciar trajeto    : T" << endl;
	cout << " Key Light          : 1" << endl;
	cout << " Fill Light         : 2" << endl;
	cout << " Back Light         : 3" << endl;
	cout << " Sair               : ESC" << endl;
	cout << "==========================================" << endl;
	cout << endl;
}