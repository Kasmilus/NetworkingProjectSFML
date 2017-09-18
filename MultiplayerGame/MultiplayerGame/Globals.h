#pragma once

// 1 meter (box2d) is more or less 64 pixels (sfml)
#define RATIO 32.0f
#define PIXELS_PER_METER RATIO

// 64 pixels (sfml) are more or less 1 meter (box2d)
#define UNRATIO (1.0F/RATIO)
#define METERS_PER_PIXEL UNRATIO

// degrees = (radians * (pi/180))
#define RADTODEG (b2_pi / 180.0)