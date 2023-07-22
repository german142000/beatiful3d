#include <windows.h>
#include <CommCtrl.h>
#include <thread>
#include <iostream>
#include <string>
#include <sstream>

#include "devToolsClass.h"

#define ID_LIST 501


using namespace std;

HWND buttonA;
HWND buttonB;
HWND buttonC;
HWND buttonD;
HWND buttonE;
HWND buttonF;
HWND buttonG;
HWND buttonH;
HWND buttonJ;
HWND buttonK;
HWND TextA;
HWND TextB;
HWND TextC;
HWND TextD;
HWND TextE;
HWND TextF;
HWND TextG;
HWND TextH;
HWND TextJ;
HWND ObjectListBox;
HWND ObjectTypeComboBox;

HWND yptext;
HWND zptext;

HWND xEdit;
HWND yEdit;
HWND zEdit;

HWND scaleText;

HWND sxptext;
HWND syptext;
HWND szptext;

HWND sxEdit;
HWND syEdit;
HWND szEdit;

HWND rotateText;

HWND rxptext;
HWND ryptext;
HWND rzptext;

HWND rxEdit;
HWND ryEdit;
HWND rzEdit;

HWND gameWindow;

HWND materialButton;
HWND physicalModelButton;
HWND scriptsButton;
HWND loadModelButton;

bool devToolsOpened = false;
bool devToolsClose = false;

HWND createSceneButton;

HWND sceneNameEditText;
HWND createSceneText;
HWND sceneOkButton;
HWND sceneCancelButton;

HWND meshNameEditText;
HWND createMeshText;
HWND meshOkButton;
HWND meshSelectTypeList;
HWND meshCancelButton;

int selectMeshID;

string floatToString(float N){
    ostringstream ss("");
    ss << N;
    return ss.str();
}

