
#include <Object.h>

Quad::Quad()
{
	vertIds[0] = vertIds[1] = vertIds[2] = vertIds[3] = -1;
	texVertIds[0] = texVertIds[1] = texVertIds[2] = texVertIds[3] = -1;

	memset(&normals, 0, 12);

	textureName = "";
	glow = 0;

	opacityMode = "";
	opacityStrength = 0;

	enviroMapTex = "";
	enviroMapStrength = 0;
}

Tris::Tris()
{
	texVertIds[0] = -1;

	textureName = "";
	glow = 0;

	opacityMode = "";
	opacityStrength = 0;

	enviroMapTex = "";
	enviroMapStrength = 0;
}

ObjObject::ObjObject()
{
	name = "";
}

void ObjObject::Save(Object loadedObj)
{
}

Vertex2D::Vertex2D()
{
	u = v = x = y = z = 0;
}

FaceRenderList::FaceRenderList()
{
	FirstQuad = 0;
	FirstTri = 0;
	NumTris = 0;
	OpType = TransType::TT_NONE;
	NumQuads = 0;
	SelfIllumination = false;
	ZWrite = true;
}

TextureBlockInfo::TextureBlockInfo()
{
	name = "";
	NumLists = 0;
}

TextureFaceList::TextureFaceList()
{
}

EnvironmentMap::EnvironmentMap()
{
	LateralShift = 0.f;
	name = "";
}

FaceList::FaceList()
{
	NumBlocks = CurrBlock = 0;
	CurrQuad = NumQuads = 0;
	CurrTri = NumTris = 0;
}

EnviroTri::EnviroTri()
{
	Shine = 0;
}

EnviroQuad::EnviroQuad()
{
	Shine = 0;
}

TextureTri::TextureTri()
{
	Glow = false;
	Opacity = 0.f;
	OpType = TransType::TT_NONE;
}

TextureQuad::TextureQuad()
{
	Glow = false;
	Opacity = 0.f;
	OpType = TransType::TT_NONE;
}

unsigned long RReadULong(vector<unsigned char>& allData, int index)
{
	return *(unsigned long*)&allData[index];
}

unsigned int RReadUInt(vector<unsigned char>& allData, int index)
{
	return *(unsigned int*)&allData[index];
}

float RReadFloat(vector<unsigned char>& allData, int index)
{
	return *(float*)&allData[index];
}

int RReadInt(vector<unsigned char>& allData, int index)
{
	return *(int*)&allData[index];
}

bool FaceList::Load(vector<unsigned char>& allData, int &index)
{
	NumBlocks = RReadUInt(allData, index); index += 4;
	CurrQuad = RReadUInt(allData, index); index += 4;
	NumQuads = RReadUInt(allData, index); index += 4;
	CurrTri = RReadUInt(allData, index); index += 4;
	NumTris = RReadUInt(allData, index); index += 4;

	Block.resize(NumBlocks);

	for (int i = 0; i < NumBlocks; i++)
		Block[i].Load(allData, index);

	return true;
}

bool BlockSection::Load(vector<unsigned char>& allData, int &index)
{
	FirstQuad = RReadULong(allData, index); index += 4;
	NumQuads = RReadULong(allData, index); index += 4;
	FirstTri = RReadULong(allData, index); index += 4;
	NumTris = RReadULong(allData, index); index += 4;

	return true;
}

bool TextureFaceList::Load(vector<unsigned char>& allData, int& index)
{
	unsigned int i;
	int size;

	FaceList::Load(allData, index);

	Tris.resize(NumTris);
	Quads.resize(NumQuads);

	if (NumTris)
		for (i = 0; i < NumTris; i++)
		{
			size = offsetof(TextureTri, OpType) + sizeof(Tris[i].OpType);
			memcpy(&Tris[i], &allData[index], size);
			index += size;
		}

	if (NumQuads)
		for (i = 0; i < NumQuads; i++)
		{
			size = offsetof(TextureQuad, OpType) + sizeof(Quads[i].OpType);
			memcpy(&Quads[i], &allData[index], size);
			index += size;
		}

	Info.resize(NumBlocks);

	for (i = 0; i < NumBlocks; i++)
		Info[i].Load(allData, index);

	return true;
}

