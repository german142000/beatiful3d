#include "meshClass.h"
#include "vertexClass.h"
#include "normalsClass.h"
#include <iostream>

Mesh::Mesh(string name, bool ishangeable, int mid){
    changeable = ishangeable;
    materialId = mid;
    meshName = name;
};

Mesh::Mesh(string name, float vx, float vy, float vz, float sx, float sy, float sz, float rx, float ry, float rz, int mid){
    changeable = false;
    x = vx;
    y = vy;
    z = vz;
    sizeX = sx;
    sizeY = sy;
    sizeZ = sz;
    rotateX = rx;
    rotateY = ry;
    rotateZ = rz;
    materialId = mid;
    meshName = name;
};

bool Mesh::addVertex(float x, float y, float z, float vUvx, float vUvy, bool tiuv){
    isUpdateV = true;
    vertexes.push_back(Vertex(x, y, z, vUvx, vUvy, tiuv));
    return true;
};

bool Mesh::addNormal(Normal norms){
    isUpdateV = true;
    norm.push_back(norms);
    return true;
};

vector<Vertex> Mesh::getVertexes(){
    return vertexes;
};

vector<Normal> Mesh::getNormals(){
    return norm;
};

bool Mesh::isUpdate(){
    return isUpdateV;
};

bool Mesh::setPosition(float vx, float vy, float vz){
    if(changeable){
        isUpdateV = true;
        x = vx;
        y = vy;
        z = vz;
        return true;
    } else return false;

};

bool Mesh::setSize(float vx, float vy, float vz){
    if(changeable){
        isUpdateV = true;
        sizeX = vx;
        sizeY = vy;
        sizeZ = vz;
        return true;
    } else return false;
};

bool Mesh::setRotation(float vx, float vy, float vz){
    if(changeable){
        isUpdateV = true;
        rotateX = vx;
        rotateY = vy;
        rotateZ = vz;
        return true;
    } else return false;
};

void Mesh::updateRead(){
    isUpdateV = false;
};

string Mesh::getName(){
    return meshName;
};

bool Mesh::isMoved(){
    return changeable;
}

vector<float> Mesh::getPosition(){
    vector<float> pos;
    pos.push_back(x);
    pos.push_back(y);
    pos.push_back(z);
    return pos;
};

vector<float> Mesh::getSize(){
    vector<float> pos;
    pos.push_back(sizeX);
    pos.push_back(sizeY);
    pos.push_back(sizeZ);
    return pos;
};

vector<float> Mesh::getRotation(){
    vector<float> pos;
    pos.push_back(rotateX);
    pos.push_back(rotateY);
    pos.push_back(rotateZ);
    return pos;
};

int Mesh::getMaterialId(){
    return materialId;
};

bool Mesh::setVertexes(Vertex* vrtx, int vertexNumber){
    vertexes.clear();
    for(int i = 0; i < vertexNumber; i++) vertexes.push_back(vrtx[i]);
    isUpdateV = true;
    return true;
};

bool Mesh::clearVertexes(){
    vertexes.clear();
};

bool Mesh::clearNormals(){
    norm.clear();
};






