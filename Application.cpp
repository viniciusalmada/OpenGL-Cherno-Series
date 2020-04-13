#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Include all GLM core / GLSL features
#include <glm/glm.hpp> // vec2, vec3, mat4, radians
// Include all GLM extensions
#include <glm/ext.hpp> // perspective, translate, rotate

#include <iostream>
#include <fstream>
#include <sstream>

#define VERTEX_SHADER_PATH "res/shaders/Vertex.glsl"
#define FRAGMENT_SHADER_PATH "res/shaders/Fragment.glsl"


glm::mat4 mpv() {
	glm::mat4 Proj = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
	);
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	return Proj * View * Model;
}

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader() {
	std::ifstream stream = std::ifstream(VERTEX_SHADER_PATH);
	
	std::string line;
	std::stringstream ss[2];
	while (getline(stream, line)) {
		ss[0] << line << '\n';
	}
	
	stream = std::ifstream(FRAGMENT_SHADER_PATH);
	while (getline(stream, line)) {
		ss[1] << line << '\n';
	}
	
	return ShaderProgramSource{ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(unsigned int type, const std::string &source) {
	unsigned int id = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	
	int result;
	// iv is the type
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (!result) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char message[length];
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile "
		          << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
		          << " shader!"
		          << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}
	
	return id;
}

static unsigned int CreateShader(const std::string &vertexShader, const std::string &fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	
	glDeleteShader(vs);
	glDeleteShader(fs);
	
	return program;
}

int main() {
	GLFWwindow *window;
	
	/* Initialize the library */
	if (!glfwInit())
		return -1;
	
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(480, 480, "Hello World", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	
	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;
	
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	
	float positions[] = {
			-0.5f, -0.5f, 0.0f,
			+0.5f, -0.5f, 0.0f,
			+0.5f, +0.5f, 0.0f,
			-0.5f, +0.5f, 0.0f,
	};
	
	unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
	};
	
	unsigned int buffers[2];
	glGenBuffers(1, &buffers[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
	
	glGenBuffers(1, &buffers[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	ShaderProgramSource source = ParseShader();
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shader);
	
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		
		/* Swap front and back buffers */
		glfwSwapBuffers(window);
		
		/* Poll for and process events */
		glfwPollEvents();
	}
	glDeleteProgram(shader);
	
	glfwTerminate();
	return 0;
}