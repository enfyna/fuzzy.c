#include "helper.h"

float lerpf(float min, float max, float v)
{
    return (max - min) * v + min;
}
