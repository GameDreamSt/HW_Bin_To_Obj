
#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <string>

#include <Conversion.h>
#include <Helper.h>

using namespace std;

void BatchMode(int argc, char* argv[])
{
    string pathToConvert = argv[1];

    for (int i = 0; i < argc; i++)
    {
        if (!filesystem::exists(pathToConvert))
            continue;

        if (filesystem::is_directory(pathToConvert))
            ConvertFolder(pathToConvert);
        else
            ConvertFile(pathToConvert);
    }
}

void UserMode()
{
    system("cls");
    cout << "Welcome to the .bin_n .bin_c .bin_b _r.bin_n to .obj converter! Made by GameDreamSt v1.1\n";

    ShowBinFilesInCurrentDir();

    cout << "Enter either a single file or a project folder path to convert (you can drag and drop):\n";

    string localPathToConvert;
    cin >> localPathToConvert;

    string fullPath;

    if (localPathToConvert.length() > 1 && localPathToConvert[1] == ':') // Absolute path (because of the C:<---)
        fullPath = localPathToConvert;
    else // otherwise a local path
        fullPath = GetExePath() + localPathToConvert;

    if (filesystem::is_directory(fullPath))
        ConvertFolder(fullPath);
    else
    {
        if (filesystem::exists(fullPath))
            ConvertFile(fullPath);
        else
            cout << "Such file does not seem to exist...\n";
    }
}

int main(int argc, char* argv[])
{
    if (argc > 1)
    {
        BatchMode(argc, argv);
        return 0;
    }

    system("title Bin_To_OBJ_Converter");

    do
    {
        UserMode();

        cout << "Want to continue? [Y/N]: ";
        string answer;
        cin >> answer;

        if (answer == "N" || answer == "n") // Exit program
            return 0;

    } while (true);
}