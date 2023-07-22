#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/wglext.h>
#include <iostream>
#include <string>
#include <iostream>
#include <sstream>
#include <ctime>
#include <thread>
#include <vector>

#include "vertexClass.h"
#include "normalsClass.h"
#include "textureLoaderClass.h"
#include "devToolsClass.h"
#include "sceneClass.h"
#include "OBJformatLoader.h"
#include "materialClass.h"

using namespace std;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HMODULE hLib;
void (*dpi)();

HINSTANCE inst;

HGLRC hRC;
HGLRC temphRC;

//Переменные editor mode
bool editorMode = false;

HDC phdc;

RECT windowSize;

//Переменные, отвечающие за передачу данных из главного потока и потока окна devTools
bool devToolsThreadReadWrite = false;
bool mainThreadReadWrite = false;

bool exitEngine = false;
string sceneName = "";
string objectName = "";
string objectType = "";
string openModelFileName = "";

bool getObjects = false;
bool objectsNamesReturns = false;
vector<string> objects;

bool getMeshInfo = false;
int meshId = 0;
bool meshInfoRetunred = false;
meshInfo* mi = new meshInfo;

//Сцены
vector<Scene> scenes;

Scene currentScene("", 0, 0, 0, 0);

//default materials 2x2 px
//diffuse 174 167 147
//specular and glossiness 255 255 255
vector<uint8_t> defaultDiffuse;
vector<uint8_t> defaultSpecular;
vector<int> defaultMaterialSize;

Material defaultMaterial(256, "DSG");

GLuint VBO;
GLuint VAO;

GLuint program;

int triangleNumber = 0;

vector<float> convertSceneMeshToOpenGLVertex(Scene scn){
    triangleNumber = 0;
    vector<float> vrtxs;
    for(int i = 0; i < scn.getMeshes().size(); i++){
        Mesh emesh = scn.getMeshes()[i];
        for(int r = 0; r < emesh.getVertexes().size(); r++){
            vrtxs.push_back(emesh.getVertexes()[r].getX());
            vrtxs.push_back(emesh.getVertexes()[r].getY());
            vrtxs.push_back(emesh.getVertexes()[r].getZ());
            vrtxs.push_back(emesh.getPosition()[0]);
            vrtxs.push_back(emesh.getPosition()[1]);
            vrtxs.push_back(emesh.getPosition()[2]);
            vrtxs.push_back(emesh.getSize()[0]);
            vrtxs.push_back(emesh.getSize()[1]);
            vrtxs.push_back(emesh.getSize()[2]);
            vrtxs.push_back(emesh.getRotation()[0]);
            vrtxs.push_back(emesh.getRotation()[1]);
            vrtxs.push_back(emesh.getRotation()[2]);
            vrtxs.push_back(emesh.getNormals()[r].getX());
            vrtxs.push_back(emesh.getNormals()[r].getY());
            vrtxs.push_back(emesh.getNormals()[r].getZ());
            triangleNumber++;
        }
    }
    return vrtxs;
}

bool sceneChanged(Scene scn){
    for(int i = 0; i < scn.getMeshes().size(); i++){
        if(scn.getMeshes()[i].isUpdate()){
            scn.getMeshes()[i].updateRead();
            return true;
        }
    }
    return false;
}

string mintToString(int N){
    ostringstream ss("");
    ss << N;
    return ss.str();
}

GLuint CreateShader(GLenum shaderType, const char* shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::string infoLog(infoLogLength, ' ');
        glGetShaderInfoLog(shader, infoLogLength, nullptr, &infoLog[0]);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
        return 0;
    }

    return shader;
}

