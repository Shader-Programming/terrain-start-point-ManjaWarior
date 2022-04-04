#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Shader.h"
#include "Camera.h"

#include <Model.h>
#include "Terrain.h"
#include "water.h"
#include "TextureManager.h"

#include<string>
#include <numeric>

#include "Source.h"


// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

//own variables
glm::vec3 dirLightPos(1.4f, -.6f, 0.4f);

unsigned int NormalMap = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
//unsigned int loadTexture2(char const * path);
void setVAO(vector <float> vertices);

void setWaterFBOs();
void refractionPass(Terrain* terrain, Shader shader);
void reflectionPass(Terrain* terrain, Shader shader);

glm::vec4 waterPlane = glm::vec4(0.f, -1.f, 0.f, 90.f);

unsigned int refractionFBO, reflectionFBO, refractionTex, reflectionTex;

void setLightUniforms(Shader& shader, Shader* waterShader);
void updatePerFrameUniforms(Shader& shader, Shader* waterShader);

// camera
Camera camera(glm::vec3(260, 125, 300));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::mat4 view = camera.GetViewMatrix();
glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1200.0f);
glm::mat4 model = glm::mat4(1.0f);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "IMAT3907", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	setWaterFBOs();

	TextureManager* texMan = new TextureManager();

	// simple vertex and fragment shader - add your own tess and geo shader
	Shader shader("..\\shaders\\tessVert.vs", "..\\shaders\\phongDirFrag.fs", "..\\shaders\\FlatShadingGeo.gs", "..\\shaders\\tessControlShader.tcs", "..\\shaders\\tessEvaluationShader.tes");

	unsigned int normalMap = texMan->loadTexture("..\\resources\\newPath\\Ground_Dirt_009_Normal.jpg");
	unsigned int output_img = texMan->createTexture(512, 512);
	unsigned int normal_img = texMan->createTexture(512, 512);

	unsigned int waterNormalMap = texMan->loadTexture("..\\resources\\water\\waterNormals.png");
	unsigned int waterDUDV = texMan->loadTexture("..\\resources\\water\\waterDUDV.png");

	//Terrain Constructor ; number of grids in width, number of grids in height, gridSize
	Terrain* terrain = new Terrain(50, 50, 10);
	terrain->assignTextures(output_img, normal_img, normalMap);

	Water water(90, 50, 10);
	water.assignTextures(waterNormalMap, waterDUDV);

	shader.use();
	setLightUniforms(shader, water.waterShader);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		updatePerFrameUniforms(shader, water.waterShader);

		shader.use();
		terrain->drawTerrain();

		glEnable(GL_CLIP_DISTANCE0);
		reflectionPass(terrain, shader);
		refractionPass(terrain, shader);
		glDisable(GL_CLIP_DISTANCE0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		water.waterShader->use();
		water.waterShader->setFloat("time", glfwGetTime() * 0.5);
		water.renderWater(reflectionTex, refractionTex);


		//texMan->drawTexture(normal_img);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)//normal map swap for flat shading or tri planer normal mapping
	{
		if (NormalMap == 1) NormalMap = 0;
		else NormalMap = 1;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}





// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void setLightUniforms(Shader& tess, Shader* waterShader) {//think about moving this stuff to terrain class, issue is currently aterrain even with all GLM header files cannot use perspective 
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1200.0f);
	model = glm::mat4(1.0f);

	tess.use();

	tess.setInt("perlin_img", 0);
	tess.setInt("normals_img", 1);
	tess.setInt("normalMap", 2);

	//light properties
	tess.setVec3("dirLight.direction", dirLightPos);
	tess.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
	tess.setVec3("dirLight.diffuse", 0.55f, 0.55f, 0.55f);
	tess.setVec3("dirLight.specular", 0.6f, 0.6f, 0.6f);
	//material properties
	tess.setVec3("mat.ambient", 0.3, 0.387, 0.317);
	tess.setVec3("mat.diffuse", 0.3, 0.3, 0.3);
	tess.setVec3("mat.specular", 0.297f, 0.308f, 0.306f);
	tess.setFloat("mat.shininess", 0.9f);

	//other properties
	tess.setMat4("projection", projection);
	tess.setMat4("model", model);

	tess.setInt("scale", 1); //scale of perlin noise generation 
	tess.setInt("octaves", 10);//number of octaves in perlin noise

	//fog stuff
	tess.setFloat("DENS", 0.005f);//density of the fog
	tess.setFloat("G", 1.2f);//gradient of the fog

	const float RED = 0.5f;
	const float GREEN = 0.5f;
	const float BLUE = 0.5f;
	glClearColor(RED, GREEN, BLUE, 1.0); //default sky colour
	tess.setVec3("sky", glm::vec3(RED, GREEN, BLUE));//grey colour for fog

	waterShader->use();
	waterShader->setFloat("screenW", SCR_WIDTH);
	waterShader->setFloat("screenH", SCR_HEIGHT);
	waterShader->setVec3("lightDir", dirLightPos);
	waterShader->setInt("normalMap", 3);
	waterShader->setInt("DuDvMap", 4);
}

void updatePerFrameUniforms(Shader& tess, Shader* waterShader)
{
	tess.use();
	view = camera.GetViewMatrix();
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1200.0f);
	model = glm::mat4(1.0f);
	if (camera.Position.y < 89)
	{
		tess.setFloat("DENS", 0.03f);
	}
	else if (camera.Position.y >= 89)
	{
		tess.setFloat("DENS", 0.005f);
	}

	tess.setMat4("projection", projection);
	tess.setMat4("view", view);
	tess.setMat4("model", model);
	tess.setVec3("viewPos", camera.Position);
	tess.setInt("Map", NormalMap);

	waterShader->use();
	waterShader->setMat4("projection", projection);
	waterShader->setMat4("view", view);
	waterShader->setMat4("model", model);
	waterShader->setVec3("viewPos", camera.Position);
	waterShader->setInt("Map", NormalMap);
}

void setWaterFBOs()
{
	glGenFramebuffers(1, &refractionFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
	glGenTextures(1, &refractionTex);
	glBindTexture(GL_TEXTURE_2D, refractionTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTex, 0);

	glGenFramebuffers(1, &reflectionFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
	glGenTextures(1, &reflectionTex);
	glBindTexture(GL_TEXTURE_2D, reflectionTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void reflectionPass(Terrain* terrain, Shader shader)
{
	shader.setVec4("plane", -waterPlane);
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	terrain->drawTerrain();
}

void refractionPass(Terrain* terrain, Shader shader)
{
	shader.setVec4("plane", waterPlane);
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	terrain->drawTerrain();
}