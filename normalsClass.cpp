#include "normalsClass.h"

Normal::Normal(float vx, float vy, float vz){
    x = vx;
    y = vy;
    z = vz;
};

float Normal::getX(){
    return x;
};

float Normal::getY(){
    return y;
};

float Normal::getZ(){
    return z;
};