GLuint CreateProgram() {
    const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 objectPosition;
layout (location = 2) in vec3 objectScale;
layout (location = 3) in vec3 objectRotation;
layout (location = 4) in vec3 normal;

out vec3 Normal;
out vec3 FragPos;

uniform vec3 cameraPosition;
uniform vec3 cameraTarget;
uniform vec3 projectionProperties1;
uniform vec2 projectionProperties2;

void main() {

    vec4 mmx = vec4(1.0, 0.0, 0.0, 0.0);
    vec4 mmy = vec4(0.0, 1.0, 0.0, 0.0);
    vec4 mmz = vec4(0.0, 0.0, 1.0, 0.0);
    vec4 mmw = vec4(objectPosition.x, objectPosition.y, objectPosition.z, 1.0);
    mat4 moveMatrix = mat4(mmx, mmy, mmz, mmw);

    vec4 msx = vec4(objectScale.x, 0.0, 0.0, 0.0);
    vec4 msy = vec4(0.0, objectScale.y, 0.0, 0.0);
    vec4 msz = vec4(0.0, 0.0, objectScale.z, 0.0);
    vec4 msw = vec4(0.0, 0.0, 0.0, 1.0);
    mat4 scaleMatrix = mat4(msx, msy, msz, msw);

    vec4 mrxx = vec4(1.0, 0.0, 0.0, 0.0);
    vec4 mrxy = vec4(0.0, cos(radians(objectRotation.x)), sin(radians(objectRotation.x)), 0.0);
    vec4 mrxz = vec4(0.0, -sin(radians(objectRotation.x)), cos(radians(objectRotation.x)), 0.0);
    vec4 mrxw = vec4(0.0, 0.0, 0.0, 1.0);
    mat4 rotateXMatrix = mat4(mrxx, mrxy, mrxz, mrxw);

    vec4 mryx = vec4(cos(radians(objectRotation.y)), 0.0, -sin(radians(objectRotation.y)), 0.0);
    vec4 mryy = vec4(0.0, 1.0, 0.0, 0.0);
    vec4 mryz = vec4(sin(radians(objectRotation.y)), 0.0, cos(radians(objectRotation.y)), 0.0);
    vec4 mryw = vec4(0.0, 0.0, 0.0, 1.0);
    mat4 rotateYMatrix = mat4(mryx, mryy, mryz, mryw);

    vec4 mrzx = vec4(cos(radians(objectRotation.z)), sin(radians(objectRotation.z)), 0.0, 0.0);
    vec4 mrzy = vec4(-sin(radians(objectRotation.z)), cos(radians(objectRotation.z)), 0.0, 0.0);
    vec4 mrzz = vec4(0.0, 0.0, 1.0, 0.0);
    vec4 mrzw = vec4(0.0, 0.0, 0.0, 1.0);
    mat4 rotateZMatrix = mat4(mrzx, mrzy, mrzz, mrzw);

    vec3 cameraDirection = normalize(cameraPosition - cameraTarget);

    vec3 up = vec3(0.0, 1.0, 0.0);

    vec3 cameraRight = normalize(cross(up, cameraDirection));

    vec3 cameraUp = cross(cameraDirection, cameraRight);

    vec4 cmx = vec4(cameraRight.x, cameraUp.x, cameraDirection.x, 0.0);
    vec4 cmy = vec4(cameraRight.y, cameraUp.y, cameraDirection.y, 0.0);
    vec4 cmz = vec4(cameraRight.z, cameraUp.z, cameraDirection.z, 0.0);
    vec4 cmw = vec4(0.0, 0.0, 0.0, 1.0);

    mat4 direction = mat4(cmx, cmy, cmz, cmw);

    vec4 cpmx = vec4(1.0, 0.0, 0.0, 0.0);
    vec4 cpmy = vec4(0.0, 1.0, 0.0, 0.0);
    vec4 cpmz = vec4(0.0, 0.0, 1.0, 0.0);
    vec4 cpmw = vec4(-cameraPosition.x, -cameraPosition.y, -cameraPosition.z, 1);

    mat4 cposition = mat4(cpmx, cpmy, cpmz, cpmw);

    mat4 LookAt = direction * cposition;

    mat4 projection;

    if(projectionProperties1.x == 0) {

        float aspect = projectionProperties2.x / projectionProperties2.y;
        float zfar = projectionProperties1.y;
        float znear = 0.1;
        float fov = projectionProperties1.z;
        float tgfov = tan(radians(fov / 2));

        vec4 pmx = vec4(1 / tgfov, 0.0, 0.0, 0.0);
        vec4 pmy = vec4(0.0, aspect / tgfov, 0.0, 0.0);
        vec4 pmz = vec4(0.0, 0.0, -(zfar + znear) / (zfar - znear), -1.0);
        vec4 pmw = vec4(0.0, 0.0, -(2.0 * zfar * znear) / (zfar - znear), 0.0);
        projection = mat4(pmx, pmy, pmz, pmw);

    }

    mat4 normalMatrix = transpose(inverse(moveMatrix * (rotateXMatrix * rotateYMatrix * rotateZMatrix) * scaleMatrix));

	vec4 transformedVector = moveMatrix * (rotateXMatrix * rotateYMatrix * rotateZMatrix) * scaleMatrix * vec4(position.x, position.y, position.z, 1.0);
	//vec4 transformedNormal = moveMatrix * (rotateXMatrix * rotateYMatrix * rotateZMatrix) * scaleMatrix * vec4(normal, 1.0);


	FragPos = vec3(transformedVector.x, transformedVector.y, transformedVector.z);
    vec4 nm = normalMatrix * vec4(normal, 1.0);
    Normal = vec3(nm.x, nm.y, nm.z);

    vec4 finalVector = projection * LookAt * transformedVector;
    gl_Position = vec4(finalVector.x / finalVector.w, finalVector.y / finalVector.w, finalVector.z / finalVector.w, 1.0);

}
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core

        in vec3 FragPos;
        in vec3 Normal;

        out vec4 fragColor;
        uniform vec3 cameraPosition;

        void main() {
            vec3 objectColor = vec3(1.0, 0.5, 0.0);
            vec3 lightColor = vec3(1.0, 1.0, 1.0);
            vec3 norm = normalize(Normal);

            float ambientStrength = 0.5;
            vec3 ambient = ambientStrength * lightColor;

            vec3 lightDir = normalize(cameraPosition - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;
            vec3 result = (ambient + diffuse) * objectColor;
            fragColor = vec4(result, 1.0);
        }
    )";

    GLuint vertexShader = CreateShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::string infoLog(infoLogLength, ' ');
        glGetProgramInfoLog(program, infoLogLength, nullptr, &infoLog[0]);
        std::cerr << "Program linking failed:\n" << infoLog << std::endl;
        return 0;
    }

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

