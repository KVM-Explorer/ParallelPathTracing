#pragma once
#include "camera.h"
#include "image.h"
#include "scene.h"
#include <array>

const int MaxDepth = 50;

class RayTracer {
  public:
    virtual void render() = 0;
    virtual Vec testPixel(const Ray &r, Scene &Scene) = 0;
    virtual std::string name() = 0;
};