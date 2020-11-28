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
// #include "helper_3dmath.h"
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
        speedh = (int)(x*40);
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
uint8_t updateVel(VectorFloat *vel, VectorFloat *acc, Quaternion *q2, float t)
{
    // V(t+1) = V(t)+Q^(-1)*acc*t
    // Q = [ 
    //       (w*w + x*x - y*y - z*z)  (2*x*y + 2*w*z)          (2*x*z - 2*w*y)
    //       (2*x*y - 2*w*z)          (w*w - x*x + y*y - z*z)  (2*y*z + 2*w*x)
    //       (2*x*z + 2*w*y)          (2*y*z - 2*w*x)          (w*w - x*x - y*y + z*z) 
    //     ]
    // p = [x y z]'

    Quaternion temp_q; // axis_i
    temp_q = q2->getConjugate();
    float w = temp_q.w;
    float x = temp_q.x;
    float y = temp_q.y;
    float z = temp_q.z;
    vel->x += (w*w + x*x - y*y - z*z)*(t*acc->x) + (2*x*y + 2*w*z)*(t*acc->y) + (2*x*z - 2*w*y)*(t*acc->z);
    vel->y += (2*x*y - 2*w*z)*(t*acc->x) + (w*w - x*x + y*y - z*z)*(t*acc->y) + (2*y*z + 2*w*x)*(t*acc->z);
    vel->z += (2*x*z + 2*w*y)*(t*acc->x) + (2*y*z - 2*w*x)*(t*acc->y) + (w*w - x*x - y*y + z*z)*(t*acc->z);
    // vel->x = ()
    // vel->x = (vPrev->x * q12->x + vPrev->y * q12->x + vPrev->z * q12->x) + t*acc->x;
    // vel->y = (vPrev->x * q12->y + vPrev->y * q12->y + vPrev->z * q12->y) + t*acc->y;
    // vel->z = (vPrev->x * q12->z + vPrev->y * q12->z + vPrev->z * q12->z) + t*acc->z;
    // printf("vel = [%f, %f, %f]\n", vel->x, vel->y, vel->z);
    return 0;
}

/* calculate displacement being transmitted */
/* @@ */
uint8_t getDisplace(VectorFloat *v, VectorFloat *transGain, VectorFloat *rotGain, VectorFloat *trans, VectorFloat *rot)
{
    // x : the axis to measure horizontal movement
    // z : the axis to measure vertical   movement
    v->x = transGain->x * trans->x + rotGain->z * rot->z;
    v->x = (v->x)>0 ? (v->x) : -(v->x);
    v->y = transGain->z * trans->z + rotGain->x * rot->x;
    v->y = (v->y)>0 ? (v->y) : -(v->y);
    return 0;
}

// uint8_t call()
// {
//     VectorFloat xp, yp, zp; // x', y', z' from t to t+1
//     VectorFloat rTrans, rRot; // raw data of acc
//     VectorFloat vTrans, vRot; // raw data vel
//     VectorFloat vTransPrev, vRotPrev;
//     VectorFloat coefVTrans, coefVRot; // coefficients of velocity of translation and rotation
//     Quaternion q1, q2, q12; // quaternion tInit->t1, tInit->t2 and t1->t2
//     Quaternion q; // quaternion given by dmp
//     float period;

//     // init vTrans, vRot, period
//     // init vTransPrev, vRotPrev
//     // init q1 <- q

//     while(1){
//         // get quaternion, rTrans, rRot
//         // offset subtraction (gravity, static offset ...)

//         // q2 is the current quaternion
//         q12 = q2.getProduct( q1.getConjugate() );

//         updateVel(&vTrans, &rTrans, &q12, period, &vTransPrev);
//         updateVel(&vRot, &rRot, &q12, period, &vRotPrev);

//         getRotatedX(&xp, &q2);
//         getRotatedY(&yp, &q2);
//         getRotatedZ(&zp, &q2);
//         getComponent(&coefVTrans, &vTrans, &xp, &yp, &zp);
//         getComponent(&coefVRot, &vRot, &xp, &yp, &zp);

//         vTransPrev = vTrans;
//         vRotPrev = vRot;
//     }
// }