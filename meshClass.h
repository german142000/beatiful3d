#pragma once
#include <vector>
#include "vertexClass.h"
#include "normalsClass.h"
#include <string>

using namespace std;

class Mesh {
    private:
        vector<Vertex> vertexes;
        vector<Normal> norm;
        float x = 0;
        float y = 0;
        float z = 0;
        float sizeX = 1;
        float sizeY = 1;
        float sizeZ = 1;
        float rotateX = 0;
        float rotateY = 0;
        float rotateZ = 0;
        bool isUpdateV = false;
        bool changeable = true;
        int materialId = 0;
        string meshName;
    public:
        Mesh(string name, bool ishangeable, int mid);
        Mesh(string name, float vx, float vy, float vz, float sx, float sy, float sz, float rx, float ry, float rz, int mid);
        bool addVertex(float x, float y, float z, float vUvx, float vUvy, bool tiuv);
        bool addNormal(Normal norms);
        vector<Vertex> getVertexes();
        vector<Normal> getNormals();
        bool isUpdate();
        bool setPosition(float vx, float vy, float vz);
        bool setSize(float vx, float vy, float vz);
        bool setRotation(float vx, float vy, float vz);
        vector<float> getPosition();
        vector<float> getSize();
        vector<float> getRotation();
        void updateRead();
        int getMaterialId();
        bool setMaterialId(int newMaterialId);
        string getName();
        bool isMoved();
        bool setVertexes(Vertex* vrtx, int vertexNumber);
        bool clearVertexes();
        bool clearNormals();
};
