#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <iostream>
#include "Shader.h"

class TextureManager
{
public:
	TextureManager();
	unsigned int createTexture(int height, int width);
	void drawTexture(unsigned int texture);
	unsigned int loadTexture(char const* path);

private:
	unsigned int quadVAO, quadVBO;
	Shader* textureDrawer;
};
