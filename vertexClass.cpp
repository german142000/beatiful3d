#include "vertexClass.h"

Vertex::Vertex(float vx, float vy, float vz, float vUvx, float vUvy, bool tiuv) {
    x = vx;
    y = vy;
    z = vz;
    uvx = vUvx;
    uvy = vUvy;
    thereIsUv = tiuv;
};

float Vertex::getX() {
    return x;
};

float Vertex::getY() {
    return y;
};

float Vertex::getZ() {
    return z;
};

float *Vertex::getVertex(){
    float varr[3];
    varr[0] = x;
    varr[1] = y;
    varr[2] = z;
    return varr;
};

bool Vertex::setX(float value){
    x = value;
    return true;
};

bool Vertex::setY(float value){
    y = value;
    return true;
};

bool Vertex::setZ(float value){
    z = value;
    return true;
};

float Vertex::getUvx(){
    return uvx;
};

float Vertex::getUvy(){
    return uvy;
};

bool Vertex::setUvx(float value){
    uvx = value;
    return true;
};

bool Vertex::setUvy(float value){
    uvy = value;
    return true;
};

bool Vertex::uvSetting(){
    return thereIsUv;
};
