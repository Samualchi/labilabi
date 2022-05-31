#include <iostream>
#include <fstream>
#include <conio.h>
#include <Windows.h>
#include <vector>

//typedef struct CONSOLE_FONT_INFOEX {
//    ULONG cbSize;
//    DWORD nFont;
//    COORD dwFontSize;
//    UINT FontFamily;
//    UINT FontWeight;
//    WCHAR FaceName[LF_FACESIZE];
//} CONSOLE_FONT_INFOEX, *PCONSOLE_FONT_INFOEX;

using namespace std;

enum Color
{
    Blue,
    Green,
    Red,
    Yellow,
    White,
    NSimSun,
    Consolas
};

enum Key
{
    F1 = 59,
    F2 = 60,
    F3 = 61,
    F4 = 62,
    F5 = 63,
    F6 = 64,
    F7,
    F8,
    F9,
    Esc = 27,
    Backspace = 8,
    Enter = 13
};

void SetColor(WORD wAttributes);
void SetFont(WORD wAttributes);
void SetPosition(int x, int y);

struct Data
{
    char area[24][79];

    Color areaColor[24][24];
    COORD cursor;
    Color currentColor;
    int currentFont;

    void Init()
    {
        for (int y = 0; y < 24; y++)
        {
            for (int x = 0; x < 79; x++)
            {
                areaColor[y][x] = White;
                area[y][x] = ' ';
            }
        }

        cursor.X = 0;
        cursor.Y = 0;

        currentColor = White;
        
    }

    void Draw()
    {
        for (int y = 0; y < 24; y++)
        {
            for (int x = 0; x < 79; x++)
            {
                SetPosition(x, y);
                SetColor(areaColor[y][x]);
                cout << area[y][x];
            }
        }
    }
};

void ShowInfo();
void Save(bool& newFile, Data& data, char* fileName);
void SaveToFile(Data& data, char* fileName);
void SaveToFile(Data& data);

int main()
{
    setlocale(LC_ALL, "RUSSIAN");
    bool isNewFile;

    char key;
    char fileName[20];
    Data data;
    ifstream file;

    while (true)
    {
        cout << "Create new *.txt?(y/n):";
        cin >> key;
        if (key == 'y' || key == 'n')
            break;
    }

    if (key == 'n')
    {
        string str_buf,str;
        vector <string> vec;
        isNewFile = false;
        cout << "Enter file name:";
        cin >> fileName; 
        file.open(fileName, ios::binary);
        for (; file.eof();) { 
            file >> str_buf;
            str_buf[0] = toupper(str_buf[0]);
            vec.push_back(str_buf);
            str += str_buf;
        }
        cout << str;
        //file.read((char*)&data, sizeof(Data));
        
        file.close();
    }
    else if (key == 'y')
    {
        isNewFile = true;
        data.Init();
    }

    system("cls");

    while (true)
    {
        key = _getch();

        //-----------------------------------------------------------------
        //       F1 - F9
        //-----------------------------------------------------------------
        if (key == 0)
        {
            key = _getch();

            switch (key)
            {
            case F1:
                ShowInfo();
                system("cls");
                data.Draw();
                break;
                //------------------------------------------
                // Colors
                //------------------------------------------
            case F2: data.currentColor = Blue;  break;
            case F3: data.currentColor = Green;  break;
            case F4: data.currentColor = Yellow;  break;
            case F5: data.currentColor = Red;  break;
            case F6: data.currentFont = NSimSun;  break;
            case F7: data.currentFont = Consolas; break;
            case F8: data.currentColor = White;  break;
                //------------------------------------------
                // Saves
                //------------------------------------------
            case F9: system("cls");
                Save(isNewFile, data, fileName);
            default:
                break;
            }
        }
 
        else
        {
            switch (key)
            {
            case Esc:
                SetColor(White);
                SetPosition(24, 0);
                cout << "Save changes?(y/n):";
                cin >> key;

                if (key == 'y')
                {
                    system("cls");
                    Save(isNewFile, data, fileName);
                }
                else
                    return 0;
                break;
            case Backspace:
                data.cursor.X--;

                if (data.cursor.X < 0)
                {
                    data.cursor.X = 79;
                    data.cursor.Y--;
                }
                data.area[data.cursor.Y][data.cursor.X] = ' ';
                SetPosition(data.cursor.X, data.cursor.Y);
                cout << ' ';
                break;
            case Enter:
                data.cursor.Y++;
                data.cursor.X = 0;
                break;
            default:
                SetPosition(data.cursor.X, data.cursor.Y);
                SetColor(data.currentColor);
                SetFont(data.currentFont);
                data.cursor.X++;
                cout << key;
                break;
            }
        }

        //----------------------------------------
        // Update cursor
        //----------------------------------------
        if (data.cursor.X > 79)
        {
            data.cursor.X = 0;
            data.cursor.Y++;
        }
        if (data.cursor.X < 0)
        {
            data.cursor.X = 79;
            data.cursor.Y--;
        }

        if (data.cursor.Y > 24)
            data.cursor.Y = 24;
        if (data.cursor.Y < 0)
            data.cursor.Y = 0;

        data.area[data.cursor.Y][data.cursor.X] = key;
        data.areaColor[data.cursor.Y][data.cursor.X] = data.currentColor;
    }

    system("pause");
    return 0;
}

void ShowInfo()
{
    system("cls");
    SetColor(White);
    cout << "Reference. How change color/font and save\n"
        << "F2)Blue\n"
        << "F3)Green\n"
        << "F4)Aqua\n"
        << "F5)Red\n"
        << "F6)NSimSun font\n"
        << "F7)Consolas font\n"
        << "F8)White\n\n"
        << "F9)Save\n"
        << "Press any key to exit from menu\n";

    system("pause");
    system("cls");
}

void SetColor(WORD wAttributes)
{
    HANDLE hOUTPUT = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hOUTPUT, wAttributes);
}

void SetFont(WORD wAttributes)
{
    CONSOLE_FONT_INFOEX cfon;
    cfon.cbSize = sizeof(cfon);
    cfon.nFont = 0;
    cfon.dwFontSize.X = 0;
    cfon.dwFontSize.Y = 24;
    cfon.FontFamily = FF_DONTCARE;
    cfon.FontWeight = FW_NORMAL;
    wchar_t font = wAttributes;
    wcscpy(cfon.FaceName, L"%C\n"+font);
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE,&cfon);
}

void SetPosition(int x, int y)
{
    COORD dwCursorPosition;
    dwCursorPosition.X = x;
    dwCursorPosition.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), dwCursorPosition);
}

void SaveToFile(Data& data)
{
    char fileName[20];
    cout << "File name: ";
    cin >> fileName;
    ofstream file(fileName, ios::binary);
    file.write((char*)&data, sizeof(Data));
    file.close();
}

void SaveToFile(Data& data, char* fileName)
{
    ofstream file(fileName, ios::binary);
    file.write((char*)&data, sizeof(Data));
    file.close();
}

void Save(bool& newFile, Data& data, char* fileName)
{
    system("cls");
    if (newFile)
        SaveToFile(data);
    else
        SaveToFile(data, fileName);
}