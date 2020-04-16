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

float eyeX = 1.0f;
float eyeY = 0.75f;
float eyeZ = 2.0f;

float centerX = 0.0f;
float centerY = 0.0f;
float centerZ = 0.0f;

float step = 0.25f;

glm::mat4 mpv() {
	glm::mat4 Proj = glm::perspective(
			glm::radians(20.0f),
			4.0f / 4.0f,
			0.1f,
			90.0f);
//	glm::mat4 Proj = glm::frustum()
	glm::mat4 View = glm::lookAt(
			glm::vec3(eyeX, eyeY, eyeZ),
			glm::vec3(centerX, centerY, centerZ),
			glm::vec3(0.0f, 1.0f, 0.0f)
	);
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	return Proj * View * Model;
}

glm::mat4 mv() {
	glm::mat4 View = glm::lookAt(
			glm::vec3(eyeX, eyeY, eyeZ),
			glm::vec3(centerX, centerY, centerZ),
			glm::vec3(0.0f, 1.0f, 0.0f)
	);
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	return View * Model;
}

glm::mat4 nm() {
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	return glm::inverseTranspose(Model);
}

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

void draw(unsigned int shader);

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
	const char* src = source.c_str();
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
	GLFWwindow* window;
	
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
	
	glfwSetWindowSizeCallback(window, [](GLFWwindow* wind, int w, int h) -> void {
		glViewport(0, 0, w, h);
	});
	
	glfwSetCursorPosCallback(window, [](GLFWwindow* wind, double xpos, double ypos) -> void {
//		std::cout << "x = " << xpos << "  y = " << ypos << std::endl;
	});
	
	glfwSetKeyCallback(window, [](GLFWwindow* wind, int key, int scanCode, int action, int mods) -> void {
		if (action != GLFW_PRESS) return;
		switch (key) {
			case GLFW_KEY_W:
				eyeZ -= step;
				break;
			case GLFW_KEY_S:
				eyeZ += step;
				break;
			case GLFW_KEY_A:
				eyeX -= step;
				break;
			case GLFW_KEY_D:
				eyeX += step;
				break;
			case GLFW_KEY_Q:
				eyeY -= step;
				break;
			case GLFW_KEY_E:
				eyeY += step;
				break;
			case GLFW_KEY_I:
				centerZ -= step;
				break;
			case GLFW_KEY_K:
				centerZ += step;
				break;
			case GLFW_KEY_J:
				centerX -= step;
				break;
			case GLFW_KEY_L:
				centerX += step;
				break;
			case GLFW_KEY_U:
				centerY -= step;
				break;
			case GLFW_KEY_O:
				centerY += step;
				break;
			default:
				break;
		}
		std::cout << "ex=" << eyeX << " ey=" << eyeY << " ez=" << eyeZ
		          << "cx= " << centerX << " cy=" << centerY << " cz=" << centerZ << std::endl;
		ShaderProgramSource source = ParseShader();
		unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
		glUseProgram(shader);
		
		draw(shader);
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
	};
	
	float normals[] = {
			+0.0f, +0.0f, 1.0f, // 0
			+0.0f, +0.0f, 1.0f, // 0
			+0.0f, +0.0f, 1.0f, // 0
			+0.0f, +0.0f, 1.0f, // 0
			
			+0.0f, +1.0f, 0.0f, // 0
			+0.0f, +1.0f, 0.0f, // 0
			+0.0f, +1.0f, 0.0f, // 0
			+0.0f, +1.0f, 0.0f, // 0
			
			+1.0f, +0.0f, 0.0f, // 0
			+1.0f, +0.0f, 0.0f, // 0
			+1.0f, +0.0f, 0.0f, // 0
			+1.0f, +0.0f, 0.0f, // 0
	};
	unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0,
			
			6, 4, 5,
			7, 6, 5,
			
			8, 9, 10,
			10, 11, 8
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
	};
	
	unsigned int buffers[3];
	glGenBuffers(3, buffers);
	
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
	
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	ShaderProgramSource source = ParseShader();
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shader);
	
	
	
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		draw(shader);
		
		/* Swap front and back buffers */
		glfwSwapBuffers(window);
		
		/* Poll for and process events */
		glfwWaitEvents();
	}
	glDeleteProgram(shader);
	
	glfwTerminate();
	return 0;
}

void draw(unsigned int shader) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 mvpMat = mpv();
	glm::mat4 mvMat = mv();
	glm::mat4 nmMat = nm();
	
	int loc = glGetUniformLocation(shader, "mvp");
	glUniformMatrix4fv(loc, 1, false, glm::value_ptr(mvpMat));
	int locMV = glGetUniformLocation(shader, "mv");
	glUniformMatrix4fv(locMV, 1, false, glm::value_ptr(mvMat));
	int locNM = glGetUniformLocation(shader, "nm");
	glUniformMatrix4fv(locNM, 1, false, glm::value_ptr(nmMat));
	
	glm::vec4 ambient = glm::vec4(0.21f, 0.13f, 0.05f, 1.0f);
	glm::vec4 bronzeDiffuse = glm::vec4(0.71f, 0.43f, 0.18f, 1.0f);
	glm::vec4 bronzeSpecular = glm::vec4(0.39f, 0.27f, 0.17f, 1.0f);
	float shininess = 128;
	
	int locAmb = glGetUniformLocation(shader, "ka");
	int locDiff = glGetUniformLocation(shader, "kd");
	int locSpec = glGetUniformLocation(shader, "ks");
	int locShine = glGetUniformLocation(shader, "shi");
	glUniform4fv(locAmb, 1, glm::value_ptr(ambient));
	glUniform4fv(locDiff, 1, glm::value_ptr(bronzeDiffuse));
	glUniform4fv(locSpec, 1, glm::value_ptr(bronzeSpecular));
	glUniform1f(locShine, shininess);
	
	glm::vec3 leye = glm::vec3(0.0f, 20.0f, -3.0f);
	int locLight = glGetUniformLocation(shader, "leye");
	glUniform3fv(locLight, 1, glm::value_ptr(leye));
	
	glDrawElements(GL_TRIANGLES, 24 - 6, GL_UNSIGNED_INT, nullptr);
}
