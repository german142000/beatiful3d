#pragma once
#include <vector>
#include <string>
#include <stdint.h>

using namespace std;

class TextureLoader{
    private:
        string err;
    public:
        TextureLoader();
        vector<uint8_t> loadBMP_v3_RGB24(string filename, int *width, int *height);
        vector<uint8_t> loadPNG_v10_v11(string filename);
        vector<uint8_t> loadSGF_v1_RGB24(string filename);
        vector<uint8_t> resizeTextureRGB24(vector<uint8_t> texture, int newWidth, int newHeight, int width, int height);
        string getError();
};


/**<

�������� ������� SGF ��� �������� ����������� � RGB ��� RGBA ������ ������

83 71 70 1 0 0 0 0 2 0 0 0 2 24 0 255 255 255 255 255 255 255 255 255 255 255 255  <- ����� � ���������� �������������

������ ��� ����� - SGF � ascii
��������� - ������ ������� �����
����� - ������������ �� �����-����� (1 ��� 0)
������, �������, �������, �������,
�������, ������������, �����������, ����������� - ������ � ������ ����������� ��������������
������������� - ��� �� �������
����������� - ����� ������ (0 - ��� ������)
��������� ����� - ���� �����������

*/