bool EnvironmentMap::Load(vector<unsigned char>& allData, int& index)
{
	int Len, size;

	FaceList::Load(allData, index);

	Tris.resize(NumTris);
	Quads.resize(NumQuads);

	if (NumTris)
	{
		size = sizeof(EnviroTri) * NumTris;
		memcpy(&Tris[0], &allData[index], size);
		index += size;
	}

	if (NumQuads)
	{
		size = sizeof(EnviroQuad) * NumQuads;
		memcpy(&Quads[0], &allData[index], size);
		index += size;
	}

	Len = RReadInt(allData, index); index += 4;

	if (Len)
	{
		name.resize(Len);
		memcpy(&name[0], &allData[index], Len);

		index += Len + 1;
	}

	return true;
}

bool TextureBlockInfo::Load(vector<unsigned char>& allData, int& index)
{
	int Len = RReadInt(allData, index); index += 4;

	if (Len)
	{
		name.resize(Len);
		memcpy(&name[0], &allData[index], (size_t)Len + 1);

		index += Len + 1;
	}

	NumLists = RReadULong(allData, index); index += 4;

	List.resize(NumLists);

	int size = sizeof(FaceRenderList) * NumLists;
	memcpy(&List[0], &allData[index], size);
	index += size;

	return true;
}

Matrix::Matrix()
{
	Normal = false;
	m[0] = Vertex();
	m[1] = Vertex();
	m[2] = Vertex();
	t = Vertex();
}

ObjBlock::ObjBlock()
{
	Name = "";
	Num = 0;
	Size = 0;
	Visible = false;
	CoverCheckEnabled = false;
	FirstVert = 0;
	NumVerts = 0;
	FirstLVert = 0;
	NumLVerts = 0;
	StaticTM = Matrix();
}

Keyframe::Keyframe()
{
	M = Matrix();
	Frame = 0;
}

MatrixObjBlock::MatrixObjBlock()
{
	NumKeys = 0;
}

bool Matrix::Load(vector<unsigned char>& allData, int& index)
{
	*this = *(Matrix*)(&allData[index]);
	index += sizeof(Matrix);
	return true;
}

bool ObjBlock::Load(vector<unsigned char>& allData, int& index)
{
	int Len = RReadInt(allData, index); index += 4;

	if (Len)
	{
		int size = Len + 1;
		Name.resize(Len);
		memcpy(&Name[0], &allData[index], size);
		index += size;
	}

	Num = RReadUInt(allData, index); index += 4;

	StaticTM.Load(allData, index);

	Size = RReadFloat(allData, index); index += 4;

	FirstVert = RReadULong(allData, index); index += 4;
	NumVerts = RReadULong(allData, index); index += 4;
	FirstLVert = RReadULong(allData, index); index += 4;
	NumLVerts = RReadULong(allData, index); index += 4;

	Len = RReadInt(allData, index); index += 4;

	if(Len)
		index += Len + 1; // Skip material

	return true;
}

bool MatrixObjBlock::Load(vector<unsigned char>& allData, int& index, unsigned long AnimFrames)
{
	ObjBlock::Load(allData, index);

	int Bool = RReadInt(allData, index); index += 4;
	int size;

	if (Bool)
	{
		Anims.resize(AnimFrames);
		size = AnimFrames * sizeof(Matrix);
		memcpy(&Anims[0], &allData[index], size);
		index += size;
	}

	NumKeys = RReadULong(allData, index); index += 4;

	if (NumKeys)
	{
		Keys.resize(NumKeys);
		size = NumKeys * sizeof(Keyframe);
		memcpy(&Keys[0], &allData[index], size);
		index += size;
	}

	return true;
}

