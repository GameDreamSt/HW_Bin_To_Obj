
#include <Conversion.h>

void ConvertFolder(string folderPath)
{
    int ret;

    string outputDirectory = GoUpLevel(folderPath) + "\\Extraction";
    if (!filesystem::exists(outputDirectory))
        ret = _mkdir(outputDirectory.c_str());

    string texturesDir = outputDirectory + "\\Textures";
    if (!filesystem::exists(texturesDir))
        ret = _mkdir(texturesDir.c_str());

    vector<string> binNFiles = GetFiles(folderPath, ".bin_n");
    for (int i = 0; i < binNFiles.size(); i++)
        ConvertFile(binNFiles[i], outputDirectory, true);

    vector<string> binBFiles = GetFiles(folderPath, ".bin_b");
    for (int i = 0; i < binBFiles.size(); i++)
        ConvertFile(binBFiles[i], outputDirectory, true);

    vector<string> binCFiles = GetFiles(folderPath, ".bin_c");
    for (int i = 0; i < binCFiles.size(); i++)
        ConvertFile(binCFiles[i], outputDirectory, true);
}

Vertex ReadVertex(vector<unsigned char>& allData, int index)
{
    return *(Vertex*)(&allData[index]);
}

float ReadFloat(vector<unsigned char>& allData, int index)
{
    return *(float*)&allData[index];
}

unsigned long ReadULong(vector<unsigned char>& allData, int index)
{
    return *(unsigned long*)&allData[index];
}

unsigned int ReadUInt(vector<unsigned char>& allData, int index)
{
    return *(unsigned int*)&allData[index];
}

int ReadInt(vector<unsigned char>& allData, int index)
{
    return *(int*)&allData[index];
}

void SeekUntilNull(vector<unsigned char> &bytes, int &cursor)
{
    int size = (int)bytes.size();
    while (cursor < size && bytes[cursor] != '\0')
        cursor++;

    cursor++;
}

bool ReadAllBytes(string filePath, vector<unsigned char>& data)
{
    ifstream ifs(filePath, ios::binary | ios::ate);

    if (!ifs.is_open())
        return false;

    ifstream::pos_type pos = ifs.tellg();

    if (pos == 0)
        return false;

    ifs.seekg(0, ios::beg);

    vector<char> tempData;
    tempData.resize((size_t)pos);

    ifs.read(&tempData[0], pos);

    std::copy(tempData.begin(), tempData.end(), std::back_inserter(data));

    ifs.close();

    return true;
}

void ConvertFile(string filePath, string outputFolder, bool useTextureFolder)
{
    try
    {
        if (!filesystem::exists(filePath))
        {
            printf("File does not exist!\n");
            return;
        }

        vector<unsigned char> bytes;
        if (!ReadAllBytes(filePath, bytes))
        {
            std::cout << "Error finding the file\n";
            return;
        }

        if (bytes[0] != 'B' || bytes[1] != 'S' || bytes[2] != '0' )//|| bytes[3] != '9')
        {
            printf("File header is incorrect!\n");
            return;
        }

        unsigned int i;
        unsigned int vertexSize = sizeof(Vertex); // 16
        unsigned int lightVertSize = sizeof(LightVert); // 48

        int cursor = 8;

        Object loadedObj;
        loadedObj.Load(bytes, cursor);

        vector<ObjObject> objects = loadedObj.ConvertToObj();

        string pathName = filesystem::path(filePath).filename().string();
        string outputFilepath;
        if (outputFolder != "")
            outputFilepath = outputFolder + "\\" + pathName + ".obj";
        else
            outputFilepath = filePath + "\\..\\" + pathName + ".obj";

        if (useTextureFolder)
            WriteObjFile(objects, outputFilepath, "Textures/");
        else
            WriteObjFile(objects, outputFilepath, "");

        std::cout << "Conversion complete! \n";
    }
    catch (...)
    {
        std::cout << "Failed to convert a file! \n" << filePath << "\n";
    }
}

void ConvertFile(string filePath)
{
    ConvertFile(filePath, "", false);
}

void SaveFile(ObjObject obj, string outputFile)
{
    ofstream write(outputFile, ofstream::out | ofstream::binary);

    if (!write.is_open())
    {
        std::cout << "Failed to open the write file!\n";
        return;
    }

    write << (int)(obj.name.length());
    write << obj.name;

    write << (unsigned long)(obj.verts.size());
    for (size_t i = 0; i < obj.verts.size(); i++)
    {
        Vertex vert;
        vert.x = obj.verts[i].x;
        vert.y = obj.verts[i].y;
        vert.z = obj.verts[i].z;
        vert.w = 0;

        write.write((char*)&vert, sizeof(vert));
    }

    write << (unsigned long)(obj.verts.size());
    write << (unsigned long)(obj.verts.size());
    write << (unsigned long)(obj.verts.size());
    for (size_t i = 0; i < obj.verts.size(); i++)
    {
        Vertex vert;
        vert.x = 0;
        vert.y = 1;
        vert.z = 0;
        vert.w = 0;
        write.write((char*)&vert, sizeof(vert));

        write << (unsigned long)(i);
    }

    write << (unsigned long)(0);
    write << (unsigned long)(0);

    write << (unsigned long)(BASE_OBJECT_SPECIFIC_DATA_COUNT);
    write << (float)(BASE_OBJECT_SPECIFIC_DATA_COUNT);

    write << (float)(0);

    Vertex vertMin, vertMax, StaticTMOffset, BoundingCentreOffset, BoundingBoxExtents, SmallBoundingBoxScale, RecipBoundingBoxExtents;
    write.write((char*)&vertMin, sizeof(Vertex));
    write.write((char*)&vertMax, sizeof(Vertex));
    write.write((char*)&StaticTMOffset, sizeof(Vertex));

    write.write((char*)&StaticTMOffset, sizeof(Vertex));
    write.write((char*)&BoundingBoxExtents, sizeof(Vertex));
    write.write((char*)&SmallBoundingBoxScale, sizeof(Vertex));
    write.write((char*)&RecipBoundingBoxExtents, sizeof(Vertex));

    write << (float)(0); //BoundingSphereRadius

    for (int i = 0; i < MAX_FACELISTS; i++)
    {
        int Zero = 0;
        int One = 1;

        /*if (ObjFaceList[i])
        {
            ASSERT(fwrite(&One, sizeof(int), 1, F));
        }
        else
        {
            ASSERT(fwrite(&Zero, sizeof(int), 1, F));
        }*/
    }

    for (int i = 0; i < MAX_FACELISTS; i++)
    {
        /*if (ObjFaceList[i])
        {
            ASSERT(ObjFaceList[i]->Save(F));
        }*/
    }

    write.close();
}