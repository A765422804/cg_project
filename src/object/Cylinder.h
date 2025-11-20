#pragma once
#include "Object.h"

class Cylinder : public Object
{
public:
    Cylinder(const char *vertexPath, const char *fragmentPath, Material *material,
             float radius = 0.5f, float height = 1.0f, unsigned int segments = 36);

    void render(glm::mat4 model, glm::mat4 view, glm::mat4 projection) override;
    ~Cylinder();
    void renderVertex() override;

private:
    float radius;
    float height;
    unsigned int segments;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void generateCylinderData();
};