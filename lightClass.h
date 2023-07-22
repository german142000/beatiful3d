#pragma once

using namespace std;

class Light {
    private:
        uint8_t type = 0; //0 - point, 1 - direction light, 2 - spotlight
        float x = 0;
        float y = 0;
        float z = 0;
        float tx = 0;
        float ty = 0;
        float tz = 0;
        float diameter = 1;
    public:
        Light(uint8_t type);
        bool setPostion(vector<float> pos);
};
