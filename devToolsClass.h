#pragma once
#include <windows.h>
#include <vector>

using namespace std;

struct meshInfo{
    float x = 0;
    float y = 0;
    float z = 0;
    float sx = 0;
    float sy = 0;
    float sz = 0;
    float rx = 0;
    float ry = 0;
    float rz = 0;
    bool type = false;
    int materialId = 0;
};

extern bool devToolsThreadReadWrite;
extern bool mainThreadReadWrite;

extern bool exitEngine;
extern string sceneName;
extern string objectName;
extern string objectType;
extern bool getObjects;
extern vector<string> objects;
extern bool objectsNamesReturns;

extern bool getMeshInfo;
extern int meshId;
extern meshInfo* mi;
extern bool meshInfoRetunred;

extern string openModelFileName;


class DevTools {
    private:

    public:
        DevTools(HINSTANCE hInstance, HWND mainWindow);
};
