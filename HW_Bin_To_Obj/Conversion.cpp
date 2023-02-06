
#include <Conversion.h>

void ConvertFolder(string folderPath)
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);

    int ret;

    string outputDirectory = string(buffer) + "Extraction";

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

void ConvertFile(string filePath)
{
    ConvertFile(filePath, "", false);
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
            cout << "Error finding the file\n";
            return;
        }

        if (bytes[0] != 'B' || bytes[1] != 'S' || bytes[2] != '0' || bytes[3] != '9')
        {
            printf("File header is incorrect!\n");
            return;
        }

        unsigned int i;
        unsigned int vertexSize = sizeof(Vertex); // 16
        unsigned int lightVertSize = sizeof(LightVert); // 48

        int cursor = 8;

        unsigned long nameLength = ReadULong(bytes, cursor); cursor += 4;

        char* Name = (char*)(&bytes[cursor]); // s01 file path in local dir
        cursor += nameLength + 1; //SeekUntilNull(bytes, cursor);

        unsigned int numberOfVerts = ReadUInt(bytes, cursor); cursor += 4;

        vector<TransVert> verts;
        for (i = 0; i < numberOfVerts; i++)
        {
            TransVert tVert{};
            tVert.V = *(Vertex*)(&bytes[cursor]);
            tVert.V.x *= -modelScaleDown;
            tVert.V.y *= modelScaleDown;
            tVert.V.z *= modelScaleDown;
            cursor += vertexSize;
            verts.push_back(tVert);
        }

        unsigned long numberOfLVerts = ReadULong(bytes, cursor); cursor += 4;
        unsigned long numberOfBaseVerts = ReadULong(bytes, cursor); cursor += 4;
        unsigned long numberOfCurrVerts = ReadULong(bytes, cursor); cursor += 4;

        vector<LightVert> lightVerts;
        for (i = 0; i < numberOfLVerts; i++)
        {
            LightVert newVert{};
            
            Vertex vec = *(Vertex*)(&bytes[cursor]);
            cursor += vertexSize;
            newVert.N = vec;

            unsigned long VIndex = ReadULong(bytes, cursor); cursor += 4;
            newVert.VIndex = VIndex;

            newVert.TV = &verts[VIndex];

            lightVerts.push_back(newVert);
        }

        unsigned long numBlocks = ReadULong(bytes, cursor); cursor += 4;
        unsigned long animFrames = ReadULong(bytes, cursor); cursor += 4;

        unsigned long instanceLongs[BASE_OBJECT_SPECIFIC_DATA_COUNT]{};
        memcpy(instanceLongs, &bytes[cursor], sizeof(unsigned long) * BASE_OBJECT_SPECIFIC_DATA_COUNT);
        cursor += BASE_OBJECT_SPECIFIC_DATA_COUNT * 4;

        float instanceFloats[BASE_OBJECT_SPECIFIC_DATA_COUNT]{};
        memcpy(instanceLongs, &bytes[cursor], sizeof(float) * BASE_OBJECT_SPECIFIC_DATA_COUNT);
        cursor += BASE_OBJECT_SPECIFIC_DATA_COUNT * 4;

        float size = ReadFloat(bytes, cursor); cursor += 4;

        Vertex vertMin = ReadVertex(bytes, cursor); cursor += vertexSize;
        Vertex vertMax = ReadVertex(bytes, cursor); cursor += vertexSize;
        Vertex staticTMOffset = ReadVertex(bytes, cursor); cursor += vertexSize;

        Vertex boundingCentreOffset = ReadVertex(bytes, cursor); cursor += vertexSize;
        Vertex boundingBoxExtents = ReadVertex(bytes, cursor); cursor += vertexSize;
        Vertex smallBoundingBoxScale = ReadVertex(bytes, cursor); cursor += vertexSize;
        Vertex recipBoundingBoxExtents = ReadVertex(bytes, cursor); cursor += vertexSize;

        float boundingSphereRadius = ReadFloat(bytes, cursor); cursor += 4;

        FaceList* objFaceList[MAX_FACELISTS]{};

        for (i = 0; i < MAX_FACELISTS; i++)
        {
            int FLValid = ReadInt(bytes, cursor); cursor += 4;

            if (FLValid)
            {
                switch (i)
                {
                case FACES_BUMP:
                    break;
                case FACES_TEXTURE:
                    objFaceList[FACES_TEXTURE] = new TextureFaceList();
                    break;
                case FACES_REFLECT:
                    objFaceList[FACES_REFLECT] = new EnvironmentMap();
                    break;
                }
            }
        }

        for (i = 0; i < MAX_FACELISTS; i++)
            if (objFaceList[i])
                objFaceList[i]->Load(bytes, cursor);


        // ************************
        // Object load end
        // Start matrix object load

        //vector<MatrixObjBlock> MatBlocks;
        vector<SuperObjBlock> MatBlocks;
        MatBlocks.resize(numBlocks);

        for (i = 0; i < numBlocks; i++)
            MatBlocks[i].Load(bytes, cursor, animFrames);


        //int NumLOD = ReadInt(bytes, cursor); cursor += 4;

        // *****************************************
        // Conversion to obj writer readable format:

        vector<ObjObject> objects;

        //cout << lightVerts.size() << '\n';

        int realVertCount = 0;
        for (unsigned long i = 0; i < numBlocks; i++)
        {
            int startVert = MatBlocks[i].FirstVert;
            int endVert = startVert + MatBlocks[i].NumVerts;
            realVertCount += endVert - startVert;
        }

        for (unsigned long i = 0; i < numBlocks; i++) // numBlocks assumed as => Number of name, vert, Tvert, quads, tris objects
        {
            ObjBlock* blockData = &MatBlocks[i];
            ObjObject newObj;

            newObj.name = blockData->Name;

            newObj.frameAnimStart = 0;

            newObj.position = Vector3(blockData->StaticTM.t.x, blockData->StaticTM.t.y, blockData->StaticTM.t.z);
            newObj.position *= modelScaleDown;

            int startVert = blockData->FirstVert;
            int endVert = startVert + blockData->NumVerts;

            for (; startVert < endVert; startVert++)
                newObj.verts.push_back(Vector3(verts[startVert].V.x, verts[startVert].V.y, verts[startVert].V.z));

            TextureFaceList* textureFaceList = (TextureFaceList*)(objFaceList[1]);
            TextureBlockInfo* texInfoBlock = &(textureFaceList->Info[i]);

            int startQuad = texInfoBlock->List[0].FirstQuad;
            int endQuad = startQuad + texInfoBlock->List[0].NumQuads;

            //cout << '\n';

            for (; startQuad < endQuad; startQuad++)
            {
                Quad newQuad;

                TextureQuad* TQuad = &(textureFaceList->Quads[startQuad]);
                newQuad.textureName = texInfoBlock->name;
                
                int lvFisrt = blockData->FirstLVert;
                newQuad.vertIds[0] = lightVerts[(size_t)TQuad->a + lvFisrt].VIndex + 1;
                newQuad.vertIds[1] = lightVerts[(size_t)TQuad->b + lvFisrt].VIndex + 1;
                newQuad.vertIds[2] = lightVerts[(size_t)TQuad->c + lvFisrt].VIndex + 1;
                newQuad.vertIds[3] = lightVerts[(size_t)TQuad->d + lvFisrt].VIndex + 1;

                if (newQuad.vertIds[0] > realVertCount || newQuad.vertIds[1] > realVertCount || newQuad.vertIds[2] > realVertCount || newQuad.vertIds[3] > realVertCount)
                {
                    newQuad.vertIds[0] = lightVerts[TQuad->a].VIndex + 1;
                    newQuad.vertIds[1] = lightVerts[TQuad->b].VIndex + 1;
                    newQuad.vertIds[2] = lightVerts[TQuad->c].VIndex + 1;
                    newQuad.vertIds[3] = lightVerts[TQuad->d].VIndex + 1;
                    //cout << "Vert ID vent out of vert array range!\n";
                }

                //cout << newQuad.vertIds[0] << ' ' << newQuad.vertIds[1] << ' ' << newQuad.vertIds[2] << ' ' << newQuad.vertIds[3] << '\n';

                if (newObj.textureFilenames.size() == 0 && texInfoBlock->name.size() > 0)
                    newObj.textureFilenames.push_back(texInfoBlock->name);

                newObj.texVerts.push_back(pair<float, float>(TQuad->TVert[0].u, 1 - TQuad->TVert[0].v));
                newQuad.texVertIds[0] = (int)newObj.texVerts.size();
                newObj.texVerts.push_back(pair<float, float>(TQuad->TVert[1].u, 1 - TQuad->TVert[1].v));
                newQuad.texVertIds[1] = (int)newObj.texVerts.size();
                newObj.texVerts.push_back(pair<float, float>(TQuad->TVert[2].u, 1 - TQuad->TVert[2].v));
                newQuad.texVertIds[2] = (int)newObj.texVerts.size();
                newObj.texVerts.push_back(pair<float, float>(TQuad->TVert[3].u, 1 - TQuad->TVert[3].v));
                newQuad.texVertIds[3] = (int)newObj.texVerts.size();

                newObj.quads.push_back(newQuad);
            }

            //cout << '\n';

            int startTri = texInfoBlock->List[0].FirstTri;
            int endTri = startTri + texInfoBlock->List[0].NumTris;

            for (; startTri < endTri; startTri++)
            {
                Tris newTris;
                
                TextureTri* TTri = &(textureFaceList->Tris[startTri]);
                newTris.textureName = texInfoBlock->name;

                int lvFisrt = blockData->FirstLVert;
                newTris.vertIds[0] = lightVerts[(size_t)TTri->a + lvFisrt].VIndex + 1;
                newTris.vertIds[1] = lightVerts[(size_t)TTri->b + lvFisrt].VIndex + 1;
                newTris.vertIds[2] = lightVerts[(size_t)TTri->c + lvFisrt].VIndex + 1;

                if (newTris.vertIds[0] > realVertCount || newTris.vertIds[1] > realVertCount || newTris.vertIds[2] > realVertCount)
                {
                    newTris.vertIds[0] = lightVerts[TTri->a].VIndex + 1;
                    newTris.vertIds[1] = lightVerts[TTri->b].VIndex + 1;
                    newTris.vertIds[2] = lightVerts[TTri->c].VIndex + 1;
                    //cout << "Vert ID vent out of vert array range!\n";
                }

                if (newObj.textureFilenames.size() == 0 && texInfoBlock->name.size() > 0)
                    newObj.textureFilenames.push_back(texInfoBlock->name);

                newObj.texVerts.push_back(pair<float, float>(TTri->TVert[0].u, 1 - TTri->TVert[0].v));
                newTris.texVertIds[0] = (int)newObj.texVerts.size();
                newObj.texVerts.push_back(pair<float, float>(TTri->TVert[1].u, 1 - TTri->TVert[1].v));
                newTris.texVertIds[1] = (int)newObj.texVerts.size();
                newObj.texVerts.push_back(pair<float, float>(TTri->TVert[2].u, 1 - TTri->TVert[2].v));
                newTris.texVertIds[2] = (int)newObj.texVerts.size();

                newObj.tris.push_back(newTris);
            }

            objects.push_back(newObj);
        }

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

        cout << "Conversion complete! \n";
    }
    catch (...)
    {
        cout << "Failed to convert a file! \n" << filePath << "\n";
    }
}