/* Hello Squares 3D - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Modificado por Gabriela Spanemberg Bado
 * para a disciplina de Computação Gráfica - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 05/05/2026
 */

// ---- HEADERS GLFW, GLAD, GLM ----
#include <iostream>
#include <string>
#include <assert.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

// ---- STRUCTS ----
struct Cube {
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
Cube cube = {
    glm::vec3(0.0f, 0.0f, 0.0f),
    1.0f,

    false, false, false,

    0.0f, 0.0f, 0.0f
};
const GLuint WIDTH = 1000, HEIGHT = 1000;

// ---- DECLARAÇÃO DE FUNÇÕES ----
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
int setupShader();
int setupGeometry();

// ---- SHADERS ----
// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar* vertexShaderSource = "#version 450\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"out vec4 finalColor;\n"
"void main()\n"
"{\n"
//...pode ter mais linhas de código aqui!
"gl_Position = model * vec4(position, 1.0);\n"
"finalColor = vec4(color, 1.0);\n"
"}\0";

// Código fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar* fragmentShaderSource = "#version 450\n"
"in vec4 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = finalColor;\n"
"}\n\0";

// ---- FUNÇÃO MAIN ----
int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Olá Squares 3D -- Gabriela Bado", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	GLuint shaderID = setupShader();
	GLuint VAO = setupGeometry();

	glUseProgram(shaderID);

	glm::mat4 model = glm::mat4(1);
	GLint modelLoc = glGetUniformLocation(shaderID, "model");
	model = glm::rotate(model, /*(GLfloat)glfwGetTime()*/glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		float angle = (GLfloat)glfwGetTime();

		model = glm::mat4(1); 
		if (rotateX)
		{
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
			
		}
		else if (rotateY)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

		}
		else if (rotateZ)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));

		}

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glDrawArrays(GL_POINTS, 0, 36);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);

	glfwTerminate();
	return 0;
}

// ---- IMPLEMENTAÇÃO DAS FUNÇÕES ----

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		cube.rotateX = true;
		cube.rotateY = false;
		cube.rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		cube.rotateX = false;
		cube.rotateY = true;
		cube.rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		cube.rotateX = false;
		cube.rotateY = false;
		cube.rotateZ = true;
	}

	if (key == GLFW_KEY_A)
    cube.position.x -= 0.1f;
	if (key == GLFW_KEY_D)
			cube.position.x += 0.1f;

	if (key == GLFW_KEY_W)
			cube.position.z -= 0.1f;
	if (key == GLFW_KEY_S)
			cube.position.z += 0.1f;

	if (key == GLFW_KEY_I)
			cube.position.y += 0.1f;
	if (key == GLFW_KEY_J)
			cube.position.y -= 0.1f;

	if (key == GLFW_KEY_LEFT_BRACKET)
    cube.scale -= 0.1f;

	if (key == GLFW_KEY_RIGHT_BRACKET)
		cube.scale += 0.1f;
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
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

int setupGeometry()
{
	GLfloat vertices[] = {
		//x    y    z   r g b
		-0.5,-0.5, 0.5, 1,0,0,
    0.5,-0.5, 0.5, 1,0,0,
    0.5, 0.5, 0.5, 1,0,0,

    -0.5,-0.5, 0.5, 1,0,0,
    0.5, 0.5, 0.5, 1,0,0,
    -0.5, 0.5, 0.5, 1,0,0,

    -0.5,-0.5,-0.5, 0,1,0,
    0.5, 0.5,-0.5, 0,1,0,
    0.5,-0.5,-0.5, 0,1,0,

    -0.5,-0.5,-0.5, 0,1,0,
    -0.5, 0.5,-0.5, 0,1,0,
    0.5, 0.5,-0.5, 0,1,0,

    -0.5,-0.5,-0.5, 0,0,1,
    -0.5,-0.5, 0.5, 0,0,1,
    -0.5, 0.5, 0.5, 0,0,1,

    -0.5,-0.5,-0.5, 0,0,1,
    -0.5, 0.5, 0.5, 0,0,1,
    -0.5, 0.5,-0.5, 0,0,1,

    0.5,-0.5,-0.5, 1,1,0,
    0.5, 0.5, 0.5, 1,1,0,
    0.5,-0.5, 0.5, 1,1,0,

    0.5,-0.5,-0.5, 1,1,0,
    0.5, 0.5,-0.5, 1,1,0,
    0.5, 0.5, 0.5, 1,1,0,

    -0.5, 0.5,-0.5, 1,0,1,
    -0.5, 0.5, 0.5, 1,0,1,
    0.5, 0.5, 0.5, 1,0,1,

    -0.5, 0.5,-0.5, 1,0,1,
    0.5, 0.5, 0.5, 1,0,1,
    0.5, 0.5,-0.5, 1,0,1,

    -0.5,-0.5,-0.5, 0,1,1,
    0.5,-0.5, 0.5, 0,1,1,
    -0.5,-0.5, 0.5, 0,1,1,

    -0.5,-0.5,-0.5, 0,1,1,
    0.5,-0.5,-0.5, 0,1,1,
    0.5,-0.5, 0.5, 0,1,1,
};

	GLuint VBO, VAO;

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	
	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

