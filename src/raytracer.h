#pragma once
#include "image.h"
#include "camera.h"
#include "scene.h"
const int MaxDepth = 50;

class RayTracer {
    public:
      virtual void render() = 0;
      virtual Vec testPixel(const Ray& r, Scene &Scene) = 0;
};