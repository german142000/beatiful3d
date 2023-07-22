#pragma once
#include "meshClass.h"
#include "materialClass.h"
#include "lightClass.h"

using namespace std;

class Scene {
    private:
        vector<Mesh> meshes;
        vector<Material> materials;
        vector<Light> lights;
        string name = "unkown scene";
        float cameraX = 4;
        float cameraY = 4;
        float cameraZ = 4;
        float zfar = 20;
        float cameraTargetX = 0;
        float cameraTargetY = 0;
        float cameraTargetZ = 0;
        float fov = 90;
    public:
        Scene(string sceneName, float csx, float csy, float csz, float ztfar);
        bool addMesh(Mesh newMesh);
        bool addMaterial(Material newMaterial);
        bool addLight(Light newLight);
        vector<Mesh> getMeshes();
        vector<Material> getMaterials();
        vector<Light> getLights();
        bool setCameraPosition(vector<float> position);
        bool setCameraTargetPosition(vector<float> position);
        vector<float> getCameraPosition();
        vector<float> getCameraTargetPosition();
        string getSceneName();
        bool setSceneName(string sceneName);
        float getCameraFOV();
        float getZFARvalue();
        bool replaceMesh(Mesh mesh, int id);
};

