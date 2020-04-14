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

#define COLOR(R, G, B) (R)/255.0f, (G)/255.0f, (B)/255.0f

glm::mat4 mpv() {
	glm::mat4 Proj = glm::perspective(
			glm::radians(20.0f),
			4.0f / 3.0f,
			0.1f,
			90.0f);
	glm::mat4 View = glm::lookAt(
			glm::vec3(2.0f, 1.0f, 1.0f),
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
	const int WIDTH = 480;
	const int HEIGHT = 480;
	GLFWwindow *window;
	
	/* Initialize the library */
	if (!glfwInit())
		return -1;
	
	glfwWindowHint(GL_LINE_SMOOTH_HINT, 4);
	
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WIDTH, HEIGHT, "Hello World", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	
	glfwSetWindowSizeCallback(window, [](GLFWwindow *wind, int w, int h) -> void {
		glViewport(0, 0, w, h);
	});
	
	glfwSetCursorPosCallback(window, [](GLFWwindow *wind, double xpos, double ypos) -> void {
		std::cout << "x = " << xpos << "  y = " << ypos << std::endl;
	});
	
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	
	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;
	
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	/*
	 *
	 *    (11)--(10)5
	 *   /      /  |
	 * (8)--(9)/   |
	 * 3-----2(7)  4
	 * |     |    /
	 * |     |   /
	 * 0-----1(6)
	 *
	 */
	float positions[] = {
			-0.5f, -0.5f, 0.0f, // 0
			+0.5f, -0.5f, 0.0f, // 1 (6)
			+0.5f, +0.5f, 0.0f, // 2 (7) (9)
			-0.5f, +0.5f, 0.0f, // 3 (8)
			
			+0.5f, -0.5f, -0.5f, // 4
			+0.5f, +0.5f, -0.5f, // 5 (10)
			+0.5f, -0.5f, 0.0f,  // 6 (1)
			+0.5f, +0.5f, 0.0f,  // 7 (2)
			
			-0.5f, +0.5f, 0.0f,  // 8 (3)
			+0.5f, +0.5f, 0.0f,  // 9 (2)
			+0.5f, +0.5f, -0.5f, // 10 (5)
			-0.5f, +0.5f, -0.5f, // 11
			
			-0.5f, -0.5f, 0.0f,  // 12 (0)
			-0.5f, +0.5f, 0.0f,  // 13 (3)
			-0.5f, +0.5f, -0.5f, // 14 (11)
			-0.5f, -0.5f, -0.5f, // 15
	};
	unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0,
			
			6, 4, 5,
			7, 6, 5,
			
			8, 9, 10,
			10, 11, 8,
			
			12, 13, 14,
			14, 15, 12,
	};
	
	float colors[] = {
			COLOR(255.0f, 0.0f, 0.f),
			COLOR(255.0f, 0.0f, 0.f),
			COLOR(255.0f, 0.0f, 0.f),
			COLOR(255.0f, 0.0f, 0.f),
			
			COLOR(0.0f, 255.0f, 0.f),
			COLOR(0.0f, 255.0f, 0.f),
			COLOR(0.0f, 255.0f, 0.f),
			COLOR(0.0f, 255.0f, 0.f),
			
			COLOR(0.0f, 0.0f, 255.f),
			COLOR(0.0f, 0.0f, 255.f),
			COLOR(0.0f, 0.0f, 255.f),
			COLOR(0.0f, 0.0f, 255.f),
			
			COLOR(0.0f, 255.0f, 0.f),
			COLOR(0.0f, 255.0f, 0.f),
			COLOR(0.0f, 255.0f, 0.f),
			COLOR(0.0f, 255.0f, 0.f),
	};
	
	unsigned int buffers[3];
	glGenBuffers(3, buffers);
	
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
	
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	ShaderProgramSource source = ParseShader();
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shader);
	
	glm::mat4 m = mpv();
	
	int loc = glGetUniformLocation(shader, "mvp");
	glUniformMatrix4fv(loc, 1, false, glm::value_ptr(m));
	
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, nullptr);
		
		/* Swap front and back buffers */
		glfwSwapBuffers(window);
		
		/* Poll for and process events */
		glfwPollEvents();
	}
	glDeleteProgram(shader);
	
	glfwTerminate();
	return 0;
}