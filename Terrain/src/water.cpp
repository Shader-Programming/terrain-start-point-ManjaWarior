#include "Water.h"
#include "textureManager.h"

Water::Water(int heightIn, int widthIn, int stepSizeIn)
{
    height = heightIn;
    width = widthIn;
    stepSize = stepSizeIn;
    createWater();
}

Water::Water()
{
    width = 50;
    height = 50;
    stepSize = 10;
    createWater();
}

void Water::createWater()
{
    for (int y = 0; y < width - 1; y++)
    { //Move up creating layers
        float offsety = y * stepSize;
        for (int x = 0; x < width - 1; x++)
        {
            float offsetx = x * stepSize;
            MakeVertex(offsetx, offsety, &vertices);
            MakeVertex(offsetx, offsety + stepSize, &vertices);
            MakeVertex(offsetx + stepSize, offsety, &vertices);
            MakeVertex(offsetx + stepSize, offsety, &vertices);
            MakeVertex(offsetx, offsety + stepSize, &vertices);
            MakeVertex(offsetx + stepSize, offsety + stepSize, &vertices);
        }
    }

    glGenVertexArrays(1, &waterVAO);
    glGenBuffers(1, &waterVBO);

    glBindVertexArray(waterVAO);
    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glBufferData(GL_ARRAY_BUFFER, (vertices.size() * sizeof(GLfloat)), vertices.data(), GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // uv attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void Water::renderWater(unsigned int& reflectionTex, unsigned int& refractionTex)
{
    waterShader->use();
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, reflectionTex);
    waterShader->setInt("reflection", 5);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, refractionTex);
    waterShader->setInt("refraction", 6);
    //attach the framebuffers

    model = glm::mat4(1.0);
    waterShader->setMat4("model", model);
    glBindVertexArray(waterVAO);
    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glDrawArrays(GL_PATCHES, 0, vertices.size());
}

void Water::assignTextures(unsigned int normalMap, unsigned int DuDvMap)
{
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, normalMap);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, DuDvMap);
}

void Water::MakeVertex(int x, int y, std::vector<float>* vertices)
{
    //Pos
    vertices->push_back((float)x);
    vertices->push_back((float)height);
    vertices->push_back((float)y);

    //Norms
    vertices->push_back((float)0.0);
    vertices->push_back((float)1.0);
    vertices->push_back((float)0.0);

    //UV
    vertices->push_back((float)x / (width * stepSize));
    vertices->push_back((float)y / (width * stepSize));
}
