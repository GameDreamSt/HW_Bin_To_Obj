#pragma once

#include <filesystem>
#include <windows.h>
#include <string>
#include <direct.h>
#include <iostream>
#include <fstream>

#include <ObjWriter.h>
#include <Helper.h>

using namespace std;

const float modelScaleDown = 0.0225f;

void ConvertFolder(string folderPath);
void ConvertFile(string filePath);
void ConvertFile(string filePath, string outputFolder, bool useTextureFolder);