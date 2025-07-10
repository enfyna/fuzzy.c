#pragma once

#define lerp_fz(fs, v) \
    lerpf((fs)->bounds[0], (fs)->bounds[1], v)

float lerpf(float min, float max, float v);
