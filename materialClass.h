#pragma once
#include <stdint.h>
#include <string>
#include "textureLoaderClass.h"

using namespace std;

class Material{
    private:
        int type = 0;//DSG - 0, BMR - 1
        int resolution = 256;
        TextureLoader resizer;
        vector<uint8_t> diffuseMap;
        vector<uint8_t> specularMap;
        vector<uint8_t> glossinessMap;
        vector<uint8_t> baseColor;
        vector<uint8_t> metallicMap;
        vector<uint8_t> roughnessMap;
        vector<uint8_t> normalMap;
        vector<uint8_t> ambientOcclusionMap;
    public:
        Material(int res, string mType);
        bool setDSGtextures(vector<uint8_t> d, vector<uint8_t> s, vector<uint8_t> g, vector<int> wh);
        bool setBMRtextures(vector<uint8_t> b, vector<uint8_t> m, vector<uint8_t> r, vector<int> wh);
        bool setNormalMap(vector<uint8_t> nm, int w, int h);
        bool setAmbientOcclusionMap(vector<uint8_t> aom, int w, int h);
        vector<uint8_t> getDiffuseMap();
        vector<uint8_t> getSpecularMap();
        vector<uint8_t> getGlossinessMap();
        vector<uint8_t> getBaseColor();
        vector<uint8_t> getMetallicMap();
        vector<uint8_t> getRoughnessMap();
        vector<uint8_t> getNormalMap();
        vector<uint8_t> getAmbientOcclusionMap();
};
