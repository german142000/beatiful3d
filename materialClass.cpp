#include "materialClass.h"

Material::Material(int res, string mType){
    resolution = res;
    if(mType == "DSG") type = 0;
    else type = 1;
};

bool Material::setDSGtextures(vector<uint8_t> d, vector<uint8_t> s, vector<uint8_t> g, vector<int> wh){
    if(type == 0){
        diffuseMap = resizer.resizeTextureRGB24(d, resolution, resolution, wh[0], wh[1]);
        specularMap = resizer.resizeTextureRGB24(s, resolution, resolution, wh[2], wh[3]);
        glossinessMap = resizer.resizeTextureRGB24(g, resolution, resolution, wh[4], wh[5]);
        return true;
    } else return false;
};

bool Material::setBMRtextures(vector<uint8_t> b, vector<uint8_t> m, vector<uint8_t> r, vector<int> wh){
    if(type == 1){
        baseColor = resizer.resizeTextureRGB24(b, resolution, resolution, wh[0], wh[1]);
        metallicMap = resizer.resizeTextureRGB24(m, resolution, resolution, wh[2], wh[3]);
        roughnessMap = resizer.resizeTextureRGB24(r, resolution, resolution, wh[4], wh[5]);;
        return true;
    } else return false;
};

bool Material::setNormalMap(vector<uint8_t> nm, int w, int h){
    normalMap = resizer.resizeTextureRGB24(nm, resolution, resolution, w, h);
    return true;
};

bool Material::setAmbientOcclusionMap(vector<uint8_t> aom, int w, int h){
    ambientOcclusionMap = resizer.resizeTextureRGB24(aom, resolution, resolution, w, h);
    return true;
};

vector<uint8_t> Material::getDiffuseMap(){
    return diffuseMap;
};

vector<uint8_t> Material::getSpecularMap(){
    return specularMap;
};

vector<uint8_t> Material::getGlossinessMap(){
    return glossinessMap;
};

vector<uint8_t> Material::getBaseColor(){
    return baseColor;
};

vector<uint8_t> Material::getMetallicMap(){
    return metallicMap;
};

vector<uint8_t> Material::getRoughnessMap(){
    return roughnessMap;
};

vector<uint8_t> Material::getNormalMap(){
    return normalMap;
};

vector<uint8_t> Material::getAmbientOcclusionMap(){
    return ambientOcclusionMap;
};
