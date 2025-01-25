#include "perlin.hpp"
#include "NoiseGenerator.hpp"

// Display
GLFWwindow* _window;

mat4 projectionMatrix;
mat4 viewMatrix;
bool keyStates[348] = {false};
bool ignoreMouseEvent = false;

int windowHeight = W_HEIGHT;
int windowWidth = W_WIDTH;

// FPS counter
int frameCount = 0;
double lastFrameTime = 0.0;
double currentFrameTime = 0.0;
double fps = 0.0;
double triangleDrown = 0.0;

double offsetX = 0.0;
double offsetY = 0.0;
double speed = 200.0;

// std::vector<Chunk> chunks;
NoiseGenerator noise_gen(489465416);


void calculateFps()
{
	frameCount++;
	currentFrameTime = glfwGetTime();

	double timeInterval = currentFrameTime - lastFrameTime;

	if (timeInterval > 1)
	{
		fps = frameCount / timeInterval;

		lastFrameTime = currentFrameTime;
		frameCount = 0;

		std::stringstream title;
		title << "Not ft_minecraft | FPS: " << fps;
		glfwSetWindowTitle(_window, title.str().c_str());
	}
}

void generateNoiseTexture(NoiseGenerator& noiseGen, std::vector<unsigned char>& pixels) {
    // Ensure the pixels vector has the correct size
    pixels.resize(W_WIDTH * W_HEIGHT * 3);

    for (int y = 0; y < W_HEIGHT; ++y) {
        for (int x = 0; x < W_WIDTH; ++x) {
            // Scale coordinates to fit noise sampling
            double nx = static_cast<double>(x + offsetX) / W_WIDTH;
            double ny = static_cast<double>(y + offsetY) / W_HEIGHT;

			vec2 pos = noiseGen.getBorderWarping(nx, ny);
			noiseGen.setNoiseData(NoiseData());
            // Generate noise at the scaled coordinates
            double value = noiseGen.noise(pos.x, pos.y);

            // Normalize noise value from [-1, 1] to [0, 255]
        	value = (value + 1.0) * 0.5; // Map from [-1, 1] to [0, 1]
			value = std::clamp(value, 0.0, 1.0);
            unsigned char color = static_cast<unsigned char>(value * 255);

            // Set the pixel values (R, G, B are the same for grayscale)
            size_t index = (y * W_WIDTH + x) * 3;
            pixels[index + 0] = color; // Red
            pixels[index + 1] = color; // Green
            pixels[index + 2] = color; // Blue
        }
    }
}

bool isWSL() {
	return (std::getenv("WSL_DISTRO_NAME") != nullptr); // WSL_DISTRO_NAME is set in WSL
}

void keyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	(void)window;
	(void)scancode;
	(void)mods;

	if (action == GLFW_PRESS) keyStates[key] = true;
	else if (action == GLFW_RELEASE) keyStates[key] = false;
	if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(_window, GL_TRUE);
}

void display(GLFWwindow* window)
{
	(void)window;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);


	calculateFps();
	glfwSwapBuffers(_window);
}

void update(GLFWwindow* window)
{
	(void)window;
	display(_window);
}

void reshape(GLFWwindow* window, int width, int height)
{
	(void)window;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	
	projectionMatrix = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 1.0f, 1000.0f);
	glLoadMatrixf(glm::value_ptr(projectionMatrix));
}

int initGLFW()
{
	_window = glfwCreateWindow(windowWidth, windowHeight, "Not_ft_minecraft | FPS: 0", NULL, NULL);
	if (!_window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 0;
	}

	glfwMakeContextCurrent(_window);
	glfwSetFramebufferSizeCallback(_window, reshape);
	glfwSetKeyCallback(_window, keyPress);
	return 1;
}

void initGLEW() {
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cerr << "GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
		return ;
	}
}

int main()
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	if (!initGLFW())
		return 1;

	initGLEW();

	std::vector<unsigned char> pixels(W_WIDTH * W_HEIGHT * 3);
	generateNoiseTexture(noise_gen, pixels);



	GLuint texture = -1;
	// Main loop
	while (!glfwWindowShouldClose(_window))
	{
		if (keyStates[GLFW_KEY_W])
			offsetY += speed;
		if (keyStates[GLFW_KEY_A])
			offsetX -= speed;
		if (keyStates[GLFW_KEY_S])
			offsetY -= speed;
		if (keyStates[GLFW_KEY_D])
			offsetX += speed;

		generateNoiseTexture(noise_gen, pixels);
		glDeleteTextures(1, &texture);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W_WIDTH, W_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Clear screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw fullscreen quad
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
		glEnd();

		// Swap buffers and poll events
		glfwSwapBuffers(_window);
		glfwPollEvents();
	}

	glDeleteTextures(1, &texture);
	glfwDestroyWindow(_window);
	glfwTerminate();
	return 0;
}
