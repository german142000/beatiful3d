#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdint.h>
#include "avir.h"
#include "textureLoaderClass.h"

using namespace std;

std::string int_to_hex(int i){
    std::ostringstream ss;
    ss << std::hex << i;
    return ss.str();
};

int hexString_to_int(string hexs){
    int hexNumber;
    sscanf(hexs.c_str(), "%x", &hexNumber);
    return hexNumber;
};

std::string intToString(int N)
{
    ostringstream ss("");
    ss << N;
    return ss.str();
}

TextureLoader::TextureLoader(){
    err = "Ошибок нет";
};

vector<uint8_t> TextureLoader::loadBMP_v3_RGB24(string filename, int *width, int *height){
    vector<uint8_t> texture;
    std::ifstream file;
    file.open(filename, std::ios::binary);
    file.seekg(0, std::ios::end);
    int size = file.tellg();
    file.seekg(0, std::ios::beg);
    unsigned char* buff = new unsigned char[size];
    file.read((char*)buff, size);
    file.close();
    if(buff[0] == 66 && buff[1] == 77 && buff[14] == 40){
        string w = "";
        string h = "";
        int iw = 0;
        int ih = 0;
        w = int_to_hex(buff[18]) + int_to_hex(buff[19]) + int_to_hex(buff[20]) + int_to_hex(buff[21]);
        iw = hexString_to_int(w);
        h += int_to_hex(buff[22]) + int_to_hex(buff[23]) + int_to_hex(buff[24]) + int_to_hex(buff[25]);
        ih = hexString_to_int(h);
        *width = iw;
        *height = ih;
        if(buff[28] != 24) {
            delete[] buff;
            err = "Ошибка загрузки BMP файла - данный формат не поддерживается";
            return texture;
        } else {
            int headerOffset = hexString_to_int(int_to_hex(buff[13]) + int_to_hex(buff[12]) + int_to_hex(buff[11]) + int_to_hex(buff[10]));
            int offset = 0;
            for(int i = 0; i < ih; i++){
                for(int r = 0; r < iw; r++){
                    offset = headerOffset + (iw * i) * 3 + r * 3;
                    texture.push_back(buff[offset]);
                    texture.push_back(buff[offset + 1]);
                    texture.push_back(buff[offset + 2]);
                }
            }
            delete[] buff;
            return texture;
        }
    } else {
        delete[] buff;
        err = "Ошибка загрузки BMP файла - данный формат не поддерживается";
        return texture;
    }
};

vector<uint8_t> TextureLoader::loadPNG_v10_v11(string filename){

};

vector<uint8_t> TextureLoader::loadSGF_v1_RGB24(string filename){

/**<

Описание формата SGF для хранения изображения в RGB или RGBA первой версии

83 71 70 1 0 0 0 0 2 0 0 0 2 24 0 255 255 255 255 255 255 255 255 255 255 255 255  <- байты в десятичном представлении

первые три байта - SGF в ascii
четвертый - версия формата файла
пятый - используется ли альфа-канал (1 или 0)
шестой, седьмой, восьмой, девятый,
десятый, одиннадцатый, двенадцатый, тринадцатый - ширина и высота изображения соответственно
четырнадцатый - бит на пиксель
пятнадцатый - метод сжатия (0 - без сжатия)
остальные байты - само изображение

*/


};

vector<uint8_t> TextureLoader::resizeTextureRGB24(vector<uint8_t> texture, int newWidth, int newHeight, int oldWidth, int oldHeight){
    uint8_t* t = &texture[0];
    uint8_t newTextureArray[newWidth * newHeight * 3];
    avir::CImageResizer<> textureResizer;
    textureResizer.resizeImage(t, oldWidth, oldHeight, 0, newTextureArray, newWidth, newHeight, 3, 0, NULL);
    vector<uint8_t> newTexture;
    newTexture.assign(newTextureArray, newTextureArray + newWidth * newHeight * 3);
    return newTexture;
};

string TextureLoader::getError(){
    return err;
};
