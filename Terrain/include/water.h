#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "Shader.h"

class Water {
public:
    Water();
    Water(int widthIn, int heightIn, int stepSizeIn);
    void createWater();
    void renderWater(unsigned int& reflection, unsigned int& refraction);
    Shader* waterShader = new Shader("..\\shaders\\water\\Water.vs", "..\\shaders\\water\\Water.fs", "..\\shaders\\water\\Water.gs", "..\\shaders\\water\\Water.tcs", "..\\shaders\\water\\Water.tes");
private:
    float height;
    float width;
    float stepSize;
    std::vector<float> vertices;
    void MakeVertex(int x, int y, std::vector<float>* vertices);
    unsigned int waterVBO, waterVAO;
};