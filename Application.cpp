#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

#define VERTEX_SHADER_PATH "res/shaders/Vertex.glsl"
#define FRAGMENT_SHADER_PATH "res/shaders/Fragment.glsl"

#define ASSERT(x) if (!(x)) __builtin_trap(); // Use __debugbreak() in windows
#define GLCall(x) GLClearError(); \
    x; \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char *function, const char *file, int line) {
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] (" << error << "): " << function <<
		          " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
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
	GLCall(unsigned int id = glCreateShader(type))
	const char *src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr))
	GLCall(glCompileShader(id))
	
	int result;
	// iv is the type
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result))
	if (!result) {
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length))
		char message[length];
		GLCall(glGetShaderInfoLog(id, length, &length, message))
		std::cout << "Failed to compile "
		          << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
		          << " shader!"
		          << std::endl;
		std::cout << message << std::endl;
		GLCall(glDeleteShader(id))
		return 0;
	}
	
	return id;
}

static unsigned int CreateShader(const std::string &vertexShader, const std::string &fragmentShader) {
	GLCall(unsigned int program = glCreateProgram())
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	
	GLCall(glAttachShader(program, vs))
	GLCall(glAttachShader(program, fs))
	GLCall(glLinkProgram(program))
	GLCall(glValidateProgram(program))
	
	GLCall(glDeleteShader(vs))
	GLCall(glDeleteShader(fs))
	
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
	
	float positions[] = {
			-0.5f, -0.5f,
			+0.5f, -0.5f,
			+0.5f, +0.5f,
			-0.5f, +0.5f,
	};
	
	unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
	};
	
	unsigned int buffer;
	GLCall(glGenBuffers(1, &buffer))
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer))
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW))
	
	GLCall(glEnableVertexAttribArray(0))
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr))
	
	unsigned int ibo; // indices buffer object
	GLCall(glGenBuffers(1, &ibo))
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo))
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW))
	
	ShaderProgramSource source = ParseShader();
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	GLCall(glUseProgram(shader))
	
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr))
		
		/* Swap front and back buffers */
		glfwSwapBuffers(window);
		
		/* Poll for and process events */
		glfwPollEvents();
	}
	GLCall(glDeleteProgram(shader))
	
	glfwTerminate();
	return 0;
}