vector<string> split(char* str, char delim){
    string sstr = "";
    vector<string> strv;
    int vz = 0;
    for(int h = 0; h < 1024; h++){
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

bool EnableAllProperties(bool enable){

    EnableWindow(ObjectTypeComboBox, enable);

    EnableWindow(xEdit, enable);
    EnableWindow(yEdit, enable);
    EnableWindow(zEdit, enable);

    EnableWindow(sxEdit, enable);
    EnableWindow(syEdit, enable);
    EnableWindow(szEdit, enable);

    EnableWindow(rxEdit, enable);
    EnableWindow(ryEdit, enable);
    EnableWindow(rzEdit, enable);

    EnableWindow(materialButton, enable);
    EnableWindow(physicalModelButton, enable);
    EnableWindow(scriptsButton, enable);
    EnableWindow(loadModelButton, enable);

    return true;
}

int wmId, wmEvent;
LRESULT CALLBACK ToolWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE:
            //DestroyWindow(hWnd);
            //CloseWindow(hWnd);
            //PostQuitMessage(0);
            break;
        case WM_DESTROY:
            return 0;
        case WM_KEYDOWN:{
            switch(wParam){
                case VK_ESCAPE:
                    ShowWindow(hWnd, SW_HIDE);
                return 0;
            }
        }
        break;
        case WM_COMMAND:{
            if(lParam == (LPARAM)buttonA) {
                SetWindowText(TextC, "Move");
                return 0;
            }
            if(lParam == (LPARAM)buttonF) {
                SetWindowText(TextC, "Close engine");
                if(!mainThreadReadWrite){
                    devToolsThreadReadWrite = true;
                    exitEngine = true;
                    devToolsThreadReadWrite = false;
                }
                return 0;
            }
            if(lParam == (LPARAM)buttonG) {
                SetWindowText(TextC, "Create new object");
                ShowWindow(meshNameEditText, SW_SHOW);
                ShowWindow(createMeshText, SW_SHOW);
                ShowWindow(meshSelectTypeList, SW_SHOW);
                ShowWindow(meshOkButton, SW_SHOW);
                ShowWindow(meshCancelButton, SW_SHOW);
                SetFocus(meshNameEditText);
                return 0;
            }
            if(lParam == (LPARAM)buttonH) {
                SetWindowText(TextC, "Edit initial loading screen");
                return 0;
            }
            if(lParam == (LPARAM)buttonJ) {
                TCHAR szFileName[MAX_PATH];
                GetModuleFileName(0, szFileName, MAX_PATH);
                WinExec(szFileName, 0);
                if(!mainThreadReadWrite){
                    devToolsThreadReadWrite = true;
                    exitEngine = true;
                    devToolsThreadReadWrite = false;
                }
                return 0;
            }
            if(lParam == (LPARAM)createSceneButton) {
                ShowWindow(sceneNameEditText, SW_SHOW);
                ShowWindow(createSceneText, SW_SHOW);
                ShowWindow(sceneOkButton, SW_SHOW);
                ShowWindow(sceneCancelButton, SW_SHOW);
                SetFocus(sceneNameEditText);
            }
            if(lParam == (LPARAM)sceneCancelButton) {
                ShowWindow(sceneNameEditText, SW_HIDE);
                ShowWindow(createSceneText, SW_HIDE);
                ShowWindow(sceneOkButton, SW_HIDE);
                ShowWindow(sceneCancelButton, SW_HIDE);
            }
            if(lParam == (LPARAM)meshCancelButton) {
                ShowWindow(meshNameEditText, SW_HIDE);
                ShowWindow(createMeshText, SW_HIDE);
                ShowWindow(meshSelectTypeList, SW_HIDE);
                ShowWindow(meshOkButton, SW_HIDE);
                ShowWindow(meshCancelButton, SW_HIDE);
            }
            if(lParam == (LPARAM)sceneOkButton) {
                TCHAR buff[1024];
                GetWindowText(sceneNameEditText, buff, 1024);
                string scnm(buff);
                if(scnm == "" || scnm == "Enter name"){
                    MessageBox(NULL, "Enter the scene name", "Error!", MB_ICONEXCLAMATION | MB_OK);
                } else {
                    ShowWindow(sceneNameEditText, SW_HIDE);
                    ShowWindow(createSceneText, SW_HIDE);
                    ShowWindow(sceneOkButton, SW_HIDE);
                    ShowWindow(sceneCancelButton, SW_HIDE);
                    if(!mainThreadReadWrite){
                        devToolsThreadReadWrite = true;
                        sceneName = scnm;
                        devToolsThreadReadWrite = false;
                    }
                    SetWindowText(TextB, ("Scene: " + scnm).c_str());
                }
            }
            if(lParam == (LPARAM)meshOkButton) {
                TCHAR buff[1024];
                GetWindowText(meshNameEditText, buff, 1024);
                string scnm(buff);
                if(scnm == "" || scnm == "Enter name"){
                    MessageBox(NULL, "Enter the object name", "Error!", MB_ICONEXCLAMATION | MB_OK);
                } else {
                    TCHAR buff2[1024];
                    GetWindowText(TextB, buff2, 1024);
                    string scnm2(buff2);
                    if(scnm2 == "Scene: no scene"){
                        MessageBox(NULL, "Перед добавлением объекта неободимо создать сцену", "Error!", MB_ICONEXCLAMATION | MB_OK);
                    } else {
                        TCHAR buff3[1024];
                        GetWindowText(meshSelectTypeList, buff3, 1024);
                        string scnm3(buff3);
                        ShowWindow(meshNameEditText, SW_HIDE);
                        ShowWindow(createMeshText, SW_HIDE);
                        ShowWindow(meshSelectTypeList, SW_HIDE);
                        ShowWindow(meshOkButton, SW_HIDE);
                        ShowWindow(meshCancelButton, SW_HIDE);
                        if(!mainThreadReadWrite){
                            devToolsThreadReadWrite = true;
                            objectName = scnm;
                            objectType = scnm3;
                            devToolsThreadReadWrite = false;
                        }
                    }

                    //if(!mainThreadReadWrite){
                    //    devToolsThreadReadWrite = true;
                    //    sceneName = scnm;
                    //    devToolsThreadReadWrite = false;
                    //}
                    //SetWindowText(TextB, ("Scene: " + scnm).c_str());
                }
            }
            if(lParam == (LPARAM)loadModelButton) {
                OPENFILENAME ofn={0};
                char szDirect[260];
                char szFileName[260];
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = NULL;
                ofn.lpstrFile = szDirect;
                *(ofn.lpstrFile) = 0;
                ofn.nMaxFile =sizeof(szDirect);
                ofn.lpstrFilter = "(*.obj) Wavefront object\0*.obj\0";
                ofn.nFilterIndex = 0;
                ofn.lpstrFileTitle = szFileName;
                *(ofn.lpstrFileTitle) = 0;
                ofn.nMaxFileTitle = sizeof(szFileName);
                ofn.lpstrInitialDir = NULL;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                if(GetOpenFileName(&ofn)){
                    devToolsThreadReadWrite = true;
                    meshId = selectMeshID;
                    openModelFileName = string(szDirect);
                    cout << openModelFileName << endl;
                    devToolsThreadReadWrite = false;
                };
            }

            wmId    = LOWORD(wParam);
            wmEvent = HIWORD(wParam);

            switch (wmId){
                case ID_LIST:
                if (wmEvent == LBN_SELCHANGE){
                    char* name = new char[1024];
                    int number = SendMessage(ObjectListBox, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                    SendMessage(ObjectListBox, LB_GETTEXT,(WPARAM)number, (LPARAM)name);
                    vector<string> meshName = split(name, '-');
                    if(meshName[0] == "Mesh"){
                        meshId = stoi(meshName[1]);
                        selectMeshID = meshId;
                        getMeshInfo = true;
                    }
                }
            }
            return 0;
        }
        break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
};

DevTools::DevTools(HINSTANCE hInstance, HWND mainWindow){

    gameWindow = mainWindow;

    const char* className = "DevToolsClass";
    const char* windowTitle = "DevTools";
    const int screenWidth = 2000;
    const int screenHeight = 2000;

    if(!devToolsOpened) {
        WNDCLASS wc = {};
        wc.lpfnWndProc = ToolWndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = className;
        wc.hbrBackground = CreateSolidBrush(RGB(0,255,255));//CreateSolidBrush(RGB(238,238,238));
        //wc.style=CS_HREDRAW | CS_VREDRAW;

        if (!RegisterClass(&wc)) {
            MessageBox(NULL, "DevTool Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        }
    }

    devToolsOpened = true;

    HWND hWnd = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        className,
        windowTitle,
        WS_VISIBLE | WS_POPUP,
        0,
        0,
        screenWidth,
        screenHeight,
        NULL,
        NULL,
        hInstance,
        NULL
    );


    if (hWnd == NULL) {
        MessageBox(NULL, "DevTool Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    }

    SetLayeredWindowAttributes(hWnd, RGB(0,255,255), 0, LWA_COLORKEY);

    buttonA = CreateWindow(
        "BUTTON",  // Predefined class; Unicode assumed
        "Move",      // Button text
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles
        0,         // x position
        0,         // y position
        80,        // Button width
        40,        // Button height
        hWnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    buttonB = CreateWindow(
        "BUTTON",  // Predefined class; Unicode assumed
        "Scale",      // Button text
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles
        80,         // x position
        0,         // y position
        80,        // Button width
        40,        // Button height
        hWnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    buttonC = CreateWindow(
        "BUTTON",  // Predefined class; Unicode assumed
        "Rotate",      // Button text
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles
        160,         // x position
        0,         // y position
        80,        // Button width
        40,        // Button height
        hWnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    buttonD = CreateWindow(
        "BUTTON",  // Predefined class; Unicode assumed
        "Save scene",      // Button text
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles
        240,         // x position
        0,         // y position
        120,        // Button width
        40,        // Button height
        hWnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    buttonE = CreateWindow(
        "BUTTON",  // Predefined class; Unicode assumed
        "Open scene",      // Button text
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles
        360,         // x position
        0,         // y position
        120,        // Button width
        40,        // Button height
        hWnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    buttonF = CreateWindow(
        "BUTTON",  // Predefined class; Unicode assumed
        "Close engine",      // Button text
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles
        480,         // x position
        0,         // y position
        120,        // Button width
        40,        // Button height
        hWnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    buttonG = CreateWindow(
        "BUTTON",  // Predefined class; Unicode assumed
        "Create new object",      // Button text
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles
        600,         // x position
        0,         // y position
        160,        // Button width
        40,        // Button height
        hWnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    buttonH = CreateWindow(
        "BUTTON",  // Predefined class; Unicode assumed
        "Edit initial loading screen",      // Button text
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles
        760,         // x position
        0,         // y position
        210,        // Button width
        40,        // Button height
        hWnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    buttonJ = CreateWindow(
        "BUTTON",  // Predefined class; Unicode assumed
        "Close DevTools and start game",      // Button text
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles
        970,         // x position
        0,         // y position
        260,        // Button width
        40,        // Button height
        hWnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    buttonK = CreateWindow(
        "BUTTON",  // Predefined class; Unicode assumed
        "Create new material",      // Button text
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles
        1230,         // x position
        0,         // y position
        180,        // Button width
        40,        // Button height
        hWnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    createSceneButton = CreateWindow(
        "BUTTON",  // Predefined class; Unicode assumed
        "Create new scene",      // Button text
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles
        1410,         // x position
        0,         // y position
        200,        // Button width
        40,        // Button height
        hWnd,     // Parent window
        NULL,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    TextA = CreateWindow(
        "static",
        "Editor mode",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10,
        50,
        120,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    TextB = CreateWindow(
        "static",
        "Scene: no scene",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        140,
        50,
        240,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    TextC = CreateWindow(
        "static",
        "Status bar",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        140,
        80,
        240,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    TextD = CreateWindow(
        "static",
        "fps",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10,
        80,
        120,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    TextE = CreateWindow(
        "static",
        "Object List",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10,
        120,
        250,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    ObjectListBox = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"LISTBOX",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | LBS_NOTIFY,
        10,
        150,
        250,
        500,
        hWnd,
        HMENU(ID_LIST),
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    //SendMessageA(ObjectListBox,LB_ADDSTRING,(WPARAM)0,(LPARAM)"test");
    //SendMessageA(ObjectListBox,LB_ADDSTRING,(WPARAM)0,(LPARAM)"test2");

    TextF = CreateWindow(
        "static",
        "Properties",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10,
        650,
        250,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    TextG = CreateWindow(
        "static",
        "Type",
        WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_CENTER,
        10,
        680,
        80,
        28,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    ObjectTypeComboBox = CreateWindow(
        "ComboBox",
        "Type",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_HASSTRINGS | WS_OVERLAPPED,
        100,
        680,
        160,
        200,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    TextH = CreateWindow(
        "static",
        "Position",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10,
        720,
        250,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    TextJ = CreateWindow(
        "static",
        "X",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10,
        750,
        80,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    xEdit = CreateWindow(
        "edit",
        "0",
        WS_CHILD | WS_VISIBLE | SS_CENTER | ES_NUMBER,
        100,
        750,
        160,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    yptext = CreateWindow(
        "static",
        "Y",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10,
        780,
        80,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    yEdit = CreateWindow(
        "edit",
        "0",
        WS_CHILD | WS_VISIBLE | SS_CENTER | ES_NUMBER,
        100,
        780,
        160,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    zptext = CreateWindow(
        "static",
        "Z",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10,
        810,
        80,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    zEdit = CreateWindow(
        "edit",
        "0",
        WS_CHILD | WS_VISIBLE | SS_CENTER | ES_NUMBER,
        100,
        810,
        160,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    scaleText = CreateWindow(
        "static",
        "Scale",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10,
        840,
        250,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    sxptext = CreateWindow(
        "static",
        "X",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10,
        870,
        80,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    sxEdit = CreateWindow(
        "edit",
        "1",
        WS_CHILD | WS_VISIBLE | SS_CENTER | ES_NUMBER,
        100,
        870,
        160,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    syptext = CreateWindow(
        "static",
        "Y",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10,
        900,
        80,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    syEdit = CreateWindow(
        "edit",
        "1",
        WS_CHILD | WS_VISIBLE | SS_CENTER | ES_NUMBER,
        100,
        900,
        160,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    szptext = CreateWindow(
        "static",
        "Z",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10,
        930,
        80,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    szEdit = CreateWindow(
        "edit",
        "1",
        WS_CHILD | WS_VISIBLE | SS_CENTER | ES_NUMBER,
        100,
        930,
        160,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    rotateText = CreateWindow(
        "static",
        "Rotation",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10,
        960,
        250,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    rxptext = CreateWindow(
        "static",
        "X",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10,
        990,
        80,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    rxEdit = CreateWindow(
        "edit",
        "0",
        WS_CHILD | WS_VISIBLE | SS_CENTER | ES_NUMBER,
        100,
        990,
        160,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    ryptext = CreateWindow(
        "static",
        "Y",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10,
        1020,
        80,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    ryEdit = CreateWindow(
        "edit",
        "0",
        WS_CHILD | WS_VISIBLE | SS_CENTER | ES_NUMBER,
        100,
        1020,
        160,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    rzptext = CreateWindow(
        "static",
        "Z",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10,
        1050,
        80,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    rzEdit = CreateWindow(
        "edit",
        "0",
        WS_CHILD | WS_VISIBLE | SS_CENTER | ES_NUMBER,
        100,
        1050,
        160,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    materialButton = CreateWindow(
        "BUTTON",
        "Material",
        WS_CHILD | WS_VISIBLE | SS_CENTER | ES_NUMBER,
        10,
        1080,
        250,
        40,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    physicalModelButton = CreateWindow(
        "BUTTON",
        "Physical model",
        WS_CHILD | WS_VISIBLE | SS_CENTER | ES_NUMBER,
        10,
        1130,
        250,
        40,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    scriptsButton = CreateWindow(
        "BUTTON",
        "Scripts",
        WS_CHILD | WS_VISIBLE | SS_CENTER | ES_NUMBER,
        10,
        1180,
        250,
        40,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    loadModelButton = CreateWindow(
        "BUTTON",
        "Load model",
        WS_CHILD | WS_VISIBLE | SS_CENTER | ES_NUMBER,
        10,
        1230,
        250,
        40,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    createSceneText = CreateWindow(
        "static",
        "Create new scene",
        WS_CHILD | SS_CENTER,
        1200,
        50,
        300,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    sceneNameEditText = CreateWindow(
        "edit",
        "Enter name",
        WS_CHILD | SS_CENTER,
        1200,
        80,
        300,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    sceneOkButton = CreateWindow(
        "Button",
        "Create",
        WS_CHILD | SS_CENTER,
        1200,
        110,
        145,
        30,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    sceneCancelButton = CreateWindow(
        "Button",
        "Cancel",
        WS_CHILD | SS_CENTER,
        1355,
        110,
        145,
        30,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    createMeshText = CreateWindow(
        "static",
        "Create new object",
        WS_CHILD | SS_CENTER,
        600,
        50,
        300,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    meshNameEditText = CreateWindow(
        "edit",
        "Enter name",
        WS_CHILD | SS_CENTER,
        600,
        80,
        300,
        20,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    meshSelectTypeList =  CreateWindow(
        "ComboBox",
        "Type",
        WS_CHILD | CBS_DROPDOWN | CBS_HASSTRINGS | WS_OVERLAPPED,
        600,
        110,
        300,
        500,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    meshOkButton = CreateWindow(
        "Button",
        "Create",
        WS_CHILD | SS_CENTER,
        600,
        150,
        145,
        30,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    meshCancelButton = CreateWindow(
        "Button",
        "Cancel",
        WS_CHILD | SS_CENTER,
        755,
        150,
        145,
        30,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    /** Типы объектов
     * Static object - статичный объект, который может обладать только физикой столкновений
     * Moving object - движущийся объект, может обладать разными физическими моделями
     * Point light - лампа, испускающая свет в разные стороны
     * Direction light - лампа, испускающая свет в определенном направлении.
     * Spotlight - прожектор, имеет направление и диаметр, в границах которого могут быть выпущены лучи
     */

    SendMessageA(ObjectTypeComboBox,CB_ADDSTRING,(WPARAM)0,(LPARAM)"Static object");
    SendMessageA(ObjectTypeComboBox,CB_ADDSTRING,(WPARAM)0,(LPARAM)"Moving object");
    SendMessageA(ObjectTypeComboBox,CB_ADDSTRING,(WPARAM)0,(LPARAM)"Point light");
    SendMessageA(ObjectTypeComboBox,CB_ADDSTRING,(WPARAM)0,(LPARAM)"Direction light");
    SendMessageA(ObjectTypeComboBox,CB_ADDSTRING,(WPARAM)0,(LPARAM)"Spotlight");

    SendMessageA(meshSelectTypeList,CB_ADDSTRING,(WPARAM)0,(LPARAM)"Static object");
    SendMessageA(meshSelectTypeList,CB_ADDSTRING,(WPARAM)0,(LPARAM)"Moving object");
    SendMessageA(meshSelectTypeList,CB_ADDSTRING,(WPARAM)0,(LPARAM)"Point light");
    SendMessageA(meshSelectTypeList,CB_ADDSTRING,(WPARAM)0,(LPARAM)"Direction light");
    SendMessageA(meshSelectTypeList,CB_ADDSTRING,(WPARAM)0,(LPARAM)"Spotlight");
    SendMessageA(meshSelectTypeList,CB_SELECTSTRING,(WPARAM)0,(LPARAM)"Static object");

    EnableAllProperties(false);

    MSG msg = {};
    while (!devToolsClose && GetMessage(&msg, NULL, 0, 0)) {
        if(!mainThreadReadWrite){
            devToolsThreadReadWrite = true;
            if(objectsNamesReturns){
                SendMessageA(ObjectListBox, LB_RESETCONTENT,(WPARAM)0,(LPARAM)0);
                for(int i = 0; i < objects.size(); i++){
                    SendMessageA(ObjectListBox, LB_ADDSTRING,(WPARAM)0,(LPARAM)objects[i].c_str());
                    //cout << "add " + name << endl;
                }
                objects.clear();
                objectsNamesReturns = false;
            }

            if(meshInfoRetunred){
                EnableAllProperties(true);

                if(mi->type) SendMessageA(ObjectTypeComboBox, CB_SELECTSTRING,(WPARAM)0,(LPARAM)"Moving object");
                else SendMessageA(ObjectTypeComboBox, CB_SELECTSTRING,(WPARAM)0,(LPARAM)"Static object");

                SendMessage(xEdit, WM_SETTEXT,(WPARAM)0,(LPARAM)floatToString(mi->x).c_str());
                SendMessage(yEdit, WM_SETTEXT,(WPARAM)0,(LPARAM)floatToString(mi->y).c_str());
                SendMessage(zEdit, WM_SETTEXT,(WPARAM)0,(LPARAM)floatToString(mi->z).c_str());

                SendMessage(sxEdit, WM_SETTEXT,(WPARAM)0,(LPARAM)floatToString(mi->sx).c_str());
                SendMessage(syEdit, WM_SETTEXT,(WPARAM)0,(LPARAM)floatToString(mi->sy).c_str());
                SendMessage(szEdit, WM_SETTEXT,(WPARAM)0,(LPARAM)floatToString(mi->sz).c_str());

                SendMessage(rxEdit, WM_SETTEXT,(WPARAM)0,(LPARAM)floatToString(mi->rx).c_str());
                SendMessage(ryEdit, WM_SETTEXT,(WPARAM)0,(LPARAM)floatToString(mi->ry).c_str());
                SendMessage(rzEdit, WM_SETTEXT,(WPARAM)0,(LPARAM)floatToString(mi->rz).c_str());

                meshInfoRetunred = false;
            }

            devToolsThreadReadWrite = false;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
};
