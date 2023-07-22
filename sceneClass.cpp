#include "sceneClass.h"

using namespace std;

Scene::Scene(string sceneName, float csx, float csy, float csz, float ztfar){
    name = sceneName;
    cameraX = csx;
    cameraY = csy;
    cameraZ = csz;
    zfar = ztfar;
};

bool Scene::addMesh(Mesh newMesh){
    meshes.push_back(newMesh);
    return true;
};

bool Scene::addMaterial(Material newMaterial){
    materials.push_back(newMaterial);
    return true;
};

vector<Mesh> Scene::getMeshes(){
    return meshes;
};

vector<Material> Scene::getMaterials(){
    return materials;
};

bool Scene::addLight(Light newLight){
    lights.push_back(newLight);
    return true;
};

vector<Light> Scene::getLights(){
    return lights;
};

bool Scene::setCameraPosition(vector<float> position){
    cameraX = position[0];
    cameraY = position[1];
    cameraZ = position[2];
    return true;
};

bool Scene::setCameraTargetPosition(vector<float> position){
    cameraTargetX = position[0];
    cameraTargetY = position[1];
    cameraTargetZ = position[2];
    return true;
};

vector<float> Scene::getCameraPosition(){
    vector<float> position;
    position.push_back(cameraX);
    position.push_back(cameraY);
    position.push_back(cameraZ);
    return position;
};

vector<float> Scene::getCameraTargetPosition(){
    vector<float> position;
    position.push_back(cameraTargetX);
    position.push_back(cameraTargetY);
    position.push_back(cameraTargetZ);
    return position;
};

string Scene::getSceneName(){
    return name;
};

bool Scene::setSceneName(string sceneName){
    name = sceneName;
    return true;
};

float Scene::getCameraFOV(){
    return fov;
};

float Scene::getZFARvalue(){
    return zfar;
};

bool Scene::replaceMesh(Mesh mesh, int id){
    meshes[id] = mesh;
    return true;
};
