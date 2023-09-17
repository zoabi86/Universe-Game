#pragma once

#include <glm/glm.hpp>

class Planet {
public:
    explicit Planet();
    glm::vec3 _position;     // Position
    float _radius;          // Radius
    float _gravityFactor;
    unsigned int _texture;
};