vector<string> getObjectsName(Scene scn){
    vector<string> objNames;
    string nms;
    vector<Mesh> msh = scn.getMeshes();
    for(int b = 0; b < msh.size(); b++){
        nms = msh[b].getName();
        nms = "Mesh-" + mintToString(b) + "-" + nms;
        objNames.push_back(nms);
    }
    return objNames;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    defaultDiffuse.push_back(174);
    defaultDiffuse.push_back(167);
    defaultDiffuse.push_back(147);
    defaultDiffuse.push_back(174);
    defaultDiffuse.push_back(167);
    defaultDiffuse.push_back(147);
    defaultDiffuse.push_back(174);
    defaultDiffuse.push_back(167);
    defaultDiffuse.push_back(147);
    defaultDiffuse.push_back(174);
    defaultDiffuse.push_back(167);
    defaultDiffuse.push_back(147);

    defaultSpecular.push_back(255);
    defaultSpecular.push_back(255);
    defaultSpecular.push_back(255);
    defaultSpecular.push_back(255);
    defaultSpecular.push_back(255);
    defaultSpecular.push_back(255);
    defaultSpecular.push_back(255);
    defaultSpecular.push_back(255);
    defaultSpecular.push_back(255);
    defaultSpecular.push_back(255);
    defaultSpecular.push_back(255);
    defaultSpecular.push_back(255);

    defaultMaterialSize.push_back(4);
    defaultMaterialSize.push_back(4);
    defaultMaterialSize.push_back(4);
    defaultMaterialSize.push_back(4);
    defaultMaterialSize.push_back(4);
    defaultMaterialSize.push_back(4);



    defaultMaterial.setDSGtextures(defaultDiffuse, defaultSpecular, defaultSpecular, defaultMaterialSize);

    inst = hInstance;

    hLib = LoadLibrary("User32.dll");
    (FARPROC &)dpi = GetProcAddress(hLib, "SetProcessDPIAware");
    dpi();

    const char* className = "OpenGLAppClass";
    const char* windowTitle = "OpenGL";
    const int screenWidth = 800;
    const int screenHeight = 600;

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HWND hWnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        className,
        windowTitle,
        WS_VISIBLE | WS_POPUP | WS_MAXIMIZE,
        2,
        2,
        screenWidth,
        screenHeight,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (hWnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HDC hdc = GetDC(hWnd);

    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    if (!pixelFormat){
        CloseWindow(hWnd);
        MessageBox(NULL, "Failed to choose pixel format.", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    if (!SetPixelFormat(hdc, pixelFormat, &pfd)){
        CloseWindow(hWnd);
        MessageBox(NULL, "Failed to set pixel format.", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    temphRC = wglCreateContext(hdc);
    if (!temphRC){
        CloseWindow(hWnd);
        MessageBox(NULL, "Failed to create OpenGL rendering context.", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    if (!wglMakeCurrent(hdc, temphRC)){
        CloseWindow(hWnd);
        MessageBox(NULL, "Failed to activate OpenGL rendering context.", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(temphRC);

    if (!wglCreateContextAttribsARB){
        CloseWindow(hWnd);
        MessageBox(NULL, "wglCreateContextAttribsARB fail.", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    hRC = wglCreateContextAttribsARB(hdc, 0, attribs);

    if (!hRC || !wglMakeCurrent(hdc, hRC)){
        CloseWindow(hWnd);
        MessageBox(NULL, "Creating render context fail.", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        CloseWindow(hWnd);
        MessageBox(NULL, (const char*) glewGetErrorString(err), "GLEW error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    program = CreateProgram();
    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    phdc = GetDC(hWnd);

    GetWindowRect(hWnd, &windowSize);
    //OBJloader objLoader("C:\\Users\\gfjf\\Desktop\\cube5.obj");

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        if(sceneChanged(currentScene)){
            vector<float> vertx = convertSceneMeshToOpenGLVertex(currentScene);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertx.size(), &vertx[0], GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (GLvoid*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (GLvoid*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (GLvoid*)(9 * sizeof(float)));
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (GLvoid*)(12 * sizeof(float)));
            glEnableVertexAttribArray(4);
            glBindVertexArray(0);
            //cout << "update scene" << endl;
        }
        if(!devToolsThreadReadWrite){
            mainThreadReadWrite = true;

            if(exitEngine) PostQuitMessage(0);

            if(sceneName != "") {
                scenes.push_back(Scene(sceneName, 10, 10, 10, 30));
                currentScene = scenes[scenes.size() - 1];
                currentScene.addMaterial(defaultMaterial);
                sceneName = "";
            }

            if(objectName != "" && objectType != ""){
                if(objectType == "Static object"){
                    currentScene.addMesh(Mesh(objectName, false, 0));
                    getObjects = true;
                    objectName = "";
                    objectType = "";
                } else if(objectType == "Moving object"){
                    currentScene.addMesh(Mesh(objectName, true, 0));
                    getObjects = true;
                    objectName = "";
                    objectType = "";
                }
            }

            if(getObjects){
                objects = getObjectsName(currentScene);
                getObjects = false;
                objectsNamesReturns = true;
            }

            if(getMeshInfo){
                vector<float> psr = currentScene.getMeshes()[meshId].getPosition();
                mi->x = psr[0];
                mi->y = psr[1];
                mi->z = psr[2];
                psr = currentScene.getMeshes()[meshId].getSize();
                mi->sx = psr[0];
                mi->sy = psr[1];
                mi->sz = psr[2];
                psr = currentScene.getMeshes()[meshId].getRotation();
                mi->rx = psr[0];
                mi->ry = psr[1];
                mi->rz = psr[2];
                mi->type = currentScene.getMeshes()[meshId].isMoved();
                mi->materialId = currentScene.getMeshes()[meshId].getMaterialId();
                getMeshInfo = false;
                meshInfoRetunred = true;
            }

            if(openModelFileName != ""){
                OBJloader loader(openModelFileName);
                vector<Vertex> vrt = loader.getLoadedVertexes();
                vector<Normal> nrt = loader.getLoadedNormals();
                Mesh emesh = currentScene.getMeshes()[meshId];
                for(int i = 0; i < vrt.size(); i++){
                    emesh.addVertex(vrt[i].getX(), vrt[i].getY(), vrt[i].getZ(), vrt[i].getUvx(), vrt[i].getUvy(), vrt[i].uvSetting());
                    emesh.addNormal(nrt[i]);
                }
                currentScene.replaceMesh(emesh, meshId);
                //cout << "load model " << vrt.size() << " vertixes" << endl;
                openModelFileName = "";
            }

            mainThreadReadWrite = false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    glUseProgram(0);

    ReleaseDC(hWnd, hdc);
    DestroyWindow(hWnd);
    UnregisterClass(className, hInstance);

    return msg.wParam;
}

vector<float> cmp;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        break;
        case WM_PAINT:
            {
                cmp = currentScene.getCameraPosition();

                GLint cameraPosition = glGetUniformLocation(program, "cameraPosition");
                glUniform3f(cameraPosition, cmp[0], cmp[1], cmp[2]);

                cmp = currentScene.getCameraTargetPosition();

                GLint cameraTargetPosition = glGetUniformLocation(program, "cameraTarget");
                glUniform3f(cameraTargetPosition, cmp[0], cmp[1], cmp[2]);

                GLint pp1 = glGetUniformLocation(program, "projectionProperties1");
                glUniform3f(pp1, 0, currentScene.getZFARvalue(), currentScene.getCameraFOV());

                GLint pp2 = glGetUniformLocation(program, "projectionProperties2");
                glUniform2f(pp2, windowSize.right, windowSize.bottom);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glUseProgram(program);

                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, triangleNumber);
                glBindVertexArray(0);

                SwapBuffers(phdc);
                return 0;
            }
            break;
        case WM_KEYDOWN:{
            switch(wParam){
                case VK_F1:{
                    EnableWindow(hWnd, false);
                    editorMode = true;
                    std::thread t1([&](){
                        DevTools dev(inst, hWnd);
                    });
                    t1.detach();
                    return 0;
                }
                break;
                case VK_ESCAPE:
                    CloseWindow(hWnd);
                    //PostQuitMessage(0);
                    return 0;
                default:

                break;
            }
        }
        break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}
