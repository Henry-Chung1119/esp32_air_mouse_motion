#include <mouse_motion_cal.h>
int8_t horizontal_motion_cal(float x, float y, float z) {
    return (int)(-z*40);
}
int8_t vertical_motion_cal(float x, float y, float z) {
    return (int)(y*40);
}