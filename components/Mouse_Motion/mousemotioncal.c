#include "mousemotioncal.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "helper_3dmath.h"
// #include "button.h"
#define TAG "BUTTON"

// typedef struct {
// 	uint8_t pin;
//     bool inverted;
// 	uint16_t history;
//     uint64_t down_time;
// } debounce_t;
// int pin_count = -1;
// debounce_t * debounce;
// QueueHandle_t * queue;

int8_t horizontal_motion_cal(float x, float y, float z) {
    int speedh = 0;
    // if(abs(z-lastz) > 1.0) {
        speedh = (int)(-z*40);
    // }
    return speedh;
}
int8_t vertical_motion_cal(float x, float y, float z) {
    int speedv = 0;
    // if(abs(y-lasty) > 1.0) {
        speedv = (int)(y*40);
    // }
    return speedv;
    // return (int)(y*40);
}


/* v = Qx[1 0 0]' */
uint8_t getRotatedX(VectorFloat *v, Quaternion *q)
{
    v -> x = (q->w * q->w) + (q->x * q->x) - (q->y*q->y) - (q->z*q->z);
    v -> y = 2 * (q->x * q->y - q->w * q->z);
    v -> z = 2 * (q->x * q->z + q->w * q->y);
    return 0;
}
/* v = Qx[0 1 0]' */
uint8_t getRotatedY(VectorFloat *v, Quaternion *q)
{
    v -> x = 2 * (q->x * q->y + q->w * q->z);
    v -> y = (q->w * q->w) - (q->x * q->x) + (q->y*q->y) - (q->z*q->z);
    v -> z = 2 * (q->y * q->z - q->w * q->x);
    return 0;
}
/* v = Qx[0 0 1]' */
uint8_t getRotatedZ(VectorFloat *v, Quaternion *q) 
{
    v -> x = 2 * (q->x * q->z - q->w * q->y);
    v -> y = 2 * (q->y * q->z + q->w * q->x);
    v -> z = (q->w * q->w) - (q->x * q->x) - (q->y*q->y) + (q->z*q->z);
    return 0;
}

/* decompose r into given vectors(x, y, z) */
/*
    v: output
    r: decomposed vector
    x, y, z : given basis
*/
uint8_t getComponent(VectorFloat *v, VectorFloat *r, VectorFloat *x, VectorFloat *y, VectorFloat *z)
{
    v->x = r->x * x->x + r->y * x->y + r->z * x->z;
    v->y = r->x * y->x + r->y * y->y + r->z * y->z;
    v->z = r->x * z->x + r->y * z->y + r->z * z->z;
    return 0;
}

/* update previous raw vel to current raw vel (rotatition considered) */
/*
    vel: output
    acc: raw acceleration data
    q  : quaternion from t to t+1 (not the one from dmp, it is from t=0 to t=t+1)
    t  : period
    vPrev: previous raw velocity
*/
uint8_t updateVel(VectorFloat *vel, VectorFloat *acc, Quaternion *q, float t, VectorFloat *vPrev)
{
    VectorFloat ax_x, ax_y, ax_z; // axis_i
    getRotatedX(&ax_x, q); // ax_x = x'
    getRotatedY(&ax_y, q); // ax_y = y'
    getRotatedZ(&ax_z, q); // ax_z = z'
    vel->x = (vPrev->x * ax_x.x + vPrev->y * ax_y.x + vPrev->z * ax_z.x) + t*acc->x;
    vel->y = (vPrev->x * ax_x.y + vPrev->y * ax_y.y + vPrev->z * ax_z.y) + t*acc->y;
    vel->z = (vPrev->x * ax_x.z + vPrev->y * ax_y.z + vPrev->z * ax_z.z) + t*acc->z;
}

/* calculate displacement being transmitted */
/* @@ */
uint8_t getDisplace(VectorFloat *v, VectorFloat *transGain, VectorFloat *rotGain, VectorFloat *trans, VectorFloat *rot)
{
    // x : the axis to measure horizontal movement
    // z : the axis to measure vertical   movement
    v->x = transGain->x * transGain->x + rot->z * rotGain->z;
    v->y = transGain->z * transGain->z + rot->x * rotGain->x;
    return 0;
}

uint8_t call()
{
    VectorFloat xp, yp, zp; // x', y', z' from t to t+1
    VectorFloat rTrans, rRot; // raw data of acc
    VectorFloat vTrans, vRot; // raw data vel
    VectorFloat vTransPrev, vRotPrev;
    VectorFloat coefVTrans, coefVRot; // coefficients of velocity of translation and rotation
    Quaternion q1, q2, q12; // quaternion tInit->t1, tInit->t2 and t1->t2
    Quaternion q; // quaternion given by dmp
    float period;

    // init vTrans, vRot, period
    // init vTransPrev, vRotPrev
    // init q1 <- q

    while(1){
        // get quaternion, rTrans, rRot
        // offset subtraction (gravity, static offset ...)

        // q2 is the current quaternion
        q12 = q2.getProduct( q1.getConjugate() );

        updateVel(&vTrans, &rTrans, &q12, period, &vTransPrev);
        updateVel(&vRot, &rRot, &q12, period, &vRotPrev);

        getRotatedX(&xp, &q2);
        getRotatedY(&yp, &q2);
        getRotatedZ(&zp, &q2);
        getComponent(&coefVTrans, &vTrans, &xp, &yp, &zp);
        getComponent(&coefVRot, &vRot, &xp, &yp, &zp);

        vTransPrev = vTrans;
        vRotPrev = vRot;
    }
}