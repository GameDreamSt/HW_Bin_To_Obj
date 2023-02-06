
#include <Helper.h>

string exePathCache = "";
string GetExePath()
{
    if (exePathCache != "")
        return exePathCache;

    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    string myPath = string(buffer);

    size_t it = myPath.find_last_of("\\");
    exePathCache = myPath.substr(0, it);

    return exePathCache;
}

vector<string> GetFiles(string path, string ext)
{
    vector<string> paths;

    for (auto& p : filesystem::recursive_directory_iterator(path))
        if (p.path().extension().string() == ext)
            paths.push_back(p.path().string());

    return paths;
}

void ShowBinFilesInCurrentDir()
{
    vector<string> files;
    vector<string> bin = GetFiles(GetExePath(), ".bin_n");
    files.insert(files.end(), bin.begin(), bin.end());
    bin = GetFiles(GetExePath(), ".bin_c");
    files.insert(files.end(), bin.begin(), bin.end());
    bin = GetFiles(GetExePath(), ".bin_b");
    files.insert(files.end(), bin.begin(), bin.end());

    string output = "\nFound .bin files in .exe dir:\n";
    for (int i = 0; i < files.size(); i++)
    {
        filesystem::path tempPath = filesystem::path(files[i]);
        output += tempPath.filename().string() + '\n';
    }

    if (files.size() == 0)
        return;

    cout << output << '\n';
}