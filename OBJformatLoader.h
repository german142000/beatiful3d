#include <vector>

#include "vertexClass.h"
#include "normalsClass.h"

using namespace std;

///Не рекомендуется загружать большие файлы

class OBJloader{
    private:

        vector<string> parseString;
        vector<string> parseFaceString;

        vector<string> split(char* str, char delim);
        vector<string> split(const char* str, char delim);

        int vertexNumber = 0;
        int numberString = 0;

        bool load(string filePath);

        float* vertexProcArray;
        float* uvProcArray;
        float* normalProcArray;

        bool clearCharArray50(char* arr);

        bool getString(char* from, char* to, int number);

        int vertexProcArrayCounter = 0;
        int uvProcArrayCounter = 0;
        int normalProcArrayCounter = 0;

        Vertex* exitVertex;
        Normal* exitNormal;

        int exitVertexCounter = 0;

        bool exportVertexes = false;
        bool exportNormals = false;

    public:
        OBJloader(string fileName);
        int getVertexesNumber();
        vector<Vertex> getLoadedVertexes();
        vector<Normal> getLoadedNormals();
};