bool MorphObjBlock::Load(vector<unsigned char>& allData, int& index)
{
	ObjBlock::Load(allData, index);

	NumKeys = RReadInt(allData, index); index += 4;
	if (NumKeys)
	{
		Keys.resize(NumKeys);
		int size = NumKeys * sizeof(KeyVerts);
		memcpy(&Keys[0], &allData[index], size);
		index += size;
	}

	return true;
}

bool SuperObjBlock::Load(vector<unsigned char>& allData, int& index, unsigned long AnimFrames)
{
	ObjBlock::Load(allData, index);

	numKeyVerts = RReadInt(allData, index); index += 4;

	if (numKeyVerts > 1) // Morph mode
	{
		keyVerts.resize(numKeyVerts);
		int size = numKeyVerts * sizeof(KeyVerts);
		memcpy(&keyVerts[0], &allData[index], size);
		index += size;
		return true;
	}

	int size;

	if (numKeyVerts)
	{
		numKeyVerts = 0;

		Anims.resize(AnimFrames);
		size = AnimFrames * sizeof(Matrix);
		memcpy(&Anims[0], &allData[index], size);
		index += size;
	}

	numKeyFrames = RReadULong(allData, index); index += 4;

	if (numKeyFrames)
	{
		keyFrames.resize(numKeyFrames);
		size = numKeyFrames * sizeof(Keyframe);
		memcpy(&keyFrames[0], &allData[index], size);
		index += size;
	}

	return true;
}

#define ReadData(x) memcpy(&x, &bytes[cursor], sizeof(x)); cursor += sizeof(x);

const float modelScaleDown = 0.0225f;

void Object::Load(vector<unsigned char>& bytes, int& cursor)
{
	unsigned int i;
	unsigned int vertexSize = sizeof(Vertex); // 16
	unsigned int lightVertSize = sizeof(LightVert); // 48

	unsigned long nameLength; ReadData(nameLength);

	char* Name = (char*)(&bytes[cursor]); // s01 file path in local dir
	cursor += nameLength + 1;
	name = string(Name);

	ReadData(numberOfVerts);

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

	ReadData(numberOfLVerts);
	ReadData(numberOfBaseVerts);
	ReadData(numberOfCurrVerts);

	for (i = 0; i < numberOfLVerts; i++)
	{
		LightVert newVert{};

		Vertex vec = *(Vertex*)(&bytes[cursor]);
		cursor += vertexSize;
		newVert.N = vec;

		ReadData(newVert.VIndex);
		newVert.TV = &verts[newVert.VIndex];

		lightVerts.push_back(newVert);
	}

	ReadData(numBlocks);
	ReadData(animFrames);

	memcpy(instanceLongs, &bytes[cursor], sizeof(unsigned long) * BASE_OBJECT_SPECIFIC_DATA_COUNT);
	cursor += BASE_OBJECT_SPECIFIC_DATA_COUNT * 4;

	memcpy(instanceFloats, &bytes[cursor], sizeof(float) * BASE_OBJECT_SPECIFIC_DATA_COUNT);
	cursor += BASE_OBJECT_SPECIFIC_DATA_COUNT * 4;

	ReadData(size);

	ReadData(vertMin);
	ReadData(vertMax);
	ReadData(staticTMOffset);

	ReadData(boundingCentreOffset);
	ReadData(boundingBoxExtents);
	ReadData(smallBoundingBoxScale);
	ReadData(recipBoundingBoxExtents);

	ReadData(boundingSphereRadius);

	objFaceList.resize(MAX_FACELISTS);

	for (i = 0; i < MAX_FACELISTS; i++)
	{
		int FLValid; ReadData(FLValid);

		if (FLValid > 1) // Skip weird rubbish bytes
		{
			i--;
			continue;
		}

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

	MatBlocks.resize(numBlocks);

	for (i = 0; i < numBlocks; i++)
		MatBlocks[i].Load(bytes, cursor, animFrames);
}

vector<ObjObject> Object::ConvertToObj()
{
	vector<ObjObject> objects;

	if (objFaceList[1] == nullptr)
	{
		printf("Can't convert this model because it has no faces/tris!\n");
		return objects;
	}

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

	return objects;
}
