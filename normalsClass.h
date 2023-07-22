#pragma once
#include <vector>

using namespace std;

class Normal {
    private:
        float x = 0;
        float y = 0;
        float z = 0;
    public:
        Normal(float vx = 0, float vy = 0, float vz = 0);
        float* getNormal();
        float getX();
        float getY();
        float getZ();
};
