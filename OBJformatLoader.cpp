#include <fstream>
#include <string>
#include <sstream>
#include <windows.h>
#include <iostream>
#include <thread>
#include <stdlib.h>
#include <malloc.h>
#include <string>

#include "OBJformatLoader.h"
#include "vertexClass.h"
#include "normalsClass.h"

using namespace std;

OBJloader::OBJloader(string fileName){
    int numLines = 0;
    ifstream in(fileName);
    string unused;
    while (getline(in, unused)) ++numLines;

    numberString = numLines;

    vertexProcArray = (float*) malloc(numLines * sizeof(float) * 6);
    uvProcArray = (float*) malloc(numLines * sizeof(float) * 4);
    normalProcArray = (float*) malloc(numLines * sizeof(float) * 6);
    exitVertex = (Vertex*) malloc(numLines * 2 * sizeof(Vertex));
    exitNormal = (Normal*) malloc(numLines * 2 * sizeof(Normal));

    if(vertexProcArray == NULL || uvProcArray == NULL || normalProcArray == NULL || exitVertex == NULL || exitNormal == NULL){
        free(vertexProcArray);
        free(uvProcArray);
        free(normalProcArray);
        free(exitVertex);
        free(exitNormal);
        MessageBox(NULL, "Unable to load the model - not enough memory", "OBJ Error", MB_OK | MB_ICONERROR);
        return;
    }

    OBJloader::load(fileName);
};

bool OBJloader::getString(char* from, char* to, int number){
    int num = 0;
    int numsch = 0;
    int tonum = 0;
    char chrs;
    while(num != number){
        chrs = from[numsch];
        if(chrs == '\n') num++;
        numsch++;
    }
    chrs = ' ';
    while(chrs != '\r' && from[numsch] != '\n' && tonum < 37){
        chrs = from[numsch];
        to[tonum] = chrs;
        numsch++;
        tonum++;
    }
    return true;
};

vector<string> OBJloader::split(char* str, char delim){
    string sstr = "";
    vector<string> strv;
    int vz = 0;
    for(int h = 0; h < 40; h++){
        if(str[h] == '\n') break;
        if(str[h] != delim) sstr += str[h];
        else {
            strv.push_back(sstr);
            sstr = "";
            vz++;
        }
    }
    if(sstr != "") strv.push_back(sstr);
    return strv;
};

vector<string> OBJloader::split(const char* str, char delim){
    string sstr = "";
    vector<string> strv;
    for(int h = 0; h < 40; h++){
        if(str[h] == '\n' || str[h] == '\0') break;
        if(str[h] != delim) sstr += str[h];
        else {
            strv.push_back(sstr);
            sstr = "";
        }
    }
    if(sstr != "") strv.push_back(sstr);
    return strv;
};

bool OBJloader::clearCharArray50(char* arr){
    for(int r = 0; r < 50; r++) arr[r] = '\0';
};

