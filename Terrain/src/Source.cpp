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
#include "SkyBox.h"

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

void createWaterFBOs();//create and bind frame buffers for reflection and refraction
void refractionPass(Terrain* terrain, Shader shader, SkyBox skyBox, Shader skyBoxShader);//draw the terrain for refraction on surface
void reflectionPass(Terrain* terrain, Shader shader, SkyBox skyBox, Shader skyBoxShader);//draw the terrain for reflection on surface

glm::vec4 waterPlane = glm::vec4(0.f, -1.f, 0.f, 90.f);//plane for plane clipping the water

unsigned int refractionFBO, reflectionFBO, refractionTexture, reflectionTexture;

void setLightUniforms(Shader& shader, Shader* waterShader, SkyBox skyBox, Shader skyBoxShader);//sets the initial uniforms
void updatePerFrameUniforms(Shader& shader, Shader* waterShader, Shader skyBoxShader);//updates the uniforms every frame

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

	createWaterFBOs();

	TextureManager* texMan = new TextureManager();

	// simple vertex and fragment shader - add your own tess and geo shader
	Shader shader("..\\shaders\\tessVert.vs", "..\\shaders\\phongDirFrag.fs", "..\\shaders\\FlatShadingGeo.gs", "..\\shaders\\tessControlShader.tcs", "..\\shaders\\tessEvaluationShader.tes");//terrain shader
	Shader skyBoxShader("..\\shaders\\Skybox\\SB.vs", "..\\shaders\\Skybox\\SB.fs");

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

	SkyBox skyBox;
	skyBox.createSkyBox();

	shader.use();
	setLightUniforms(shader, water.waterShader, skyBox, skyBoxShader);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		updatePerFrameUniforms(shader, water.waterShader, skyBoxShader);

		shader.use();
		terrain->drawTerrain();
		skyBox.renderSkyBox(skyBoxShader);

		glEnable(GL_CLIP_DISTANCE0);
		reflectionPass(terrain, shader, skyBox, skyBoxShader);
		refractionPass(terrain, shader, skyBox, skyBoxShader);
		glDisable(GL_CLIP_DISTANCE0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		water.waterShader->use();
		water.renderWater(reflectionTexture, refractionTexture);


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

void setLightUniforms(Shader& tess, Shader* waterShader, SkyBox skyBox, Shader skyBoxShader) {//think about moving this stuff to terrain class, issue is currently aterrain even with all GLM header files cannot use perspective 
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

	const float RED = 0.4f;
	const float GREEN = 0.6f;
	const float BLUE = 0.4f;
	glClearColor(RED, GREEN, BLUE, 1.0); //default sky colour
	tess.setVec3("sky", glm::vec3(RED, GREEN, BLUE));//greenish colour for fog

	waterShader->use();
	waterShader->setFloat("screenWidth", SCR_WIDTH);
	waterShader->setFloat("screenHeight", SCR_HEIGHT);
	waterShader->setVec3("lightDir", dirLightPos);
	waterShader->setInt("normalMap", 3);
	waterShader->setInt("DuDvMap", 4);

	waterShader->setFloat("waves[0].amp", 1.5f);
	waterShader->setVec2("waves[0].waveDir", glm::vec2(1.0, 0.0));
	waterShader->setFloat("waves[0].crestdist", 50.0f);
	waterShader->setFloat("waves[0].speed", 0.5f);

	waterShader->setFloat("waves[1].amp", 1.0f);
	waterShader->setVec2("waves[1].waveDir", glm::vec2(0.75, 0.0));
	waterShader->setFloat("waves[1].crestdist", 30.0f);
	waterShader->setFloat("waves[1].speed", 0.75f);

	waterShader->setFloat("waves[2].amp", 1.5f);
	waterShader->setVec2("waves[2].waveDir", glm::vec2(0.5, 0.0));
	waterShader->setFloat("waves[2].crestdist", 25.0f);
	waterShader->setFloat("waves[2].speed", 1.0f);

	waterShader->setFloat("waves[3].amp", 0.5f);
	waterShader->setVec2("waves[3].waveDir", glm::vec2(0.25, 0.0));
	waterShader->setFloat("waves[3].crestdist", 65.0f);
	waterShader->setFloat("waves[3].speed", 1.0f);//sets the uniforms for each wave

	skyBoxShader.use();
	skyBoxShader.setMat4("view", view);
	skyBoxShader.setMat4("projection", projection);
	skyBoxShader.setInt("skybox", 7);//skybox uniforms
}

void updatePerFrameUniforms(Shader& tess, Shader* waterShader, Shader skyBoxShader)
{
	tess.use();
	view = camera.GetViewMatrix();
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1200.0f);
	model = glm::mat4(1.0f);
	if (camera.Position.y < 89)//if below water
	{
		tess.setFloat("DENS", 0.03f);
	}
	else if (camera.Position.y >= 89)//if above water
	{
		tess.setFloat("DENS", 0.0025f);
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
	waterShader->setFloat("time", glfwGetTime() * 0.3);
	 
	skyBoxShader.use();
	skyBoxShader.setMat4("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
	skyBoxShader.setMat4("projection", projection);
}

void createWaterFBOs()
{
	glGenFramebuffers(1, &refractionFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
	glGenTextures(1, &refractionTexture);
	glBindTexture(GL_TEXTURE_2D, refractionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTexture, 0);//creates refraction framebuffer

	glGenFramebuffers(1, &reflectionFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
	glGenTextures(1, &reflectionTexture);
	glBindTexture(GL_TEXTURE_2D, reflectionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);//creates reflection framebuffer

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void reflectionPass(Terrain* terrain, Shader shader, SkyBox skyBox, Shader skyBoxShader)
{
	shader.setVec4("plane", -waterPlane);
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	skyBox.renderSkyBox(skyBoxShader);
	shader.use();
	terrain->drawTerrain();//both terrain and skybox need to be drawn for reflection on water surface
	
}

void refractionPass(Terrain* terrain, Shader shader, SkyBox skyBox, Shader skyBoxShader)
{
	shader.setVec4("plane", waterPlane);
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.use();
	terrain->drawTerrain();//will refract the appearance of terrain underwater
}