#ifndef TERRAIN_H
#define TERRAIN_H


#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


#include "PerlinNoise.h"
#include "Shader.h"
#include "TextureManager.h"


class Terrain
{
public:
	Terrain(int widthIn, int heightIn, int stepSizeIn);
	Terrain();
	unsigned int getVAO();
	int getSize();
	void drawTerrain();

	//unsigned int output_img;
	//unsigned int normal_img;
private:
	std::vector<float> vertices;
	unsigned int VAO, VBO;
	int width;
	int height;
	int stepSize;
	void makeVertices(std::vector<float> *vertices);
	void makeVertex(int x, int y, std::vector<float> *vertices);
	PerlinNoise perlin;
	std::vector<float> getVertices();
	void assignComputeShadersTextures();


	Shader* compute = new Shader("..\\shaders\\ComputeShader.cs");
	Shader* normalsCompute = new Shader("..\\shaders\\normalsCompute.cs");
	TextureManager* texMan = new TextureManager();
};
#endif












