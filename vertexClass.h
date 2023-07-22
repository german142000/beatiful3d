#pragma once

class Vertex {
    private:
        float x = 0;
        float y = 0;
        float z = 0;
        float uvx = 0;
        float uvy = 0;
        bool thereIsUv = false;
    public:
        Vertex(float vx = 0, float vy = 0, float vz = 0, float vUvx = 0, float vUvy = 0, bool tiuv = false);
        float getX();
        float getY();
        float getZ();
        float *getVertex();
        bool setX(float value);
        bool setY(float value);
        bool setZ(float value);
        float getUvx();
        float getUvy();
        bool setUvx(float value);
        bool setUvy(float value);
        bool uvSetting();
};

