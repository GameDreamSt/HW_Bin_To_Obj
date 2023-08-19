#pragma once

#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <string>

using namespace std;

string GoUpLevel(string path);
string RemoveQuotes(string path);
string GetExePath();
vector<string> GetFiles(string path, string ext);
void ShowBinFilesInCurrentDir();