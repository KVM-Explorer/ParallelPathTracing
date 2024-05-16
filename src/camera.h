#pragma once
#include "utils/types.h"
#include "utils/image.h"

struct Camera {
    Vec position;
    Vec direction;
    Vec cx;
    Vec cy;

    Camera(Vec position, Vec direction,const Image& image) : position(position), direction(direction) {
        cx = Vec(image.width * .5135 / image.height);
        cy = (cx % direction).norm() * .5135;
    }



};