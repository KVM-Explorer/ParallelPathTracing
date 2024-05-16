#pragma once
#include "utils/image.h"
#include "camera.h"
#include "scene.h"

const int MaxDepth = 5;

class RayTracer {
    public:
      virtual void render(Image &image, Camera camera, Scene &scene, int samples) = 0;
      virtual void testPixel(Scene &Scene) = 0;
      
};