bool OBJloader::load(string filePath){
    vector<string> parseString(40);
    vector<string> parseFaceString(40);

    std::ifstream file;
    file.open(filePath, std::ios::binary);
    file.seekg(0, std::ios::end);
    int size = file.tellg();
    file.seekg(0, std::ios::beg);
    char* buff = new char[size];
    file.read((char*)buff, size);
    file.close();

    float vx = 0;
    float vy = 0;
    float vz = 0;
    float uvx = 0;
    float uvy = 0;
    float nx = 0;
    float ny = 0;
    float nz = 0;
    char* nextString = new char[50];

    for(int g = 0; g < numberString; g++){
        OBJloader::getString(buff, nextString, g);
        if(true){
            parseString = OBJloader::split(nextString, ' ');
            if(parseString[0] == "v"){
                vertexProcArray[vertexProcArrayCounter] = stof(parseString[1]);
                vertexProcArrayCounter++;
                vertexProcArray[vertexProcArrayCounter] = stof(parseString[2]);
                vertexProcArrayCounter++;
                vertexProcArray[vertexProcArrayCounter] = stof(parseString[3]);
                vertexProcArrayCounter++;
            }
            if(parseString[0] == "vn"){
                normalProcArray[normalProcArrayCounter] = stof(parseString[1]);
                normalProcArrayCounter++;
                normalProcArray[normalProcArrayCounter] = stof(parseString[2]);
                normalProcArrayCounter++;
                normalProcArray[normalProcArrayCounter] = stof(parseString[3]);
                normalProcArrayCounter++;
            }
            if(parseString[0] == "vt"){
                uvProcArray[uvProcArrayCounter] = stof(parseString[1]);
                uvProcArrayCounter++;
                uvProcArray[uvProcArrayCounter] = stof(parseString[2]);
                uvProcArrayCounter++;
            }
            if(parseString[0] == "f"){
                if(parseString.size() > 5){
                    MessageBox(NULL, "The engine supports only triangular polygons", "OBJ Error", MB_OK | MB_ICONERROR);
                    return false;
                } else {
                    parseFaceString = split(parseString[1].c_str(), '/');
                    if(parseFaceString.size() == 1){

                        ///Если в описании поверхностей есть данные только о вершинах

                        vx = vertexProcArray[(stoi(parseString[1]) - 1) * 3];
                        vy = vertexProcArray[(stoi(parseString[1]) - 1) * 3 + 1];
                        vz = vertexProcArray[(stoi(parseString[1]) - 1) * 3 + 2];

                        exitVertex[exitVertexCounter] = Vertex(vx, vy, vz, 0, 0, false);
                        exitNormal[exitVertexCounter] = Normal(0, 0, 0);
                        exitVertexCounter++;

                        vx = vertexProcArray[(stoi(parseString[2]) - 1) * 3];
                        vy = vertexProcArray[(stoi(parseString[2]) - 1) * 3 + 1];
                        vz = vertexProcArray[(stoi(parseString[2]) - 1) * 3 + 2];

                        exitVertex[exitVertexCounter] = Vertex(vx, vy, vz, 0, 0, false);
                        exitNormal[exitVertexCounter] = Normal(0, 0, 0);
                        exitVertexCounter++;

                        vx = vertexProcArray[(stoi(parseString[3]) - 1) * 3];
                        vy = vertexProcArray[(stoi(parseString[3]) - 1) * 3 + 1];
                        vz = vertexProcArray[(stoi(parseString[3]) - 1) * 3 + 2];

                        exitVertex[exitVertexCounter] = Vertex(vx, vy, vz, 0, 0, false);
                        exitNormal[exitVertexCounter] = Normal(0, 0, 0);
                        exitVertexCounter++;

                    } else if(parseFaceString.size() == 2){

                        ///Если в описании поверхностей есть данные только о вершинах и текстурных координатах

                        vx = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3];
                        vy = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 1];
                        vz = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 2];
                        uvx = uvProcArray[(stoi(parseFaceString[1]) - 1)];
                        uvy = uvProcArray[(stoi(parseFaceString[1]) - 1) + 1];

                        exitVertex[exitVertexCounter] = Vertex(vx, vy, vz, uvx, uvy, true);
                        exitNormal[exitVertexCounter] = Normal(0, 0, 0);
                        exitVertexCounter++;
                        parseFaceString.clear();

                        parseFaceString = split(parseString[2].c_str(), '/');
                        vx = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3];
                        vy = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 1];
                        vz = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 2];
                        uvx = uvProcArray[(stoi(parseFaceString[1]) - 1)];
                        uvy = uvProcArray[(stoi(parseFaceString[1]) - 1) + 1];

                        exitVertex[exitVertexCounter] = Vertex(vx, vy, vz, uvx, uvy, true);
                        exitNormal[exitVertexCounter] = Normal(0, 0, 0);
                        exitVertexCounter++;
                        parseFaceString.clear();

                        parseFaceString = split(parseString[3].c_str(), '/');
                        vx = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3];
                        vy = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 1];
                        vz = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 2];
                        uvx = uvProcArray[(stoi(parseFaceString[1]) - 1)];
                        uvy = uvProcArray[(stoi(parseFaceString[1]) - 1) + 1];

                        exitVertex[exitVertexCounter] = Vertex(vx, vy, vz, uvx, uvy, true);
                        exitNormal[exitVertexCounter] = Normal(0, 0, 0);
                        exitVertexCounter++;

                    } else if(parseFaceString.size() == 3 && parseFaceString[1] != ""){

                        ///Если в описании поверхностей есть данные о вершинах, текстурных координатах и нормалях

                        vx = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3];
                        vy = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 1];
                        vz = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 2];
                        uvx = uvProcArray[(stoi(parseFaceString[1]) - 1)];
                        uvy = uvProcArray[(stoi(parseFaceString[1]) - 1) + 1];
                        nx = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3];
                        ny = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3 + 1];
                        nz = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3 + 2];

                        exitVertex[exitVertexCounter] = Vertex(vx, vy, vz, uvx, uvy, true);
                        exitNormal[exitVertexCounter] = Normal(nx, ny, nz);
                        exitVertexCounter++;
                        parseFaceString.clear();


                        parseFaceString = split(parseString[2].c_str(), '/');
                        vx = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3];
                        vy = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 1];
                        vz = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 2];
                        uvx = uvProcArray[(stoi(parseFaceString[1]) - 1)];
                        uvy = uvProcArray[(stoi(parseFaceString[1]) - 1) + 1];
                        nx = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3];
                        ny = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3 + 1];
                        nz = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3 + 2];

                        exitVertex[exitVertexCounter] = Vertex(vx, vy, vz, uvx, uvy, true);
                        exitNormal[exitVertexCounter] = Normal(nx, ny, nz);
                        exitVertexCounter++;
                        parseFaceString.clear();

                        parseFaceString = split(parseString[3].c_str(), '/');
                        vx = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3];
                        vy = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 1];
                        vz = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 2];
                        uvx = uvProcArray[(stoi(parseFaceString[1]) - 1)];
                        uvy = uvProcArray[(stoi(parseFaceString[1]) - 1) + 1];
                        nx = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3];
                        ny = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3 + 1];
                        nz = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3 + 2];

                        exitVertex[exitVertexCounter] = Vertex(vx, vy, vz, uvx, uvy, true);
                        exitNormal[exitVertexCounter] = Normal(nx, ny, nz);
                        exitVertexCounter++;

                    } else if(parseFaceString.size() == 3 && parseFaceString[1] == ""){

                        ///Если в описании поверхностей есть данные о вершинах и нормалях

                        vx = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3];
                        vy = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 1];
                        vz = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 2];

                        nx = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3];
                        ny = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3 + 1];
                        nz = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3 + 2];

                        exitVertex[exitVertexCounter] = Vertex(vx, vy, vz, 0, 0, false);
                        exitNormal[exitVertexCounter] = Normal(nx, ny, nz);
                        exitVertexCounter++;

                        parseFaceString.clear();

                        //cout << nx << " " << ny << " " << nz << " " << endl;

                        parseFaceString = split(parseString[2].c_str(), '/');
                        vx = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3];
                        vy = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 1];
                        vz = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 2];

                        nx = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3];
                        ny = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3 + 1];
                        nz = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3 + 2];

                        //cout << nx << " " << ny << " " << nz << " " << endl;

                        exitVertex[exitVertexCounter] = Vertex(vx, vy, vz, 0, 0, false);
                        exitNormal[exitVertexCounter] = Normal(nx, ny, nz);
                        exitVertexCounter++;

                        parseFaceString.clear();

                        parseFaceString = split(parseString[3].c_str(), '/');
                        vx = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3];
                        vy = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 1];
                        vz = vertexProcArray[(stoi(parseFaceString[0]) - 1) * 3 + 2];

                        nx = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3];
                        ny = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3 + 1];
                        nz = normalProcArray[(stoi(parseFaceString[2]) - 1) * 3 + 2];

                        //cout << nx << " " << ny << " " << nz << " " << endl;

                        exitVertex[exitVertexCounter] = Vertex(vx, vy, vz, 0, 0, false);
                        exitNormal[exitVertexCounter] = Normal(nx, ny, nz);
                        exitVertexCounter++;

                    }
                }
                parseFaceString.clear();
            }
        }
        OBJloader::clearCharArray50(nextString);
    }
    vertexNumber = exitVertexCounter;
    free(vertexProcArray);
    free(uvProcArray);
    free(normalProcArray);
};

int OBJloader::getVertexesNumber(){
    return vertexNumber;
};

vector<Vertex> OBJloader::getLoadedVertexes(){
    vector<Vertex> vert;
    if(!exportVertexes){
        for(int i = 0; i < vertexNumber; i++) vert.push_back(exitVertex[i]);
        free(exitVertex);
        exportVertexes = true;
        return vert;
    } else return vert;
};

vector<Normal> OBJloader::getLoadedNormals(){
    vector<Normal> norm;
    if(!exportNormals){
        for(int i = 0; i < vertexNumber; i++) norm.push_back(exitNormal[i]);
        free(exitNormal);
        exportNormals = true;
        return norm;
    } else return norm;
};
